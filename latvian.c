#include "main.h"

#ifdef LATVIAN
void put_dash(char c, byte x, byte y) {
    static const byte dash[] = { 0x3c, 0x00, 0x00, 0x38, 0x00, 0x00, 0x70 };
    const byte *ptr = dash;

    if (c == 'i') {
	ptr += 4;
    }
    else if (c == 'I') {
	ptr += 3;
    }
    else if (LOWER_CASE(c)) {
	ptr += 2;
    }

    put_diacritic(ptr, x, y - 1);
}

void put_check(char c, byte x, byte y) {
    static const byte check[] = { 0x24, 0x18, 0x00, 0x28, 0x10 };
    const byte *ptr = check;

    if (LOWER_CASE(c)) ptr += 2;

    put_diacritic(ptr, x, y - 2);
}

void put_tick(char c, byte x, byte y) {
    static const byte tick[] = { 0x08, 0x10, 0x00 };

    if (LOWER_CASE(c)) x--;

    if (c == 'k') x--;

    put_diacritic(tick, x, y + 7);
}

const Text tutorial[] = {
    { .pos = POS(32,   8), .str = "Lai saglab`atu motiv`aciju iet cop`et," },
    { .pos = POS(32,  20), .str = "katru dienu j`ano^ker lielais makans." },
#if defined(ZXS) || defined(C64)
    { .pos = POS(32,  48), .str = "1. Valk`aties pa ezeru Q&0 A&1 O&2 P&3" },
    { .pos = POS(32,  60), .str = "2. Izv`el`eties copes vietu SPACE" },
    { .pos = POS(32,  72), .str = "3. Izurbt caurumu led`u O&2 P&3" },
    { .pos = POS(32,  84), .str = "4. ^Gorg`at mormeni var ar SPACE" },
    { .pos = POS(46,  96), .str = "Zivis ne^kersies, ja l`eni ~gorg`as" },
    { .pos = POS(32, 108), .str = "5. Lai piecirstu zivi j`aspie~z Q&0" },
    { .pos = POS(32, 120), .str = "6. Izvilkt zivi var spie~zot O&2 P&3" },
#else
    { .pos = POS(32,  48), .str = "1. Valk`aties pa ezeru &0 &1 &2 &3" },
    { .pos = POS(32,  60), .str = "2. Izv`el`eties copes vietu SPACE" },
    { .pos = POS(32,  72), .str = "3. Izurbt caurumu led`u &2 &3" },
    { .pos = POS(32,  84), .str = "4. ^Gorg`at mormeni var ar SPACE" },
    { .pos = POS(46,  96), .str = "Zivis ne^kersies, ja l`eni ~gorg`as" },
    { .pos = POS(32, 108), .str = "5. Lai piecirstu zivi j`aspie~z &0" },
    { .pos = POS(32, 120), .str = "6. Izvilkt zivi var spie~zot &2 &3" },
#endif
    { .pos = POS(46, 132), .str = "Velkot p`ar`ak `atri p`artr`uks aukla" },
    { .pos = POS(46, 144), .str = "Velkot p`ar`ak l`eni zivs aizies" },
    { .pos = POS(32, 172), .str = "Jo liel`aka zivs, jo tuv`ak t`a makanam" },
    TEXT_SENTINEL,
};

const Text choices[] = {
    { .pos = POS(88,  96), .str = "1 - Klaviat`ura" },
    { .pos = POS(88, 112), .str = "2 - D~zoistiks" },
    { .pos = POS(88, 128), .str = "3 - Pam`ac`iba" },
    { .pos = POS( 6, 174), .str = "Sp`ele: snauts" },
#if defined(AY)
    { .pos = POS( 1, 184), .str = "M`uzika: hazard_pht" },
#endif
    TEXT_SENTINEL,
};

const Text fish_map[] = {
    { .pos = POS(204,  92), .mask = 0xff, .str = "Makans" },
    { .pos = POS(145, 115), .mask = 0xff, .str = "Asari" },
    { .pos = POS(85,  145), .mask = 0x0f, .str = "^K`i~si" },
    { .pos = POS(22,  170), .mask = 0xf0, .str = "Zacepi" },
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
    "100g - krup^naks, makanam j`ab`ut tuvu.",
    "Jopcik, vot tas ir makans!",
};

const char* const stat_strs[] = {
    "Makani:", "Asari:", "^K`i~si:", "Svars:", "Rangs:",
};

const char* const rank_strs[] = {
    "Za^lais gur^kis",
    "Auklas p`ud`et`ajs",
    "Ledgrauzis",
    "Copes meistars",
    "Zivju bende",
};

const Text day0_text[] = {
    { .pos = POS(64, 26), .str = "K`ads prieks sald`et nagus?" },
    TEXT_SENTINEL,

    { .pos = POS(24, 90), .str = "Ja br`ivlaiku var pavad`it," },
    TEXT_SENTINEL,

    { .pos = POS(64, 154), .str = "darot pat`ikam`akas lietas." },
    TEXT_SENTINEL,
    TEXT_SENTINEL,
};

const Text day1_text[] = {
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

const Text day2_text[] = {
    { .pos = POS(72, 26), .str = "Kad ir no^kerts lielais makans," },
    TEXT_SENTINEL,

    { .pos = POS(16, 90), .str = "rodas dab`iga v`elme atz`im`et." },
    TEXT_SENTINEL,

    { .pos = POS(88, 148), .str = "Parasti tas beidzas" },
    { .pos = POS(88, 160), .str = "t`a k`a tas beidzas." },
    TEXT_SENTINEL,

    { .pos = POS(255,  64), .str = "- Otr`a diena -" },
    { .pos = POS(255, 116),
      .str = "Kur~s to b`utu dom`ajis, ka d~zeki aizgul`esies?" },
    TEXT_SENTINEL,
};

const Text day3_text[] = {
    { .pos = POS(72, 26), .str = "T`adu lomu b`utu gr`eks laist zudum`a." },
    TEXT_SENTINEL,

    { .pos = POS(24, 90), .str = "Liel`as zivis ap`ed`am pa~si," },
    TEXT_SENTINEL,

    { .pos = POS(64, 154), .str = "bet maz`as izbaroj`am ka^kiem." },
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

const Text goodbye[] = {
    { .pos = POS(32, 78), .str = "Tre~s`as dienas vakar`a p`ar^nem apzi^na," },
    { .pos = POS(32, 90), .str = "ka ezers nevar dot bezgal`igi daudz." },
    { .pos = POS(32, 102), .str = "Beidzot iest`ajas gandar`ijuma saj`uta." },
    TEXT_SENTINEL,
};

const Text the_end = { .pos = POS(16, 35), .str = "BEIGAS" };
#endif
