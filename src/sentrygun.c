#include "sentrygun.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "sphere.h"
#include "specific/function_stubs.h"

void TriggerAutogunFlamethrower(ITEM_INFO *item)
{
    SPARKS *sptr;
    PHD_VECTOR pos;
    PHD_VECTOR vel;
    sint32 v, i;

    for (i = 0; i < 3; i++) {
        sptr = &spark[GetFreeSpark()];
        sptr->On = 1;
        sptr->sB = 255;
        sptr->sR = (GetRandomControl() & 0x1F) + 48;
        sptr->sG = 48;
        sptr->dR = (GetRandomControl() & 0x3F) + 192;
        sptr->dG = (GetRandomControl() & 0x3F) + 128;
        sptr->dB = 32;
        sptr->ColFadeSpeed = 12;
        sptr->FadeToBlack = 8;
        sptr->Life = (GetRandomControl() & 0x1F) + 16;
        sptr->sLife = sptr->Life;
        sptr->TransType = 2;
        pos.x = -140;
        pos.y = -30;
        pos.z = -4;
        GetJointAbsPosition(item, &pos, 7);
        sptr->x = (GetRandomControl() & 0x1F) + pos.x - 16;
        sptr->y = (GetRandomControl() & 0x1F) + pos.y - 16;
        sptr->z = (GetRandomControl() & 0x1F) + pos.z - 16;
        v = (GetRandomControl() & 0x3F) + 192;
        vel.x = -280;
        vel.y = -30;
        vel.z = -4;
        GetJointAbsPosition(item, &vel, 7);
        sptr->Xvel = (sint16)(v * (vel.x - pos.x) / 10);
        sptr->Yvel = (sint16)(v * (vel.y - pos.y) / 10);
        sptr->Zvel = (sint16)(v * (vel.z - pos.z) / 10);
        sptr->Friction = 85;
        sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
        sptr->MaxYvel = 0;
        sptr->Flags = 538;
        if (GlobalCounter & 1) {
            v = 255;
            sptr->Flags = 539;
        }
        sptr->Scalar = 3;
        v = (v * ((GetRandomControl() & 7) + 60)) >> 8;
        sptr->Size = sptr->sSize = (uint8)(v >> 4);
        sptr->dSize = (uint8)v;
    }
}

void InitialiseAutogun(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->item_flags[0] = 0;
    item->item_flags[1] = 768;
    item->item_flags[2] = 0;
}
