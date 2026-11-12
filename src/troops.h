#ifndef TROOPS_H
#define TROOPS_H

#include "items.h"
#include "box.h"

extern BITE_INFO troop_gun;

void InitialiseTroop(sint16 item_number);
void TroopControl(sint16 item_number);

#endif
