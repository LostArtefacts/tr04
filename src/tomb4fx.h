#ifndef TOMB4FX_H
#define TOMB4FX_H

#include "types.h"
#include "3dglodef.h"

typedef struct fire_list {
    sint32 x;
    sint32 y;
    sint32 z;
    sint8 on;
    sint8 size;
    sint16 room_number;
} FIRE_LIST;

typedef struct shockwave_struct {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 InnerRad;
    sint16 OuterRad;
    sint16 XRot;
    sint16 Flags;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 life;
    sint16 Speed;
    sint16 Temp;
} SHOCKWAVE_STRUCT;

void TriggerShatterSmoke(sint32 x, sint32 y, sint32 z);
void TriggerBlood(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 num);
void CreateBubble(PHD_3DPOS *pos, sint16 room_number, sint32 size, sint32 biggest);
sint32 ExplodingDeath2(sint16 item_number, sint32 mesh_bits, sint16 Flags);
void AddFire(sint32 x, sint32 y, sint32 z, sint32 size, sint16 room_number, sint16 fade);
void TriggerShockwave(PHD_VECTOR *pos, sint32 InnerOuterRads, sint32 speed, sint32 rgb, sint32 XRotFlags);
void ClearFires(void);
void SetFadeClip(sint16 height, sint16 speed);
void UpdateFadeClip(void);
void SetScreenFadeOut(sint16 speed, sint16 back);

extern sint16 FlashFadeR;
extern sint16 FlashFadeG;
extern sint16 FlashFadeB;
extern sint16 FlashFader;
extern sint16 ScreenFade;
extern sint16 dScreenFade;
extern sint16 ScreenFadeBack;
extern sint16 ScreenFadedOut;
extern sint16 ScreenFading;
extern sint16 FadeScreenHeight;
extern sint16 DestFadeScreenHeight;
extern sint16 FadeClipSpeed;
extern sint16 ScreenFadeSpeed;
extern FIRE_LIST fires[32];
extern SHOCKWAVE_STRUCT ShockWaves[16];

#endif
