typedef signed char int8;
typedef signed short int16;
typedef unsigned char byte;
typedef unsigned short word;

#define false		0
#define true		1

#define NULL		((void *) 0)
#define ADDR(obj)	((word) (obj))
#define BYTE(addr)	(* (volatile byte *) (addr))
#define WORD(addr)	(* (volatile word *) (addr))
#define SIZE(array)	(sizeof(array) / sizeof(*(array)))
#define PTR(addr)	((byte *) (addr))
#define BIT(pos)	(1 << (pos))

#define SCREEN(x)	PTR(0x4000 + (x))
#define COLOUR(x)	PTR(0x5800 + (x))
#define STAGING_AREA	PTR(0x5b00)

typedef struct {
    const byte *img;
    byte x, y;
} Frame;

typedef struct {
    const char *str;
    byte x, y;
} Text;
