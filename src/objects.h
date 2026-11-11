#ifndef OBJECTS_H
#define OBJECTS_H

#include "items.h"
#include "collide.h"

extern PHD_VECTOR PolePos;
extern sint16 PoleBounds[12];
extern PHD_VECTOR PolePosR;
extern sint16 StatuePlinthBounds[12];

void BridgeFlatCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
sint32 GetOffset(ITEM_INFO *item, sint32 x, sint32 z);
void BridgeTilt1Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void BridgeTilt2Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void BridgeTilt1Floor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void BridgeTilt2Floor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void BridgeFlatFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void SmashObject(sint16 item_number);
void SmashObjectControl(sint16 item_number);
void ControlAnimatingSlots(sint16 item_number);
void ControlWaterfall(sint16 item_number);
void ControlTriggerTriggerer(sint16 item_number);
void ControlMapper(sint16 item_number);
void TriggerRopeFlame(PHD_VECTOR *pos);
void BurningRopeCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void ControlBurningRope(sint16 item_number);
void PoleCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll);
void StatuePlinthCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void EarthQuake(sint16 item_number);

#endif
