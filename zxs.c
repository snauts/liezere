#define zxs(x)		(x)
#define BP8_BP4(a, b)	(a)
#define SETUP_STACK()	__asm__("ld sp, #0xfdfc")
#define FONT_ADDRESS	PTR(0x3c00)
#define IRQ_BASE	0xfe00

#define SCREEN(x)	PTR(0x4000 + (x))
#define COLOUR(x)	PTR(0x5800 + (x))
#define SYMBOL(x) 	PTR(0x7700 + ((x) << 3))
#define STAGING_AREA	PTR(0x5b00)

#define	CTRL_FIRE	0x10
#define	CTRL_DIR	0x0f
#define	CTRL_UP		0x08
#define	CTRL_DOWN	0x04
#define	CTRL_LEFT	0x02
#define	CTRL_RIGHT	0x01

#define attributes(from, c, len) memset(COLOUR(from), c, len)

#define reset_attributes(color) attributes(0, color, 0x300)
