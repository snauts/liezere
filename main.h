typedef signed char int8;
typedef signed short int16;
typedef unsigned char byte;
typedef unsigned short word;

#define AY
#define ZXS

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

#define POS(X, Y)	{ .x = X, .y = Y }
#define TEXT_SENTINEL	{ .str = NULL }

void memset(byte *ptr, byte data, word len);
void select_music(void *ptr);
void stop_music(void);

typedef struct {
    byte x, y;
} Pos;

typedef struct {
    const byte *img;
    Pos pos;
} Frame;

typedef struct {
    const char *str;
    Pos pos;
} Text;

typedef struct {
    Frame *frame;
    Text *text;
} Panel;

typedef struct {
    Pos pos;
    byte weight;
    byte distance;
} Hole;

enum {
    FISH_SNAP = 0,
    FISH_ESCAPE,
    FISH_WEEDS,
    FISH_RUFFE,
    FISH_PERCH,
    FISH_ASARIS,
    FISH_DECENT,
    FISH_LARGE,
    FISH_MAKANS,
};

enum {
    STATS_MAKANI = 0,
    STATS_ASARI,
    STATS_RUFFES,
    STATS_SVARS,
    STATS_RANGS,
    STATS_COUNT, // this shoud be last
};

#define SPRITES(x)	(sprites + (x))
#define IMG_REPORT	SPRITES(0)
#define IMG_PULL1	SPRITES(0)
#define IMG_PULL2	SPRITES(4)
#define IMG_DRILL1	SPRITES(7)
#define IMG_DRILL2	SPRITES(8)
#define IMG_HOLE	SPRITES(13)
#define IMG_DEBRIS	SPRITES(16)
#define IMG_TITLE	SPRITES(21)
#define IMG_EZERS	SPRITES(22)
#define IMG_ENDING	SPRITES(23)
#define IMG_SYMBOL	SPRITES(24)
#define IMG_COPENE(x)	SPRITES(14 - x)
