static void font_rom(void) {
    __asm__(".incbin \"font.rom\"");
}

static void interrupt(void) __naked {
    __asm__("pha");
    __asm__("lda #0xff");
    __asm__("sta 0xd019");
    __asm__("sta _vblank");
    __asm__("pla");
    __asm__("rti");
}

static void setup_system_c64(void) {
    __asm__ ("sei");
    BYTE(0xd011) = 0x2b; /* bitmap mode */
    BYTE(0xd016) = 0xc8; /* standard mode */
    BYTE(0xd018) = 0x38; /* memory regions */
    BYTE(0xd015) = 0x00; /* disable sprites */
    BYTE(0xd020) = 0x00; /* black border */
    BYTE(0xd021) = 0x00; /* black background */
    BYTE(0xdc0d) = 0x7f; /* disable timer irq */
    BYTE(0xdd0d) = 0x7f; /* disable timer irq */
    BYTE(0xdc0d); /* clear pending irq */
    BYTE(0xdd0d); /* clear pending irq */
    BYTE(0xd01a) = 0x01; /* genereate raster irq */
    BYTE(0xd012) = 0x00; /* generate on line 0 */
    WORD(0xfffe) = (word) &interrupt;
    memset((byte *) 0x8c00, 0x00, 1000);
    memset((byte *) 0xa000, 0x00, 8192);
    BYTE(0xd011) = 0x3b;

    /* keyboard input */
    BYTE(0xdc02) = 0xff;
    BYTE(0xdc03) = 0x00;

    /* sound config */
    memset((byte *) 0xd400, 0, 28);
    BYTE(0xd418) = 0x0f;
    BYTE(0xd403) = 0x08;
    BYTE(0xd40a) = 0x08;
    BYTE(0xd406) = 0xf0;
    BYTE(0xd40d) = 0xf0;
    __asm__ ("cli");
}
