#ifndef AHMET_H
#define AHMET_H

#include "items.h"
#include "collide.h"

void InitialiseAhmet(sint16 item_number);
void ExplodeAhmet(ITEM_INFO *item);
sint32 ReTriggerAhmet(sint16 item_number);
void ScalesControl(sint16 item_number);
void ScalesCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);

extern sint16 ScalesBounds[12];

#endif
