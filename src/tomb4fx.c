#include "tomb4fx.h"
#include "sound.h"

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
