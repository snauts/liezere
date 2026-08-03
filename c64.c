#define c64(x)		(x)
#define SETUP_STACK()	__asm__("ldx #0xff"); __asm__("txs");
#define FONT_ADDRESS	(((byte *) &font_rom) - 0x100)

#define SCREEN(x)	PTR(0xa000 + (x))
#define COLOUR(x)	PTR(0x8c00 + (x))
#define SYMBOL(x) 	PTR(0x6000 + ((x) << 3))
#define STAGING_AREA	PTR(0x6100)

#define PIXEL(x, y)	BYTE(map_y[(y) >> 3] + ((y) & 0x07) + ((x) & 0xf8))
#define MASK(x)		(0x80 >> ((x) & 7))
#define SHIFT		0

#define	CTRL_FIRE	0x10
#define	CTRL_DIR	0x0f
#define	CTRL_RIGHT	0x08
#define	CTRL_LEFT	0x04
#define	CTRL_DOWN	0x02
#define	CTRL_UP		0x01

#define reset_attributes(color) \
    memset(COLOUR(0), color, 1000);

void font_rom(void) {
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

static void attributes(byte value, byte start, byte count) {
    while (count-- != 0) memset(color[start++], value, 0x20);
}

static void clear_framebuffer(void) {
    reset_attributes(0x00);
    memset(SCREEN(0), 0, 0x2000);
}

static void setup_system_c64(void) {
    __asm__ ("sei");
    BYTE(0xdd00) = (BYTE(0xdc00) & ~3) | 1;

    BYTE(0x0001) = 0x35;
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
    clear_framebuffer();
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

static byte c64_read_key(byte row) {
    BYTE(0xdc00) = row;
    return BYTE(0xdc01);
}

#define c64_key(row) c64_read_key((byte) ~row)

static byte in_joy(byte a) {
    return a;
}

static byte read_123(void) {
    byte three = c64_key(BIT(1));
    byte one_two = c64_key(BIT(7));
    return ~((one_two & 1) | ((one_two >> 2) & 2) | (three << 2));
}

static byte read_QAOP(void) {
    byte ret = 0;
    byte key = c64_key(BIT(7));
    if (key & 0x40) ret |= CTRL_UP;
    if (key & 0x10) ret |= CTRL_FIRE;
    if (c64_key(BIT(1)) & BIT(2)) ret |= CTRL_DOWN;
    if (c64_key(BIT(4)) & BIT(6)) ret |= CTRL_LEFT;
    if (c64_key(BIT(5)) & BIT(1)) ret |= CTRL_RIGHT;
    return ~ret;
}
