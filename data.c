#include "main.h"
#include "data.h"

const Frame apkaime[] = {
    { .img = puduris1, .pos = POS( 2, 14) },
    { .img = puduris1, .pos = POS( 5, 11) },
    { .img = puduris1, .pos = POS( 7, 22) },
    { .img = puduris1, .pos = POS(10, 10) },
    { .img = puduris1, .pos = POS(16,  8) },
    { .img = puduris1, .pos = POS(18,  4) },
    { .img = puduris1, .pos = POS(22, 18) },
    { .img = puduris2, .pos = POS( 0, 17) },
    { .img = puduris2, .pos = POS(15, 22) },
    { .img = puduris2, .pos = POS(20, 21) },
    { .img = puduris2, .pos = POS(29,  3) },
    { .img = puduris2, .pos = POS(30,  8) },
    { .img = puduris2, .pos = POS(30, 13) },
    { .img = niedres,  .pos = POS( 2, 19) },
    { .img = niedres,  .pos = POS( 7, 15) },
    { .img = niedres,  .pos = POS( 8, 14) },
    { .img = niedres,  .pos = POS( 8, 14) },
    { .img = niedres,  .pos = POS(12, 13) },
    { .img = niedres,  .pos = POS(13, 22) },
    { .img = niedres,  .pos = POS(20,  7) },
    { .img = niedres,  .pos = POS(21, 10) },
    { .img = niedres,  .pos = POS(21, 17) },
    { .img = niedres,  .pos = POS(23,  4) },
    { .img = niedres,  .pos = POS(26,  1) },
    { .img = niedres,  .pos = POS(30,  7) },
    { .img = niedres,  .pos = POS(30, 11) },
    { .img = niedres,  .pos = POS(30, 21) },
    { .img = NULL },
};

const Frame horizonts[] = {
    { .img = egle1,  .pos = POS( 2,  3) },
    { .img = egle1,  .pos = POS( 6,  1) },
    { .img = egle1,  .pos = POS( 8,  3) },
    { .img = egle1,  .pos = POS(13,  3) },
    { .img = egle1,  .pos = POS(15,  1) },
    { .img = egle1,  .pos = POS(19,  3) },
    { .img = egle1,  .pos = POS(22,  3) },
    { .img = egle1,  .pos = POS(25,  3) },
    { .img = egle1,  .pos = POS(26,  3) },
    { .img = egle1,  .pos = POS(28,  1) },
    { .img = egle2,  .pos = POS( 0,  2) },
    { .img = egle2,  .pos = POS( 3,  2) },
    { .img = egle2,  .pos = POS( 5,  2) },
    { .img = egle2,  .pos = POS( 9,  2) },
    { .img = egle2,  .pos = POS(11,  2) },
    { .img = egle2,  .pos = POS(14,  2) },
    { .img = egle2,  .pos = POS(17,  2) },
    { .img = egle2,  .pos = POS(20,  2) },
    { .img = egle2,  .pos = POS(23,  2) },
    { .img = egle2,  .pos = POS(27,  2) },
    { .img = egle2,  .pos = POS(30,  2) },
    { .img = egle3,  .pos = POS( 1,  2) },
    { .img = egle3,  .pos = POS( 4,  2) },
    { .img = egle3,  .pos = POS( 7,  2) },
    { .img = egle3,  .pos = POS(10,  2) },
    { .img = egle3,  .pos = POS(12,  2) },
    { .img = egle3,  .pos = POS(16,  2) },
    { .img = egle3,  .pos = POS(18,  2) },
    { .img = egle3,  .pos = POS(21,  2) },
    { .img = egle3,  .pos = POS(24,  2) },
    { .img = egle3,  .pos = POS(29,  2) },
    { .img = egle3,  .pos = POS(31,  2) },
    { .img = krasts, .pos = POS( 0, 13) },
    { .img = krasts, .pos = POS( 3, 13) },
    { .img = krasts, .pos = POS( 8, 13) },
    { .img = krasts, .pos = POS(11, 13) },
    { .img = krasts, .pos = POS(14, 13) },
    { .img = krasts, .pos = POS(19, 13) },
    { .img = krasts, .pos = POS(23, 13) },
    { .img = krasts, .pos = POS(28, 13) },
    { .img = NULL },
};

static const Frame day0_frame[] = {
    { .img = panel_0a, .pos = POS( 0,  0) },
    { .img = panel_0b, .pos = POS(22,  8) },
    { .img = panel_0c, .pos = POS( 0, 16) },
};

static const Frame day1_frame[] = {
    { .img = panel_1a, .pos = POS( 0,  0) },
    { .img = panel_1b, .pos = POS(22,  8) },
    { .img = panel_1c, .pos = POS( 4, 16) },
};

static const Frame day2_frame[] = {
    { .img = panel_2a, .pos = POS( 0,  0) },
    { .img = panel_2b, .pos = POS(22,  6) },
    { .img = panel_2c, .pos = POS( 1, 16) },
};

static const Frame day3_frame[] = {
    { .img = panel_3a, .pos = POS( 0,  0) },
    { .img = panel_3b, .pos = POS(22,  7) },
    { .img = panel_3c, .pos = POS( 0, 16) },
};

const Frame sprites[] = {
    { .img = velk1,   .pos = POS(13, 11) },
    { .img = aukla2,  .pos = POS(15, 14) },
    { .img = loms,    .pos = POS(15, 11) },
    { .img = NULL },
    { .img = velk2,   .pos = POS(14, 12) },
    { .img = aukla1,  .pos = POS(15, 16) },
    { .img = NULL },
    { .img = urbis,   .pos = POS(14,  8) },
    { .img = swirl,   .pos = POS(14, 10) },
    { .img = drill,   .pos = POS(16, 15) },
    { .img = NULL },
    { .img = copene3, .pos = POS(16,  0) },
    { .img = copene2, .pos = POS(16,  0) },
    { .img = copene1, .pos = POS(16,  0) },
    { .img = hole,    .pos = POS(12, 19) },
    { .img = NULL },
    { .img = motils,  .pos = POS( 9, 18) },
    { .img = motils,  .pos = POS(24, 20) },
    { .img = motils,  .pos = POS(21, 19) },
    { .img = motils,  .pos = POS( 6, 21) },
    { .img = NULL },
    { .img = title,   .pos = POS( 0,  0) },
    { .img = ezers,   .pos = POS( 0,  0) },
    { .img = beigas,  .pos = POS( 0,  0) },
    { .img = symbols },
};

const Frame fishes[] = {
    { .img = NULL },
    { .img = mormene, .pos = POS(18, 12) },
    { .img = weeds,   .pos = POS(18, 12) },
    { .img = ruffe,   .pos = POS(18, 12) },
    { .img = perch,   .pos = POS(18, 12) },
    { .img = asaris,  .pos = POS(18, 12) },
    { .img = decent,  .pos = POS(18, 12) },
    { .img = large,   .pos = POS(18, 12) },
    { .img = makans , .pos = POS(18, 12) },
};

extern const Text day0_text[];
extern const Text day1_text[];
extern const Text day2_text[];
extern const Text day3_text[];

const Panel panels[] = {
    { .frame = day0_frame, .text = day0_text },
    { .frame = day1_frame, .text = day1_text },
    { .frame = day2_frame, .text = day2_text },
    { .frame = day3_frame, .text = day3_text },
};
