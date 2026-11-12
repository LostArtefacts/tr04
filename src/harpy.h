#ifndef HARPY_H
#define HARPY_H

#include "items.h"





void TriggerHarpyMissile(PHD_3DPOS *pos, sint16 room_number, sint16 mesh);
void InitialiseHarpy(sint16 item_number);
void TriggerHarpySparks(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv);

#endif
