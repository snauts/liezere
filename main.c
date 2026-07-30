#include "main.h"

#define MOVE_COOLDOWN	5
#define JERK_INTERVAL	15
#define BITE_INTERVAL	30
#define BITE_DELAY	15
#define SNAP_PENALTY	5
#define DRILL_MOVES	5
#define PULL_MOVES	5
#define PULL_FAST	20
#define PULL_SLOW	80
#define WALK_TIME	5
#define JERK_TIME	5

void reset(void);
void start_up(void) __naked {
    __asm__("jp _reset");
}

static volatile byte vblank;
static const byte *cached;
static byte *map_y[192];
static byte *line[96];
static byte text_mask;
static byte no_text;
static byte hints;
static word seed;

static void *decompress(byte *dst, const byte *src);

extern const char* const stat_strs[];
extern const char* const rank_strs[];
extern const char* const reports[];
extern const Frame horizonts[];
extern const Frame apkaime[];
extern const Frame sprites[];
extern const Frame fishes[];
extern const Text stat_title[];
extern const Text fish_map[];
extern const Text tutorial[];
extern const Text choices[];
extern const Text goodbye[];
extern const Text the_end;
extern const Panel panels[];
extern const byte symbols[];

#define DISABLE_IRQ()	__asm__("di");

#if defined(ZXS)
#define ZXS_CPC(a, b)	(a)
#define SETUP_STACK()	__asm__("ld sp, #0xfdfc")
#define FONT_ADDRESS	PTR(0x3c00)
#define IRQ_BASE	0xfe00

#define set_attributes(from, c, len) memset(COLOUR(from), c, len);
#define reset_attributes(color) set_attributes(0, color, 0x300)
#define select_palette(index, color)
#endif

#if defined(CPC)
#define ZXS_CPC(a, b)	(b)
#define SETUP_STACK()	__asm__("ld sp, #0x81fc")
#define FONT_ADDRESS	(((byte *) &font_rom) - 0x100)
#define IRQ_BASE	0x8200

#define set_attributes(from, c, len)
#define reset_attributes(color)
#endif

#if defined(ZXS)
#define	CTRL_FIRE	0x10
#define	CTRL_DIR	0x0f
#define	CTRL_UP		0x08
#define	CTRL_DOWN	0x04
#define	CTRL_LEFT	0x02
#define	CTRL_RIGHT	0x01
#endif

#if defined(CPC)
#define	CTRL_FIRE	0x30
#define	CTRL_DIR	0x0f
#define	CTRL_DOWN	0x08
#define	CTRL_RIGHT	0x04
#define	CTRL_UP		0x02
#define	CTRL_LEFT	0x01
#endif

#define	IMAGE_DATA(x)	((x) + 2)

static void interrupt(void) __naked {
    __asm__("di");
    __asm__("push af");
    __asm__("push bc");

#if defined(CPC)
    __asm__("ld b, #0xf5");
    __asm__("in a, (c)");
    __asm__("and a, #1");
    __asm__("jp z, skip_irq");
#endif

    __asm__("ld a, #1");
    __asm__("ld (_vblank), a");

#if defined(AY)
    __asm__("ld a, (_enable_AY)");
    __asm__("and a");
    __asm__("jp z, skip_irq");

    __asm__("push hl");
    __asm__("push de");
    __asm__("push ix");
    __asm__("push iy");
    __asm__("call _Player_Decode");
    __asm__("call _Player_CopyAY");
    __asm__("pop iy");
    __asm__("pop ix");
    __asm__("pop de");
    __asm__("pop hl");
#endif

    __asm__("skip_irq:");

    __asm__("pop bc");
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

#if defined(CPC)
#include "cpc.c"
#endif

static void wait_vblank(void) {
    vblank = 0;
    while (!vblank) { }
}

static byte in_key(byte a) {
#if defined(ZXS)
    __asm__("in a, (#0xfe)");
#endif
#if defined(CPC)
    __asm__("di");
    a = cpc_key(a);
    __asm__("ei");
#endif
    return a;
}

static byte in_joy(byte a) {
    __asm__("in a, (#0x1f)"); a;
    return a;
}

static void out_fe(byte data) {
    __asm__("out (#0xfe), a"); data;
}

static byte inc10(byte a) __naked {
    __asm__("inc a"); a;
    __asm__("daa");
    __asm__("ret");
}

void memset(byte *ptr, byte data, word len) {
    while (len-- > 0) { *ptr++ = data; }
}

static void strcpy(char *dst, const char *src) {
    do { *(dst++) = *(src++); } while (src[-1]);
}

void memcpy(void *dst, const void *src, word len) __naked {
    __asm__("___memcpy:");
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

static void beep(byte p) {
    byte c = 0;
    vblank = 0;
    while (!vblank) {
	out_fe((c >> 3) & 0x10);
	c += p;
    }
    out_fe(0x00);
}

static void swoosh(int8 f, int8 n, int8 s) {
    wait_vblank();
    while (n-- > 0) {
	beep(f);
	f += s;
    }
}

static void setup_system(void) {
    byte top = (byte) ((IRQ_BASE >> 8) - 1);
    word jmp_addr = (top << 8) | top;
    BYTE(jmp_addr + 0) = 0xc3;
    WORD(jmp_addr + 1) = ADDR(&interrupt);
    memset((byte *) IRQ_BASE, top, 0x101);
    setup_irq(IRQ_BASE >> 8);

#if defined(CPC)
    text_mask = 0xff;
    setup_system_amstrad_cpc();
#endif
}

static void precalculate(void) {
    no_text = false;
    decompress(SYMBOL(0), symbols);
    for (byte y = 0; y < 192; y++) {
#if defined(ZXS)
	byte f = ((y & 7) << 3) | ((y >> 3) & 7) | (y & 0xc0);
	map_y[y] = SCREEN(f << 5);
#endif
#if defined(CPC)
	word f = ((y & 7) << 11) | mul80(y >> 3);
	map_y[y] = SCREEN(f);
#endif
    }
}

static void clear_screen(void) {
    out_fe(0);
    cached = NULL;
#if defined(ZXS)
    reset_attributes(0);
    memset(SCREEN(0), 0, 0x1800);
#endif
#if defined(CPC)
    reset_palette();
    memset(SCREEN(0), 0, 0x4000);
#endif
}

static void draw_symbol(const byte *addr, byte x, byte y, byte n) {
    byte shift = x & ZXS_CPC(7, 3);
    byte offset = x >> ZXS_CPC(3, 2);
    for (byte i = 0; i < n; i++) {
	byte data = *addr++;
	byte *ptr = map_y[y + i] + offset;
#if defined(ZXS)
	ptr[0] |= (data >> shift);
	ptr[1] |= (data << (8 - shift));
#elif defined(CPC)
	byte value = data >> shift;
	ptr[0] |= ((value & 0xf0) | (value >> 4)) & text_mask;
	ptr[1] |= ((value & 0x0f) | (value << 4)) & text_mask;

	byte rest = (data << (4 - shift)) & 0xf;
	ptr[2] |= (rest | (rest << 4)) & text_mask;
#endif
    }
}

static void put_symbol(const byte *addr, byte x, byte y, byte n) {
    if (!no_text) draw_symbol(addr, x, y, n);
}

static void put_char(char symbol, byte x, byte y) {
    put_symbol(FONT_ADDRESS + (symbol << 3), x, y, 8);
}

void put_diacritic(const byte *addr, byte x, byte y) {
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

void put_dash(char c, byte x, byte y);
void put_check(char c, byte x, byte y);
void put_tick(char c, byte x, byte y);

static byte special_symbol(char c, byte x, byte y) {
    put_symbol(SYMBOL(c - '0'), x, y, 8);
    return 8;
}

static byte put_str(const char *msg, byte x, byte y) {
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
	else if (symbol == '&') {
	    x += special_symbol(*(msg++), x, y);
	}
	else {
	    if (x > 0) x -= leading(symbol);
	    put_char(symbol, x, y);
	    x += trailing(symbol);
	}
    }
    return x;
}

static byte str_len(const char *msg) {
    no_text = true;
    byte len = put_str(msg, 0, 0);
    no_text = false;
    return len;
}

static byte center(const char *msg) {
    return 128 - (str_len(msg) >> 1);
}

static byte mask_sym(byte b, byte f) {
    return b == 0 ? (b | f) : (b & ~(f | (f << 4)));
}

static void corner_symbol(byte *sym) {
#if defined(ZXS)
    for (byte *ptr = SCREEN(0x10de); ptr < SCREEN(0x1800); ptr += 0x100) {
	*ptr = *sym++;
    }
#endif
#if defined(CPC)
    byte *ptr = SCREEN(0x071c);
    while (ptr != SCREEN(0xff1c)) {
	byte data = *sym++;
	ptr[0] = mask_sym(0xff, data >> 4);
	ptr[1] = mask_sym(0xff, data & 0xf);
	ptr += 0x800;
    }
#endif
}

#if defined(ZXS)
static void corner_color(byte color) {
    *COLOUR(0x2de) = color;
}
#else
#define corner_color(color)
#endif

static void hint_symbol(byte *sym) {
    if (hints) {
	corner_symbol(sym);
	corner_color(0x78);
    }
}

static char *to_decimal(char *str, word num, word pad) {
    byte index = 0;
    static const word pow[] = {
	10000, 1000, 100, 10, 1
    };
    for (byte i = 0; i < SIZE(pow); i++) {
	byte count = 0;
	word x = pow[i];
	while (num >= x) {
	    num -= x;
	    count++;
	}
	if (count > 0 || index > 0 || x == pad) {
	    str[index++] = '0' + count;
	}
	if (pad > 1 && x == pad) {
	    str[index++] = '.';
	}
    }
    return str + index;
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

static void draw_image(byte *ptr, byte x, byte y) {
    byte w = *(ptr++);
    byte h = *(ptr++);

    y = y << 3;

    for (byte i = 0; i < h; i++) {
	byte *dst = map_y[y + i] + ZXS_CPC(x, x << 1);
	memcpy(dst, ptr, w);
	ptr += w;
    }

#if defined(ZXS)
    byte *dst = COLOUR(y << 2) + x;
    for (byte i = 0; i < h; i += 8) {
	memcpy(dst, ptr, w);
	dst += 0x20;
	ptr += w;
    }
#endif
}

static byte *cache_image(const byte *src) {
    byte *ptr = STAGING_AREA;
    if (src != cached) {
	decompress(ptr, src);
	cached = src;
    }
    return ptr;
}

static byte use_joy;
static byte last_input;

static byte input_change(byte input) {
    byte change = input & (input ^ last_input);
    last_input = input;
    return change;
}

static byte read_123(void) {
#if defined(ZXS)
    return ~in_key(0xf7) & 7;
#endif
#if defined(CPC)
    return (~in_key(8) & 3) | ((~in_key(7) & 2) << 1);
#endif
}

static void animate_title_line(void) {
    for (byte y = 0; y < 34; y++) {
	byte *ptr = map_y[y] + ZXS_CPC(22, 44);
	*ptr ^= ZXS_CPC(0x10, 0x11);
    }
}

static byte wait_123(void) {
    byte ticks = 0;
    last_input = read_123();
    while (!input_change(read_123())) {
	if (vblank) {
	    vblank = 0;
	    ticks++;
	}
	if (ticks == 5) {
	    animate_title_line();
	    ticks = 0;
	}
    }
    use_joy = last_input & 2;
    return last_input;
}

static void show_frame(const Frame *frame) {
    draw_image(cache_image(frame->img), frame->pos.x, frame->pos.y);
}

static void show_series(const Frame *series) {
    while (series->img) { show_frame(series++); }
}

static void show_text(const Text *text) {
    byte x = text->pos.x;
    x = x < 255 ? x : center(text->str);
    put_str(text->str, x, text->pos.y);
}

static const Text *show_text_series(const Text *text) {
    while (text->str) {
	byte mask = text->mask;
	if (mask) text_mask = mask;
	show_text(text++);
    }
    return text + 1;
}

#define PIXEL(x, y) BYTE(map_y[y] + ((x) >> ZXS_CPC(3, 2)))
#define MASK(x) (ZXS_CPC(0x80, 0x88) >> ZXS_CPC((x) & 7, (x) & 3))

#if defined(ZXS)
static byte is_white(byte x, byte y) {
    word offset = ((y & ~7) << 2) + (x >> 3);
    return BYTE(COLOUR(offset)) == 0x47;
}
#endif

static byte get_pixel(byte x, byte y) {
    return PIXEL(x, y) & MASK(x);
}

static void set_pixel(byte x, byte y) {
    PIXEL(x, y) ^= MASK(x);
}

static byte good_spot(byte x, byte y) {
#if defined(ZXS)
    return is_white(x, y) && get_pixel(x, y);
#endif
#if defined(CPC)
    return get_pixel(x, y);
#endif
}

static byte read_QAOP(void) {
#if defined(ZXS)
    byte ret = 0;
    byte hit = in_key(0x7f);
    ret |= hit & (hit >> 2);
    ret <<= 1;
    ret |= (in_key(0xfb) & 1);
    ret <<= 1;
    ret |= (in_key(0xfd) & 1);
    ret <<= 2;
    ret |= (in_key(0xdf) & 3);
#endif
#if defined(CPC)
    byte ret = 0;
    ret |= (cpc_key(0x5) & 0x80) >> 3;
    ret |= (cpc_key(0x0) & 0x07) << 1;
    ret |= (cpc_key(0x1) & 0x01);
#endif
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

static byte wait_space(void) {
    last_input = read_input();
    return wait_asserted(CTRL_FIRE);
}

static const int8 cursor[] = {
    0,  1,  0, -1,
    1, -1, -1,  1,
    1,  0, -1,  0,
    1,  1, -1, -1,
};

static Pos fish[2];
static byte count;

static Pos pos;
static byte cursor_frame;

static byte steps;
static Hole *hole_now;
static Hole *hole_end;
static Hole hole_map[64];
static byte hole_check;

static byte cooldown;
static byte minute;
static byte hour;
static byte day;

static word stats[STATS_COUNT];

static void draw_cursor(void) {
    const int8 *dir = cursor + (cursor_frame & 0xf);
    set_pixel(pos.x + dir[0], pos.y + dir[1]);
    set_pixel(pos.x + dir[2], pos.y + dir[3]);
}

static void reset_cursor(void) {
    steps = 0;
    pos.x = 8;
    pos.y = 180;
    cursor_frame = 0;
    hole_end = hole_map;
    count = (day == 3) ? 2 : 1;
    hour = (day == 2) ? 0x11 : 0x09;
    cooldown = 0;
    minute = 0;
}

static byte sky;
static void put_digit(byte digit, byte x, byte y) {
    byte *addr = FONT_ADDRESS;
    addr += (('0' + digit) << 3);
#if defined(CPC)
    x = x << 1;
#endif
    for (byte i = 0; i < 8; i++) {
#if defined(ZXS)
	BYTE(map_y[y + i] + x) = *addr++;
#endif
#if defined(CPC)
	byte bits = *addr++;
	BYTE(map_y[y + i] + x + 0) = mask_sym(sky, bits >> 4);
	BYTE(map_y[y + i] + x + 1) = mask_sym(sky, bits & 0xf);
#endif
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
    hole_now = hole_map;
    while (hole_now < hole_end) {
	if (x == hole_now->pos.x && y == hole_now->pos.y) {
	    return true;
	}
	hole_now++;
    }
    return false;
}

static void set_cursor(byte x, byte y, byte moves) {
    draw_cursor();
    if (good_spot(x, y) || visited(x, y)) {
	steps += moves;
	if (steps >= WALK_TIME) {
	    steps -= WALK_TIME;
	    advance_time(1);
	}
	pos.x = x;
	pos.y = y;
    }
    draw_cursor();
}

static byte difference(byte x, byte y) {
    return x > y ? x - y : y - x;
}

static word square(byte value) {
    return mul(value, value);
}

static byte bisect(word value, byte a, byte b) {
    if (a == b) {
	return a;
    }
    else {
	byte middle = a + ((b - a) >> 1);

	if (value < square(middle)) {
	    return bisect(value, a, middle);
	}
	if (middle > a) {
	    return bisect(value, middle, b);
	}
	else {
	    return b;
	}
    }
}

static byte sqrt(word value) {
    return bisect(value, 0, 255) - 1;
}

static byte distance(byte i) {
    word a = square(difference(fish[i].x, pos.x));
    word b = square(difference(fish[i].y, pos.y));
    word c = a + b;

    return (c >= a && c >= b) ? sqrt(c) : 255;
}

static void remove_fish(byte place) {
    for (byte i = place + 1; i < count; i++) {
	fish[i - 1] = fish[i];
    }
    count--;
}

static byte closest;
static byte total_distance(void) {
    byte minimum = 255;
    for (byte i = 0; i < count; i++) {
	byte length = distance(i);
	if (length < minimum) {
	    minimum = length;
	    closest = i;
	}
    }
    return minimum;
}

static void clear_weight(void) {
    for (byte y = 16; y < 24; y++) {
	memset(map_y[y], 0, 8);
    }
}

static void print_weight(byte weight) {
    static const char str[] = "&5 100g";
    strcpy(to_decimal(str + 3, weight, 1), "g");
    put_str(str, 0, 16);
}

static void show_weight(byte weight) {
    if (weight > 0) {
	clear_weight();
	print_weight(weight);
	set_attributes(0x40, 7, 8);
    }
}

static void check_hole(void) {
    if (visited(pos.x, pos.y) && total_distance() == hole_now->distance) {
	show_weight(hole_now->weight);
    }
}

static void move_cursor(void) {
    byte button = read_input();
    byte moves = 0;
    byte x = pos.x;
    byte y = pos.y;

    if (button & CTRL_UP) {
	moves++;
	y--;
    }
    if (button & CTRL_DOWN) {
	moves++;
	y++;
    }
    if (button & CTRL_LEFT) {
	moves++;
	x--;
    }
    if (button & CTRL_RIGHT) {
	moves++;
	x++;
    }
    if (moves == 0) {
	if (!hole_check) check_hole();
	hole_check = true;
	cooldown = 0;
	return;
    }

    if (cooldown == 0 || cooldown > MOVE_COOLDOWN) {
	set_cursor(x, y, moves);
    }
    if (cooldown <= MOVE_COOLDOWN) {
	cooldown++;
    }
    if (hole_check) {
	set_attributes(0x40, 0, 8);
	hole_check = false;
    }
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

static void block_fill(byte y1, byte y2, byte color) {
    for (; y1 < y2; y1++) memset(map_y[y1], color, 0x40);
}

static void show_forest(void) {
    sky = 0xf0;
    clear_screen();
#ifdef CPC
    block_fill(0x00, 0x20, 0xf0);
    block_fill(0x70, 0xc0, 0xff);
#endif
    set_attributes(0x000, 0x28, 0x80);
    set_attributes(0x1c0, 0x7f, 0x140);
    show_series(horizonts);
    select_palette(1, 0x46);
    select_palette(3, 0x4B);
    wait_vblank();
}

static void animate_drill(void) {
    select_palette(2, 0x55);
    for (byte i = 0; i < DRILL_MOVES; i++) {
	advance_time(1);
	hint_symbol(SYMBOL(2));
	show_frame(IMG_DRILL1);
	wait_asserted(CTRL_LEFT);
	swoosh(1, 3, 1);
	hint_symbol(SYMBOL(3));
	show_series(IMG_DRILL2);
	wait_asserted(CTRL_RIGHT);
	swoosh(4, 2, -1);
    }
}

static void add_hole(void) {
    hole_now = hole_end;
    hole_now->pos = pos;
    hole_now->weight = 0;
    hole_now->distance = 0;
    hole_end++;
}

static void drill_hole(void) {
    if (!visited(pos.x, pos.y) && not_late()) {
	add_hole();
	show_forest();
	animate_drill();
    }
}

static void draw_holes(void) {
    Hole *ptr = hole_map;
    while (ptr < hole_end) {
	set_pixel(ptr->pos.x, ptr->pos.y);
	ptr++;
    }
}

static void generate_fish(Pos *f) {
    do {
	word r = random();
	f->x = r & 0xff;
	f->y = r >> 8;
    } while (!good_spot(f->x, f->y));
}

static void put_fish(void) {
    if (hole_end == hole_map) {
	for (byte i = 0; i < count; i++) {
	    generate_fish(fish + i);
	}
    }
}

static void show_lake(void) {
    sky = 0x00;
    clear_screen();
    hole_check = false;
    show_frame(IMG_EZERS);
    show_series(apkaime);
    select_palette(1, 0x46);
    select_palette(2, 0x40);
    select_palette(3, 0x4B);
    draw_holes();
    put_fish();
    put_time();
}

#define COPENE1 (STAGING_AREA + 0x100)
#define COPENE2 (STAGING_AREA + 0x200)

static void draw_tip(byte *ptr) {
    draw_image(ptr, 16, 0);
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

static void noop(byte cycles) {
    for (byte i = 0; i < cycles; i++) { }
}

static void eye_cue_for_pull(void) {
    set_pixel(126, 98);
    set_pixel(129, 98);
    set_pixel(126, 97);
    set_pixel(129, 97);
}

static void pull_hint(byte button) {
    hint_symbol(button == CTRL_LEFT ? SYMBOL(2) : SYMBOL(3));
}

static void hint_mashing(void) {
    static byte blink;
    hint_symbol(blink++ & 4 ? SYMBOL(6) : SYMBOL(7));
}

static byte wait_button(byte button, byte state, byte cutoff) {
    byte ticks = 0;
    while ((read_input() & button) != state) {
	if (vblank && ticks < 255) {
	    vblank = 0;
	    if (cutoff && ticks == PULL_FAST) {
		eye_cue_for_pull();
		pull_hint(button);
	    }
	    if (hints && button == CTRL_FIRE) {
		hint_mashing();
	    }
	    if (++ticks == cutoff) {
		return cutoff;
	    }
	}
    }
    return ticks;
}

static byte jerk_amount;

static void reset_jerk(void) {
    jerk_amount = BITE_DELAY + (random() & 0xf);
}

static void clear_tip(void) {
    draw_tip(COPENE1);
#if defined(ZXS)
    BYTE(COLOUR(0x50)) = 0x7d;
#endif

    byte *ptr = SCREEN(0x50);
    for (byte i = 0; i < 8; i++) {
	BYTE(ptr) = i & 3 ? 0x00 : 0x80;
	ptr += 0x100;
    }
}

static const Frame *debris;
static void hook_failure(void) {
    if (debris->img) {
	show_frame(debris);
	debris++;
    }
}

static byte fish_bite(void) {
    byte ticks = BITE_INTERVAL;
    show_frame(IMG_COPENE(3));
    hint_symbol(SYMBOL(0));
    last_input = read_input();
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
    hook_failure();
    return false;
}

static void jerk_tip(byte *img, byte dir) {
    if (wait_button(CTRL_FIRE, dir, 0) > JERK_INTERVAL) {
	reset_jerk();
    }
    else {
	out_fe(dir ? 0x10 : 0);
    }
    wait_vblank();
    draw_tip(img);
    fishing_line();
}

static byte jerk_fish(void) {
    byte ticks = 0;
    debris = IMG_DEBRIS;

    reset_jerk();
    while (not_late()) {
	jerk_tip(COPENE2, CTRL_FIRE);
	jerk_tip(COPENE1, 0);

	if (++ticks >= JERK_TIME) {
	    advance_time(1);
	    ticks = 0;
	}

	if (--jerk_amount == 0) {
	    if (fish_bite()) return true;
	}
    }
    return false;
}

static void draw_fish(byte fish) {
    if (fish > 0) show_frame(fishes + fish);
}

static void moment_of_weight(byte fish, byte weight) {
    const char *str = reports[fish];

    if (weight > 0) {
	to_decimal((void *) str, weight, 1);
	stats[STATS_SVARS] += weight;
	stats[STATS_ASARI]++;
    }

    hole_now->weight = weight;
    put_str(str, center(str), 64);
    set_attributes(0xe0, 5, 0x60);
    show_series(IMG_REPORT);
    draw_fish(fish);

    wait_space();
}

static void moment_of_truth(byte fish) {
    moment_of_weight(fish, 0);
}

static byte wait_pull(byte button, byte fast) {
    byte ticks = wait_button(button, button, PULL_SLOW);

    corner_color(0x7f);
    if (fast && ticks <= PULL_FAST) {
	advance_time(SNAP_PENALTY);
	moment_of_truth(FISH_SNAP);
	return true;
    }
    if (ticks >= PULL_SLOW) {
	moment_of_truth(FISH_ESCAPE);
	return true;
    }
    return false;
}

static byte get_weight(byte distance) {
    return 100 + ((51 - distance) << 1);
}

static byte report_fish(byte distance) {
    hole_now->distance = distance;

    if (distance <= 1) {
	moment_of_truth(FISH_MAKANS);
	remove_fish(closest);
	stats[STATS_MAKANI]++;
	return count == 0;
    }
    else if (distance <= 11) {
	moment_of_weight(FISH_LARGE, get_weight(distance));
	return false;
    }
    else if (distance <= 31) {
	moment_of_weight(FISH_DECENT, get_weight(distance));
	return false;
    }
    else if (distance <= 51) {
	moment_of_weight(FISH_ASARIS, get_weight(distance));
	return false;
    }
    else if (distance <= 80) {
	moment_of_weight(FISH_PERCH, 150 - distance);
	return false;
    }
    else if (distance < 150) {
	moment_of_truth(FISH_RUFFE);
	stats[STATS_RUFFES]++;
	return false;
    }
    else {
	moment_of_truth(FISH_WEEDS);
	return false;
    }
}

static byte pull_fish(void) {
    clear_screen();
    show_forest();
    for (byte i = 0; i < PULL_MOVES; i++) {
	advance_time(1);
	show_frame(IMG_PULL1);
	if (wait_pull(CTRL_LEFT, i)) {
	    return false;
	}
	show_series(IMG_PULL2);
	if (wait_pull(CTRL_RIGHT, 1)) {
	    return false;
	}
    }
    return report_fish(total_distance());
}

static byte ice_fish(void) {
    clear_screen();
    reset_attributes(0x7d);
    set_attributes(0x00, 0x78, 0x20);
    decompress(COPENE1, IMG_COPENE(1)->img);
    decompress(COPENE2, IMG_COPENE(2)->img);
    show_series(IMG_HOLE);
    starting_line();
    put_time();

    return jerk_fish() && pull_fish();
}

static byte catch_fish(void) {
    return not_late() && ice_fish();
}

static byte fishing(void) {
    while (not_late()) {
	show_lake();
	walk_lake();
	drill_hole();
	if (catch_fish()) {
	    return true;
	}
    }
    return false;
}

static void init_variables(void) {
    memset(stats, 0, sizeof(stats));
    last_input = read_input();
    init_fishing_line();
    seed = 0xfeed;
    hints = true;
    day = 1;
}

static void wait_and_update_seed(void) {
    byte some = wait_space();
    seed = (seed << 5) | (some & 0x1f);
}

static void text_wall_color(void) {
    select_palette(3, 0x4B);
    reset_attributes(5);
}

static void wall_of_text(const Text *text) {
    clear_screen();
    show_text_series(text);
    text_wall_color();
    wait_space();
}

static void draw_panel(Panel *panel) {
    const Text *text = panel->text;
    for (byte i = 0; i < 3; i++) {
	text = show_text_series(text);
	show_frame(panel->frame + i);
	wait_and_update_seed();
    }

    if (text->str) wall_of_text(text);
}

static void report_number(char *buf, word amount) {
    *to_decimal(buf, amount, 1) = 0;
}

static void report_weight(char *buf, word amount) {
    strcpy(to_decimal(buf, amount, 1000) - 2, "kg");
}

static const word ranges_makani[] = {    1,    2,    4 };
static const word ranges_ruffes[] = {    2,    4,    8 };
static const word ranges_asari[]  = {   10,   20,   40 };
static const word ranges_svari[]  = { 2000, 4000, 8000 };

static const word * const ranges[] = {
    ranges_makani, ranges_asari, ranges_ruffes, ranges_svari
};

static byte get_rank(byte i) {
    byte rank;
    const word *ptr = ranges[i];
    for (rank = 0; rank < 3; rank++) {
	if (stats[i] < ptr[rank]) break;
    }
    return rank;
}

static byte show_stars(byte i, byte y) {
    byte rank = get_rank(i);
    static const char str[] = "&4&4&4";
    put_str(str + ((3 - rank) << 1), 196, y);
    set_attributes(((y & ~7) << 2) + 0x18, 0x46, 4);
    return rank;
}

static const byte rank_map[] = {
    0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4,
};

static byte report_amount(byte i, byte y, byte rank) {
    static char buf[8];
    word amount = stats[i];
    switch (i) {
    case STATS_SVARS:
	report_weight(buf, amount);
	break;
    case STATS_RANGS:
	put_str(rank_strs[rank_map[rank]], 136, y);
	return 0;
    default:
	report_number(buf, amount);
	break;
    }
    put_str(buf, 136, y);
    return show_stars(i, y);
}

static void statistics(void) {
    byte rank = 0, y = 64;

    clear_screen();
    reset_attributes(0x4);
    show_text_series(stat_title);
    for (byte i = 0; i < STATS_COUNT; i++) {
	const char *str = stat_strs[i];
	put_str(str, 120 - str_len(str), y);
	rank += report_amount(i, y, rank);
	y = y + 16;
    }
    wait_space();
}

static void fade_out_screen(void) {
    for (byte i = 0; i < 8; i++) {
	wait_vblank();
	for (word addr = 0x5800; addr < 0x5b00; addr++) {
	    byte x = BYTE(addr);
	    if (x & 0x07) x -= 0x01;
	    if (x & 0x38) x -= 0x08;
	    BYTE(addr) = x;
	}
    }
}

static void game_done(void) {
    fade_out_screen();
    wall_of_text(goodbye);
    fade_out_screen();
    show_frame(IMG_ENDING);
    show_text(&the_end);
    wait_space();
    statistics();
}

static void select_panel_color(byte num) {
    switch (num) {
    case 1:
	select_palette(1, 0x55);
	select_palette(2, 0x4A);
	break;
    }
}

static void current_panel(byte num) {
    seed = 1;
    clear_screen();
    text_wall_color();
    select_panel_color(num);
    draw_panel(panels + num);
}

static void game_fail(void) {
    fade_out_screen();
    current_panel(0);
    statistics();
    reset();
}

static void show_tutorial(void) {
    wall_of_text(tutorial);
    clear_screen();
    show_frame(IMG_RANGES);
    show_text_series(fish_map);
    select_palette(1, 0x4F);
    select_palette(2, 0x59);
    select_palette(3, 0x43);
    wait_space();
    reset();
}

static void show_title(void) {
    clear_screen();
    show_frame(IMG_TITLE);
    select_palette(3, 0x4B);
    show_text_series(choices);
    set_attributes(0x140, 0x47, 0x1c0);
    if (wait_123() & 4) show_tutorial();
}

static void music(void) {
    __asm__(".incbin \"music.pt3\"");
}

static void game_loop(void) {
    byte done;
    init_variables();
    while (day <= 3) {
	current_panel(day);
	select_music(&music);
	reset_cursor();
	done = fishing();
	hints = false;
	stop_music();
	if (!done) {
	    game_fail();
	}
	day++;
    }
    game_done();
}

void reset(void) {
    DISABLE_IRQ();
    SETUP_STACK();
    stop_music();
    setup_system();
    precalculate();
    show_title();
    game_loop();
    reset();
}
