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
    __asm__("ret");
}

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
}

static void font_rom(void) {
    __asm__(".incbin \"font.rom\"");
}

static void gate_array(byte reg) {
    __asm__("ld bc, #0x7f00");
    __asm__("out (c), a"); reg;
}

static const byte pal[] = {
    0x9D, 0x10, 0x54, 0, 0x54, 1, 0x54, 2, 0x54, 3, 0x54,
};

static void reset_palette(void) {
    for (byte i = 0; i < sizeof(pal); i++) gate_array(pal[i]);
}

static void select_palette(byte index, byte color) {
    gate_array(0x9d);
    gate_array(index);
    gate_array(color);
}
