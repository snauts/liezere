#define cpc(x)		(x)
#define SETUP_STACK()	__asm__("ld sp, #0x81fc")
#define FONT_ADDRESS	(((byte *) &font_rom) - 0x100)
#define IRQ_BASE	0x8200

#define SCREEN(x)	PTR(0xc000 + (x))
#define SYMBOL(x) 	PTR(0xb800 + ((x) << 3))
#define STAGING_AREA	PTR(0x8400)

#define PIXEL(x, y)	BYTE(map_y[y] + ((x) >> 2))
#define MASK(x)		(0x88 >> ((x) & 3))
#define SHIFT		1

#define	CTRL_FIRE	0x10
#define	CTRL_DIR	0x0f
#define	CTRL_RIGHT	0x08
#define	CTRL_LEFT	0x04
#define	CTRL_DOWN	0x02
#define	CTRL_UP		0x01

#define reset_attributes(color)

static word mul80(word x) {
    return (x << 6) + (x << 4);
}

static byte cpc_psg(byte reg, byte val) __naked {
    __asm__("di");
    __asm__("ld b, #0xf4");
    __asm__("ld c, a"); reg;
    __asm__("out (c), c");
    __asm__("ld bc, #0xf6c0");
    __asm__("out (c), c");
    __asm__("ld bc, #0xf600");
    __asm__("out (c), c");
    __asm__("ld bc, #0xf680");
    __asm__("out (c), c");
    __asm__("ld b, #0xf4");
    __asm__("ld c, l"); val;
    __asm__("out (c), c");
    __asm__("ld bc, #0xf600");
    __asm__("out (c), c");
    __asm__("ei");
    __asm__("ret");
}

static byte cpc_key(byte line) __naked {
    __asm__("di");
    __asm__("ld bc, #0xf782");
    __asm__("out (c), c");
    __asm__("ld bc, #0xf40e");
    __asm__("out (c), c");
    __asm__("ld bc, #0xf6c0");
    __asm__("out (c), c");
    __asm__("ld bc, #0xf600");
    __asm__("out (c), c");
    __asm__("ld bc, #0xf792");
    __asm__("out (c), c");
    __asm__("ld b, #0xf6");
    __asm__("or a, #0x40"); line;
    __asm__("ld c, a");
    __asm__("out (c), c");
    __asm__("ld b, #0xf4");
    __asm__("in a, (c)");
    __asm__("ld bc, #0xf782");
    __asm__("out (c), c");
    __asm__("ld bc, #0xf600");
    __asm__("out (c), c");
    __asm__("ei");
    __asm__("ret");
}

static byte sfx_on;
static void setup_system_amstrad_cpc(void) {
    __asm__("ld bc, #0xbc0c");
    __asm__("out (c), c");
    __asm__("ld bc, #0xbd33");
    __asm__("out (c), c");

    __asm__("ld bc, #0xbc0d");
    __asm__("out (c), c");
    __asm__("ld bc, #0xbdd4");
    __asm__("out (c), c");

    cpc_psg(7, 0xB8);
    cpc_psg(8, 0x00);
    cpc_psg(9, 0x00);
    sfx_on = false;
}

static void font_rom(void) {
    __asm__(".incbin \"font.rom\"");
}

static void gate_array(byte reg) {
    __asm__("di");
    __asm__("ld bc, #0x7f00");
    __asm__("out (c), a"); reg;
    __asm__("ei");
}

static const byte pal[] = {
    0x9D, 0x10, 0x54, 0, 0x54, 1, 0x54, 2, 0x54, 3, 0x54,
};

static void reset_palette(void) {
    for (byte i = 0; i < sizeof(pal); i++) gate_array(pal[i]);
}

static void attributes(byte index, byte color) {
    gate_array(0x9d);
    gate_array(index);
    gate_array(color);
}

static byte in_key(byte a) {
    __asm__("di");
    a = cpc_key(a);
    __asm__("ei");
    return a;
}

static byte in_joy(byte a) {
    __asm__("di");
    a = cpc_key(9);
    __asm__("ei");
    return (~a & 0x1f) | ((~a >> 1) & 0x10);
}

static byte read_123(void) {
    return (~in_key(8) & 3) | ((~in_key(7) & 2) << 1);
}

static byte read_QAOP(void) {
    byte ret = 0;
    byte dir = cpc_key(0x0);
    ret |= (cpc_key(0x5) & 0x80) >> 3;
    ret |= (dir & 0x01) | ((dir & 0x04) >> 1);
    ret |= ((cpc_key(0x1) & 0x01) | (dir & 0x02)) << 2;
    return ~ret;
}

static void block_fill(byte y1, byte y2, byte color) {
    for (; y1 < y2; y1++) memset(map_y[y1], color, 0x40);
}

static void fade_out_screen(void) {
    for (byte i = 0; i < 8; i++) {
	wait_vblank();
	static const byte pal[] = {
	    0x40, 0x40, 0x5E, 0x46, 0x56,
	    0x58, 0x5C, 0x44, 0x54, 0x54,
	};
	cpc(attributes(1, pal[i + 0]));
	cpc(attributes(2, pal[i + 1]));
	cpc(attributes(3, pal[i + 2]));
    }
}

static void sound_fx(word period) {
    if (!sfx_on) {
	silence_music();
	cpc_psg(0x7, 0x3D);
	cpc_psg(0x9, 0x0F);
	sfx_on = true;
    }
    cpc_psg(2, period & 0xff);
    cpc_psg(3, period >> 8);
}

static void sound_off(void) {
    if (sfx_on) {
	sfx_on = false;
	cpc_psg(0x7, 0x3F);
	resume_music();
    }
}
