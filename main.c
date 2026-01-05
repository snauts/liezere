#include "main.h"

void start_up(void) __naked {
    __asm__("di");
    __asm__("jp _reset");
}

static volatile byte vblank;
static byte *map_y[192];

#define SETUP_STACK()	__asm__("ld sp, #0xfdfc")
#define FONT_ADDRESS	PTR(0x3c00)
#define IRQ_BASE	0xfe00

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

static void put_symbol(byte *addr, byte x, byte y, byte n) {
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

static void put_diacritic(byte *addr, byte x, byte y) {
    put_symbol(addr, x, y, 2);
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

static void put_dash(char c, byte x, byte y) {
    static const byte small_dash[] = { 0x00, 0x38 };
    static const byte large_dash[] = { 0x3c, 0x00 };

    if (c == 'i') {
	put_diacritic(small_dash, x - 1, y);
    }
    else if (c == 'I') {
	put_diacritic(small_dash, x, y - 2);
    }
    else if (c & 0x20) {
	put_diacritic(small_dash, x, y - 1);
    }
    else {
	put_diacritic(large_dash, x, y - 1);
    }
}

static void put_check(char c, byte x, byte y) {
    static const byte small_check[] = { 0x28, 0x10 };
    static const byte large_check[] = { 0x24, 0x18 };

    if (c & 0x20) {
	put_diacritic(small_check, x, y - 1);
    }
    else {
	put_diacritic(large_check, x, y - 2);
    }
}

static void put_tick(char c, byte x, byte y) {
    static const byte tick[] = { 0x08, 0x10 };

    if (c & 0x20) x--;

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

static void show_title(void) {
    put_str("Liezere", 16, 16);
    put_str("AaCcEeGgIiKkLlNnSsUuZz", 16, 32);
    put_str("`A`a~C~c`E`e^G~g`I`i^K^k^L^l^N^n~S~s`U`u~Z~z", 16, 48);
    put_str("~Saurslie~zu Dzelzce^l~s", 16, 64);
    memset(COLOUR(0x00), 5, 0x300);
}

void reset(void) {
    SETUP_STACK();
    setup_system();
    precalculate();
    clear_screen();
    show_title();
    while (1) { }
}
