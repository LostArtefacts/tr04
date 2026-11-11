#include "effect2.h"
#include "control.h"
#include "objnums.h"
#include "lara.h"
#include "anim.h"
#include "tomb4fx.h"
#include "effects.h"
#include "specific/3dmath.h"
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

sint32 GetFreeSpark(void)
{
    SPARKS *sptr;
    sint32 i, free, min_life;

    free = next_spark;
    sptr = &spark[next_spark];
    for (i = 0; i < 256; i++) {
        if (sptr->On) {
            if (free == 255) {
                sptr = &spark[0];
                free = 0;
            } else {
                free++;
                sptr++;
            }
        } else {
            next_spark = (free + 1) & 0xFF;
            spark[free].extras = 0;
            spark[free].Dynamic = -1;
            spark[free].Def = (uint8)objects[DEFAULT_SPRITES].mesh_index;
            return free;
        }
    }
    free = 0;
    min_life = 4095;
    sptr = &spark[0];
    for (i = 0; i < 256; sptr++, i++) {
        if (sptr->Life < min_life && sptr->Dynamic == -1 && !(sptr->Flags & 0x20)) {
            free = i;
            min_life = sptr->Life;
        }
    }
    next_spark = (free + 1) & 0xFF;
    spark[free].extras = 0;
    spark[free].Dynamic = -1;
    spark[free].Def = (uint8)objects[DEFAULT_SPRITES].mesh_index;
    return free;
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

void TriggerFireFlame(sint32 x, sint32 y, sint32 z, sint32 body_part, sint32 type)
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
    if (type != 2) {
        if (type == 254) {
            sptr->sG = 255;
            sptr->sB = 48 + (GetRandomControl() & 31);
            sptr->sR = 48;
            sptr->dG = 192 + (GetRandomControl() & 63);
            sptr->dB = 128 + (GetRandomControl() & 63);
            sptr->dR = 32;
        } else {
            sptr->sR = 255;
            sptr->sG = 48 + (GetRandomControl() & 31);
            sptr->sB = 48;
        }
    } else {
        sptr->sR = 48 + (GetRandomControl() & 31);
        sptr->sG = sptr->sR;
        sptr->sB = 192 + (GetRandomControl() & 63);
    }
    if (type != 254) {
        sptr->dR = 192 + (GetRandomControl() & 63);
        sptr->dG = 128 + (GetRandomControl() & 63);
        sptr->dB = 32;
    }
    if (body_part != -1) {
        sptr->ColFadeSpeed = 8 + (GetRandomControl() & 3);
        sptr->FadeToBlack = 16;
        sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 28;
    } else {
        if (type != 2 && type != 255 && type != 254) {
            sptr->ColFadeSpeed = 20 + (GetRandomControl() & 3);
            sptr->FadeToBlack = 8;
            sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 40;
        } else {
            sptr->ColFadeSpeed = 5 + (GetRandomControl() & 3);
            sptr->FadeToBlack = 6;
            sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 16 + ((type >= 254) ? 8 : 0);
        }
    }
    sptr->TransType = 2;
    if (body_part != -1) {
        sptr->x = (GetRandomControl() & 31) - 16;
        sptr->y = 0;
        sptr->z = (GetRandomControl() & 31) - 16;
    } else {
        if (type == 0 || type == 1) {
            sptr->x = x + ((GetRandomControl() & 31) - 16);
            sptr->y = y;
            sptr->z = z + ((GetRandomControl() & 31) - 16);
        } else if (type >= 254) {
            sptr->x = x + ((GetRandomControl() & 63) - 32);
            sptr->y = y;
            sptr->z = z + ((GetRandomControl() & 63) - 32);
        } else {
            sptr->x = x + ((GetRandomControl() & 15) - 8);
            sptr->y = y;
            sptr->z = z + ((GetRandomControl() & 15) - 8);
        }
    }
    if (type != 2) {
        sptr->Xvel = (GetRandomControl() & 255) - 128;
        sptr->Yvel = -(GetRandomControl() & 15) - 16;
        sptr->Zvel = (GetRandomControl() & 255) - 128;
        if (type == 1)
            sptr->Friction = 3 | (3 << 4);
        else
            sptr->Friction = 5;
    } else {
        sptr->Xvel = (GetRandomControl() & 31) - 16;
        sptr->Yvel = -(GetRandomControl() & 511) - 1024;
        sptr->Zvel = (GetRandomControl() & 31) - 16;
        sptr->Friction = 4 | (4 << 4);
    }
    if (GetRandomControl() & 1) {
        if (body_part != -1) {
            sptr->Flags = 2 | 8 | 16 | 64 | 512;
            sptr->FxObj = body_part;
            sptr->Gravity = -(GetRandomControl() & 63) - 32;
            sptr->MaxYvel = -(GetRandomControl() & 7) - 24;
        } else {
            sptr->Gravity = -(GetRandomControl() & 31) - 16;
            sptr->MaxYvel = -(GetRandomControl() & 7) - 16;
            sptr->Flags = 2 | 8 | 16 | 512;
        }
        sptr->RotAng = GetRandomControl() & 4095;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -(GetRandomControl() & 15) - 16;
        else
            sptr->RotAdd = (GetRandomControl() & 15) + 16;
    } else {
        if (body_part != -1) {
            sptr->Flags = 2 | 8 | 64 | 512;
            sptr->FxObj = body_part;
            sptr->Gravity = -(GetRandomControl() & 63) - 32;
            sptr->MaxYvel = -(GetRandomControl() & 7) - 24;
        } else {
            sptr->Flags = 2 | 8 | 512;
            sptr->Gravity = -(GetRandomControl() & 31) - 16;
            sptr->MaxYvel = -(GetRandomControl() & 7) - 16;
        }
    }
    sptr->Scalar = 2;
    if (type == 0)
        size = (GetRandomControl() & 31) + 128;
    else if (type == 1)
        size = (GetRandomControl() & 31) + 64;
    else if (type >= 254)
        size = (GetRandomControl() & 15) + 48;
    else {
        sptr->Gravity = sptr->MaxYvel = 0;
        size = (GetRandomControl() & 31) + 32;
    }
    sptr->Size = sptr->sSize = size;
    if (type != 2) {
        sptr->dSize = size >> 4;
        if (type == 7) {
            sptr->ColFadeSpeed >>= 2;
            sptr->FadeToBlack >>= 2;
            sptr->Life >>= 2;
            sptr->sLife = sptr->Life;
        }
    } else
        sptr->dSize = size >> 2;
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

void TriggerWaterfallMist(sint32 x, sint32 y, sint32 z, sint32 angle)
{
    sint32 xsize, zsize, tangle, dist, lp;
    sint32 dist_tab[4] = {512 + 64, 171 + 32, -171 - 32, -512 - 64};
    SPARKS *sptr;

    for (lp = 0; lp < 4; lp++) {
        sptr = &spark[GetFreeSpark()];
        sptr->On = 1;
        sptr->sR = 128;
        sptr->sG = 128;
        sptr->sB = 128;
        sptr->dR = 192;
        sptr->dG = 192;
        sptr->dB = 192;
        sptr->ColFadeSpeed = 2;
        sptr->FadeToBlack = 4;
        sptr->sLife = sptr->Life = 6 + (GetRandomControl() & 3);
        sptr->TransType = 2;
        dist = dist_tab[lp] + (GetRandomControl() & 31) - 16;
        tangle = (angle + 1024) & 4095;
        xsize = (rcossin_tbl[tangle << 1] * dist) >> 12;
        zsize = (rcossin_tbl[(tangle << 1) + 1] * dist) >> 12;
        sptr->x = x + xsize + ((GetRandomControl() & 15) - 8);
        sptr->y = y + ((GetRandomControl() & 15) - 8);
        sptr->z = z + zsize + ((GetRandomControl() & 15) - 8);
        sptr->Xvel = rcossin_tbl[angle << 1] >> 12;
        sptr->Zvel = rcossin_tbl[(angle << 1) + 1] >> 12;
        sptr->Yvel = 0;
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
        sptr->Scalar = 6;
        sptr->Gravity = 0;
        sptr->MaxYvel = 0;
        xsize = (GetRandomControl() & 7) + 12;
        sptr->Size = sptr->sSize = xsize >> 1;
        sptr->dSize = xsize;
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

void KillEverything(void)
{
    KillEverythingFlag = 0;
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

void TriggerDynamic(sint32 x, sint32 y, sint32 z, sint32 falloff, sint32 r, sint32 g, sint32 b)
{
    DYNAMIC *dl;

    if (number_dynamics == MAX_DYNAMICS || !falloff)
        return;
    dl = &dynamics[number_dynamics];
    dl->on = 1;
    dl->x = x;
    dl->y = y;
    dl->z = z;
    dl->falloff = (uint16)(falloff << 8);
    if (falloff >= 8) {
        dl->r = (uint8)r;
        dl->g = (uint8)g;
        dl->b = (uint8)b;
    } else {
        dl->r = (uint8)((r * falloff) >> 3);
        dl->g = (uint8)((g * falloff) >> 3);
        dl->b = (uint8)((b * falloff) >> 3);
    }
    dl->FalloffScale = 0x200000 / (falloff << 8);
    number_dynamics++;
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
