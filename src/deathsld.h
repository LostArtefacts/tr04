#ifndef DEATHSLD_H
#define DEATHSLD_H

#include "items.h"
#include "collide.h"

extern sint16 DeathSlideBounds[12];
extern PHD_VECTOR DeathSlidePosition;

void InitialiseDeathSlide(sint16 item_number);
void DeathSlideCollision(sint16 item_number, ITEM_INFO *laraitem, COLL_INFO *coll);

#endif
