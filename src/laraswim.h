#ifndef LARASWIM_H
#define LARASWIM_H

#include "lara.h"
#include "collide.h"

void LaraSwimCollision(ITEM_INFO *item, COLL_INFO *coll);
void SwimTurn(ITEM_INFO *item);
void lara_as_dive(ITEM_INFO *item, COLL_INFO *coll);

#endif
