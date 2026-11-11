#ifndef EFFECT2_H
#define EFFECT2_H

#include "3dglodef.h"

#define MAX_DYNAMICS 32

typedef struct dynamic {
    sint32 x;
    sint32 y;
    sint32 z;
    uint8 on;
    uint8 r;
    uint8 g;
    uint8 b;
    uint16 falloff;
    uint8 used;
    uint8 pad1[1];
    sint32 FalloffScale;
} DYNAMIC;

typedef struct ripple_struct {
    sint32 x;
    sint32 y;
    sint32 z;
    char flags;
    uint8 life;
    uint8 size;
    uint8 init;
} RIPPLE_STRUCT;

void DetatchSpark(sint32 num, sint32 type);
void KillAllCurrentItems(sint16 item_number);
void TriggerRicochetSpark(GAME_VECTOR *pos, sint32 ang, sint32 num, sint32 smoke_only);
void ClearDynamics(void);
void TriggerUnderwaterBlood(sint32 x, sint32 y, sint32 z, sint32 size);

extern sint32 KillEverythingFlag;
extern DYNAMIC dynamics[MAX_DYNAMICS * 2];
extern sint32 number_dynamics;
extern RIPPLE_STRUCT ripples[16];

#endif
