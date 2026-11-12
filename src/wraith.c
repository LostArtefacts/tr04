#include "wraith.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "specific/function_stubs.h"

void InitialiseWraith(sint16 item_number)
{
    ITEM_INFO *item;
    WRAITH_STRUCT *data;
    sint32 i;

    item = &items[item_number];
    data = (WRAITH_STRUCT *)game_malloc(8 * sizeof(WRAITH_STRUCT));
    item->data = data;
    item->item_flags[0] = 0;
    item->hit_points = 0;
    item->speed = WraithSpeed;
    for (i = 0; i < 8; i++) {
        data->pos.x = item->pos.x_pos;
        data->pos.y = item->pos.y_pos;
        data->pos.z = item->pos.z_pos;
        data->zv = 0;
        data->yv = 0;
        data->xv = 0;
        data->b = 0;
        data->g = 0;
        data->r = 0;
        data++;
    }
}

void TriggerWraithFlame(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv, sint32 objnum)
{
    SPARKS *sptr;
    sint32 size;
    uint8 col;

    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    if (objnum == WRAITH1) {
        sptr->sR = (GetRandomControl() & 0x1F) + 128;
        sptr->sG = (GetRandomControl() & 0x1F) + 48;
        sptr->sB = 24;
        sptr->dR = (GetRandomControl() & 0x1F) + 128;
        sptr->dG = (GetRandomControl() & 0x1F) + 64;
        sptr->dB = 24;
    } else if (objnum == WRAITH2) {
        sptr->sB = (GetRandomControl() & 0x1F) + 128;
        sptr->sG = (GetRandomControl() & 0x1F) + 48;
        sptr->sR = 24;
        sptr->dB = (GetRandomControl() & 0x1F) + 128;
        sptr->dG = (GetRandomControl() & 0x1F) + 64;
        sptr->dR = 24;
    } else {
        col = (GetRandomControl() & 0xF) + 64;
        sptr->sR = sptr->sG = sptr->sB = sptr->dR = sptr->dG = col;
        sptr->dB = (GetRandomControl() & 0xF) + sptr->sB;
    }
    sptr->ColFadeSpeed = 4;
    sptr->FadeToBlack = 7;
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 12;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0x1F) + x - 16;
    sptr->y = y;
    sptr->z = (GetRandomControl() & 0x1F) + z - 16;
    sptr->Friction = 85;
    sptr->Flags = 522;
    sptr->Xvel = xv;
    sptr->Yvel = yv;
    sptr->Zvel = zv;
    sptr->Gravity = 0;
    sptr->MaxYvel = 0;
    sptr->Scalar = 2;
    size = (GetRandomControl() & 0x1F) + 48;
    sptr->dSize = 2;
    sptr->Size = sptr->sSize = size;
}
