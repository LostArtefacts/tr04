#ifndef LARASWIM_H
#define LARASWIM_H

#include "lara.h"
#include "collide.h"

#define UW_MAXSPEED 200
#define WATER_FRICTION 6

void LaraUnderWater(ITEM_INFO *item, COLL_INFO *coll);
sint32 GetWaterDepth(sint32 x, sint32 y, sint32 z, sint16 room_number);
void LaraSwimCollision(ITEM_INFO *item, COLL_INFO *coll);
void SwimTurn(ITEM_INFO *item);
void lara_as_dive(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_waterroll(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_swim(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_glide(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_tread(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_dive(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_uwdeath(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_waterroll(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_swim(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_glide(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_tread(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_uwdeath(ITEM_INFO *item, COLL_INFO *coll);
void LaraTestWaterDepth(ITEM_INFO *item, COLL_INFO *coll);

#endif
