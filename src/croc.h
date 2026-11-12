#ifndef CROC_H
#define CROC_H

#include "items.h"

typedef struct {
    PHD_3DPOS pos;
    sint16 room_number;
    sint16 speed;
    sint16 Counter;
    sint16 LaraTarget;
    sint8 XTarget;
    sint8 ZTarget;
    uint8 On;
    uint8 flags;
} LOCUST_STRUCT;

sint32 GetFreeLocust(void);
void InitialiseCroc(sint16 item_number);
void TriggerLocust(ITEM_INFO *item);
void InitialiseLocustEmitter(sint16 item_number);
void ControlLocustEmitter(sint16 item_number);
sint32 GetFreeLocust(void);
void InitialiseCrocgod(sint16 item_number);
void DrawLocusts(void);

extern LOCUST_STRUCT Locusts[64];
extern sint32 next_locust;

#endif
