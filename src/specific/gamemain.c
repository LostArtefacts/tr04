#include <stdlib.h>

#include "gamemain.h"

uint16 GetRandom(WATERTAB *w, sint32 n)
{
    sint32 i;
    uint16 random;

    do {
        random = rand() & 0xFC;
        for (i = 0; i < n; i++) {
            if (w[i].random == random)
                break;
        }
        if (i == n)
            return random;
    } while (1);
    return 0;
}
