#include "main.h"

#ifdef ENGLISH
const Text tutorial[] = {
    { .pos = POS(32,   8), .str = "To keep motivation go ice fishing," },
    { .pos = POS(32,  20), .str = "each day you need to catch the lunker." },
    { .pos = POS(32,  48), .str = "1. Walk around the lake Q&0 A&1 O&2 P&3" },
    { .pos = POS(32,  60), .str = "2. Choose place to fish with SPACE" },
    { .pos = POS(32,  72), .str = "3. Use O&2 P&3 to drill hole in ice" },
    { .pos = POS(32,  84), .str = "4. Jerk mormyshka by mashing SPACE" },
    { .pos = POS(46,  96), .str = "Fish won't bite if you mash to slow" },
    { .pos = POS(32, 108), .str = "5. To hook the fish press Q&0" },
    { .pos = POS(32, 120), .str = "6. Pull fish in with keys O&2 P&3" },
    { .pos = POS(46, 132), .str = "If you pull too fast line will snap" },
    { .pos = POS(46, 144), .str = "If you pull too slow fish will escape" },
    { .pos = POS(32, 172), .str = "Larger fish are closer to the lunker" },
    TEXT_SENTINEL,
};

const char* const reports[] = {
    "You pulled too fast, fishing line snapped!",
    "You pulled too snow, fish escaped!",
    "A twig, fish is not even close.",
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
    "Distinguished fisherman",
    "Fish butcher",
};

const Text choices[] = {
    { .pos = POS(88,  96), .str = "1 - Keyboard" },
    { .pos = POS(88, 112), .str = "2 - Joystick" },
    { .pos = POS(88, 128), .str = "3 - Tutorial" },
    TEXT_SENTINEL,
};

const Text stat_title[] = {
    { .pos = POS(255, 24), .str = "Statistics" },
    { .pos = POS(255, 32), .str = "=========" },
    TEXT_SENTINEL,
};

const Text the_end = { .pos = POS(16, 35), .str = "THE END" };
#endif
