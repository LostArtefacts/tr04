#ifndef FLMTORCH_H
#define FLMTORCH_H

#include "items.h"
#include "collide.h"

void GetFlameTorch(void);
void TriggerTorchFlame(sint16 item_number, sint32 node);
void FireCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);

#endif
