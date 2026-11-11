#ifndef TRAPS_H
#define TRAPS_H

#include "items.h"
#include "collide.h"

extern PHD_VECTOR FloorTrapDoorPos;
extern sint16 FloorTrapDoorBounds[12];
extern PHD_VECTOR CeilingTrapDoorPos;
extern sint16 CeilingTrapDoorBounds[12];

void LaraBurn(void);
void LavaBurn(ITEM_INFO *item);
void CloseTrapDoor(ITEM_INFO *item);
void OpenTrapDoor(ITEM_INFO *item);
void TrapDoorControl(sint16 item_number);
void FallingCeiling(sint16 item_number);
sint32 OnTwoBlockPlatform(ITEM_INFO *item, sint32 x, sint32 z);
void FallingBlockFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void FallingBlockCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void FallingBlock(sint16 item_number);
void ControlFallingBlock2(sint16 item_number);
void FallingBlockCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void TwoBlockPlatformFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void TwoBlockPlatformCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void ControlPlough(sint16 item_number);
void ControlChain(sint16 item_number);
void ControlStargate(sint16 item_number);
void ControlSmashableBikeWall(sint16 item_number);
void ControlCatwalkBlade(sint16 item_number);
void ControlMovingBlade(sint16 item_number);
void ControlPlinthBlade(sint16 item_number);
void ControlFallingSquishyBlock(sint16 item_number);
void FloorTrapDoorCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void CeilingTrapDoorCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void ControlSlicerDicer(sint16 item_number);

#endif
