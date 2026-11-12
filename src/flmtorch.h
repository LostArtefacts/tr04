#ifndef FLMTORCH_H
#define FLMTORCH_H

#include "items.h"
#include "collide.h"

void GetFlameTorch(void);
void TriggerTorchFlame(sint16 item_number, sint32 node);
void FireCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void FlameTorchControl(sint16 item_number);

extern ITEM_INFO *TorchItem;
extern sint16 FireBounds[12];

#endif
