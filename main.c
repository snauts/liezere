#include "main.h"
#include "image.h"

#define MOVE_COOLDOWN	5
#define JERK_INTERVAL	10
#define BITE_INTERVAL	25
#define BITE_DELAY	15
#define PULL_FAST	20
#define PULL_SLOW	60
#define WALK_TIME	3
#define JERK_TIME	5

void reset(void);
void start_up(void) __naked {
    __asm__("di");
    __asm__("jp _reset");
}

static volatile byte vblank;
static byte *map_y[192];
static byte *line[96];
static byte no_text;
static word seed;

extern const char* const reports[];
extern const byte* const fishes[];
extern const Frame horizonts[];
extern const Frame apkaime[];
extern const Text tutorial[];
extern const Text choices[];
extern const Panel panels[];

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
}

static void precalculate(void) {
    no_text = false;
    for (byte y = 0; y < 192; y++) {
	byte f = ((y & 7) << 3) | ((y >> 3) & 7) | (y & 0xc0);
	map_y[y] = SCREEN(f << 5);
    }
}

static void reset_attributes(byte color) {
    memset(COLOUR(0), color, 0x300);
}

static void clear_screen(void) {
    out_fe(0);
    reset_attributes(0);
    memset(SCREEN(0), 0, 0x1800);
}

static void draw_symbol(const byte *addr, byte x, byte y, byte n) {
    byte shift = x & 7;
    byte offset = x >> 3;
    for (byte i = 0; i < n; i++) {
	byte data = *addr++;
	byte *ptr = map_y[y + i] + offset;
	ptr[0] |= (data >> shift);
	ptr[1] |= (data << (8 - shift));
    }
}

static void put_symbol(const byte *addr, byte x, byte y, byte n) {
    if (!no_text) draw_symbol(addr, x, y, n);
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

static byte special_symbol(char c, byte x, byte y) {
    put_symbol(STAGING_AREA + ((c - '0') << 3), x, y, 8);
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

static byte read_123(void) {
    return ~in_key(0xf7) & 7;
}

static void animate_title_line(void) {
    for (byte y = 0; y < 34; y++) {
	byte *ptr = map_y[y] + 22;
	*ptr ^= 0x10;
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
    show_image(frame->img, frame->x, frame->y);
}

static void show_series(const Frame *series) {
    while (series->img) { show_frame(series++); }
}

static void show_text(const Text *text) {
    byte x = text->x < 255 ? text->x : center(text->str);
    put_str(text->str, x, text->y);
}

static const Text *show_text_series(const Text *text) {
    while (text->str) { show_text(text++); }
    return text + 1;
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

static byte wait_space(void) {
    return wait_asserted(CTRL_FIRE);
}

static const int8 cursor[] = {
    0,  1,  0, -1,
    1, -1, -1,  1,
    1,  0, -1,  0,
    1,  1, -1, -1,
};

static byte fish_x[2];
static byte fish_y[2];
static byte count;

static byte cursor_x;
static byte cursor_y;

static byte cursor_frame;

static byte steps;
static byte *hole_end;
static byte hole_map[192];

static byte cooldown;
static byte minute;
static byte hour;
static byte day;

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
    count = (day == 3) ? 2 : 1;
    hour = (day == 2) ? 0x11 : 0x09;
    cooldown = 0;
    minute = 0;
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
	if (++steps == WALK_TIME) {
	    advance_time(1);
	    steps = 0;
	}
	cursor_x = x;
	cursor_y = y;
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
    word a = square(difference(fish_x[i], cursor_x));
    word b = square(difference(fish_y[i], cursor_y));
    word c = a + b;

    return (c >= a && c >= b) ? sqrt(c) : 255;
}

static void remove_fish(byte place) {
    for (byte i = place + 1; i < count; i++) {
	fish_x[i - 1] = fish_x[i];
	fish_y[i - 1] = fish_y[i];
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
	cooldown = 0;
	return;
    }

    if (cooldown == 0 || cooldown > MOVE_COOLDOWN) {
	set_cursor(x, y);
    }
    if (cooldown <= MOVE_COOLDOWN) {
	cooldown++;
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
	swoosh(1, 3, 1);
	show_image(swirl, 14, 10);
	show_image(drill, 16, 15);
	wait_asserted(CTRL_RIGHT);
	swoosh(4, 2, -1);
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

static byte valid_fish(byte i) {
    return good_spot(fish_x[i], fish_y[i]);
}

static void generate_fish(byte i) {
    do {
	word r = random();
	fish_x[i] = r & 0xff;
	fish_y[i] = r >> 8;
    } while (!valid_fish(i));
}

static void put_fish(void) {
    if (hole_end == hole_map) {
	for (byte i = 0; i < count; i++) {
	    generate_fish(i);
	}
    }
}

static void show_lake(void) {
    clear_screen();
    show_image(ezers, 0, 0);
    show_series(apkaime);
    draw_holes();
    put_fish();
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

static byte wait_button(byte button, byte state, byte cutoff) {
    byte ticks = 0;
    while ((read_input() & button) != state) {
	if (vblank && ticks < 255) {
	    vblank = 0;
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
    BYTE(COLOUR(0x50)) = 0x7d;

    byte *ptr = SCREEN(0x50);
    for (byte i = 0; i < 8; i++) {
	BYTE(ptr) = i & 3 ? 0x00 : 0x80;
	ptr += 0x100;
    }
}

static byte fish_bite(void) {
    byte ticks = BITE_INTERVAL;
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
    if (wait_button(CTRL_FIRE, dir, 0) > JERK_INTERVAL) {
	reset_jerk();
    }
    out_fe(dir ? 0x10 : 0);
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
    if (fish > 0) show_image(fishes[fish], 18, 12);
}

static void update_num(char *str, byte num) {
    byte dec = 0;
    while (num >= 10) {
	num -= 10;
	dec++;
    }
    str[1] = '0' + dec;
    str[2] = '0' + num;
}

static const Frame raise[] = {
    { .img = velk1,  .x = 13, .y = 11 },
    { .img = aukla2, .x = 15, .y = 14 },
    { .img = loms,   .x = 15, .y = 11 },
    { .img = NULL },
};

static void moment_of_weight(byte fish, int8 weight) {
    const char *str = reports[fish];

    if (weight >= 0) update_num((void *) str, weight);

    put_str(str, center(str), 64);
    memset(COLOUR(0x100), 5, 0x20);
    show_series(raise);
    draw_fish(fish);

    wait_space();
}

static void moment_of_truth(byte fish) {
    moment_of_weight(fish, -1);
}

static byte wait_pull(byte button, byte fast) {
    byte ticks = wait_button(button, button, PULL_SLOW);
    if (fast && ticks <= PULL_FAST) {
	advance_time(5);
	moment_of_truth(FISH_SNAP);
	return true;
    }
    if (ticks >= PULL_SLOW) {
	moment_of_truth(FISH_ESCAPE);
	return true;
    }
    return false;
}

static byte report_fish(byte distance) {
    if (distance <= 1) {
	moment_of_truth(FISH_MAKANS);
	remove_fish(closest);
	return count == 0;
    }
    else if (distance <= 50) {
	moment_of_weight(FISH_ASARIS, (51 - distance) << 1);
	return false;
    }
    else if (distance <= 80) {
	moment_of_weight(FISH_PERCH, (150 - distance));
	return false;
    }
    else {
	moment_of_truth(FISH_RUFFE);
	return false;
    }
}

static byte pull_fish(void) {
    clear_screen();
    show_forest();
    for (byte i = 0; i < 5; i++) {
	advance_time(1);
	show_frame(raise);
	if (wait_pull(CTRL_LEFT, i)) {
	    return false;
	}
	show_image(velk2, 14, 12);
	show_image(aukla1, 15, 16);
	if (wait_pull(CTRL_RIGHT, 1)) {
	    return false;
	}
    }
    return report_fish(total_distance());
}

static byte ice_fish(void) {
    clear_screen();
    reset_attributes(0x7d);
    memset(COLOUR(0x00), 0x78, 0x20);
    decompress(COPENE1, IMAGE_DATA(copene1));
    decompress(COPENE2, IMAGE_DATA(copene2));
    show_image(copene1, 16, 0);
    show_image(hole, 12, 19);
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
    last_input = read_input();
    init_fishing_line();
    day = 1;
}

static void wait_and_update_seed(void) {
    byte some = wait_space();
    seed = (seed << 5) | (some & 0x1f);
}

static void wall_of_text(const Text *text) {
    clear_screen();
    show_text_series(text);
    reset_attributes(5);
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

static void game_done(void) {
    show_image(beigas, 0, 0);
    wait_space();
}

static void current_panel(byte num) {
    seed = 1;
    clear_screen();
    reset_attributes(5);
    draw_panel(panels + num);
}

static void game_fail(void) {
    current_panel(0);
    reset();
}

static void show_tutorial(void) {
    decompress(STAGING_AREA, IMAGE_DATA(symbols));
    wall_of_text(tutorial);
    reset();
}

static void show_title(void) {
    show_image(title, 0, 0);
    show_text_series(choices);
    memset(COLOUR(0x140), 0x47, 0x100);
    if (wait_123() & 4) show_tutorial();
}

static void game_loop(void) {
    init_variables();
    while (day <= 3) {
	current_panel(day);
	reset_cursor();
	if (!fishing()) {
	    game_fail();
	}
	day++;
    }
    game_done();
}

void reset(void) {
    SETUP_STACK();
    setup_system();
    precalculate();
    clear_screen();
    show_title();
    game_loop();
    reset();
}
