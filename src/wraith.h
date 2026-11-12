#ifndef WRAITH_H
#define WRAITH_H

#include "items.h"

typedef struct wraith_struct {
    PHD_VECTOR pos;
    sint16 xv;
    sint16 yv;
    sint16 zv;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 pad[3];
} WRAITH_STRUCT;

extern sint16 WraithSpeed;

void InitialiseWraith(sint16 item_number);
void TriggerWraithFlame(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv, sint32 objnum);
void TriggerWraithEffect(sint32 x, sint32 y, sint32 z, sint16 vel, sint32 objnum);
void WraithControl(sint16 item_number);

#endif
