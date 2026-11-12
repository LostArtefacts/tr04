#ifndef TRAPS_H
#define TRAPS_H

#include "items.h"
#include "collide.h"

extern PHD_VECTOR FloorTrapDoorPos;
extern sint16 FloorTrapDoorBounds[12];
extern PHD_VECTOR CeilingTrapDoorPos;
extern sint16 CeilingTrapDoorBounds[12];
extern sint16 SPxzoffs[8];
extern char LibraryTab[8];
extern sint16 SPyoffs[8];
extern sint16 SPDETyoffs[8];

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
void RollingBallCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void ControlTwoBlockPlatform(sint16 item_number);
void ControlSpikeball(sint16 item_number);
void Control4xFloorRoofBlade(sint16 item_number);
void ControlBirdBlade(sint16 item_number);
void ControlSethBlade(sint16 item_number);
void DartsControl(sint16 item_number);
void DartEmitterControl(sint16 item_number);
void MineCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void ControlRaisingBlock(sint16 item_number);
void FallingSquishyBlockCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
sint32 TestBoundsCollideTeethSpikes(ITEM_INFO *item);
void FlameEmitter2Control(sint16 item_number);
void ControlHammer(sint16 item_number);
void ControlMineHelicopter(sint16 item_number);
void ControlBurningFloor(sint16 item_number);
void ControlRollingBall(sint16 item_number);
void ControlSprinkler(sint16 item_number);
void FlameControl(sint16 fx_number);
void FlameEmitter3Control(sint16 item_number);
void FlameEmitterControl(sint16 item_number);
void DrawScaledSpike(ITEM_INFO *item);

extern sint16 floor_fires[48];
extern sint16 deadly_floor_fires[8];
extern uint8 Flame3xzoffs[16][2];

#endif
