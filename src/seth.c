#include "seth.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void TriggerSethMissile(PHD_3DPOS *pos, sint16 room_number, sint16 type)
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
        fx->pos.y_rot = pos->y_rot;
        fx->pos.z_rot = 0;
        fx->room_number = room_number;
        fx->counter = (sint16)(2 * GetRandomControl() + 0x8000);
        fx->speed = (GetRandomControl() & 0x1F) - (type == 1 ? 64 : 0) + 96;
        fx->flag1 = type;
        fx->object_number = BUBBLES;
        fx->frame_number = objects[BUBBLES].mesh_index + 2 * type;
    }
}

void TriggerSethSparks(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv)
{
    SPARKS *sptr;
    sint32 dx, dz;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = 0;
    sptr->sB = 0;
    sptr->dR = 64;
    sptr->dG = (GetRandomControl() & 0x7F) + 64;
    sptr->dB = sptr->dG + 32;
    sptr->sLife = sptr->Life = 16;
    sptr->ColFadeSpeed = 4;
    sptr->TransType = 2;
    sptr->FadeToBlack = 4;
    sptr->x = x;
    sptr->y = y;
    sptr->z = z;
    sptr->Xvel = xv;
    sptr->Yvel = yv;
    sptr->Zvel = zv;
    sptr->Friction = 34;
    sptr->Scalar = 1;
    sptr->Size = sptr->sSize = (GetRandomControl() & 3) + 4;
    sptr->dSize = (GetRandomControl() & 1) + 1;
    sptr->MaxYvel = 0;
    sptr->Gravity = 0;
    sptr->Flags = 0;
}
