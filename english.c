#include "main.h"

#ifdef ENGLISH
const Text tutorial[] = {
    { .pos = POS(32,   8), .str = "To keep motivation go ice fishing," },
    { .pos = POS(32,  20), .str = "each day you need to catch the Big Fish." },
    { .pos = POS(32,  48), .str = "1. Walk around the lake Q&0 A&1 O&2 P&3" },
    { .pos = POS(32,  60), .str = "2. Choose place to fish with SPACE" },
    { .pos = POS(32,  72), .str = "3. Use O&2 P&3 to drill hole in ice" },
    { .pos = POS(32,  84), .str = "4. Jerk mormyshka by mashing SPACE" },
    { .pos = POS(46,  96), .str = "Fish won't bite if you mash to slow" },
    { .pos = POS(32, 108), .str = "5. To hook the fish press Q&0" },
    { .pos = POS(32, 120), .str = "6. Pull fish in with keys O&2 P&3" },
    { .pos = POS(46, 132), .str = "If you pull too fast line will snap" },
    { .pos = POS(46, 144), .str = "If you pull too slow fish will escape" },
    { .pos = POS(32, 172), .str = "Larger fish are closer to the Big Fish" },
    TEXT_SENTINEL,
};

const Text choices[] = {
    { .pos = POS(88,  96), .str = "1 - Keyboard" },
    { .pos = POS(88, 112), .str = "2 - Joystick" },
    { .pos = POS(88, 128), .str = "3 - Tutorial" },
    TEXT_SENTINEL,
};
#endif
