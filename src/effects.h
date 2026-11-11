#ifndef EFFECTS_H
#define EFFECTS_H

#include "items.h"

typedef struct fx_info {
    PHD_3DPOS pos;
    sint16 room_number;
    sint16 object_number;
    sint16 next_fx;
    sint16 next_active;
    sint16 speed;
    sint16 fallspeed;
    sint16 frame_number;
    sint16 counter;
    sint16 shade;
    sint16 flag1;
    sint16 flag2;
} FX_INFO;

void lara_hands_free(ITEM_INFO *item);
sint16 DoBloodSplat(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 direction, sint16 room_number);

extern FX_INFO *effects;
extern sint16 next_fx_active;

#endif
