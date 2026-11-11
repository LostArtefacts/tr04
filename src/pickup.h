#ifndef PICKUP_H
#define PICKUP_H

#include "items.h"
#include "collide.h"

extern uint8 RPickups[16];
extern uint8 NumRPickups;
extern sint16 KeyHoleBounds[12];
extern PHD_VECTOR KeyHolePosition;
extern sint16 PuzzleBounds[12];
extern sint16 SarcophagusBounds[12];
extern PHD_VECTOR SarcophagusPos;

void AnimatingPickUp(sint16 item_number);
void PickUpCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll);
void PuzzleDoneCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll);
void PuzzleDone(ITEM_INFO *item, sint16 item_number);
sint32 KeyTrigger(sint16 item_num);
sint32 PickupTrigger(sint16 item_num);
void RegeneratePickups(void);
sint16 *FindPlinth(ITEM_INFO *item);
void KeyHoleCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void PuzzleHoleCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll);

#endif
