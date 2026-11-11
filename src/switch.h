#ifndef SWITCH_H
#define SWITCH_H

#include "types.h"
#include "items.h"
#include "collide.h"

void SwitchControl(sint16 item_number);
void SwitchCollision2(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);

extern sint16 Switch2Bounds[12];
extern PHD_VECTOR Switch2Pos;

#endif
