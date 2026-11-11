#ifndef SWITCH_H
#define SWITCH_H

#include "types.h"
#include "items.h"
#include "collide.h"

void SwitchControl(sint16 item_number);
void SwitchCollision2(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void TestTriggersAtXYZ(sint32 x, sint32 y, sint32 z, sint16 room_number, sint16 heavy, sint16 flags);
void JumpSwitchCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);

extern sint16 Switch2Bounds[12];
extern PHD_VECTOR Switch2Pos;
extern sint16 JumpSwitchBounds[12];
extern PHD_VECTOR JumpSwitchPos;

#endif
