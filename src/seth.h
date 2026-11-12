#ifndef SETH_H
#define SETH_H

#include "items.h"

void TriggerSethMissile(PHD_3DPOS *pos, sint16 room_number, sint16 type);
void TriggerSethSparks(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv);
void TriggerSethFlame(sint16 item_number, uint8 NodeNumber, sint32 size);

#endif
