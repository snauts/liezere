#include "main.h"

#ifdef SPANISH
void put_dash(char c, byte x, byte y) {
    static const byte dash[] = { 0x00, 0x08, 0x10, 0x00};
    const byte *ptr = dash;

    if (!LOWER_CASE(c)) {
	ptr += 1;
    }
    else if (c == 'i') {
	x--; y++;
    }

    put_diacritic(ptr, x, y - 2);
}

void put_check(char c, byte x, byte y) {
    static const byte check[] = { 0x00, 0x14, 0x28, 0x00 };
    const byte *ptr = check;

    if (!LOWER_CASE(c)) ptr += 1;

    put_diacritic(ptr, x, y - 2);
}

void put_tick(char c, byte x, byte y) {
    static const byte tick[] = { 0x24, 0x00, 0x00, 0x028, 0x00 };
    const byte *ptr = tick;

    if (LOWER_CASE(c)) ptr += 2;

    put_diacritic(ptr, x, y - 1);
}
#endif
