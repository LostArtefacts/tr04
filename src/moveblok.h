#ifndef MOVEBLOK_H
#define MOVEBLOK_H

#include "items.h"
#include "collide.h"

extern sint16 MovingBlockBounds[12];
extern PHD_VECTOR MovingBlockPos;

void ClearMovableBlockSplitters(sint32 x, sint32 y, sint32 z, sint16 room_number);
void InitialiseMovingBlock(sint16 item_number);
void DrawPlanetEffect(ITEM_INFO *item);
sint32 TestBlockPush(ITEM_INFO *item, sint32 height, uint16 quadrant);
sint32 TestBlockPull(ITEM_INFO *item, sint32 height, uint16 quadrant);
void ControlPlanetEffect(sint16 item_number);
void InitialisePlanetEffect(sint16 item_number);
void MovableBlock(sint16 item_number);
void MovableBlockCollision(sint16 item_number, ITEM_INFO *laraitem, COLL_INFO *coll);

#endif
