#ifndef LARASURF_H
#define LARASURF_H

#include "lara.h"
#include "collide.h"

#define SURF_MAXSPEED 60
#define DIVE_COUNT 10

void LaraSurfaceCollision(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_surfswim(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_surfback(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_surfleft(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_surfright(ITEM_INFO *item, COLL_INFO *coll);
void lara_as_surftread(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_surfback(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_surfleft(ITEM_INFO *item, COLL_INFO *coll);

#endif
