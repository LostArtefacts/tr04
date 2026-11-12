#ifndef HARPY_H
#define HARPY_H

#include "items.h"





typedef struct {
    sint16 x;
    sint16 y;
    sint16 z;
    sint8 mesh_num;
} HARPY_HAND;

void TriggerHarpyMissile(PHD_3DPOS *pos, sint16 room_number, sint16 mesh);
void InitialiseHarpy(sint16 item_number);
void TriggerHarpySparks(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv);
void TriggerHarpyFlame(sint16 item_number, uint8 NodeNumber, sint32 size);
void TriggerHarpyMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv);
void DoHarpyEffects(ITEM_INFO *item, sint16 item_number);

extern HARPY_HAND harpy_left_hand;
extern HARPY_HAND harpy_right_hand;

#endif
