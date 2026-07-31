#include "main.h"

#ifdef ENGLISH
void put_dash(char c, byte x, byte y) {
    c; x; y;
}

void put_check(char c, byte x, byte y) {
    c; x; y;
}

void put_tick(char c, byte x, byte y) {
    c; x; y;
}

const Text tutorial[] = {
    { .pos = POS(24,   8), .str = "To keep motivation go ice fishing," },
    { .pos = POS(24,  20), .str = "you need to catch the lunker every day." },
    { .pos = POS(24,  48), .str = "1. Walk around the lake Q&0 A&1 O&2 P&3" },
    { .pos = POS(24,  60), .str = "2. Choose place to fish with SPACE" },
    { .pos = POS(24,  72), .str = "3. Use O&2 P&3 to drill hole in ice" },
    { .pos = POS(24,  84), .str = "4. Jerk mormyshka by mashing SPACE" },
    { .pos = POS(38,  96), .str = "Fish won't bite if you mash too slow" },
    { .pos = POS(24, 108), .str = "5. To hook the fish press Q&0" },
    { .pos = POS(24, 120), .str = "6. Pull fish in with keys O&2 P&3" },
    { .pos = POS(38, 132), .str = "Pull too fast and the line will snap" },
    { .pos = POS(38, 144), .str = "Pull too slow and the fish will escape" },
    { .pos = POS(24, 172),
      .str = "The bigger the fish, the closer the lunker" },
    TEXT_SENTINEL,
};

const char* const reports[] = {
    "You pulled too fast, line snapped!",
    "You pulled too slow, fish escaped!",
    "A snag, fish is not even close.",
    "Measly ruffe.",
    "70g - small perch.",
    "100g - perch as a perch.",
    "100g - a decent perch.",
    "100g - large one, the lunker must be close.",
    "Damn, this is the lunker!",
};

const char* const stat_strs[] = {
    "Lunkers:", "Perches:", "Ruffes:", "Weight:", "Rank:",
};

const char* const rank_strs[] = {
    "Greenhorn",
    "Line waster",
    "Ice grinder",
    "Master baiter",
    "Fish butcher",
};

const Text day0_text[] = {
    { .pos = POS(64, 26), .str = "What fun is in freezing my ass off?" },
    TEXT_SENTINEL,

    { .pos = POS(0, 90), .str = "If winter break could be spent," },
    TEXT_SENTINEL,

    { .pos = POS(64, 154), .str = "doing more enjoyable things." },
    TEXT_SENTINEL,
    TEXT_SENTINEL,
};

const Text day1_text[] = {
    { .pos = POS(64, 22), .str = "Hey guys, would you like to come to" },
    { .pos = POS(64, 34), .str = "my countryside during winter break?" },
    TEXT_SENTINEL,

    { .pos = POS(24, 90), .str = "Hell yeah, but where is it?" },
    TEXT_SENTINEL,

    { .pos = POS(96, 154), .str = "It is in Liezere!" },
    TEXT_SENTINEL,

    { .pos = POS(255, 64), .str = "- Day 1 -" },
    { .pos = POS(255, 116), .str = "Fish is going to bite like crazy, right?" },
    TEXT_SENTINEL,
};

const Text day2_text[] = {
    { .pos = POS(72, 26), .str = "When you catch the lunker," },
    TEXT_SENTINEL,

    { .pos = POS(16, 90), .str = "a desire to celebrate arises." },
    TEXT_SENTINEL,

    { .pos = POS(80, 154), .str = "Usually it ends how it ends." },
    TEXT_SENTINEL,

    { .pos = POS(255,  64), .str = "- Day 2 -" },
    { .pos = POS(255, 116), .str = "Who would have thought, guys slept in." },
    TEXT_SENTINEL,
};

const Text day3_text[] = {
    { .pos = POS(72, 18), .str = "It would be a shame" },
    { .pos = POS(72, 30), .str = "to waste such a catch." },
    TEXT_SENTINEL,

    { .pos = POS(0, 90), .str = "Big fish were eaten by ourselves," },
    TEXT_SENTINEL,

    { .pos = POS(64, 154), .str = "but small fish were fed to cats." },
    TEXT_SENTINEL,

    { .pos = POS(255,  64), .str = "- Day 3 -" },
    { .pos = POS(255, 116), .str = "Appetite comes with eating." },
    { .pos = POS(255, 128), .str = "Today you need to catch two lunkers." },
    TEXT_SENTINEL,
};

const Text choices[] = {
    { .pos = POS(88,  96), .str = "1 - Keyboard" },
    { .pos = POS(88, 112), .str = "2 - Joystick" },
    { .pos = POS(88, 128), .str = "3 - Tutorial" },
    { .pos = POS( 4, 174), .str = "Game: snauts" },
    { .pos = POS( 1, 184), .str = "Music: hazard_pht" },
    TEXT_SENTINEL,
};

const Text fish_map[] = {
    { .pos = POS(204,  92), .mask = 0xff, .str = "Lunker" },
    { .pos = POS(145, 115), .mask = 0xff, .str = "Perches" },
    { .pos = POS(80,  143), .mask = 0x0f, .str = "Ruffes" },
    { .pos = POS(22,  170), .mask = 0xf0, .str = "Snags" },
    TEXT_SENTINEL,
};

const Text stat_title[] = {
    { .pos = POS(255, 24), .str = "Statistics" },
    { .pos = POS(255, 32), .str = "=========" },
    TEXT_SENTINEL,
};

const Text goodbye[] = {
    { .pos = POS(40, 72),
      .str = "On the evening of the third day" },
    { .pos = POS(40, 84),
      .str = "comes the realization that a lake" },
    { .pos = POS(40, 96),
      .str = "can not provide indefinitely." },
    { .pos = POS(40, 108),
      .str = "A feeling of satisfaction sets in." },
    TEXT_SENTINEL,
};

const Text the_end = { .pos = POS(16, 35), .str = "THE END" };
#endif
