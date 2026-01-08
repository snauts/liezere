#include "main.h"
#include "data.h"

void start_up(void) __naked {
    __asm__("di");
    __asm__("jp _reset");
}

static volatile byte vblank;
static byte *map_y[192];
static byte *line[96];
static word seed;

extern const Frame horizonts[];
extern const Frame apkaime[];

#define SETUP_STACK()	__asm__("ld sp, #0xfdfc")
#define FONT_ADDRESS	PTR(0x3c00)
#define IRQ_BASE	0xfe00

#define	CTRL_FIRE	0x10
#define	CTRL_DIR	0x0f
#define	CTRL_UP		0x08
#define	CTRL_DOWN	0x04
#define	CTRL_LEFT	0x02
#define	CTRL_RIGHT	0x01

#define	IMAGE_DATA(x)	((x) + 2)

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

static void delay(byte ticks) {
    while (ticks-- > 0) { wait_vblank(); }
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
    decompress(ptr, IMAGE_DATA(src));

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

static void show_series(const Frame *series) {
    while (series->img) {
	show_image(series->img, series->x, series->y);
	series++;
    }
}

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

static byte asserted(byte ctrl) {
    return input_change(read_input()) & ctrl;
}

static byte wait_asserted(byte ctrl) {
    byte ticks = 0;
    while (!asserted(ctrl)) {
	ticks++;
    }
    return ticks;
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
static byte *hole_end;
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
    cursor_x = 8;
    cursor_y = 180;
    cursor_frame = 0;
    hole_end = hole_map;
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
    while (ptr < hole_end) {
	byte hole_x = *(ptr++);
	byte hole_y = *(ptr++);
	if (x == hole_x && y == hole_y) {
	    return true;
	}
    }
    return false;
}

static void set_cursor(byte x, byte y) {
    draw_cursor();
    if (good_spot(x, y) || visited(x, y)) {
	if (++steps == 3) {
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
	if (asserted(CTRL_FIRE)) {
	    break;
	}
    }
    draw_cursor();
}

static void show_forest(void) {
    clear_screen();
    memset(COLOUR(0), 0x28, 0x80);
    memset(COLOUR(0x1c0), 0x78, 0x140);
    show_series(horizonts);
}

static void animate_drill(void) {
    for (byte i = 0; i < 5; i++) {
	advance_time(1);
	show_image(urbis, 14, 8);
	wait_asserted(CTRL_LEFT);
	show_image(swirl, 14, 10);
	show_image(drill, 16, 15);
	wait_asserted(CTRL_RIGHT);
    }
}

static void drill_hole(void) {
    if (!visited(cursor_x, cursor_y) && not_late()) {
	*(hole_end++) = cursor_x;
	*(hole_end++) = cursor_y;
	show_forest();
	animate_drill();
    }
}

static void draw_holes(void) {
    byte *ptr = hole_map;
    while (ptr < hole_end) {
	byte x = *(ptr++);
	byte y = *(ptr++);
	set_pixel(x, y);
    }
}

static void show_lake(void) {
    clear_screen();
    show_image(ezers, 0, 0);
    show_series(apkaime);
    draw_holes();
    put_time();
}

#define COPENE1 (STAGING_AREA + 0x100)
#define COPENE2 (STAGING_AREA + 0x200)

static void draw_tip(byte *ptr) {
    for (byte i = 0; i < 16; i++) {
	byte *dst = map_y[i] + 16;
	memcpy(dst, ptr, 3);
	ptr += 3;
    }
}

static void starting_line(void) {
    for (byte y = 16; y <= 152; y += 4) {
	set_pixel(128, y);
    }
}

static byte *line_addr(byte y) {
    return map_y[y] + 16;
}

static void init_fishing_line(void) {
    byte **ptr = line;
    memset(line, 0, sizeof(line));
    for (byte y = 16; y <= 152; y += 2) {
	*(ptr++) = line_addr(y);
    }
    for (byte y = 162; y <= 167; y++) {
	*(ptr++) = line_addr(y);
    }
    *(ptr++) = line_addr(157);
    *(ptr++) = line_addr(158);
}

static void fishing_line(void) {
    byte **ptr = line;
    while (*ptr != NULL) {
	**(ptr++) ^= 0x80;
    }
}

static byte wait_jerk(byte state) {
    byte ticks = 0;
    while ((read_input() & CTRL_FIRE) != state) {
	if (vblank && ticks < 255) {
	    vblank = 0;
	    ticks++;
	}
    }
    return ticks;
}

static byte jerk_amount;

static void reset_jerk(void) {
    jerk_amount = 15 + (random() & 0xf);
}

static void clear_tip(void) {
    draw_tip(COPENE1);
    BYTE(COLOUR(0x50)) = 0x7d;

    byte *ptr = SCREEN(0x50);
    for (byte i = 0; i < 8; i++) {
	BYTE(ptr) = i & 3 ? 0x00 : 0x80;
	ptr += 0x100;
    }
}

static byte fish_bite(void) {
    byte ticks = 25;
    show_image(copene3, 16, 0);
    while (ticks > 0) {
	if (asserted(CTRL_UP)) {
	    return true;
	}
	if (vblank) {
	    vblank = 0;
	    ticks--;
	}
    }
    clear_tip();
    reset_jerk();
    return false;
}

static void jerk_tip(byte *img, byte dir) {
    if (wait_jerk(dir) > 10) {
	reset_jerk();
    }
    wait_vblank();
    draw_tip(img);
    fishing_line();
}

static byte jerk_fish(void) {
    byte ticks = 0;

    reset_jerk();
    while (not_late()) {
	jerk_tip(COPENE2, CTRL_FIRE);
	jerk_tip(COPENE1, 0);

	if (++ticks >= 5) {
	    advance_time(1);
	    ticks = 0;
	}

	if (--jerk_amount == 0) {
	    if (fish_bite()) return true;
	}
    }
    return false;
}

static void pull_fish(void) {
    clear_screen();
    show_forest();
    for (;;) {
	show_image(velk1, 13, 11);
	delay(25);
	show_image(velk2, 14, 12);
	delay(25);
    }
}

static void show_ice(void) {
    clear_screen();
    memset(COLOUR(0x00), 0x78, 0x20);
    memset(COLOUR(0x20), 0x7d, 0x2e0);
    decompress(COPENE1, IMAGE_DATA(copene1));
    decompress(COPENE2, IMAGE_DATA(copene2));
    show_image(copene1, 16, 0);
    show_image(hole, 12, 19);
    starting_line();
    put_time();

    if (jerk_fish()) {
	pull_fish();
    }
}

static void fishing(void) {
    while (not_late()) {
	show_lake();
	walk_lake();
	drill_hole();
	if (not_late()) {
	    show_ice();
	}
    }
}

static void init_variables(void) {
    last_input = read_input();
    init_fishing_line();
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
    fishing();
    reset();
}
