#ifndef MOVEBLOK_H
#define MOVEBLOK_H

#include "items.h"

void ClearMovableBlockSplitters(sint32 x, sint32 y, sint32 z, sint16 room_number);
void InitialiseMovingBlock(sint16 item_number);
void DrawPlanetEffect(ITEM_INFO *item);
sint32 TestBlockPush(ITEM_INFO *item, sint32 height, uint16 quadrant);

#endif
