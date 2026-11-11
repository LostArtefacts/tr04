#ifndef FOOTPRNT_H
#define FOOTPRNT_H

#include "items.h"

typedef struct footprint {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 YRot;
    sint16 Active;
} FOOTPRINT;

void AddFootPrint(ITEM_INFO *item);

extern uint8 footsounds[14];
extern FOOTPRINT FootPrint[32];
extern sint32 FootPrintNum;

#endif
