#include <stdlib.h>
#include "effect2.h"
#include "control.h"
#include "objnums.h"
#include "lara.h"
#include "anim.h"
#include "tomb4fx.h"
#include "gameflow.h"
#include "effects.h"
#include "missile.h"
#include "seth.h"
#include "demigod.h"
#include "harpy.h"
#include "croc.h"
#include "draw.h"
#include "traps.h"
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

void UpdateSparks(void)
{
    SPARKS *sptr;
    SP_DYNAMIC *dynamic;
    sint16 *bounds;
    sint32 i, j, fade, uw, rad, rnd, x, y, z, r, g, b, falloff;

    bounds = GetBoundsAccurate(lara_item);
    DeadlyBounds[0] = lara_item->pos.x_pos + bounds[0];
    DeadlyBounds[1] = lara_item->pos.x_pos + bounds[1];
    DeadlyBounds[2] = lara_item->pos.y_pos + bounds[2];
    DeadlyBounds[3] = lara_item->pos.y_pos + bounds[3];
    DeadlyBounds[4] = lara_item->pos.z_pos + bounds[4];
    DeadlyBounds[5] = lara_item->pos.z_pos + bounds[5];
    sptr = spark;
    for (i = 256; i > 0; i--, sptr++) {
        if (!sptr->On)
            continue;
        sptr->Life--;
        if (!sptr->Life) {
            if (sptr->Dynamic != -1)
                spark_dynamics[sptr->Dynamic].On = 0;
            sptr->On = 0;
            continue;
        }
        if (sptr->sLife - sptr->Life < sptr->ColFadeSpeed) {
            fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
            sptr->R = (uint8)(sptr->sR + ((fade * (sptr->dR - sptr->sR)) >> 16));
            sptr->G = (uint8)(sptr->sG + ((fade * (sptr->dG - sptr->sG)) >> 16));
            sptr->B = (uint8)(sptr->sB + ((fade * (sptr->dB - sptr->sB)) >> 16));
        } else if (sptr->Life < sptr->FadeToBlack) {
            fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
            sptr->R = (uint8)((sptr->dR * fade) >> 16);
            sptr->G = (uint8)((sptr->dG * fade) >> 16);
            sptr->B = (uint8)((sptr->dB * fade) >> 16);
            if (sptr->R < 8 && sptr->G < 8 && sptr->B < 8) {
                sptr->On = 0;
                continue;
            }
        } else {
            sptr->R = sptr->dR;
            sptr->G = sptr->dG;
            sptr->B = sptr->dB;
        }
        if (sptr->Life == sptr->FadeToBlack && (sptr->Flags & 0x800))
            sptr->dSize >>= 2;
        if (sptr->Flags & 0x10)
            sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;
        if (sptr->sLife - sptr->Life == sptr->extras >> 3 && (sptr->extras & 7)) {
            if (sptr->Flags & 0x800)
                uw = 1;
            else if (sptr->Flags & 0x2000)
                uw = 2;
            else
                uw = 0;
            for (j = 0; j < (sptr->extras & 7); j++) {
                TriggerExplosionSparks(sptr->x, sptr->y, sptr->z, (sptr->extras & 7) - 1, sptr->Dynamic, uw, sptr->RoomNumber);
                sptr->Dynamic = -1;
            }
            if (sptr->Flags & 0x800)
                TriggerExplosionBubble(sptr->x, sptr->y, sptr->z, sptr->RoomNumber);
            sptr->extras = 0;
        }
        fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
        sptr->Yvel += sptr->Gravity;
        if (sptr->MaxYvel) {
            if (sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
                sptr->Yvel = sptr->MaxYvel << 5;
        }
        if (sptr->Friction & 0xF) {
            sptr->Xvel -= sptr->Xvel >> (sptr->Friction & 0xF);
            sptr->Zvel -= sptr->Zvel >> (sptr->Friction & 0xF);
        }
        if (sptr->Friction & 0xF0)
            sptr->Yvel -= sptr->Yvel >> (sptr->Friction >> 4);
        sptr->x += sptr->Xvel >> 5;
        sptr->y += sptr->Yvel >> 5;
        sptr->z += sptr->Zvel >> 5;
        if (sptr->Flags & 0x100) {
            sptr->x += SmokeWindX >> 1;
            sptr->z += SmokeWindZ >> 1;
        }
        sptr->Size = (uint8)(sptr->sSize + ((fade * (sptr->dSize - sptr->sSize)) >> 16));
        if ((sptr->Flags & 1) && !lara.burn || (sptr->Flags & 0x400)) {
            rad = sptr->Size << sptr->Scalar >> 1;
            if (sptr->x + rad > DeadlyBounds[0] && sptr->x - rad < DeadlyBounds[1] && sptr->y + rad > DeadlyBounds[2] && sptr->y - rad < DeadlyBounds[3] && sptr->z + rad > DeadlyBounds[4] && sptr->z - rad < DeadlyBounds[5]) {
                if (sptr->Flags & 1)
                    LaraBurn();
                else
                    lara_item->hit_points -= 2;
            }
        }
    }
    sptr = spark;
    for (i = 256; i > 0; i--, sptr++) {
        if (!sptr->On || sptr->Dynamic == -1)
            continue;
        dynamic = &spark_dynamics[sptr->Dynamic];
        if (dynamic->Flags & 3) {
            rnd = GetRandomControl();
            x = sptr->x + 16 * (rnd & 0xF);
            y = sptr->y + (rnd & 0xF0);
            z = sptr->z + ((rnd >> 4) & 0xF0);
            falloff = sptr->sLife - sptr->Life - 1;
            if (falloff < 2) {
                if (dynamic->Falloff < 28)
                    dynamic->Falloff += 6;
                r = 255 - (falloff << 3) - (rnd & 0x1F);
                g = 255 - (falloff << 4) - (rnd & 0x1F);
                b = 255 - (falloff << 6) - (rnd & 0x1F);
            } else if (falloff < 4) {
                if (dynamic->Falloff < 28)
                    dynamic->Falloff += 6;
                r = 255 - (falloff << 3) - (rnd & 0x1F);
                g = 128 - (falloff << 3);
                b = 128 - (falloff << 5);
                if (b < 0)
                    b = 0;
            } else {
                if (dynamic->Falloff > 0)
                    dynamic->Falloff--;
                r = 224 + (rnd & 0x1F);
                g = 128 + ((rnd >> 4) & 0x1F);
                b = (rnd >> 8) & 0x3F;
            }
            if (sptr->Flags & 0x2000)
                TriggerDynamic(x, y, z, dynamic->Falloff > 31 ? 31 : dynamic->Falloff, b, r, g);
            else
                TriggerDynamic(x, y, z, dynamic->Falloff > 31 ? 31 : dynamic->Falloff, r, g, b);
        }
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

void UpdateSplashes(void)
{
    SPLASH_STRUCT *splash;
    RIPPLE_STRUCT *ripple;
    sint32 i;

    splash = splashes;
    for (i = 4; i > 0; i--, splash++) {
        if (!(splash->flags & 1))
            continue;
        splash->InnerRad += splash->InnerRadVel >> 5;
        splash->InnerSize += splash->InnerRadVel >> 6;
        splash->InnerRadVel -= splash->InnerRadVel >> 6;
        splash->MiddleRad += splash->MiddleRadVel >> 5;
        splash->MiddleSize += splash->MiddleRadVel >> 6;
        splash->MiddleRadVel -= splash->MiddleRadVel >> 6;
        splash->OuterRad += splash->OuterRadVel >> 5;
        splash->OuterSize += splash->OuterRadVel >> 6;
        splash->OuterRadVel -= splash->OuterRadVel >> 6;
        splash->InnerY += splash->InnerYVel >> 4;
        splash->InnerYVel += 0x400;
        if (splash->InnerYVel > 0x4000)
            splash->InnerYVel = 0x4000;
        if (splash->InnerY >= 0) {
            splash->InnerY = 0;
            splash->flags |= 4;
            splash->life -= 2;
            if (!splash->life)
                splash->flags = 0;
        } else if (splash->InnerY < -0x7000) {
            splash->InnerY = -0x7000;
        }
        splash->MiddleY += splash->MiddleYVel >> 4;
        splash->MiddleYVel += 0x380;
        if (splash->MiddleYVel > 0x4000)
            splash->MiddleYVel = 0x4000;
        if (splash->MiddleY >= 0) {
            splash->MiddleY = 0;
            splash->flags |= 8;
        } else if (splash->MiddleY < -0x7000) {
            splash->MiddleY = -0x7000;
        }
    }
    ripple = ripples;
    for (i = 16; i > 0; i--, ripple++) {
        if (!(ripple->flags & 1))
            continue;
        if (ripple->size < 252) {
            if (ripple->flags & 2)
                ripple->size += 2;
            else
                ripple->size += 4;
        }
        if (!ripple->init) {
            ripple->life -= 3;
            if (ripple->life > 250)
                ripple->flags = 0;
        } else if (ripple->init < ripple->life) {
            if (ripple->flags & 2)
                ripple->init += 8;
            else
                ripple->init += 4;
            if (ripple->init >= ripple->life)
                ripple->init = 0;
        }
    }
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

void TriggerDynamic_MIRROR(sint32 x, sint32 y, sint32 z, sint32 falloff, sint32 r, sint32 g, sint32 b)
{
    DYNAMIC *dl;
    sint32 i;

    for (i = 0; i < 2; i++) {
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
        z = 2 * gfMirrorZPlane - z;
    }
}

void ClearDynamics(void)
{
    sint32 lp;

    number_dynamics = 0;
    for (lp = 0; lp < MAX_DYNAMICS; lp++)
        dynamics[lp].on = 0;
}

void ControlSmokeEmitter(sint16 item_number)
{
    ITEM_INFO *item;
    SPARKS *sptr;
    PHD_3DPOS pos;
    sint32 size, dx, dz, normal;

    item = &items[item_number];
    normal = 0;
    if (!TriggerActive(item))
        return;
    if (item->object_number == STEAM_EMITTER && room[item->room_number].flags & ROOM_UNDERWATER) {
        if (item->item_flags[0] || !(GetRandomControl() & 0x1F) || item->trigger_flags == 1) {
            if (!(GetRandomControl() & 3) || item->item_flags[1]) {
                pos.x_pos = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
                pos.y_pos = item->pos.y_pos - (GetRandomControl() & 0x1F) - 16;
                pos.z_pos = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;
                if (item->trigger_flags == 1)
                    CreateBubble(&pos, item->room_number, 15, 15);
                else
                    CreateBubble(&pos, item->room_number, 8, 7);
                if (item->item_flags[0]) {
                    item->item_flags[0]--;
                    if (!item->item_flags[0])
                        item->item_flags[1] = 0;
                }
            }
        } else if (!(GetRandomControl() & 0x1F))
            item->item_flags[0] = (GetRandomControl() & 3) + 4;
        return;
    }
    if (item->object_number == STEAM_EMITTER && item->trigger_flags & 8) {
        if (item->item_flags[0]) {
            item->item_flags[0]--;
            if (!item->item_flags[0])
                item->item_flags[1] = (GetRandomControl() & 0x3F) + 30;
            normal = 1;
            if (item->item_flags[2])
                item->item_flags[2] -= 256;
        } else if (item->item_flags[2] < 4096) {
            item->item_flags[2] += 256;
        }
        if (item->item_flags[2]) {
            dx = lara_item->pos.x_pos - item->pos.x_pos;
            dz = lara_item->pos.z_pos - item->pos.z_pos;
            if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
                return;
            sptr = &spark[GetFreeSpark()];
            sptr->On = 1;
            sptr->sR = 96;
            sptr->sG = 96;
            sptr->sB = 96;
            sptr->dR = 48;
            sptr->dG = 48;
            sptr->dB = 48;
            sptr->ColFadeSpeed = (GetRandomControl() & 3) + 6;
            sptr->FadeToBlack = 6;
            sptr->Life = (GetRandomControl() & 7) + 16;
            sptr->TransType = 2;
            sptr->sLife = sptr->Life;
            sptr->x = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
            sptr->y = (GetRandomControl() & 0x3F) + item->pos.y_pos - 32;
            sptr->z = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;
            size = item->item_flags[2];
            if (size == 4096)
                size = (GetRandomControl() & 0x7FF) + 2048;
            sptr->Xvel = (sint16)(((rcossin_tbl[((item->pos.y_rot - 32768) >> 3) & 0x1FFE] << 2) * size) >> W2V_SHIFT);
            sptr->Yvel = -16 - (GetRandomControl() & 0xF);
            sptr->Zvel = (sint16)(((rcossin_tbl[(((item->pos.y_rot - 32768) >> 3) & 0x1FFE) + 1] << 2) * size) >> W2V_SHIFT);
            sptr->Friction = 4;
            sptr->Flags = 538;
            if (!(GlobalCounter & 3))
                sptr->Flags = 1562;
            sptr->RotAng = GetRandomControl() & 0xFFF;
            if (GetRandomControl() & 1)
                sptr->RotAdd = -8 - (GetRandomControl() & 7);
            else
                sptr->RotAdd = (GetRandomControl() & 7) + 8;
            sptr->Scalar = 2;
            sptr->Gravity = -8 - (GetRandomControl() & 0xF);
            sptr->MaxYvel = -8 - (GetRandomControl() & 7);
            size = (GetRandomControl() & 0x1F) + 128;
            sptr->dSize = (uint8)size;
            sptr->Size = sptr->sSize = (uint8)(size >> 1);
            if (item->item_flags[1])
                item->item_flags[1]--;
            else
                item->item_flags[0] = item->trigger_flags >> 4;
        }
        if (!normal)
            return;
    }
    if (!(wibble & 0xF) && (item->object_number != STEAM_EMITTER || !(wibble & 0x1F))) {
        dx = lara_item->pos.x_pos - item->pos.x_pos;
        dz = lara_item->pos.z_pos - item->pos.z_pos;
        if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
            return;
        sptr = &spark[GetFreeSpark()];
        sptr->On = 1;
        sptr->sR = 0;
        sptr->sG = 0;
        sptr->sB = 0;
        sptr->dR = 64;
        sptr->dG = 64;
        sptr->dB = 64;
        sptr->FadeToBlack = 16;
        sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
        sptr->Life = (GetRandomControl() & 7) + 28;
        sptr->sLife = sptr->Life;
        if (item->object_number == SMOKE_EMITTER_BLACK)
            sptr->TransType = 3;
        else
            sptr->TransType = 2;
        sptr->x = (GetRandomControl() & 0x3F) + item->pos.x_pos - 32;
        sptr->y = (GetRandomControl() & 0x3F) + item->pos.y_pos - 32;
        sptr->z = (GetRandomControl() & 0x3F) + item->pos.z_pos - 32;
        sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
        sptr->Yvel = -16 - (GetRandomControl() & 0xF);
        sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
        sptr->Friction = 3;
        sptr->Flags = 538;
        if (room[item->room_number].flags & ROOM_OUTSIDE)
            sptr->Flags = 794;
        sptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -8 - (GetRandomControl() & 7);
        else
            sptr->RotAdd = (GetRandomControl() & 7) + 8;
        sptr->Scalar = 2;
        sptr->Gravity = -8 - (GetRandomControl() & 0xF);
        sptr->MaxYvel = -8 - (GetRandomControl() & 7);
        size = (GetRandomControl() & 0x1F) + 128;
        sptr->dSize = (uint8)size;
        sptr->sSize = (uint8)(size >> 2);
        sptr->Size = (uint8)(size >> 2);
        if (item->object_number == STEAM_EMITTER) {
            sptr->Gravity >>= 1;
            sptr->Yvel >>= 1;
            sptr->MaxYvel >>= 1;
            sptr->Life += 16;
            sptr->sLife += 16;
            sptr->dR = 32;
            sptr->dG = 32;
            sptr->dB = 32;
        }
    }
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

void ControlEnemyMissile(sint16 fx_number)
{
    FX_INFO *fx;
    FLOOR_INFO *floor;
    sint32 speed, ox, oy, oz, h, c;
    sint16 room_number, max_speed, max_turn;
    sint16 angles[2];

    fx = &effects[fx_number];
    phd_GetVectorAngles(lara_item->pos.x_pos - fx->pos.x_pos, lara_item->pos.y_pos - fx->pos.y_pos - 256, lara_item->pos.z_pos - fx->pos.z_pos, angles);
    if (fx->flag1 == 1) {
        max_turn = 512;
        max_speed = 256;
    } else if (fx->flag1 == 6) {
        if (fx->counter)
            fx->counter--;
        max_turn = 256;
        max_speed = 192;
    } else {
        max_turn = 768;
        max_speed = 192;
    }
    if (fx->speed < max_speed) {
        if (fx->flag1 == 6)
            fx->speed++;
        else
            fx->speed += 3;
        oy = (uint16)angles[0] - (uint16)fx->pos.y_rot;
        if (abs(oy) > 0x8000)
            oy = (uint16)fx->pos.y_rot - (uint16)angles[0];
        ox = (uint16)angles[1] - (uint16)fx->pos.x_rot;
        if (abs(ox) > 0x8000)
            ox = (uint16)fx->pos.x_rot - (uint16)angles[0];
        oy >>= 3;
        ox >>= 3;
        if (oy > max_turn)
            oy = max_turn;
        else if (oy < -max_turn)
            oy = -max_turn;
        if (ox > max_turn)
            ox = max_turn;
        else if (ox < -max_turn)
            ox = -max_turn;
        if (fx->flag1 != 4 && (fx->flag1 != 6 || !fx->counter))
            fx->pos.y_rot += (sint16)oy;
        fx->pos.x_rot += (sint16)ox;
    }
    fx->pos.z_rot += fx->speed << 4;
    if (fx->flag1 == 6)
        fx->pos.z_rot += fx->speed << 4;
    ox = fx->pos.x_pos;
    oy = fx->pos.y_pos;
    oz = fx->pos.z_pos;
    speed = ((rcossin_tbl[((fx->pos.x_rot >> 3) & 0x1FFE) + 1] << 2) * fx->speed) >> W2V_SHIFT;
    fx->pos.x_pos += ((rcossin_tbl[(fx->pos.y_rot >> 3) & 0x1FFE] << 2) * speed) >> W2V_SHIFT;
    fx->pos.y_pos += ((rcossin_tbl[(-fx->pos.x_rot >> 3) & 0x1FFE] << 2) * fx->speed) >> W2V_SHIFT;
    fx->pos.z_pos += ((rcossin_tbl[((fx->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * speed) >> W2V_SHIFT;
    room_number = fx->room_number;
    floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
    h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
    c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
    if (fx->pos.y_pos < h && fx->pos.y_pos > c) {
        if (ItemNearLara(&fx->pos, 200)) {
            lara_item->hit_status = 1;
            if (fx->flag1 != 6)
                ExplodeFX(fx, 0, -32);
            KillEffect(fx_number);
            if (fx->flag1 == 1) {
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xF00030, 64, 0x18008040, 0);
                TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
                LaraBurn();
                lara.BurnGreen = 1;
            } else if (fx->flag1 == 0)
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0x580018, 48, 0x10008040, ((uint8)~(uint8)room[fx->room_number].flags >> 4 & 2) << 16);
            else if (fx->flag1 == 3 || fx->flag1 == 4)
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x10004080, 0x10000);
            else if (fx->flag1 == 5)
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x10806000, 0x20000);
            else if (fx->flag1 == 2)
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x10808000, 0x20000);
            else if (fx->flag1 == 6) {
                TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xF00030, 64, 0x18806000, 0);
                fx->pos.y_pos -= 128;
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xF00030, 48, 0x10807000, 0);
                fx->pos.y_pos += 256;
                TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xF00030, 48, 0x10807000, 0);
                LaraBurn();
            }
        } else {
            if (room_number != fx->room_number)
                EffectNewRoom(fx_number, room_number);
            ox -= fx->pos.x_pos;
            oy -= fx->pos.y_pos;
            oz -= fx->pos.z_pos;
            if (wibble & 4 || fx->flag1 == 1 || fx->flag1 == 5 || fx->flag1 == 2) {
                if (fx->flag1 == 0)
                    TriggerSethMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
                else if (fx->flag1 == 1)
                    TriggerSethMissileFlame(fx_number, ox << 5, oy << 5, oz << 5);
                else if (fx->flag1 >= 3 && fx->flag1 <= 5)
                    TriggerDemigodMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
                else if (fx->flag1 == 2)
                    TriggerHarpyMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
                else if (fx->flag1 == 6)
                    TriggerCrocgodMissileFlame(fx_number, ox << 4, oy << 4, oz << 4);
            }
        }
    } else {
        fx->pos.x_pos = ox;
        fx->pos.y_pos = oy;
        fx->pos.z_pos = oz;
        if (fx->flag1 != 6)
            ExplodeFX(fx, 0, -32);
        if (fx->flag1 == 1) {
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x18008040, ((uint8)~(uint8)room[fx->room_number].flags >> 4 & 2) << 16);
            TriggerExplosionSparks(ox, oy, oz, 3, -2, 2, fx->room_number);
        } else if (fx->flag1 == 0)
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x10008040, 0);
        else if (fx->flag1 == 3 || fx->flag1 == 4)
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x10004080, 0);
        else if (fx->flag1 == 5)
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x10806000, 0);
        else if (fx->flag1 == 2)
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xA00020, 64, 0x10808000, 0);
        else if (fx->flag1 == 6) {
            TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, fx->room_number);
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xF00030, 64, 0x18806000, 0x20000);
            fx->pos.y_pos -= 128;
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xF00030, 48, 0x10807000, 0x20000);
            fx->pos.y_pos += 256;
            TriggerShockwave((PHD_VECTOR *)&fx->pos, 0xF00030, 48, 0x10807000, 0x20000);
        }
        KillEffect(fx_number);
    }
}
