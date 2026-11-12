#include "skeleton.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "effects.h"
#include "objnums.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void TriggerRiseEffect(ITEM_INFO *item)
{
    FX_INFO *fx;
    FLOOR_INFO *floor;
    SPARKS *sptr;
    sint16 fx_number, room_number;

    fx_number = CreateEffect(item->room_number);
    if (fx_number == NO_ITEM)
        return;
    fx = &effects[fx_number];
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    fx->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) - 48;
    fx->pos.x_pos = (GetRandomControl() & 0xFF) + item->pos.x_pos - 128;
    fx->pos.z_pos = (GetRandomControl() & 0xFF) + item->pos.z_pos - 128;
    fx->room_number = item->room_number;
    fx->pos.y_rot = (sint16)(GetRandomControl() << 1);
    fx->speed = (sint16)(GetRandomControl() >> 11);
    fx->fallspeed = -(sint16)(GetRandomControl() >> 10);
    fx->frame_number = objects[AHMET_MIP].mesh_index;
    fx->object_number = BODY_PART;
    fx->shade = 0x4210;
    fx->flag2 = 1537;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = 0;
    sptr->sB = 0;
    sptr->dR = 100;
    sptr->dG = 60;
    sptr->dB = 30;
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 16;
    sptr->x = fx->pos.x_pos;
    sptr->y = fx->pos.y_pos;
    sptr->z = fx->pos.z_pos;
    sptr->Xvel = phd_sin(fx->pos.y_rot) >> 2;
    sptr->Yvel = 0;
    sptr->Zvel = phd_cos(fx->pos.y_rot) >> 2;
    sptr->TransType = 2;
    sptr->Friction = 68;
    sptr->Flags = 26;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
    else
        sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    sptr->Gravity = -4 - (GetRandomControl() & 3);
    sptr->MaxYvel = -4 - (GetRandomControl() & 3);
    sptr->Scalar = 3;
    sptr->Size = sptr->sSize = (GetRandomControl() & 0xF) + 8;
    sptr->dSize = sptr->Size << 2;
}

void InitialiseSkeleton(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (!item->trigger_flags) {
        item->anim_number = objects[SKELETON].anim_index;
        item->current_anim_state = item->goal_anim_state = 0;
    } else if (item->trigger_flags == 1) {
        item->anim_number = objects[SKELETON].anim_index + 37;
        item->current_anim_state = item->goal_anim_state = 20;
    } else if (item->trigger_flags == 2) {
        item->anim_number = objects[SKELETON].anim_index + 34;
        item->current_anim_state = item->goal_anim_state = 19;
    } else if (item->trigger_flags == 3) {
        item->anim_number = objects[SKELETON].anim_index + 46;
        item->current_anim_state = item->goal_anim_state = 25;
        item->status += ITEM_ACTIVE;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}
