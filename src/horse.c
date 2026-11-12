#include "horse.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "effect2.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseHorseman(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[HORSEMAN].anim_index + 8;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 9;
    item->item_flags[0] = -1;
}

void TriggerHorsemanRicochets(PHD_VECTOR *pos, sint32 yrot, sint32 num)
{
    SPARKS *sptr;
    sint32 random, rot, i;

    for (i = num; i > 0; i--) {
        sptr = &spark[GetFreeSpark()];
        random = GetRandomControl();
        sptr->On = 1;
        sptr->sG = 128;
        sptr->sB = (random & 0xF) + 16;
        sptr->sR = 0;
        sptr->dG = 96;
        sptr->dB = (random >> 4 & 0x1F) + 48;
        sptr->dR = 0;
        sptr->ColFadeSpeed = 2;
        sptr->FadeToBlack = 4;
        sptr->Life = 9;
        sptr->sLife = 9;
        sptr->TransType = 2;
        sptr->x = pos->x;
        sptr->y = pos->y;
        sptr->z = pos->z;
        sptr->Yvel = (random & 0xFFF) - 2048;
        sptr->Gravity = random >> 7 & 0x1F;
        rot = (random >> 3 & 0x7FF) + yrot - 1024 & 0xFFF;
        sptr->Zvel = rcossin_tbl[(rot << 1) + 1] >> 2;
        sptr->Xvel = -rcossin_tbl[rot << 1] >> 2;
        sptr->Friction = 34;
        sptr->Flags = 0;
        sptr->MaxYvel = 0;
    }
    for (i = num; i > 0; i--) {
        sptr = &spark[GetFreeSpark()];
        random = GetRandomControl();
        sptr->On = 1;
        sptr->sG = 128;
        sptr->sR = 0;
        sptr->dG = 96;
        sptr->sB = (random & 0xF) + 16;
        sptr->dR = 0;
        sptr->dB = (random >> 4 & 0x1F) + 48;
        sptr->ColFadeSpeed = 2;
        sptr->FadeToBlack = 4;
        sptr->Life = 9;
        sptr->sLife = 9;
        sptr->TransType = 2;
        sptr->x = pos->x;
        sptr->y = pos->y;
        sptr->z = pos->z;
        sptr->Yvel = (random & 0xFFF) - 2048;
        sptr->Gravity = random >> 7 & 0x1F;
        sptr->RotAng = (sint16)(random >> 3);
        if (random & 1)
            sptr->RotAdd = 240 - (random & 0xF);
        else
            sptr->RotAdd = (random & 0xF) + 16;
        sptr->Scalar = 3;
        sptr->sSize = (random >> 5 & 0x7) + 4;
        sptr->Size = sptr->sSize;
        sptr->dSize = sptr->sSize >> 1;
        rot = (random >> 3 & 0x7FF) + yrot - 1024 & 0xFFF;
        sptr->Zvel = rcossin_tbl[(rot << 1) + 1] >> 2;
        sptr->Xvel = -rcossin_tbl[rot << 1] >> 2;
        sptr->Friction = 34;
        sptr->Flags = 26;
        sptr->MaxYvel = 0;
    }
}

void InitialiseHorse(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->anim_number = objects[HORSE].anim_index + 2;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
