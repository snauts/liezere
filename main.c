#include "main.h"
#include "data.h"

void start_up(void) __naked {
    __asm__("di");
    __asm__("jp _reset");
}

static volatile byte vblank;
static byte *map_y[192];
static word seed;

#define SETUP_STACK()	__asm__("ld sp, #0xfdfc")
#define FONT_ADDRESS	PTR(0x3c00)
#define IRQ_BASE	0xfe00

#define	CTRL_FIRE	0x10
#define	CTRL_DIR	0x0f
#define	CTRL_UP		0x08
#define	CTRL_DOWN	0x04
#define	CTRL_LEFT	0x02
#define	CTRL_RIGHT	0x01

static void interrupt(void) __naked {
    __asm__("di");
    __asm__("push af");

    __asm__("ld a, #1");
    __asm__("ld (_vblank), a");

    __asm__("pop af");
    __asm__("ei");
    __asm__("reti");
}

static void setup_irq(byte base) {
    __asm__("di");
    __asm__("ld i, a"); base;
    __asm__("im 2");
    __asm__("ei");
}

static void wait_vblank(void) {
    vblank = 0;
    while (!vblank) { }
}

static byte in_key(byte a) {
    __asm__("in a, (#0xfe)");
    return a;
}

static byte in_joy(byte a) {
    __asm__("in a, (#0x1f)"); a;
    return a;
}

static void out_fe(byte data) {
    __asm__("out (#0xfe), a"); data;
}

void __sdcc_call_iy(void) __naked {
    __asm__("jp (iy)");
}

void __sdcc_call_hl(void) __naked {
    __asm__("jp (hl)");
}

static byte inc10(byte a) __naked {
    __asm__("inc a"); a;
    __asm__("daa");
    __asm__("ret");
}

void memset(byte *ptr, byte data, word len) {
    while (len-- > 0) { *ptr++ = data; }
}

void memcpy(void *dst, const void *src, word len) __naked {
    __asm__("ex de, hl");
    __asm__("pop iy");
    __asm__("pop bc");
    __asm__("ld a, c");
    __asm__("or a, b");
    __asm__("jr Z, done");
    __asm__("push de");
    __asm__("ldir");
    __asm__("pop de");
    __asm__("done:");
    __asm__("jp (iy)");
    dst; src; len;
}

static word random(void) {
    seed ^= seed << 7;
    seed ^= seed >> 9;
    seed ^= seed << 8;
    return seed;
}

static word mul(byte x, byte y) {
    word r = 0;
    word n = x;
    for (int i = 0; i < 8; i++) {
        if (y & 1) r += n;
        y = y >> 1;
        n = n << 1;
    }
    return r;
}

static void setup_system(void) {
    byte top = (byte) ((IRQ_BASE >> 8) - 1);
    word jmp_addr = (top << 8) | top;
    BYTE(jmp_addr + 0) = 0xc3;
    WORD(jmp_addr + 1) = ADDR(&interrupt);
    memset((byte *) IRQ_BASE, top, 0x101);
    setup_irq(IRQ_BASE >> 8);
}

static void precalculate(void) {
    for (byte y = 0; y < 192; y++) {
	byte f = ((y & 7) << 3) | ((y >> 3) & 7) | (y & 0xc0);
	map_y[y] = SCREEN(f << 5);
    }
}

static void clear_block(byte y, byte h) {
    byte **row = map_y + y;
    for (byte i = 0; i < h; i++) {
	memset(*row++, 0, 32);
    }
}

static void clear_screen(void) {
    memset(COLOUR(0), 0x00, 0x300);
    clear_block(0, 192);
    out_fe(0);
}

static void put_symbol(const byte *addr, byte x, byte y, byte n) {
    byte shift = x & 7;
    byte offset = x >> 3;
    for (byte i = 0; i < n; i++) {
	byte data = *addr++;
	byte *ptr = map_y[y + i] + offset;
	ptr[0] |= (data >> shift);
	ptr[1] |= (data << (8 - shift));
    }
}

static void put_char(char symbol, byte x, byte y) {
    put_symbol(FONT_ADDRESS + (symbol << 3), x, y, 8);
}

static void put_diacritic(const byte *addr, byte x, byte y) {
    put_symbol(addr, x, y, 3);
}

static byte char_mask(char symbol) {
    byte mask = 0;
    byte *addr = FONT_ADDRESS + (symbol << 3);
    for (byte i = 0; i < 8; i++) {
	mask |= *addr++;
    }
    return mask;
}

static byte leading(char symbol) {
    byte i;
    byte mask = char_mask(symbol);
    for (i = 0; i < 8; i++) {
	if (mask & 0x80) goto done;
	mask = mask << 1;
    }
  done:
    return i - 1;
}

static byte trailing(char symbol) {
    byte i;
    byte mask = char_mask(symbol);
    for (i = 0; i < 8; i++) {
	if (mask & 1) goto done;
	mask = mask >> 1;
    }
  done:
    return 8 - i;
}

#define LOWER_CASE(c) ((c) & 0x20)

static void put_dash(char c, byte x, byte y) {
    static const byte dash[] = { 0x3c, 0x00, 0x00, 0x38, 0x00, 0x00, 0x70 };
    const byte *ptr = dash;

    if (c == 'i') {
	ptr += 4;
    }
    else if (c == 'I') {
	ptr += 3;
    }
    else if (LOWER_CASE(c)) {
	ptr += 2;
    }

    put_diacritic(ptr, x, y - 1);
}

static void put_check(char c, byte x, byte y) {
    static const byte check[] = { 0x24, 0x18, 0x00, 0x28, 0x10 };
    const byte *ptr = check;

    if (LOWER_CASE(c)) ptr += 2;

    put_diacritic(ptr, x, y - 2);
}

static void put_tick(char c, byte x, byte y) {
    static const byte tick[] = { 0x08, 0x10, 0x00 };

    if (LOWER_CASE(c)) x--;

    if (c == 'k') x--;

    put_diacritic(tick, x, y + 7);
}

static void put_str(const char *msg, byte x, byte y) {
    while (*msg != 0) {
	char symbol = *(msg++);
	if (symbol == ' ') {
	    x = x + 4;
	}
	else if (symbol == '`') {
	    put_dash(*msg, x, y);
	}
	else if (symbol == '~') {
	    put_check(*msg, x, y);
	}
	else if (symbol == '^') {
	    put_tick(*msg, x, y);
	}
	else {
	    if (x > 0) x -= leading(symbol);
	    put_char(symbol, x, y);
	    x += trailing(symbol);
	}
    }
}

static void *decompress(byte *dst, const byte *src) {
    while (*src) {
        byte n = *src & 0x7f;
        if (*(src++) & 0x80) {
	    while (n-- > 0) {
		*dst = *(dst - *src);
		dst++;
	    }
	    src++;
        }
        else {
	    while (n-- > 0) {
		*(dst++) = *(src++);
	    }
        }
    }
    return dst;
}

static void show_image(const byte *src, byte x, byte y) {
    byte *ptr = STAGING_AREA;
    decompress(ptr, src + 2);

    byte w = src[0];
    byte h = src[1] << 3;

    y = y << 3;

    for (byte i = 0; i < h; i++) {
	byte *dst = map_y[y + i] + x;
	memcpy(dst, ptr, w);
	ptr += w;
    }

    byte *dst = COLOUR(y << 2) + x;
    for (byte i = 0; i < h; i += 8) {
	memcpy(dst, ptr, w);
	dst += 0x20;
	ptr += w;
    }
}

static byte use_joy;
static byte last_input;

static byte input_change(byte input) {
    byte change = input & (input ^ last_input);
    last_input = input;
    return change;
}

static byte read_1_or_2(void) {
    return ~in_key(0xf7) & 3;
}

static void animate_line(void) {
    for (byte y = 0; y < 34; y++) {
	byte *ptr = map_y[y] + 22;
	*ptr ^= 0x10;
    }
}

static void wait_1_or_2(void) {
    byte ticks = 0;
    last_input = read_1_or_2();
    while (!input_change(read_1_or_2())) {
	if (vblank) {
	    vblank = 0;
	    ticks++;
	}
	if (ticks == 5) {
	    animate_line();
	    ticks = 0;
	}
    }
    use_joy = last_input & 2;
}

typedef struct {
    const byte *img;
    byte x, y;
} Frame;

static void show_series(const Frame *series) {
    while (series->img) {
	show_image(series->img, series->x, series->y);
	series++;
    }
}

static const Frame apkaime[] = {
    { .img = puduris1, .x = 2,  .y = 14 },
    { .img = puduris1, .x = 5,  .y = 11 },
    { .img = puduris1, .x = 7,  .y = 22 },
    { .img = puduris1, .x = 10, .y = 10 },
    { .img = puduris1, .x = 16, .y = 8  },
    { .img = puduris1, .x = 18, .y = 4  },
    { .img = puduris1, .x = 22, .y = 18 },
    { .img = puduris2, .x = 0,  .y = 17 },
    { .img = puduris2, .x = 15, .y = 22 },
    { .img = puduris2, .x = 20, .y = 21 },
    { .img = puduris2, .x = 29, .y = 3  },
    { .img = puduris2, .x = 30, .y = 8  },
    { .img = puduris2, .x = 30, .y = 13 },
    { .img = niedres,  .x = 2,  .y = 19 },
    { .img = niedres,  .x = 7,  .y = 15 },
    { .img = niedres,  .x = 8,  .y = 14 },
    { .img = niedres,  .x = 8,  .y = 14 },
    { .img = niedres,  .x = 12, .y = 13 },
    { .img = niedres,  .x = 13, .y = 22 },
    { .img = niedres,  .x = 20, .y = 7  },
    { .img = niedres,  .x = 21, .y = 10 },
    { .img = niedres,  .x = 21, .y = 17 },
    { .img = niedres,  .x = 23, .y = 4  },
    { .img = niedres,  .x = 26, .y = 1  },
    { .img = niedres,  .x = 30, .y = 7  },
    { .img = niedres,  .x = 30, .y = 11 },
    { .img = niedres,  .x = 30, .y = 21 },
    { .img = NULL },
};

#define PIXEL(x, y) BYTE(map_y[y] + (x >> 3))
#define PMASK(pos) (0x80 >> (pos))

static byte is_white(byte x, byte y) {
    word offset = ((y & ~7) << 2) + (x >> 3);
    return BYTE(COLOUR(offset)) == 0x47;
}

static byte get_pixel(byte x, byte y) {
    return PIXEL(x, y) & PMASK(x & 7);
}

static void set_pixel(byte x, byte y) {
    PIXEL(x, y) ^= PMASK(x & 7);
}

static byte good_spot(byte x, byte y) {
    return is_white(x, y) && get_pixel(x, y);
}

static byte read_QAOP(void) {
    byte ret = 0;
    byte hit = in_key(0x7f);
    ret |= hit & (hit >> 2);
    ret <<= 1;
    ret |= (in_key(0xfb) & 1);
    ret <<= 1;
    ret |= (in_key(0xfd) & 1);
    ret <<= 2;
    ret |= (in_key(0xdf) & 3);
    return ~ret;
}

static byte read_input(void) {
    return use_joy ? in_joy(0) : read_QAOP();
}

static byte fire_asserted(void) {
    return input_change(read_input()) & CTRL_FIRE;
}

static const int8 cursor[] = {
    0,  1,  0, -1,
    1, -1, -1,  1,
    1,  0, -1,  0,
    1,  1, -1, -1,
};

static byte cursor_x;
static byte cursor_y;

static byte cursor_frame;

static byte steps;
static byte holes;
static byte hole_map[192];

static byte minute;
static byte hour;

static void draw_cursor(void) {
    const int8 *dir = cursor + (cursor_frame & 0xf);
    set_pixel(cursor_x + dir[0], cursor_y + dir[1]);
    set_pixel(cursor_x + dir[2], cursor_y + dir[3]);
}

static void reset_cursor(void) {
    steps = 0;
    holes = 0;
    cursor_x = 8;
    cursor_y = 180;
    cursor_frame = 0;
    minute = 0;
    hour = 9;
}

static void put_digit(byte digit, byte x, byte y) {
    byte *addr = FONT_ADDRESS;
    addr += (('0' + digit) << 3);
    for (byte i = 0; i < 8; i++) {
	BYTE(map_y[y + i] + x) = *addr++;
    }
}

static void put_num(byte num, byte x, byte y) {
    put_digit(num >> 4, x, y);
    put_digit(num & 0xf, x + 1, y);
}

static void put_time(void) {
    put_num(hour, 0, 0);
    put_num(minute, 3, 0);
    put_digit(10, 2, 0);
}

static void advance_time(byte amount) {
    while (amount-- > 0) {
	minute = inc10(minute);
	if (minute == 0x60) {
	    hour = inc10(hour);
	    minute = 0;
	}
    }
    put_time();
}

static byte visited(byte x, byte y) {
    byte *ptr = hole_map;
    for (byte i = 0; i < holes; i++) {
	if (x == *(ptr++) && y == *(ptr++)) {
	    return true;
	}
    }
    return false;
}

static void set_cursor(byte x, byte y) {
    draw_cursor();
    if (good_spot(x, y) || visited(x, y)) {
	if (++steps == 4) {
	    advance_time(1);
	    steps = 0;
	}
	cursor_x = x;
	cursor_y = y;
    }
    draw_cursor();
}

static void move_cursor(void) {
    byte button = read_input();
    byte x = cursor_x;
    byte y = cursor_y;

    if (button & CTRL_UP) {
	y--;
    }
    else if (button & CTRL_DOWN) {
	y++;
    }
    else if (button & CTRL_LEFT) {
	x--;
    }
    else if (button & CTRL_RIGHT) {
	x++;
    }
    else {
	return;
    }

    set_cursor(x, y);
}

static byte not_late(void) {
    return hour < 0x16;
}

static void walk_lake(void) {
    byte ticks = 0;

    draw_cursor();
    while (not_late()) {
	if (vblank) {
	    move_cursor();
	    if (++ticks == 2) {
		draw_cursor();
		cursor_frame += 4;
		draw_cursor();
		ticks = 0;
	    }
	    vblank = 0;
	}
	if (fire_asserted()) {
	    break;
	}
    }
    draw_cursor();
}

static void drill_hole(void) {
    if (!visited(cursor_x, cursor_y)) {
	hole_map[holes++] = cursor_x;
	hole_map[holes++] = cursor_y;
	set_pixel(cursor_x, cursor_y);
	advance_time(5);
    }
}

static void show_lake(void) {
    show_image(ezers, 0, 0);
    show_series(apkaime);
    put_time();

    while (not_late()) {
	walk_lake();
	drill_hole();
    }
}

static void init_variables(void) {
    last_input = read_input();
    reset_cursor();
    seed = 0xfeed;
}

static void show_title(void) {
    show_image(title, 0, 0);
    put_str("1 - Klaviat`ura", 88, 96);
    put_str("2 - D~zoistiks", 88, 112);
    memset(COLOUR(0x140), 0x47, 0x100);
    wait_1_or_2();
}

void reset(void) {
    SETUP_STACK();
    setup_system();
    precalculate();
    clear_screen();
    show_title();
    clear_screen();
    init_variables();
    show_lake();
    reset();
}
