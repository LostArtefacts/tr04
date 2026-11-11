#include "tomb4fx.h"
#include "sound.h"

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
