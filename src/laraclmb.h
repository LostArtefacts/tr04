#ifndef LARACLMB_H
#define LARACLMB_H

#include "collide.h"

sint32 LaraTestClimbPos(ITEM_INFO *item, sint32 front, sint32 right, sint32 origin, sint32 height, sint32 *shift);
sint32 LaraTestClimb(sint32 x, sint32 y, sint32 z, sint32 xfront, sint32 zfront, sint32 item_height, sint16 item_room, sint32 *shift);
void lara_as_climbstnc(ITEM_INFO *item, COLL_INFO *coll);
void lara_col_climbstnc(ITEM_INFO *item, COLL_INFO *coll);
sint32 LaraCheckForLetGo(ITEM_INFO *item, COLL_INFO *coll);
sint32 LaraTestClimbUpPos(ITEM_INFO *item, sint32 front, sint32 right, sint32 *shift, sint32 *ledge);

#endif
