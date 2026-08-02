#define zxs(x)		(x)
#define BP8_BP4(a, b)	(a)
#define SETUP_STACK()	__asm__("ld sp, #0xfdfc")
#define FONT_ADDRESS	PTR(0x3c00)
#define IRQ_BASE	0xfe00

#define SCREEN(x)	PTR(0x4000 + (x))
#define COLOUR(x)	PTR(0x5800 + (x))
#define SYMBOL(x) 	PTR(0x7700 + ((x) << 3))
#define STAGING_AREA	PTR(0x5b00)

#define PIXEL(x, y)	BYTE(map_y[y] + ((x) >> 3))
#define MASK(x)		(0x80 >> ((x) & 7))

#define	CTRL_FIRE	0x10
#define	CTRL_DIR	0x0f
#define	CTRL_UP		0x08
#define	CTRL_DOWN	0x04
#define	CTRL_LEFT	0x02
#define	CTRL_RIGHT	0x01

#define attributes(from, c, len) memset(COLOUR(from), c, len)

#define reset_attributes(color) attributes(0, color, 0x300)

#define corner_color(color) *COLOUR(0x2de) = color

static void out_fe(byte data) {
    __asm__("out (#0xfe), a"); data;
}

static byte in_key(byte a) {
    __asm__("in a, (#0xfe)");
    return a;
}

static byte in_joy(byte a) {
    __asm__("in a, (#0x1f)");
    return a;
}

static byte read_123(void) {
    return ~in_key(0xf7) & 7;
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
