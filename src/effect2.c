#include "effect2.h"
#include "control.h"
#include "objnums.h"
#include "lara.h"
#include "anim.h"
#include "tomb4fx.h"
#include "effects.h"
#include "specific/function_stubs.h"

void DetatchSpark(sint32 num, sint32 type)
{
    sint32 lp;
    SPARKS *sptr;
    FX_INFO *fx;
    ITEM_INFO *item;

    sptr = &spark[0];
    for (lp = 0; lp < 256; lp++) {
        if (sptr->On && (sptr->Flags & type) && sptr->FxObj == num) {
            if (type == 64) {
                fx = &effects[num];
                sptr->x += fx->pos.x_pos;
                sptr->y += fx->pos.y_pos;
                sptr->z += fx->pos.z_pos;
                sptr->Flags &= ~64;
            } else if (type == 128) {
                item = &items[num];
                sptr->x += item->pos.x_pos;
                sptr->y += item->pos.y_pos;
                sptr->z += item->pos.z_pos;
                sptr->Flags &= ~128;
            }
        }
        sptr++;
    }
}

void TriggerExplosionSmokeEnd(sint32 x, sint32 y, sint32 z, sint32 uw)
{
    sint32 size;
    SPARKS *sptr;
    sint32 dx, dz;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    if (uw) {
        sptr->sR = 0;
        sptr->sG = 0;
        sptr->sB = 0;
        sptr->dR = 192;
        sptr->dG = 192;
        sptr->dB = 208;
    } else {
        sptr->sR = 144;
        sptr->sG = 144;
        sptr->sB = 144;
        sptr->dR = 64;
        sptr->dG = 64;
        sptr->dB = 64;
    }
    sptr->ColFadeSpeed = 8;
    sptr->FadeToBlack = 64;
    sptr->sLife = sptr->Life = (GetRandomControl() & 31) + 96;
    if (uw)
        sptr->TransType = 2;
    else
        sptr->TransType = 3;
    sptr->x = x + (GetRandomControl() & 31) - 16;
    sptr->y = y + (GetRandomControl() & 31) - 16;
    sptr->z = z + (GetRandomControl() & 31) - 16;
    sptr->Xvel = ((GetRandomControl() & 4095) - 2048) >> 2;
    sptr->Yvel = (GetRandomControl() & 255) - 128;
    sptr->Zvel = ((GetRandomControl() & 4095) - 2048) >> 2;
    if (uw) {
        sptr->Yvel >>= 4;
        sptr->y += 32;
        sptr->Friction = 4 | (1 << 4);
    } else
        sptr->Friction = 6;
    sptr->Flags = 2 | 8 | 16 | 512;
    sptr->RotAng = GetRandomControl() & 4095;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -(GetRandomControl() & 15) - 16;
    else
        sptr->RotAdd = (GetRandomControl() & 15) + 16;
    sptr->Scalar = 3;
    if (uw)
        sptr->Gravity = sptr->MaxYvel = 0;
    else {
        sptr->Gravity = -(GetRandomControl() & 3) - 3;
        sptr->MaxYvel = -(GetRandomControl() & 3) - 4;
    }
    size = (GetRandomControl() & 31) + 128;
    sptr->Size = sptr->sSize = size >> 2;
    sptr->dSize = size;
}

void TriggerExplosionSmoke(sint32 x, sint32 y, sint32 z, sint32 uw)
{
    sint32 size;
    SPARKS *sptr;
    sint32 dx, dz;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 144;
    sptr->sG = 144;
    sptr->sB = 144;
    sptr->dR = 64;
    sptr->dG = 64;
    sptr->dB = 64;
    sptr->ColFadeSpeed = 2;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 10;
    sptr->TransType = 3;
    sptr->x = x + (GetRandomControl() & 511) - 256;
    sptr->y = y + (GetRandomControl() & 511) - 256;
    sptr->z = z + (GetRandomControl() & 511) - 256;
    sptr->Xvel = ((GetRandomControl() & 4095) - 2048) >> 2;
    sptr->Yvel = (GetRandomControl() & 255) - 128;
    sptr->Zvel = ((GetRandomControl() & 4095) - 2048) >> 2;
    if (uw)
        sptr->Friction = 2;
    else
        sptr->Friction = 6;
    sptr->Flags = 2 | 8 | 16 | 512;
    sptr->RotAng = GetRandomControl() & 4095;
    sptr->RotAdd = (GetRandomControl() & 15) + 16;
    sptr->Scalar = 1;
    sptr->Gravity = -(GetRandomControl() & 3) - 3;
    sptr->MaxYvel = -(GetRandomControl() & 3) - 4;
    size = (GetRandomControl() & 31) + 128;
    sptr->Size = sptr->sSize = size >> 2;
    sptr->dSize = size;
    GetRandomControl();
}

void TriggerRocketSmoke(sint32 x, sint32 y, sint32 z, sint32 col)
{
    SPARKS *sptr;
    sint32 size;

    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = 0;
    sptr->sB = 0;
    sptr->dR = (uint8)(col + 64);
    sptr->dG = (uint8)(col + 64);
    sptr->dB = (uint8)(col + 64);
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
    sptr->FadeToBlack = 12;
    sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 20;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0xF) + x - 8;
    sptr->y = (GetRandomControl() & 0xF) + y - 8;
    sptr->z = (GetRandomControl() & 0xF) + z - 8;
    sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Yvel = -4 - (GetRandomControl() & 3);
    sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Friction = 4;
    if (GetRandomControl() & 1) {
        sptr->Flags = 538;
        sptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
        else
            sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    } else
        sptr->Flags = 522;
    sptr->Scalar = 3;
    sptr->Gravity = -4 - (GetRandomControl() & 3);
    sptr->MaxYvel = -4 - (GetRandomControl() & 3);
    size = (GetRandomControl() & 7) + 32;
    sptr->Size = sptr->sSize = size >> 2;
    sptr->dSize = size;
}

RIPPLE_STRUCT *SetupRipple(sint32 x, sint32 y, sint32 z, sint32 size, sint32 flags)
{
    RIPPLE_STRUCT *ripple;
    sint32 n;

    ripple = ripples;
    for (n = 0; n < 16; n++) {
        if (!(ripple->flags & 1)) {
            ripple->flags = (char)flags | 1;
            ripple->life = (GetRandomControl() & 0xF) + 48;
            ripple->size = (uint8)size;
            ripple->init = 1;
            ripple->x = x;
            ripple->y = y;
            ripple->z = z;
            if (flags & 0x40) {
                ripple->x += (GetRandomControl() & 0x7F) - 64;
                ripple->z += (GetRandomControl() & 0x7F) - 64;
            }
            return ripple;
        }
        ripple++;
    }
    return ripple;
}

void TriggerUnderwaterBlood(sint32 x, sint32 y, sint32 z, sint32 size)
{
    RIPPLE_STRUCT *ripple;
    sint32 n;

    ripple = ripples;
    for (n = 0; n < 16; n++) {
        if (!(ripple->flags & 1)) {
            ripple->flags = 49;
            ripple->life = (GetRandomControl() & 7) - 16;
            ripple->init = 1;
            ripple->size = (uint8)size;
            ripple->x = x + (GetRandomControl() & 0x3F) - 32;
            ripple->y = y;
            ripple->z = z + (GetRandomControl() & 0x3F) - 32;
            return;
        }
        ripple++;
    }
}

void TriggerDartSmoke(sint32 x, sint32 y, sint32 z, sint32 xv, sint32 zv, sint32 hit)
{
    sint32 size;
    SPARKS *sptr;
    sint32 dx, dz;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 16;
    sptr->sG = 8;
    sptr->sB = 4;
    sptr->dR = 64;
    sptr->dG = 48;
    sptr->dB = 32;
    sptr->ColFadeSpeed = 8;
    sptr->FadeToBlack = 4;
    sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 32;
    sptr->TransType = 2;
    sptr->x = x + ((GetRandomControl() & 31) - 16);
    sptr->y = y + ((GetRandomControl() & 31) - 16);
    sptr->z = z + ((GetRandomControl() & 31) - 16);
    if (hit) {
        sptr->Xvel = -xv + ((GetRandomControl() & 255) - 128);
        sptr->Yvel = -(GetRandomControl() & 3) - 4;
        sptr->Zvel = -zv + ((GetRandomControl() & 255) - 128);
    } else {
        if (xv)
            sptr->Xvel = -xv;
        else
            sptr->Xvel = (GetRandomControl() & 255) - 128;
        sptr->Yvel = -(GetRandomControl() & 3) - 4;
        if (zv)
            sptr->Zvel = -zv;
        else
            sptr->Zvel = (GetRandomControl() & 255) - 128;
    }
    sptr->Friction = 3;
    if (GetRandomControl() & 1) {
        sptr->Flags = 2 | 8 | 16 | 512;
        sptr->RotAng = GetRandomControl() & 4095;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -(GetRandomControl() & 15) - 16;
        else
            sptr->RotAdd = (GetRandomControl() & 15) + 16;
    } else
        sptr->Flags = 2 | 8 | 512;
    sptr->Scalar = 1;
    size = (GetRandomControl() & 63) + 72;
    if (hit) {
        size >>= 1;
        sptr->Size = sptr->sSize = size >> 2;
        sptr->Gravity = sptr->MaxYvel = 0;
    } else {
        sptr->Size = sptr->sSize = size >> 4;
        sptr->Gravity = -(GetRandomControl() & 3) - 4;
        sptr->MaxYvel = -(GetRandomControl() & 3) - 4;
    }
    sptr->dSize = size;
}

void KillAllCurrentItems(sint16 item_number)
{
    KillEverythingFlag = 1;
}

void TriggerExplosionBubble(sint32 x, sint32 y, sint32 z, sint16 roomnum)
{
    SPARKS *sptr;
    sint32 dx, dz, size;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 128;
    sptr->sG = 64;
    sptr->sB = 0;
    sptr->dR = 128;
    sptr->dG = 128;
    sptr->dB = 128;
    sptr->ColFadeSpeed = 8;
    sptr->FadeToBlack = 12;
    sptr->sLife = sptr->Life = 24;
    sptr->TransType = 2;
    sptr->x = x;
    sptr->y = y;
    sptr->z = z;
    sptr->Xvel = 0;
    sptr->Yvel = 0;
    sptr->Zvel = 0;
    sptr->Friction = 0;
    sptr->Flags = 2 | 8 | 2048;
    sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 13;
    sptr->Scalar = 3;
    sptr->Gravity = 0;
    sptr->MaxYvel = 0;
    size = (GetRandomControl() & 7) + 63;
    sptr->Size = sptr->sSize = size >> 1;
    sptr->dSize = size << 1;
    for (dx = 0; dx < 7; dx++) {
        PHD_3DPOS pos;
        pos.x_pos = x + (GetRandomControl() & 511) - 256;
        pos.y_pos = y + (GetRandomControl() & 127) - 64;
        pos.z_pos = z + (GetRandomControl() & 511) - 256;
        CreateBubble(&pos, roomnum, 6, 15);
    }
}

void ClearDynamics(void)
{
    sint32 lp;

    number_dynamics = 0;
    for (lp = 0; lp < MAX_DYNAMICS; lp++)
        dynamics[lp].on = 0;
}

void ControlColouredLights(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 objnum;
    uint8 colours[5][3] = {
        { 255, 0, 0 },
        { 0, 255, 0 },
        { 0, 0, 255 },
        { 255, 192, 0 },
        { 224, 224, 255 }
    };

    item = &items[item_number];
    if (TriggerActive(item)) {
        objnum = item->object_number - RED_LIGHT;
        TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 24, colours[objnum][0], colours[objnum][1], colours[objnum][2]);
    }
}
