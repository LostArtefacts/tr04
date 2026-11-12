#ifndef CROC_H
#define CROC_H

#include "box.h"
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
void TriggerCrocgodMissile(PHD_3DPOS *pos, sint16 room_number, sint16 num);
void TriggerCrocgodMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv);

extern LOCUST_STRUCT Locusts[64];
extern sint32 next_locust;
extern BITE_INFO croc_bite;
void CrocControl(sint16 item_number);
void CrocgodControl(sint16 item_number);

#endif
