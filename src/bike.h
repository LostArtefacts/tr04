#ifndef BIKE_H
#define BIKE_H

#include "items.h"

typedef struct bikeinfo {
    sint16 right_front_wheelrot;
    sint16 right_back_wheelrot;
    sint32 left_wheelrot;
    sint32 velocity;
    sint32 unused1;
    sint32 pitch1;
    sint16 move_angle;
    sint16 extra_rotation;
    sint16 rot_thing;
    sint32 bike_turn;
    sint32 pitch2;
    sint16 flags;
    sint16 light_intensity;
} BIKEINFO;

void DrawBikeBeam(ITEM_INFO *item);
void BikeStart(ITEM_INFO *item, ITEM_INFO *l);
void DrawBikeExtras(ITEM_INFO *item);

extern ITEM_INFO *GlobalBikeItem;

#endif
