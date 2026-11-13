#include "tomb4fx.h"
#include "items.h"
#include "lara.h"
#include "room.h"
#include "gameflow.h"
#include "specific/function_stubs.h"
#include "sound.h"
#include "effect2.h"
#include "control.h"
#include "specific/3dmath.h"
#include "delstuff.h"
#include "text.h"
#include "specific/specificfx.h"
#include "anim.h"
#include "objnums.h"
#include "draw.h"
#include "specific/output.h"
#include "effects.h"

sint32 GetFreeFireSpark(void)
{
    FIRE_SPARKS *sptr;
    sint32 min_life, min_life_num, free, i;

    sptr = &fire_spark[next_fire_spark];
    min_life = 4095;
    min_life_num = 0;
    for (free = next_fire_spark, i = 0; i < 20; i++) {
        if (sptr->On) {
            if (sptr->Life < min_life) {
                min_life_num = free;
                min_life = sptr->Life;
            }
            if (free == 19) {
                sptr = &fire_spark[1];
                free = 1;
            } else {
                free++;
                sptr++;
            }
        } else {
            next_fire_spark = free + 1;
            if (next_fire_spark >= 20)
                next_fire_spark = 1;
            return free;
        }
    }
    next_fire_spark = min_life_num + 1;
    if (next_fire_spark >= 20)
        next_fire_spark = 1;
    return min_life_num;
}

void keep_those_fires_burning(void)
{
    TriggerGlobalStaticFlame();
    if (!(wibble & 0xF)) {
        TriggerGlobalFireFlame();
        if (!(wibble & 0x1F))
            TriggerGlobalFireSmoke();
    }
}

void TriggerGlobalFireSmoke(void)
{
    FIRE_SPARKS *sptr;
    sint32 size;

    sptr = &fire_spark[GetFreeFireSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = 0;
    sptr->sB = 0;
    sptr->dR = 32;
    sptr->dG = 32;
    sptr->dB = 32;
    sptr->ColFadeSpeed = (GetRandomControl() & 7) + 32;
    sptr->FadeToBlack = 16;
    sptr->sLife = sptr->Life = (GetRandomControl() & 0xF) + 57;
    sptr->x = (GetRandomControl() & 0xF) - 8;
    sptr->y = -256 - (GetRandomControl() & 0x7F);
    sptr->z = (GetRandomControl() & 0xF) - 8;
    sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Yvel = -16 - (GetRandomControl() & 0xF);
    sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Friction = 4;
    if (GetRandomControl() & 1) {
        sptr->Flags = 16;
        sptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
        else
            sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    } else
        sptr->Flags = 0;
    sptr->Gravity = -16 - (GetRandomControl() & 0xF);
    sptr->MaxYvel = -8 - (GetRandomControl() & 7);
    size = (GetRandomControl() & 0x7F) + 128;
    sptr->Size = sptr->sSize = size >> 2;
    sptr->dSize = size;
}

void TriggerGlobalFireFlame(void)
{
    FIRE_SPARKS *sptr;
    sint32 size;

    sptr = &fire_spark[GetFreeFireSpark()];
    sptr->On = 1;
    sptr->sR = 255;
    sptr->sG = (GetRandomControl() & 0x1F) + 48;
    sptr->sB = 48;
    sptr->dR = (GetRandomControl() & 0x3F) + 192;
    sptr->dG = (GetRandomControl() & 0x3F) + 128;
    sptr->dB = 32;
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 32;
    sptr->x = 4 * (GetRandomControl() & 0x1F) - 64;
    sptr->y = 0;
    sptr->z = 4 * (GetRandomControl() & 0x1F) - 64;
    sptr->Xvel = 2 * (GetRandomControl() & 0xFF) - 256;
    sptr->Yvel = -16 - (GetRandomControl() & 0xF);
    sptr->Zvel = 2 * (GetRandomControl() & 0xFF) - 256;
    sptr->Friction = 5;
    sptr->Gravity = -32 - (GetRandomControl() & 0x1F);
    sptr->MaxYvel = -16 - (GetRandomControl() & 7);
    if (GetRandomControl() & 1) {
        sptr->Flags = 16;
        sptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
        else
            sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    } else
        sptr->Flags = 0;
    size = (GetRandomControl() & 0x1F) + 128;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 4;
}

void TriggerGlobalStaticFlame(void)
{
    FIRE_SPARKS *sptr;

    sptr = &fire_spark[0];
    sptr->On = 1;
    sptr->sR = sptr->dR = (GetRandomControl() & 0x3F) - 64;
    sptr->sG = sptr->dG = (GetRandomControl() & 0x3F) + 96;
    sptr->sB = sptr->dB = 64;
    sptr->ColFadeSpeed = 1;
    sptr->FadeToBlack = 0;
    sptr->sLife = sptr->Life = 8;
    sptr->x = (GetRandomControl() & 7) - 4;
    sptr->y = 0;
    sptr->z = (GetRandomControl() & 7) - 4;
    sptr->MaxYvel = 0;
    sptr->Gravity = 0;
    sptr->Friction = 0;
    sptr->Xvel = sptr->Yvel = sptr->Zvel = 0;
    sptr->Flags = 0;
    sptr->Size = sptr->sSize = sptr->dSize = (GetRandomControl() & 0x1F) + 0x80;
}

void ClearFires(void)
{
    FIRE_LIST *fire;
    sint32 i;

    fire = fires;
    for (i = 0; i < 32; i++, fire++)
        fire->on = 0;
}

void AddFire(sint32 x, sint32 y, sint32 z, sint32 size, sint16 room_number, sint16 fade)
{
    FIRE_LIST *fire;
    sint32 i;

    fire = fires;
    for (i = 0; i < 32; fire++, i++) {
        if (fire->on)
            continue;
        if (fade)
            fire->on = (sint8)fade;
        else
            fire->on = 1;
        fire->x = x;
        fire->y = y;
        fire->z = z;
        fire->size = (sint8)size;
        fire->room_number = room_number;
        break;
    }
}

sint32 GetFreeSmokeSpark(void)
{
    SMOKE_SPARKS *sptr;
    sint32 min_life, min_life_num, free, i;

    sptr = &smoke_spark[next_smoke_spark];
    min_life = 4095;
    min_life_num = 0;
    for (free = next_smoke_spark, i = 0; i < 32; i++) {
        if (sptr->On) {
            if (sptr->Life < min_life) {
                min_life_num = free;
                min_life = sptr->Life;
            }
            if (free == 31) {
                sptr = &smoke_spark[0];
                free = 0;
            } else {
                free++;
                sptr++;
            }
        } else {
            next_smoke_spark = (free + 1) & 0x1F;
            return free;
        }
    }
    next_smoke_spark = (min_life_num + 1) & 0x1F;
    return min_life_num;
}

void UpdateSmokeSparks(void)
{
    SMOKE_SPARKS *sptr;
    sint32 fade, i;

    sptr = smoke_spark;
    for (i = 32; i > 0; i--, sptr++) {
        if (!sptr->On)
            continue;
        sptr->Life -= 2;
        if (sptr->Life <= 0) {
            sptr->On = 0;
            continue;
        }
        if (sptr->sLife - sptr->Life < sptr->ColFadeSpeed) {
            fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
            sptr->Shade = (uint8)(sptr->sShade + (((sptr->dShade - sptr->sShade) * fade) >> 16));
        } else if (sptr->Life < sptr->FadeToBlack) {
            fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
            sptr->Shade = (uint8)((sptr->dShade * fade) >> 16);
            if (sptr->Shade < 8) {
                sptr->On = 0;
                continue;
            }
        } else
            sptr->Shade = sptr->dShade;
        if (sptr->Shade < 24)
            sptr->Def = (uint8)(objects[DEFAULT_SPRITES].mesh_index + 2);
        else if (sptr->Shade < 80)
            sptr->Def = (uint8)(objects[DEFAULT_SPRITES].mesh_index + 1);
        else
            sptr->Def = (uint8)objects[DEFAULT_SPRITES].mesh_index;
        if (sptr->Flags & 0x10)
            sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;
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
    }
}

void TriggerGunSmoke(sint32 x, sint32 y, sint32 z, sint32 xVel, sint32 yVel, sint32 zVel, sint32 notLara, sint32 weaponType, sint32 shade)
{
    SMOKE_SPARKS *sptr;
    sint32 size;

    sptr = &smoke_spark[GetFreeSmokeSpark()];
    sptr->On = 1;
    sptr->sShade = 0;
    sptr->dShade = (uint8)(4 * shade);
    sptr->ColFadeSpeed = 4;
    sptr->FadeToBlack = (uint8)(32 - 16 * notLara);
    sptr->Life = (GetRandomControl() & 3) + 40;
    sptr->sLife = sptr->Life;
    if ((weaponType == WEAPON_PISTOLS || weaponType == WEAPON_REVOLVER || weaponType == WEAPON_UZI) && sptr->dShade > 64u)
        sptr->dShade = 64;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0x1F) + x - 16;
    sptr->y = (GetRandomControl() & 0x1F) + y - 16;
    sptr->z = (GetRandomControl() & 0x1F) + z - 16;
    if (notLara) {
        sptr->Xvel = (sint16)((GetRandomControl() & 0x3FF) + xVel - 512);
        sptr->Yvel = (sint16)((GetRandomControl() & 0x3FF) + yVel - 512);
        sptr->Zvel = (sint16)((GetRandomControl() & 0x3FF) + zVel - 512);
    } else {
        sptr->Xvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
        sptr->Yvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
        sptr->Zvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
    }
    sptr->Friction = 4;
    if (GetRandomControl() & 1) {
        if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
            sptr->Flags = 272;
        else
            sptr->Flags = 16;
        sptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
        else
            sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    } else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
        sptr->Flags = 256;
    else
        sptr->Flags = 0;
    sptr->Gravity = -2 - (GetRandomControl() & 1);
    sptr->MaxYvel = -2 - (GetRandomControl() & 1);
    size = (GetRandomControl() & 0xF) - (weaponType != WEAPON_GRENADE ? 24 : 0) + 48;
    if (notLara) {
        sptr->sSize = size >> 1;
        sptr->Size = size >> 1;
        sptr->dSize = (size + 4) << 1;
    } else {
        sptr->sSize = size >> 2;
        sptr->Size = size >> 2;
        sptr->dSize = size;
    }
    sptr->mirror = (gfLevelFlags & GF_MIRROR) && lara_item->room_number == gfMirrorRoom;
}

void TriggerShatterSmoke(sint32 x, sint32 y, sint32 z)
{
    SMOKE_SPARKS *sptr;
    sint32 size;

    sptr = &smoke_spark[GetFreeSmokeSpark()];
    sptr->On = 1;
    sptr->sShade = 0;
    sptr->dShade = (GetRandomControl() & 0x1F) + 64;
    sptr->ColFadeSpeed = 4;
    sptr->FadeToBlack = 24 - (GetRandomControl() & 7);
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 48;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0x1F) + x - 16;
    sptr->y = (GetRandomControl() & 0x1F) + y - 16;
    sptr->z = (GetRandomControl() & 0x1F) + z - 16;
    sptr->Xvel = 2 * (GetRandomControl() & 0x1FF) - 512;
    sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 512;
    sptr->Zvel = 2 * (GetRandomControl() & 0x1FF) - 512;
    sptr->Friction = 7;
    if (GetRandomControl() & 1) {
        sptr->Flags = 16;
        sptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
        else
            sptr->RotAdd = (GetRandomControl() & 0x3F) + 64;
    } else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
        sptr->Flags = 256;
    else
        sptr->Flags = 0;
    sptr->Gravity = -4 - (GetRandomControl() & 3);
    sptr->MaxYvel = -4 - (GetRandomControl() & 3);
    size = (GetRandomControl() & 0x3F) + 64;
    sptr->Size = sptr->sSize = size >> 3;
    sptr->dSize = size;
}

sint32 GetFreeBlood(void)
{
    BLOOD_STRUCT *bptr;
    sint32 min_life, min_life_num, free, i;

    free = next_blood;
    bptr = &blood[next_blood];
    min_life = 4095;
    min_life_num = 0;
    for (i = 0; i < 32; i++) {
        if (bptr->On) {
            if (bptr->Life < min_life) {
                min_life_num = free;
                min_life = bptr->Life;
            }
            if (free == 31) {
                bptr = &blood[0];
                free = 0;
            } else {
                free++;
                bptr++;
            }
        } else {
            next_blood = (free + 1) & 0x1F;
            return free;
        }
    }
    next_blood = (min_life_num + 1) & 0x1F;
    return min_life_num;
}

void TriggerBlood(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 num)
{
    BLOOD_STRUCT *bptr;
    sint32 i, ang, speed, size;

    for (i = 0; i < num; i++) {
        bptr = &blood[GetFreeBlood()];
        bptr->On = 1;
        bptr->sShade = 0;
        bptr->dShade = (GetRandomControl() & 0x3F) + 48;
        bptr->ColFadeSpeed = 4;
        bptr->FadeToBlack = 8;
        bptr->Life = (GetRandomControl() & 7) + 24;
        bptr->sLife = bptr->Life;
        bptr->x = (GetRandomControl() & 0x1F) + x - 16;
        bptr->y = (GetRandomControl() & 0x1F) + y - 16;
        bptr->z = (GetRandomControl() & 0x1F) + z - 16;
        if (angle == -1)
            ang = GetRandomControl();
        else
            ang = (GetRandomControl() & 0x1F) + angle - 16;
        ang &= 0xFFF;
        speed = GetRandomControl() & 0xF;
        bptr->Zvel = speed * rcossin_tbl[(ang << 1) + 1] >> 7;
        bptr->Xvel = -(speed * rcossin_tbl[ang << 1]) >> 7;
        bptr->Yvel = -128 - (GetRandomControl() & 0xFF);
        bptr->Friction = 4;
        bptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            bptr->RotAdd = -64 - (GetRandomControl() & 0x3F);
        else
            bptr->RotAdd = (GetRandomControl() & 0x3F) + 64;
        bptr->Gravity = (GetRandomControl() & 0x1F) + 31;
        size = (GetRandomControl() & 7) + 8;
        bptr->Size = bptr->sSize = size;
        bptr->dSize = size >> 2;
    }
}

sint32 GetFreeGunshell(void)
{
    GUNSHELL_STRUCT *shell;
    sint32 min_life, min_life_num, free, i;

    shell = &Gunshells[next_gunshell];
    min_life = 4095;
    min_life_num = 0;
    for (free = next_gunshell, i = 0; i < 24; i++) {
        if (shell->counter) {
            if (shell->counter < min_life) {
                min_life = shell->counter;
                min_life_num = free;
            }
            if (free == 23) {
                shell = &Gunshells[0];
                free = 0;
            } else {
                free++;
                shell++;
            }
        } else {
            next_gunshell = free + 1;
            if (next_gunshell >= 24)
                next_gunshell = 0;
            return free;
        }
    }
    next_gunshell = min_life_num + 1;
    if (next_gunshell >= 24)
        next_gunshell = 0;
    return min_life_num;
}

void DrawGunshells(void)
{
    GUNSHELL_STRUCT *p;
    OBJECT_INFO *obj;
    sint32 i;

    phd_left = 0;
    phd_right = phd_winwidth;
    phd_top = 0;
    phd_bottom = phd_winheight;
    p = Gunshells;
    for (i = 24; i > 0; i--, p++) {
        if (p->counter) {
            obj = &objects[p->object_number];
            phd_PushMatrix();
            phd_TranslateAbs(p->pos.x_pos, p->pos.y_pos, p->pos.z_pos);
            phd_RotYXZ(p->pos.y_rot, p->pos.x_rot, p->pos.z_rot);
            phd_PutPolygons(meshes[obj->mesh_index], -1);
            phd_PopMatrix();
        }
    }
}

void TriggerSmallSplash(sint32 x, sint32 y, sint32 z, sint32 num)
{
    SPARKS *sptr;
    sint32 ang;

    while (num > 0) {
        sptr = &spark[GetFreeSpark()];
        sptr->On = 1;
        sptr->sR = 112;
        sptr->sG = (GetRandomControl() & 0x1F) + 128;
        sptr->sB = (GetRandomControl() & 0x1F) + 128;
        sptr->dR = sptr->sR >> 1;
        sptr->dG = sptr->sG >> 1;
        sptr->dB = sptr->sB >> 1;
        sptr->ColFadeSpeed = 4;
        sptr->FadeToBlack = 8;
        sptr->Life = 24;
        sptr->sLife = 24;
        sptr->TransType = 2;
        ang = GetRandomControl() & 0xFFF;
        sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 5;
        sptr->Yvel = -640 - (GetRandomControl() & 0xFF);
        sptr->Xvel = -rcossin_tbl[ang << 1] >> 5;
        sptr->x = x + (sptr->Xvel >> 3);
        sptr->Friction = 5;
        sptr->Flags = 0;
        sptr->y = y - (sptr->Yvel >> 5);
        sptr->z = z + (sptr->Zvel >> 3);
        sptr->Gravity = (GetRandomControl() & 0xF) + 64;
        sptr->MaxYvel = 0;
        num--;
    }
}

sint32 GetFreeBubble(void)
{
    BUBBLE_STRUCT *bubble;
    sint32 free, i;

    free = next_bubble;
    bubble = &Bubbles[next_bubble];
    for (i = 0; i < 40; i++) {
        if (bubble->size) {
            if (free == 39) {
                bubble = &Bubbles[0];
                free = 0;
            } else {
                free++;
                bubble++;
            }
        } else {
            next_bubble = free + 1;
            if (next_bubble >= 40)
                next_bubble = 0;
            return free;
        }
    }
    next_bubble = free + 1;
    if (next_bubble >= 40)
        next_bubble = 0;
    return free;
}

void CreateBubble(PHD_3DPOS *pos, sint16 room_number, sint32 size, sint32 biggest)
{
    BUBBLE_STRUCT *bubble;
    sint16 room_num, bubble_num;

    room_num = room_number;
    GetFloor(pos->x_pos, pos->y_pos, pos->z_pos, &room_num);
    if (room[room_num].flags & ROOM_UNDERWATER) {
        bubble_num = (sint16)GetFreeBubble();
        if (bubble_num != -1) {
            bubble = &Bubbles[bubble_num];
            bubble->pos.x = pos->x_pos;
            bubble->pos.y = pos->y_pos;
            bubble->pos.z = pos->z_pos;
            bubble->room_number = room_num;
            bubble->speed = (GetRandomControl() & 0xFF) + 64;
            size = (size + (biggest & GetRandomControl())) << 1;
            bubble->shade = 0;
            bubble->size = (sint16)size;
            bubble->dsize = (sint16)(size << 4);
            bubble->vel = (GetRandomControl() & 0x1F) + 32;
        }
    }
}

void LaraBubbles(ITEM_INFO *item)
{
    PHD_VECTOR pos;
    sint32 i;

    SoundEffect(SFX_LARA_BUBBLES, &item->pos, SFX_WATER);
    pos.x = 0;
    pos.y = -4;
    pos.z = 64;
    GetLaraJointPos(&pos, 8);
    for (i = (GetRandomControl() & 1) + 2; i > 0; i--) {
        CreateBubble((PHD_3DPOS *)&pos, item->room_number, 8, 7);
        if ((gfLevelFlags & GF_MIRROR) && item->room_number == gfMirrorRoom) {
            pos.z = 2 * gfMirrorZPlane - pos.z;
            CreateBubble((PHD_3DPOS *)&pos, item->room_number, 8, 7);
            pos.z = 2 * gfMirrorZPlane - pos.z;
        }
    }
}

void TriggerGunflash(SVECTOR *pos)
{
    GUNFLASH_STRUCT *flash;
    sint32 num;

    flash = Gunflashes;
    for (num = 0; num < 4; flash++, num++) {
        if (!flash->on) {
            phd_TranslateRel(pos->x, pos->y, pos->z);
            phd_RotX(-0x3FFC);
            flash->mx[M00] = phd_mxptr[M00];
            flash->mx[M01] = phd_mxptr[M01];
            flash->mx[M02] = phd_mxptr[M02];
            flash->mx[M03] = phd_mxptr[M03];
            flash->mx[M10] = phd_mxptr[M10];
            flash->mx[M11] = phd_mxptr[M11];
            flash->mx[M12] = phd_mxptr[M12];
            flash->mx[M13] = phd_mxptr[M13];
            flash->mx[M20] = phd_mxptr[M20];
            flash->mx[M21] = phd_mxptr[M21];
            flash->mx[M22] = phd_mxptr[M22];
            flash->mx[M23] = phd_mxptr[M23];
            flash->on = 1;
            return;
        }
    }
}

void SetGunFlash(sint32 weapon)
{
    SVECTOR pos;

    switch (weapon) {
    case WEAPON_REVOLVER:
        pos.y = 215;
        pos.z = 65;
        break;
    case WEAPON_UZI:
        pos.y = 190;
        pos.z = 50;
        break;
    case WEAPON_SHOTGUN:
    case WEAPON_GRENADE:
    case WEAPON_CROSSBOW:
    case WEAPON_FLARE:
        return;
    default:
        pos.y = 180;
        pos.z = 40;
        break;
    }
    pos.x = 0;
    TriggerGunflash(&pos);
}

void DrawGunflashes(void)
{
    GUNFLASH_STRUCT *flash;
    sint32 i;

    if (!Gunflashes[0].on)
        return;
    phd_top = 0;
    phd_left = 0;
    phd_right = phd_winwidth;
    phd_bottom = phd_winheight;
    phd_PushMatrix();
    GetRandomDraw();
    GetRandomDraw();
    GetRandomDraw();
    GetRandomDraw();
    flash = Gunflashes;
    for (i = 0; i < 4; flash++, i++) {
        if (!flash->on)
            break;
        phd_mxptr[M00] = flash->mx[M00];
        phd_mxptr[M01] = flash->mx[M01];
        phd_mxptr[M02] = flash->mx[M02];
        phd_mxptr[M03] = flash->mx[M03];
        phd_mxptr[M10] = flash->mx[M10];
        phd_mxptr[M11] = flash->mx[M11];
        phd_mxptr[M12] = flash->mx[M12];
        phd_mxptr[M13] = flash->mx[M13];
        phd_mxptr[M20] = flash->mx[M20];
        phd_mxptr[M21] = flash->mx[M21];
        phd_mxptr[M22] = flash->mx[M22];
        phd_mxptr[M23] = flash->mx[M23];
        phd_RotZ((sint16)(GetRandomDraw() << 1));
        GlobalAmbient = 0xFF2F2F00;
        phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], -1);
        flash->on = 0;
    }
    phd_PopMatrix();
}

void SetFadeClip(sint16 height, sint16 speed)
{
    DestFadeScreenHeight = height;
    FadeClipSpeed = speed;
}

void UpdateFadeClip(void)
{
    if (DestFadeScreenHeight < FadeScreenHeight) {
        FadeScreenHeight -= FadeClipSpeed;
        if (DestFadeScreenHeight > FadeScreenHeight)
            FadeScreenHeight = DestFadeScreenHeight;
    } else if (DestFadeScreenHeight > FadeScreenHeight) {
        FadeScreenHeight += FadeClipSpeed;
        if (DestFadeScreenHeight < FadeScreenHeight)
            FadeScreenHeight = DestFadeScreenHeight;
    }
}

void SetScreenFadeOut(sint16 speed, sint16 back)
{
    if (!ScreenFading) {
        ScreenFading = 1;
        ScreenFade = 0;
        dScreenFade = 255;
        ScreenFadeSpeed = speed;
        ScreenFadeBack = back;
        ScreenFadedOut = 0;
    }
}

void SetScreenFadeIn(sint16 speed)
{
    if (!ScreenFading) {
        ScreenFading = 1;
        ScreenFade = 255;
        dScreenFade = 0;
        ScreenFadeSpeed = speed;
        ScreenFadedOut = 0;
    }
}

void Fade(void)
{
    sint32 oldfucker;

    oldfucker = ScreenFade;
    if (dScreenFade && dScreenFade >= ScreenFade) {
        ScreenFade += ScreenFadeSpeed;
        if (ScreenFade > dScreenFade) {
            ScreenFade = dScreenFade;
            if (oldfucker >= dScreenFade) {
                ScreenFadedOut = 1;
                if (ScreenFadeBack) {
                    ScreenFadeBack = dScreenFade = 0;
                } else
                    ScreenFading = 0;
            }
        }
    } else if (dScreenFade < ScreenFade) {
        ScreenFade -= ScreenFadeSpeed;
        if (ScreenFade < dScreenFade) {
            ScreenFade = dScreenFade;
            ScreenFading = 0;
        }
    }
    if (dScreenFade || ScreenFade)
        DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), RGBA(ScreenFade, ScreenFade, ScreenFade, 98), 2, 0);
}

sint32 GetFreeDrip(void)
{
    DRIP_STRUCT *drip;
    sint32 min_life, min_life_num, free, i;
    
    free = next_drip;
    drip = &Drips[next_drip];
    min_life = 4095;
    min_life_num = 0;
    for (i = 0; i < 32; i++) {
        if (drip->On) {
            if (drip->Life < min_life) {
                min_life_num = free;
                min_life = drip->Life;
            }
            if (free == 31) {
                drip = &Drips[0];
                free = 0;
            } else {
                free++;
                drip++;
            }
        } else {
            next_drip = (free + 1) & 0x1F;
            return free;
        }
    }
    next_drip = (min_life_num + 1) & 0x1F;
    return min_life_num;
}

void TriggerLaraDrips(void)
{
    DRIP_STRUCT *drip;
    PHD_VECTOR pos;
    sint32 i;

    if (wibble & 0xF)
        return;
    for (i = 14; i >= 0; i--) {
        if (lara.wet[i] && !LaraNodeUnderwater[i] && (GetRandomControl() & 0x1FF) < lara.wet[i]) {
            pos.x = (GetRandomControl() & 0x1F) - 16;
            pos.y = (GetRandomControl() & 0xF) + 16;
            pos.z = (GetRandomControl() & 0x1F) - 16;
            GetLaraJointPos(&pos, i);
            drip = &Drips[GetFreeDrip()];
            drip->x = pos.x;
            drip->y = pos.y;
            drip->z = pos.z;
            drip->On = 1;
            drip->R = (GetRandomControl() & 7) + 16;
            drip->G = (GetRandomControl() & 7) + 24;
            drip->B = (GetRandomControl() & 7) + 32;
            drip->Yvel = (GetRandomControl() & 0x1F) + 32;
            drip->Gravity = (GetRandomControl() & 0x1F) + 32;
            drip->Life = (GetRandomControl() & 0x1F) + 16;
            drip->RoomNumber = lara_item->room_number;
            lara.wet[i] -= 4;
        }
    }
}

void S_DrawSparks(void)
{
    SPARKS *sptr;
    FX_INFO *fx;
    ITEM_INFO *item;
    PHD_VECTOR pos;
    PHD_VECTOR v;
    sint32 *mptr;
    sint32 *Z;
    sint16 *XY;
    sint16 *offsets;
    float perspz;
    sint32 x, y, z, smallest_size, i;
    sint16 mesh_num;

    for (i = 0; i < 16; i++)
        NodeOffsets[i].GotIt = 0;
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
    mptr = phd_mxptr;
    XY = (sint16 *)&scratchpad[0];
    Z = (sint32 *)&scratchpad[256];
    offsets = (sint16 *)&scratchpad[512];
    sptr = spark;
    for (i = 256; i > 0; i--, sptr++) {
        if (!sptr->On)
            continue;
        if (sptr->Flags & 0x40) {
            fx = &effects[sptr->FxObj];
            x = fx->pos.x_pos + sptr->x;
            y = sptr->y + fx->pos.y_pos;
            z = sptr->z + fx->pos.z_pos;
            if (sptr->sLife - sptr->Life > (GetRandomDraw() & 7) + 4) {
                sptr->Flags &= ~0x40;
                sptr->x = x;
                sptr->y = y;
                sptr->z = z;
            }
        } else if (sptr->Flags & 0x80) {
            item = &items[sptr->FxObj];
            if (sptr->Flags & 0x1000) {
                if (NodeOffsets[sptr->NodeNumber].GotIt) {
                    pos.x = NodeVectors[sptr->NodeNumber].x;
                    pos.y = NodeVectors[sptr->NodeNumber].y;
                    pos.z = NodeVectors[sptr->NodeNumber].z;
                } else {
                    pos.x = NodeOffsets[sptr->NodeNumber].x;
                    pos.y = NodeOffsets[sptr->NodeNumber].y;
                    pos.z = NodeOffsets[sptr->NodeNumber].z;
                    mesh_num = NodeOffsets[sptr->NodeNumber].mesh_num;
                    if (mesh_num < 0)
                        GetLaraJointPos(&pos, -mesh_num);
                    else
                        GetJointAbsPosition(item, &pos, mesh_num);
                    NodeOffsets[sptr->NodeNumber].GotIt = 1;
                    NodeVectors[sptr->NodeNumber].x = pos.x;
                    NodeVectors[sptr->NodeNumber].y = pos.y;
                    NodeVectors[sptr->NodeNumber].z = pos.z;
                }
                x = sptr->x + pos.x;
                y = sptr->y + pos.y;
                z = sptr->z + pos.z;
                if (sptr->sLife - sptr->Life > (GetRandomDraw() & 3) + 8) {
                    sptr->Flags &= ~0x1080;
                    sptr->x = x;
                    sptr->y = y;
                    sptr->z = z;
                }
            } else {
                x = sptr->x + item->pos.x_pos;
                y = sptr->y + item->pos.y_pos;
                z = sptr->z + item->pos.z_pos;
            }
        } else {
            x = sptr->x;
            y = sptr->y;
            z = sptr->z;
        }
        x -= lara_item->pos.x_pos;
        y -= lara_item->pos.y_pos;
        z -= lara_item->pos.z_pos;
        if (x < -0x5000 || x > 0x5000 || y < -0x5000 || y > 0x5000 || z < -0x5000 || z > 0x5000) {
            sptr->On = 0;
            continue;
        }
        offsets[0] = (sint16)x;
        offsets[1] = (sint16)y;
        offsets[2] = (sint16)z;
        v.x = mptr[M00] * offsets[0] + mptr[M01] * offsets[1] + mptr[M02] * offsets[2] + mptr[M03];
        v.y = mptr[M10] * offsets[0] + mptr[M11] * offsets[1] + mptr[M12] * offsets[2] + mptr[M13];
        v.z = mptr[M20] * offsets[0] + mptr[M21] * offsets[1] + mptr[M22] * offsets[2] + mptr[M23];
        perspz = f_persp / (float)v.z;
        XY[0] = (sint16)(v.x * perspz + f_centerx);
        XY[1] = (sint16)(v.y * perspz + f_centery);
        Z[0] = v.z >> W2V_SHIFT;
        if (!(sptr->Flags & 8)) {
            offsets[0] = (sint16)(x - (sptr->Xvel >> 4));
            offsets[1] = (sint16)(y - (sptr->Yvel >> 4));
            offsets[2] = (sint16)(z - (sptr->Zvel >> 4));
            v.x = mptr[M00] * offsets[0] + mptr[M01] * offsets[1] + mptr[M02] * offsets[2] + mptr[M03];
            v.y = mptr[M10] * offsets[0] + mptr[M11] * offsets[1] + mptr[M12] * offsets[2] + mptr[M13];
            v.z = mptr[M20] * offsets[0] + mptr[M21] * offsets[1] + mptr[M22] * offsets[2] + mptr[M23];
            XY += 2;
            Z += 1;
            perspz = f_persp / (float)v.z;
            XY[0] = (sint16)(v.x * perspz + f_centerx);
            XY[1] = (sint16)(v.y * perspz + f_centery);
            Z[0] = v.z >> W2V_SHIFT;
            Z -= 1;
            XY -= 2;
        }
        if ((sptr->Flags & 8) && (sptr->Flags & 2))
            smallest_size = 4;
        S_DrawDrawSparks(sptr, smallest_size, XY, Z);
    }
    phd_PopMatrix();
}

sint32 ExplodingDeath2(sint16 item_number, sint32 mesh_bits, sint16 Flags)
{
    ITEM_INFO *item;
    OBJECT_INFO *obj;
    FX_INFO *fx;
    sint32 *bone, i;
    sint16 *rotation;
    sint16 *frame;
    sint16 *extra_rotation;
    sint32 bit, poppush;
    sint16 fx_number;

    item = &items[item_number];
    obj = &objects[item->object_number];
    frame = GetBestFrame(item);
    phd_PushUnitMatrix();
    phd_mxptr[M03] = 0;
    phd_mxptr[M13] = 0;
    phd_mxptr[M23] = 0;
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    phd_TranslateRel(frame[6], frame[7], frame[8]);
    rotation = frame + 9;
    gar_RotYXZsuperpack(&rotation, 0);
    bone = &bones[obj->bone_index];
    extra_rotation = (sint16 *)item->data;
    bit = 1;
    if (mesh_bits & 1 && item->mesh_bits & 1) {
        if (Flags & 0x100 || !(GetRandomControl() & 3)) {
            fx_number = CreateEffect(item->room_number);
            if (fx_number != NO_ITEM) {
                fx = &effects[fx_number];
                fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
                fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
                fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
                fx->room_number = item->room_number;
                fx->pos.y_rot = (sint16)(GetRandomControl() << 1);
                fx->pos.x_rot = 0;
                if (Flags & 0x10)
                    fx->speed = 0;
                else if (Flags & 0x20)
                    fx->speed = (sint16)(GetRandomControl() >> 12);
                else
                    fx->speed = (sint16)(GetRandomControl() >> 8);
                if (Flags & 0x40)
                    fx->fallspeed = 0;
                else if (Flags & 0x80)
                    fx->fallspeed = (sint16)(-(GetRandomControl() >> 12));
                else
                    fx->fallspeed = (sint16)(-(GetRandomControl() >> 8));
                fx->frame_number = obj->mesh_index;
                fx->object_number = BODY_PART;
                fx->shade = 0x4210;
                fx->flag2 = Flags;
            }
            item->mesh_bits--;
        }
    }
    for (i = 1; i < obj->nmeshes; i++, bone += 3) {
        poppush = *bone++;
        if (poppush & 1)
            phd_PopMatrix();
        if (poppush & 2)
            phd_PushMatrix();
        phd_TranslateRel(bone[0], bone[1], bone[2]);
        gar_RotYXZsuperpack(&rotation, 0);
        if (poppush & 28) {
            if (poppush & 8)
                phd_RotY(*extra_rotation++);
            if (poppush & 4)
                phd_RotX(*extra_rotation++);
            if (poppush & 16)
                phd_RotZ(*extra_rotation++);
        }
        bit <<= 1;
        if (bit & mesh_bits && bit & item->mesh_bits && (Flags & 0x100 || !(GetRandomControl() & 3))) {
            fx_number = CreateEffect(item->room_number);
            if (fx_number != NO_ITEM) {
                fx = &effects[fx_number];
                fx->pos.x_pos = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
                fx->pos.y_pos = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
                fx->pos.z_pos = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
                fx->room_number = item->room_number;
                fx->pos.y_rot = (sint16)(GetRandomControl() << 1);
                fx->pos.x_rot = 0;
                if (Flags & 0x10)
                    fx->speed = 0;
                else if (Flags & 0x20)
                    fx->speed = (sint16)(GetRandomControl() >> 12);
                else
                    fx->speed = (sint16)(GetRandomControl() >> 8);
                if (Flags & 0x40)
                    fx->fallspeed = 0;
                else if (Flags & 0x80)
                    fx->fallspeed = (sint16)(-(GetRandomControl() >> 12));
                else
                    fx->fallspeed = (sint16)(-(GetRandomControl() >> 8));
                fx->frame_number = obj->mesh_index + 2 * i;
                fx->object_number = BODY_PART;
                fx->shade = 0x4210;
                fx->flag2 = Flags;
            }
            item->mesh_bits -= bit;
        }
    }
    phd_PopMatrix();
    return !item->mesh_bits;
}

sint32 GetFreeShockwave(void)
{
    SHOCKWAVE_STRUCT *sw;
    sint32 i;

    sw = ShockWaves;
    for (i = 0; i < 16; i++, sw++) {
        if (!sw->life)
            return i;
    }
    return -1;
}

void TriggerShockwave(PHD_VECTOR *pos, sint32 InnerOuterRads, sint32 speed, sint32 rgb, sint32 XRotFlags)
{
    SHOCKWAVE_STRUCT *sw;
    sint32 swn;

    swn = GetFreeShockwave();
    if (swn != -1) {
        sw = &ShockWaves[swn];
        sw->x = pos->x;
        sw->y = pos->y;
        sw->z = pos->z;
        sw->InnerRad = (sint16)InnerOuterRads;
        sw->OuterRad = (sint16)(InnerOuterRads >> 16);
        sw->XRot = (sint16)XRotFlags;
        sw->Flags = (sint16)(XRotFlags >> 16);
        sw->Speed = (sint16)speed;
        *(sint32 *)&sw->r = rgb;
        SoundEffect(SFX_DEMI_SIREN_SWAVE, (PHD_3DPOS *)pos, SFX_DEFAULT);
    }
}

void TriggerShockwaveHitEffect(sint32 x, sint32 y, sint32 z, sint32 rgb, sint16 dir, sint32 speed)
{
    SPARKS *sptr;
    sint32 dx, dz, xvel, zvel;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = 0;
    sptr->sB = 0;
    sptr->dB = CLRB(rgb);
    sptr->dG = CLRG(rgb);
    sptr->dR = CLRR(rgb);
    sptr->ColFadeSpeed = 4;
    sptr->FadeToBlack = 8;
    sptr->Life = (GetRandomControl() & 3) + 16;
    sptr->TransType = 2;
    sptr->sLife = sptr->Life;
    speed += GetRandomControl() & 0xF;
    xvel = (rcossin_tbl[(dir >> 3) & 0x1FFE] << 2) * speed >> (W2V_SHIFT - 4);
    zvel = (rcossin_tbl[((dir >> 3) & 0x1FFE) + 1] << 2) * speed >> (W2V_SHIFT - 4);
    if (GetRandomControl() & 1)
        dir += 0x4000;
    else
        dir -= 0x4000;
    speed = (GetRandomControl() & 0x1FF) - 256;
    x += (rcossin_tbl[(dir >> 3) & 0x1FFE] << 2) * speed >> W2V_SHIFT;
    z += (rcossin_tbl[((dir >> 3) & 0x1FFE) + 1] << 2) * speed >> W2V_SHIFT;
    sptr->x = (GetRandomControl() & 0x1F) + x - 16;
    sptr->y = (GetRandomControl() & 0x1F) + y - 16;
    sptr->z = (GetRandomControl() & 0x1F) + z - 16;
    sptr->Xvel = (sint16)xvel;
    sptr->Yvel = -512 - (GetRandomControl() & 0x1FF);
    sptr->Zvel = (sint16)zvel;
    sptr->Friction = 3;
    sptr->Flags = 538;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
    else
        sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    sptr->Def = (uint8)(objects[DEFAULT_SPRITES].mesh_index + 14);
    sptr->Scalar = 1;
    sptr->Gravity = (GetRandomControl() & 0x3F) + 64;
    sptr->MaxYvel = 0;
    speed = (GetRandomControl() & 0x1F) + 32;
    sptr->sSize = speed;
    sptr->Size = speed;
    sptr->dSize = speed >> 2;
}

void UpdateShockwaves(void)
{
    SHOCKWAVE_STRUCT *sw;
    sint16 *bounds;
    sint32 dx, dz, dist, i, dir;

    sw = ShockWaves;
    for (i = 16; i > 0; i--, sw++) {
        if (sw->life) {
            sw->life--;
            if (sw->life) {
                sw->OuterRad += sw->Speed;
                sw->InnerRad += sw->Speed >> 1;
                sw->Speed -= sw->Speed >> 4;
                if (lara_item->hit_points > 0 && sw->Flags & 3) {
                    bounds = GetBestFrame(lara_item);
                    dx = lara_item->pos.x_pos - sw->x;
                    dz = lara_item->pos.z_pos - sw->z;
                    dist = phd_sqrt(SQUARE(dx) + SQUARE(dz));
                    if (sw->y > lara_item->pos.y_pos + bounds[2] && sw->y < bounds[3] + lara_item->pos.y_pos + 256 && dist > sw->InnerRad && dist < sw->OuterRad) {
                        dir = phd_atan(dz, dx);
                        TriggerShockwaveHitEffect(lara_item->pos.x_pos, sw->y, lara_item->pos.z_pos, *(sint32 *)&sw->r, dir, sw->Speed);
                        lara_item->hit_points -= sw->Speed >> (((sw->Flags & 2) != 0) + 2);
                    } else
                        sw->Temp = 0;
                }
            }
        }
    }
}

void TriggerLightning(PHD_VECTOR *s, PHD_VECTOR *d, sint8 variation, sint32 rgb, uint8 flags, uint8 size, uint8 segments)
{
    LIGHTNING_STRUCT *lptr;
    sint8 *vptr;
    sint32 i, j;

    lptr = Lightning;
    for (i = 0; i < 16; i++, lptr++) {
        if (!lptr->Life) {
            lptr->Point[0].x = s->x;
            lptr->Point[0].y = s->y;
            lptr->Point[0].z = s->z;
            lptr->Point[1].x = ((s->x * 3) + d->x) >> 2;
            lptr->Point[1].y = ((s->y * 3) + d->y) >> 2;
            lptr->Point[1].z = ((s->z * 3) + d->z) >> 2;
            lptr->Point[2].x = ((d->x * 3) + s->x) >> 2;
            lptr->Point[2].y = ((d->y * 3) + s->y) >> 2;
            lptr->Point[2].z = ((d->z * 3) + s->z) >> 2;
            lptr->Point[3].x = d->x;
            lptr->Point[3].y = d->y;
            lptr->Point[3].z = d->z;
            vptr = &lptr->Xvel1;
            for (j = 0; j < 6; j++)
                *vptr++ = (GetRandomControl() % variation) - (variation >> 1);
            for (j = 0; j < 3; j++) {
                if (flags & 2)
                    *vptr++ = (GetRandomControl() % variation) - (variation >> 1);
                else
                    *vptr++ = 0;
            }
            *(sint32 *)&lptr->r = rgb;
            lptr->Flags = flags;
            lptr->Rand = variation;
            lptr->Size = size;
            lptr->Segments = segments;
            return;
        }
    }
}

void UpdateLightning(void)
{
    LIGHTNING_STRUCT *lptr;
    sint32 *pPoint;
    sint8 *pVel;
    sint32 i, j;

    lptr = Lightning;
    for (i = 16; i > 0; i--, lptr++) {
        if (lptr->Life) {
            lptr->Life -= 2;
            if (lptr->Life) {
                pPoint = &lptr->Point[1].x;
                pVel = &lptr->Xvel1;
                for (j = 9; j > 0; j--) {
                    *pPoint++ += *pVel << 1;
                    *pVel -= *pVel >> 4;
                    pVel++;
                }
            }
        }
    }
}

sint32 LSpline(sint32 x, sint32 *knots, sint32 nk)
{
    sint32 *k;
    sint32 c1, c2, c3, span;
    __int64 xx;

    x = (sint32)(((__int64)x * (nk - 3) << 16) >> 16);
    span = x >> 16;
    if (span >= nk - 3)
        span = nk - 4;
    x -= 65536 * span;
    k = &knots[3 * span];
    xx = x;
    c1 = k[3] + (k[3] >> 1) - (k[6] >> 1) - k[6] + (k[9] >> 1) + (~k[0] >> 1);
    c2 = (sint32)((c1 * xx) >> 16) + 2 * k[6] - 2 * k[3] - (k[3] >> 1) - (k[9] >> 1) + k[0];
    c3 = (sint32)((c2 * xx) >> 16) + (k[6] >> 1) + (~k[0] >> 1);
    return (sint32)((c3 * xx) >> 16) + k[3];
}

void TriggerLightningGlow(sint32 x, sint32 y, sint32 z, sint32 rgb)
{
    sint32 dx, dz, size;
    SPARKS *sptr;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sG = sptr->dG = (uint8)(rgb >> 8);
    sptr->sB = sptr->dB = (uint8)rgb;
    sptr->ColFadeSpeed = 2;
    sptr->Life = 4;
    sptr->sR = sptr->dR = (uint8)(rgb >> 16);
    sptr->FadeToBlack = 0;
    sptr->TransType = 2;
    sptr->x = x;
    sptr->sLife = 4;
    sptr->y = y;
    sptr->z = z;
    sptr->Zvel = 0;
    sptr->Yvel = 0;
    sptr->Xvel = 0;
    sptr->Flags = 10;
    sptr->Def = objects[DEFAULT_SPRITES].mesh_index + 11;
    sptr->Scalar = 3;
    sptr->MaxYvel = 0;
    sptr->Gravity = 0;
    size = (rgb >> 24) + (GetRandomControl() & 3);
    sptr->Size = sptr->sSize = sptr->dSize = size;
}

void DrawLensFlares(ITEM_INFO *item)
{
    GAME_VECTOR sun;

    sun.x = item->pos.x_pos;
    sun.y = item->pos.y_pos;
    sun.z = item->pos.z_pos;
    sun.room_number = item->room_number;
    SetUpLensFlare(0, 0, 0, &sun);
}

void DrawWeaponMissile(ITEM_INFO *item)
{
    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    phd_PutPolygons_train(meshes[objects[item->object_number].mesh_index], 0);
    phd_PopMatrix();
    if (gfLevelFlags & GF_MIRROR && item->room_number == gfMirrorRoom) {
        phd_PushMatrix();
        phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, 2 * gfMirrorZPlane - item->pos.z_pos);
        phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
        phd_PutPolygons_train(meshes[objects[item->object_number].mesh_index], 0);
        phd_PopMatrix();
    }
}
