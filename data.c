#include "main.h"
#include "data.h"

const Frame apkaime[] = {
    { .img = puduris1, .x = 2,  .y = 14 },
    { .img = puduris1, .x = 5,  .y = 11 },
    { .img = puduris1, .x = 7,  .y = 22 },
    { .img = puduris1, .x = 10, .y = 10 },
    { .img = puduris1, .x = 16, .y = 8  },
    { .img = puduris1, .x = 18, .y = 4  },
    { .img = puduris1, .x = 22, .y = 18 },
    { .img = puduris2, .x = 0,  .y = 17 },
    { .img = puduris2, .x = 15, .y = 22 },
    { .img = puduris2, .x = 20, .y = 21 },
    { .img = puduris2, .x = 29, .y = 3  },
    { .img = puduris2, .x = 30, .y = 8  },
    { .img = puduris2, .x = 30, .y = 13 },
    { .img = niedres,  .x = 2,  .y = 19 },
    { .img = niedres,  .x = 7,  .y = 15 },
    { .img = niedres,  .x = 8,  .y = 14 },
    { .img = niedres,  .x = 8,  .y = 14 },
    { .img = niedres,  .x = 12, .y = 13 },
    { .img = niedres,  .x = 13, .y = 22 },
    { .img = niedres,  .x = 20, .y = 7  },
    { .img = niedres,  .x = 21, .y = 10 },
    { .img = niedres,  .x = 21, .y = 17 },
    { .img = niedres,  .x = 23, .y = 4  },
    { .img = niedres,  .x = 26, .y = 1  },
    { .img = niedres,  .x = 30, .y = 7  },
    { .img = niedres,  .x = 30, .y = 11 },
    { .img = niedres,  .x = 30, .y = 21 },
    { .img = NULL },
};

const Frame horizonts[] = {
    { .img = egle1,  .x = 25, .y = 3  },
    { .img = egle1,  .x = 26, .y = 3  },
    { .img = egle1,  .x = 28, .y = 1  },
    { .img = egle1,  .x = 2,  .y = 3  },
    { .img = egle1,  .x = 6,  .y = 1  },
    { .img = egle1,  .x = 22, .y = 3  },
    { .img = egle1,  .x = 19, .y = 3  },
    { .img = egle1,  .x = 8,  .y = 3  },
    { .img = egle1,  .x = 13, .y = 3  },
    { .img = egle1,  .x = 15, .y = 1  },
    { .img = egle2,  .x = 14, .y = 2  },
    { .img = egle2,  .x = 9,  .y = 2  },
    { .img = egle2,  .x = 20, .y = 2  },
    { .img = egle2,  .x = 23, .y = 2  },
    { .img = egle2,  .x = 0,  .y = 2  },
    { .img = egle2,  .x = 3,  .y = 2  },
    { .img = egle2,  .x = 5,  .y = 2  },
    { .img = egle2,  .x = 27, .y = 2  },
    { .img = egle2,  .x = 30, .y = 2  },
    { .img = egle2,  .x = 17, .y = 2  },
    { .img = egle2,  .x = 11, .y = 2  },
    { .img = egle3,  .x = 10, .y = 2  },
    { .img = egle3,  .x = 12, .y = 2  },
    { .img = egle3,  .x = 16, .y = 2  },
    { .img = egle3,  .x = 18, .y = 2  },
    { .img = egle3,  .x = 21, .y = 2  },
    { .img = egle3,  .x = 7,  .y = 2  },
    { .img = egle3,  .x = 24, .y = 2  },
    { .img = egle3,  .x = 4,  .y = 2  },
    { .img = egle3,  .x = 29, .y = 2  },
    { .img = egle3,  .x = 1,  .y = 2  },
    { .img = egle3,  .x = 31, .y = 2  },
    { .img = krasts, .x = 0,  .y = 13 },
    { .img = krasts, .x = 3,  .y = 13 },
    { .img = krasts, .x = 7,  .y = 13 },
    { .img = krasts, .x = 11, .y = 13 },
    { .img = krasts, .x = 13, .y = 13 },
    { .img = krasts, .x = 17, .y = 13 },
    { .img = krasts, .x = 18, .y = 13 },
    { .img = krasts, .x = 22, .y = 13 },
    { .img = krasts, .x = 25, .y = 13 },
    { .img = krasts, .x = 28, .y = 13 },
    { .img = NULL },
};

static const Frame day0_frame[] = {
    { .img = panel_0a, .x = 0,  .y = 0  },
    { .img = panel_0b, .x = 22, .y = 8  },
    { .img = panel_0c, .x = 0,  .y = 16 },
};

static const Frame day1_frame[] = {
    { .img = panel_1a, .x = 0,  .y = 0  },
    { .img = panel_1b, .x = 22, .y = 8  },
    { .img = panel_1c, .x = 4,  .y = 16 },
};

static const Frame day2_frame[] = {
    { .img = panel_2a, .x = 0,  .y = 0  },
    { .img = panel_2b, .x = 22, .y = 6  },
    { .img = panel_2c, .x = 1,  .y = 16 },
};

static const Frame day3_frame[] = {
    { .img = panel_3a, .x = 0,  .y = 0  },
    { .img = panel_3b, .x = 22, .y = 7  },
    { .img = panel_3c, .x = 0,  .y = 16 },
};

const byte* const fishes[] = {
    NULL, mormene, weeds, ruffe, perch, asaris, asaris, asaris, makans
};

const Text tutorial[] = {
    { .x = 32, .y = 8,   .str = "Lai saglab`atu motiv`aciju iet cop`et," },
    { .x = 32, .y = 20,  .str = "katru dienu j`ano^ker lielais makans." },
    { .x = 32, .y = 48,  .str = "1. Valk`aties pa ezeru Q&0 A&1 O&2 P&3" },
    { .x = 32, .y = 60,  .str = "2. Izv`el`eties copes vietu SPACE" },
    { .x = 32, .y = 72,  .str = "3. Izurbt caurumu led`u O&2 P&3" },
    { .x = 32, .y = 84,  .str = "4. ^Gorg`at mormeni var ar SPACE" },
    { .x = 46, .y = 96,  .str = "Zivis ne^kersies, ja l`eni ~gorg`as" },
    { .x = 32, .y = 108, .str = "5. Lai piecirstu zivi j`aspie~z Q&0" },
    { .x = 32, .y = 120, .str = "6. Izvilkt zivi var spie~zot O&2 P&3" },
    { .x = 46, .y = 132, .str = "Velkot p`ar`ak `atri p`artr`uks aukla" },
    { .x = 46, .y = 144, .str = "Velkot p`ar`ak l`eni zivs aizies" },
    { .x = 32, .y = 172, .str = "Jo liel`aka zivs, jo tuv`ak t`a makanam" },
    TEXT_SENTINEL,
};

const Text choices[] = {
    { .x = 88, .y = 96,  .str = "1 - Klaviat`ura" },
    { .x = 88, .y = 112, .str = "2 - D~zoistiks" },
    { .x = 88, .y = 128, .str = "3 - Pam`ac`iba" },
    TEXT_SENTINEL,
};

const char* const reports[] = {
    "P`ar`ak stauji vilki, p`arr`avi auklu!",
    "P`ar`ak l`eni vilki, nokabin`aj`as maita!",
    "Zivis ~seit nav ne tuvu.",
    "N`ikul`igs ^k`isis.",
    " 70g - tas jau asar`itis.",
    "100g - asaris k`a asaris.",
    "100g - piekl`aj`igs asaris.",
    "100g - krup^naks, makanam j`abut tuvu.",
    "Jopcik, vot tas ir makans!",
};

static const Text day0_text[] = {
    { .x = 64,  .y = 26,  .str = "K`ads prieks sald`et nagus?" },
    TEXT_SENTINEL,

    { .x = 24,  .y = 90,  .str = "Ja br`ivlaiku var pavad`it," },
    TEXT_SENTINEL,

    { .x = 64,  .y = 154, .str = "darot pat`ikam`akas lietas." },
    TEXT_SENTINEL,
    TEXT_SENTINEL,
};

static const Text day1_text[] = {
    { .x = 64,  .y = 22,  .str = "Klau, d~zeki, negribat br`ivlaik`a" },
    { .x = 64,  .y = 34,  .str = "atbraukt pie manis uz laukiem?" },
    TEXT_SENTINEL,

    { .x = 8,   .y = 90,  .str = "S`uds jaut`ajums, bet kur tas ir?" },
    TEXT_SENTINEL,

    { .x = 96,  .y = 154, .str = "Tas ir Liez`er`e!" },
    TEXT_SENTINEL,

    { .x = 255, .y = 64,  .str = "- Pirm`a diena -" },
    { .x = 255, .y = 116, .str = "Nu tik nu ^kersies, nu tik nu b`us!" },
    TEXT_SENTINEL,
};

static const Text day2_text[] = {
    { .x = 72,  .y = 26,  .str = "Zini, kas ir j`adara, kad izcelies?" },
    TEXT_SENTINEL,

    { .x = 16,   .y = 90,  .str = "Pareizi, tev ir j`auzst`ajas." },
    TEXT_SENTINEL,

    { .x = 88,  .y = 154, .str = "L`idz neklausa vairs k`ajas." },
    TEXT_SENTINEL,

    { .x = 255, .y = 64,  .str = "- Otr`a diena -" },
    { .x = 255, .y = 116, .str = "Neizprotamu apst`ak^lu d`e^l "
				 "d~zeki aizgul`ejas." },
    TEXT_SENTINEL,
};

static const Text day3_text[] = {
    { .x = 72,   .y = 26,  .str = "T`adu lomu b`utu gr`eks laist zudum`a." },
    TEXT_SENTINEL,

    { .x = 24,   .y = 90,  .str = "Liel`as zivis j`a`ed pa~siem," },
    TEXT_SENTINEL,

    { .x = 64,  .y = 154, .str = "bet maz`as j`aatdod ka^kiem." },
    TEXT_SENTINEL,

    { .x = 255, .y = 64,  .str = "- Tre~s`a diena -" },
    { .x = 255, .y = 116, .str = "Apet`ite rodas `edot." },
    { .x = 255, .y = 128, .str = "~Sodien j`ano^ker divi makani." },
    TEXT_SENTINEL,
};

const Text the_end = { .x = 16, .y = 35, .str = "BEIGAS" };

const Panel panels[] = {
    { .frame = day0_frame, .text = day0_text },
    { .frame = day1_frame, .text = day1_text },
    { .frame = day2_frame, .text = day2_text },
    { .frame = day3_frame, .text = day3_text },
};
