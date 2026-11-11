#ifndef OBJECTS_H
#define OBJECTS_H

#include "items.h"

void BridgeFlatCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
sint32 GetOffset(ITEM_INFO *item, sint32 x, sint32 z);
void BridgeTilt1Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void BridgeTilt2Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);

#endif
