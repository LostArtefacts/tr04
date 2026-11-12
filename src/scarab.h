#ifndef SCARAB_H
#define SCARAB_H

#include "items.h"

typedef struct {
    PHD_3DPOS pos;
    sint16 room_number;
    sint16 speed;
    sint16 fallspeed;
    uint8 On;
    uint8 flags;
} SCARAB_STRUCT;

void InitialiseScarab(sint16 item_number);
void ClearScarabs(void);
sint32 GetFreeScarab(void);
void InitialiseScarabGenerator(sint16 item_number);
void TriggerScarab(sint16 item_number);
void DrawScarabs(void);

extern SCARAB_STRUCT Scarabs[128];
extern sint32 next_scarab;

#endif
