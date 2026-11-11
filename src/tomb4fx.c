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
