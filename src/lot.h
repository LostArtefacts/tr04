#ifndef LOT_H
#define LOT_H

#include "box.h"

void InitialiseLOTarray(int allocmem);
void DisableBaddieAI(sint16 item_number);
int EnableBaddieAI(sint16 item_number, int Always);
void InitialiseSlot(sint16 item_number, int slot);

extern CREATURE_INFO *baddie_slots;

#endif
