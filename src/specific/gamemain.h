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
void init_water_table(void);
sint32 S_LoadGame(sint32 slot_num);
sint32 S_SaveGame(sint32 slot_num);

extern sint32 SaveCounter;
extern WATERTAB WaterTable[22][64];
extern float vert_wibble_table[32];
extern float unused_vert_wibble_table[256];
extern uint8 water_abs[4];
extern sint16 water_shimmer[4];
extern sint16 water_choppy[4];

#endif
