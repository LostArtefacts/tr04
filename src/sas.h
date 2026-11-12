#ifndef SAS_H
#define SAS_H

#include "box.h"
#include "collide.h"

void InitialiseSas(sint16 item_number);
void InitialiseInjuredSas(sint16 item_number);
void InjuredSasControl(sint16 item_number);
void DragSASCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);

extern BITE_INFO sas_fire;
extern sint16 DragSASBounds[12];
extern PHD_VECTOR DragSASPos;

#endif
