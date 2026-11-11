#ifndef LARASWIM_H
#define LARASWIM_H

#include "lara.h"
#include "collide.h"

void LaraSwimCollision(ITEM_INFO *item, COLL_INFO *coll);
void SwimTurn(ITEM_INFO *item);
void lara_as_dive(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_waterroll(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_swim(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_glide(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_tread(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_dive(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_uwdeath(ITEM_INFO *item, COLL_INFO *coll);

#endif
