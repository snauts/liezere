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

static void init_gate_array(const byte *ptr, byte size) {
    for (byte i = 0; i < size; i++) gate_array(ptr[i]);
}

static const byte pal[] = {
    0x9D, 0x10, 0x54, 0, 0x54, 1, 0x4B, 2, 0x4B, 3, 0x4B,
    0x9D, 0x10, 0x54, 0, 0x54, 1, 0x5A, 2, 0x5D, 3, 0x4B,
};

static void amstrad_cpc_select_palette(byte offset) {
    init_gate_array(pal + offset, 11);
}
