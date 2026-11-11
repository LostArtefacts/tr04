#ifndef TRAPS_H
#define TRAPS_H

#include "items.h"

void LaraBurn(void);
void LavaBurn(ITEM_INFO *item);
void CloseTrapDoor(ITEM_INFO *item);
void OpenTrapDoor(ITEM_INFO *item);
void TrapDoorControl(sint16 item_number);
void FallingCeiling(sint16 item_number);

#endif
