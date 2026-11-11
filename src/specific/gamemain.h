#ifndef SPECIFIC_GAMEMAIN_H
#define SPECIFIC_GAMEMAIN_H

#include "../types.h"

typedef struct watertab {
    char shimmer;
    char choppy;
    uint8 random;
    uint8 abs;
} WATERTAB;

uint16 GetRandom(WATERTAB *w, sint32 n);

#endif
