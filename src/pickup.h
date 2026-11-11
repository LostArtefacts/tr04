#ifndef PICKUP_H
#define PICKUP_H

#include "items.h"
#include "collide.h"

extern uint8 RPickups[16];
extern uint8 NumRPickups;

void AnimatingPickUp(sint16 item_number);
void PuzzleDoneCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll);
void PuzzleDone(ITEM_INFO *item, sint16 item_number);
sint32 KeyTrigger(sint16 item_num);

#endif
