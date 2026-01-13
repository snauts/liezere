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

const byte* const fishes[] = {
    NULL, mormene, weeds, ruffe, perch, asaris, decent, large, makans
};

const Text tutorial[] = {
    { .pos = POS(32,   8), .str = "Lai saglab`atu motiv`aciju iet cop`et," },
    { .pos = POS(32,  20), .str = "katru dienu j`ano^ker lielais makans." },
    { .pos = POS(32,  48), .str = "1. Valk`aties pa ezeru Q&0 A&1 O&2 P&3" },
    { .pos = POS(32,  60), .str = "2. Izv`el`eties copes vietu SPACE" },
    { .pos = POS(32,  72), .str = "3. Izurbt caurumu led`u O&2 P&3" },
    { .pos = POS(32,  84), .str = "4. ^Gorg`at mormeni var ar SPACE" },
    { .pos = POS(46,  96), .str = "Zivis ne^kersies, ja l`eni ~gorg`as" },
    { .pos = POS(32, 108), .str = "5. Lai piecirstu zivi j`aspie~z Q&0" },
    { .pos = POS(32, 120), .str = "6. Izvilkt zivi var spie~zot O&2 P&3" },
    { .pos = POS(46, 132), .str = "Velkot p`ar`ak `atri p`artr`uks aukla" },
    { .pos = POS(46, 144), .str = "Velkot p`ar`ak l`eni zivs aizies" },
    { .pos = POS(32, 172), .str = "Jo liel`aka zivs, jo tuv`ak t`a makanam" },
    TEXT_SENTINEL,
};

const Text choices[] = {
    { .pos = POS(88,  96), .str = "1 - Klaviat`ura" },
    { .pos = POS(88, 112), .str = "2 - D~zoistiks" },
    { .pos = POS(88, 128), .str = "3 - Pam`ac`iba" },
    TEXT_SENTINEL,
};

const char* const reports[] = {
    "P`ar`ak stauji vilki, p`arr`avi auklu!",
    "P`ar`ak l`eni vilki, nokabin`aj`as maita!",
    "Zaceps, zivis ~seit nav ne tuvu.",
    "N`ikul`igs ^k`isis.",
    "70g - tas jau asar`itis.",
    "100g - asaris k`a asaris.",
    "100g - piekl`aj`igs asaris.",
    "100g - krup^naks, makanam j`abut tuvu.",
    "Jopcik, vot tas ir makans!",
};

const char* const stat_strs[] = {
    "Makani:", "Asari:", "^K`i~si:", "Svars:", "Rangs:",
};

const char* const rank_strs[] = {
    "Za^lais gur^kis",
    "Auklas p`ud`et`ajs",
    "Atz`istams copmanis",
    "Zivju bende",
};

static const Text day0_text[] = {
    { .pos = POS(64, 26), .str = "K`ads prieks sald`et nagus?" },
    TEXT_SENTINEL,

    { .pos = POS(24, 90), .str = "Ja br`ivlaiku var pavad`it," },
    TEXT_SENTINEL,

    { .pos = POS(64, 154), .str = "darot pat`ikam`akas lietas." },
    TEXT_SENTINEL,
    TEXT_SENTINEL,
};

static const Text day1_text[] = {
    { .pos = POS(64, 22), .str = "Klau, d~zeki, negribat br`ivlaik`a" },
    { .pos = POS(64, 34), .str = "atbraukt pie manis uz laukiem?" },
    TEXT_SENTINEL,

    { .pos = POS(8, 90), .str = "S`uds jaut`ajums, bet kur tas ir?" },
    TEXT_SENTINEL,

    { .pos = POS(96, 154), .str = "Tas ir Liez`er`e!" },
    TEXT_SENTINEL,

    { .pos = POS(255, 64), .str = "- Pirm`a diena -" },
    { .pos = POS(255, 116), .str = "Nu tik nu ^kersies, nu tik nu b`us!" },
    TEXT_SENTINEL,
};

static const Text day2_text[] = {
    { .pos = POS(72, 26), .str = "Kad ir no^kerts lielais makans," },
    TEXT_SENTINEL,

    { .pos = POS(16, 90), .str = "rodas dab`iga v`elme atz`im`et." },
    TEXT_SENTINEL,

    { .pos = POS(88, 148), .str = "Parasti tas beidzas" },
    { .pos = POS(88, 160), .str = "t`a k`a tas beidzas." },
    TEXT_SENTINEL,

    { .pos = POS(255,  64), .str = "- Otr`a diena -" },
    { .pos = POS(255, 116), .str = "Neizprotamu apst`ak^lu d`e^l "
				   "d~zeki aizgul`ejas." },
    TEXT_SENTINEL,
};

static const Text day3_text[] = {
    { .pos = POS(72, 26), .str = "T`adu lomu b`utu gr`eks laist zudum`a." },
    TEXT_SENTINEL,

    { .pos = POS(24, 90), .str = "Liel`as zivis j`a`ed pa~siem," },
    TEXT_SENTINEL,

    { .pos = POS(64, 154), .str = "bet maz`as j`aatdod ka^kiem." },
    TEXT_SENTINEL,

    { .pos = POS(255,  64), .str = "- Tre~s`a diena -" },
    { .pos = POS(255, 116), .str = "Apet`ite rodas `edot." },
    { .pos = POS(255, 128), .str = "~Sodien j`ano^ker divi makani." },
    TEXT_SENTINEL,
};

const Text stat_title[] = {
    { .pos = POS(255, 24), .str = "Statistika" },
    { .pos = POS(255, 32), .str = "=========" },
    TEXT_SENTINEL,
};

const Text the_end = { .pos = POS(16, 35), .str = "BEIGAS" };

const Panel panels[] = {
    { .frame = day0_frame, .text = day0_text },
    { .frame = day1_frame, .text = day1_text },
    { .frame = day2_frame, .text = day2_text },
    { .frame = day3_frame, .text = day3_text },
};
