#ifndef DEMIGOD_H
#define DEMIGOD_H

#include "items.h"

void InitialiseDemigod(sint16 item_number);
void TriggerDemigodMissile(PHD_3DPOS *pos, sint16 room_number, sint16 type);
void TriggerDemigodMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv);
void TriggerHammerSmoke(sint32 x, sint32 y, sint32 z, sint32 num);
void DoDemigodEffects(sint16 item_number);
void DemigodControl(sint16 item_number);

#endif
