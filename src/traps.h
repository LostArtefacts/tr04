#ifndef TRAPS_H
#define TRAPS_H

#include "items.h"

void LaraBurn(void);
void LavaBurn(ITEM_INFO *item);
void CloseTrapDoor(ITEM_INFO *item);
void OpenTrapDoor(ITEM_INFO *item);
void TrapDoorControl(sint16 item_number);
void FallingCeiling(sint16 item_number);
sint32 OnTwoBlockPlatform(ITEM_INFO *item, sint32 x, sint32 z);
void FallingBlockFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
void FallingBlockCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);

#endif
