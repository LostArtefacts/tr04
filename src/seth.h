#ifndef SETH_H
#define SETH_H

#include "box.h"
#include "items.h"

extern BITE_INFO seth_left_hand;
extern BITE_INFO seth_right_hand;

void TriggerSethMissile(PHD_3DPOS *pos, sint16 room_number, sint16 type);
void TriggerSethSparks(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv);
void TriggerSethFlame(sint16 item_number, uint8 NodeNumber, sint32 size);
void TriggerSethMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv);
void DoSethEffects(sint16 item_number);
void InitialiseSeth(sint16 item_number);
void SethControl(sint16 item_number);

#endif
