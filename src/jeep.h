#ifndef JEEP_H
#define JEEP_H

#include "items.h"

typedef struct jeepinfo {
    sint16 right_front_wheelrot;
    sint16 left_front_wheelrot;
    sint16 left_back_wheelrot;
    sint16 right_back_wheelrot;
    sint32 velocity;
    sint32 unused1;
    sint32 pitch1;
    sint32 turn_rate;
    sint32 camera_angle;
    sint16 move_angle;
    sint16 extra_rotation;
    sint16 rot_thing;
    sint32 pitch2;
    sint16 flags;
    sint16 unused2;
    sint16 gear;
} JEEPINFO;

void JeepStart(ITEM_INFO *item, ITEM_INFO *l);
sint32 DoShift(ITEM_INFO *item, PHD_VECTOR *pos, PHD_VECTOR *old);
void DrawJeepExtras(ITEM_INFO *item);
sint32 JeepCheckGetOut(void);

#endif
