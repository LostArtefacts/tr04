#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "items.h"
#include "collide.h"

extern sint16 ElementPuzzleBounds[12];

void InitialiseElementPuzzle(sint16 item_number);
void ControlElementPuzzle(sint16 item_number);
void ElementPuzzleCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);

#endif
