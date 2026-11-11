#ifndef SWITCH_H
#define SWITCH_H

#include "types.h"
#include "items.h"
#include "collide.h"

void SwitchControl(sint16 item_number);

extern uint8 CurrentSequence;
extern uint8 Sequences[3];
extern uint8 SequenceUsed[6];
extern uint8 SequenceResults[3][3][3];
void SwitchCollision2(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void TestTriggersAtXYZ(sint32 x, sint32 y, sint32 z, sint16 room_number, sint16 heavy, sint16 flags);
void JumpSwitchCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void FullBlockSwitchControl(sint16 item_number);
void UnderwaterSwitchCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);

extern sint16 Switch2Bounds[12];
extern PHD_VECTOR Switch2Pos;
extern sint16 JumpSwitchBounds[12];
extern PHD_VECTOR JumpSwitchPos;
extern sint16 UnderwaterSwitchBounds[12];
extern sint16 UnderwaterSwitchBounds2[12];
extern PHD_VECTOR UnderwaterSwitchPos;
extern PHD_VECTOR UnderwaterSwitchPos2;

#endif
