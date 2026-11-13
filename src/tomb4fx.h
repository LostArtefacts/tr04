#ifndef TOMB4FX_H
#define TOMB4FX_H

#include "types.h"
#include "3dglodef.h"
#include "items.h"

typedef struct fire_list {
    sint32 x;
    sint32 y;
    sint32 z;
    uint8 on;
    uint8 size;
    sint16 room_number;
} FIRE_LIST;

typedef struct nodeoffset_info {
    sint16 x;
    sint16 y;
    sint16 z;
    sint8 mesh_num;
    uint8 GotIt;
} NODEOFFSET_INFO;

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

typedef struct {
    PHD_VECTOR Point[4];
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 Life;
    sint8 Xvel1;
    sint8 Yvel1;
    sint8 Zvel1;
    sint8 Xvel2;
    sint8 Yvel2;
    sint8 Zvel2;
    sint8 Xvel3;
    sint8 Yvel3;
    sint8 Zvel3;
    uint8 Size;
    uint8 Flags;
    uint8 Rand;
    uint8 Segments;
    uint8 Pad[3];
} LIGHTNING_STRUCT;

typedef struct smoke_sparks {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 Xvel;
    sint16 Yvel;
    sint16 Zvel;
    sint16 Gravity;
    sint16 RotAng;
    sint16 Flags;
    uint8 sSize;
    uint8 dSize;
    uint8 Size;
    uint8 Friction;
    uint8 Scalar;
    uint8 Def;
    sint8 RotAdd;
    sint8 MaxYvel;
    uint8 On;
    uint8 sShade;
    uint8 dShade;
    uint8 Shade;
    uint8 ColFadeSpeed;
    uint8 FadeToBlack;
    sint8 sLife;
    sint8 Life;
    uint8 TransType;
    uint8 FxObj;
    uint8 NodeNumber;
    uint8 mirror;
} SMOKE_SPARKS;

typedef struct fire_sparks {
    sint16 x;
    sint16 y;
    sint16 z;
    sint16 Xvel;
    sint16 Yvel;
    sint16 Zvel;
    sint16 Gravity;
    sint16 RotAng;
    sint16 Flags;
    uint8 sSize;
    uint8 dSize;
    uint8 Size;
    uint8 Friction;
    uint8 Scalar;
    uint8 Def;
    sint8 RotAdd;
    sint8 MaxYvel;
    uint8 On;
    uint8 sR;
    uint8 sG;
    uint8 sB;
    uint8 dR;
    uint8 dG;
    uint8 dB;
    uint8 R;
    uint8 G;
    uint8 B;
    uint8 ColFadeSpeed;
    uint8 FadeToBlack;
    uint8 sLife;
    uint8 Life;
} FIRE_SPARKS;

typedef struct blood_struct {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 Xvel;
    sint16 Yvel;
    sint16 Zvel;
    sint16 Gravity;
    sint16 RotAng;
    uint8 sSize;
    uint8 dSize;
    uint8 Size;
    uint8 Friction;
    sint8 RotAdd;
    uint8 On;
    uint8 sShade;
    uint8 dShade;
    uint8 Shade;
    uint8 ColFadeSpeed;
    uint8 FadeToBlack;
    sint8 sLife;
    sint8 Life;
    sint8 Pad;
} BLOOD_STRUCT;

typedef struct gunshell_struct {
    PHD_3DPOS pos;
    sint16 fallspeed;
    sint16 room_number;
    sint16 speed;
    sint16 counter;
    sint16 DirXrot;
    sint16 object_number;
} GUNSHELL_STRUCT;

typedef struct bubble_struct {
    PHD_VECTOR pos;
    sint16 room_number;
    sint16 speed;
    sint16 size;
    sint16 dsize;
    uint8 shade;
    uint8 vel;
    sint16 pad;
} BUBBLE_STRUCT;

typedef struct drip_struct {
    sint32 x;
    sint32 y;
    sint32 z;
    uint8 On;
    uint8 R;
    uint8 G;
    uint8 B;
    sint16 Yvel;
    uint8 Gravity;
    uint8 Life;
    sint16 RoomNumber;
    uint8 Outside;
    uint8 Pad;
} DRIP_STRUCT;

typedef struct gunflash_struct {
    sint32 mx[12];
    sint16 on;
} GUNFLASH_STRUCT;

void TriggerShatterSmoke(sint32 x, sint32 y, sint32 z);
void TriggerFlashSmoke(sint32 x, sint32 y, sint32 z, sint16 room_number);
void TriggerBlood(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 num);
void CreateBubble(PHD_3DPOS *pos, sint16 room_number, sint32 size, sint32 biggest);
sint32 ExplodingDeath2(sint16 item_number, sint32 mesh_bits, sint16 Flags);
void AddFire(sint32 x, sint32 y, sint32 z, sint32 size, sint16 room_number, sint16 fade);
void TriggerShockwave(PHD_VECTOR *pos, sint32 InnerOuterRads, sint32 speed, sint32 rgb, sint32 XRotFlags);
void TriggerShockwaveHitEffect(sint32 x, sint32 y, sint32 z, sint32 rgb, sint16 dir, sint32 speed);
void ClearFires(void);
void SetFadeClip(sint16 height, sint16 speed);
void UpdateFadeClip(void);
void SetScreenFadeOut(sint16 speed, sint16 back);
void SetScreenFadeIn(sint16 speed);
sint32 GetFreeShockwave(void);
sint32 GetFreeSmokeSpark(void);
sint32 GetFreeFireSpark(void);
sint32 GetFreeBlood(void);
sint32 GetFreeGunshell(void);
sint32 GetFreeBubble(void);
sint32 GetFreeDrip(void);
void TriggerGunShell(sint16 leftright, sint16 objnum, sint32 weapon);
void TriggerGunSmoke(sint32 x, sint32 y, sint32 z, sint32 xVel, sint32 yVel, sint32 zVel, sint32 notLara, sint32 weaponType, sint32 shade);
void TriggerGlobalStaticFlame(void);
void TriggerGlobalFireSmoke(void);
void TriggerGlobalFireFlame(void);
void keep_those_fires_burning(void);
void TriggerSmallSplash(sint32 x, sint32 y, sint32 z, sint32 num);
void LaraBubbles(ITEM_INFO *item);
void TriggerGunflash(SVECTOR *pos);
void SetGunFlash(sint32 weapon);
void Fade(void);
void DrawLensFlares(ITEM_INFO *item);
void TriggerLightningGlow(sint32 x, sint32 y, sint32 z, sint32 rgb);
void DrawGunshells(void);
void UpdateLightning(void);
void TriggerLightning(PHD_VECTOR *s, PHD_VECTOR *d, sint8 variation, sint32 rgb, uint8 flags, uint8 size, uint8 segments);
void DrawWeaponMissile(ITEM_INFO *item);
void DrawGunflashes(void);
void TriggerLaraDrips(void);
void UpdateShockwaves(void);
void UpdateFireSparks(void);
void UpdateSmokeSparks(void);
void UpdateBubbles(void);
void UpdateBlood(void);
void UpdateDrips(void);
void UpdateGunShells(void);
void S_DrawFires(void);
void S_DrawSparks(void);

extern NODEOFFSET_INFO NodeOffsets[];
extern PHD_VECTOR NodeVectors[16];
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
extern SMOKE_SPARKS smoke_spark[32];
extern sint32 next_smoke_spark;
extern FIRE_SPARKS fire_spark[20];
extern sint32 next_fire_spark;
extern BLOOD_STRUCT blood[32];
extern sint32 next_blood;
extern GUNSHELL_STRUCT Gunshells[24];
extern sint32 next_gunshell;
extern BUBBLE_STRUCT Bubbles[40];
extern sint32 next_bubble;
extern DRIP_STRUCT Drips[32];
extern sint32 next_drip;
extern GUNFLASH_STRUCT Gunflashes[4];
extern SHOCKWAVE_STRUCT ShockWaves[16];
extern LIGHTNING_STRUCT Lightning[16];
extern char scratchpad[1024];

#endif
