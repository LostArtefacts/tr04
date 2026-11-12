#include "demigod.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "specific/function_stubs.h"

void TriggerDemigodMissile(PHD_3DPOS *pos, sint16 room_number, sint16 type)
{
    FX_INFO *fx;
    sint16 fx_number;

    fx_number = CreateEffect(room_number);
    if (fx_number != NO_ITEM) {
        fx = &effects[fx_number];
        fx->pos.x_pos = pos->x_pos;
        fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
        fx->pos.z_pos = pos->z_pos;
        fx->pos.x_rot = pos->x_rot;
        if (type >= 4)
            fx->pos.y_rot = pos->y_rot + (GetRandomControl() & 0x7FF) - 1024;
        else
            fx->pos.y_rot = pos->y_rot;
        fx->pos.z_rot = 0;
        fx->room_number = room_number;
        fx->counter = (sint16)(2 * GetRandomControl() + 0x8000);
        fx->speed = (GetRandomControl() & 0x1F) + 96;
        fx->flag1 = type;
        fx->object_number = BUBBLES;
        if (type >= 4)
            type--;
        fx->frame_number = objects[BUBBLES].mesh_index + type * 2;
    }
}

void TriggerDemigodMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv)
{
    SPARKS *sptr;
    sint32 dx, dz, size;

    dx = lara_item->pos.x_pos - effects[fx_number].pos.x_pos;
    dz = lara_item->pos.z_pos - effects[fx_number].pos.z_pos;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    if (effects[fx_number].flag1 == 3 || effects[fx_number].flag1 == 4) {
        sptr->sR = 0;
        sptr->sB = (GetRandomControl() & 0x7F) + 32;
        sptr->sG = sptr->sB + 64;
        sptr->dR = 0;
        sptr->dG = (GetRandomControl() & 0x7F) + 32;
        sptr->dB = sptr->dG + 64;
    } else {
        sptr->sR = (GetRandomControl() & 0x7F) + 32;
        sptr->sG = sptr->sR - (GetRandomControl() & 0x1F);
        sptr->sB = 0;
        sptr->dR = (GetRandomControl() & 0x7F) + 32;
        sptr->dG = sptr->dR - (GetRandomControl() & 0x1F);
        sptr->dB = 0;
    }
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 16;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0xF) - 8;
    sptr->y = 0;
    sptr->z = (GetRandomControl() & 0xF) - 8;
    sptr->Xvel = (sint16)xv;
    sptr->Yvel = (sint16)yv;
    sptr->Zvel = (sint16)zv;
    sptr->Friction = 68;
    sptr->Flags = 602;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -32 - (GetRandomControl() & 0x1F);
    else
        sptr->RotAdd = (GetRandomControl() & 0x1F) + 32;
    sptr->Gravity = 0;
    sptr->MaxYvel = 0;
    sptr->FxObj = (uint8)fx_number;
    sptr->Scalar = 2;
    size = (GetRandomControl() & 7) + 64;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 5;
}

void InitialiseDemigod(sint16 item_number)
{
    ITEM_INFO *item, *item2;
    sint16 i;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[item->object_number].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->goal_anim_state = 0;
    item->current_anim_state = 0;
    item2 = items;
    for (i = 0; i < level_items; i++, item2++) {
        if (item != item2 && item2->object_number == DEMIGOD3 && !item2->item_flags[0]) {
            item->item_flags[0] = i;
            return;
        }
    }
}
