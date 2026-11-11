#ifndef EFFECT2_H
#define EFFECT2_H

#include "3dglodef.h"

#define MAX_DYNAMICS 32

typedef struct dynamic {
    sint32 x;
    sint32 y;
    sint32 z;
    uint8 on;
    uint8 r;
    uint8 g;
    uint8 b;
    uint16 falloff;
    uint8 used;
    uint8 pad1[1];
    sint32 FalloffScale;
} DYNAMIC;

typedef struct sparks {
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
    char RotAdd;
    char MaxYvel;
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
    uint8 TransType;
    uint8 extras;
    char Dynamic;
    uint8 FxObj;
    uint8 RoomNumber;
    uint8 NodeNumber;
} SPARKS;

typedef struct ripple_struct {
    sint32 x;
    sint32 y;
    sint32 z;
    char flags;
    uint8 life;
    uint8 size;
    uint8 init;
} RIPPLE_STRUCT;

typedef struct splash_setup {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 InnerRad;
    sint16 InnerSize;
    sint16 InnerRadVel;
    sint16 InnerYVel;
    sint16 pad1;
    sint16 MiddleRad;
    sint16 MiddleSize;
    sint16 MiddleRadVel;
    sint16 MiddleYVel;
    sint16 pad2;
    sint16 OuterRad;
    sint16 OuterSize;
    sint16 OuterRadVel;
    sint16 pad3;
} SPLASH_SETUP;

void DetatchSpark(sint32 num, sint32 type);
sint32 GetFreeSpark(void);
void KillAllCurrentItems(sint16 item_number);
void TriggerRicochetSpark(GAME_VECTOR *pos, sint32 ang, sint32 num, sint32 smoke_only);
void ClearDynamics(void);
void SetupSplash(SPLASH_SETUP *setup);
void TriggerUnderwaterBlood(sint32 x, sint32 y, sint32 z, sint32 size);
void TriggerDynamic(sint32 x, sint32 y, sint32 z, sint32 falloff, sint32 r, sint32 g, sint32 b);
RIPPLE_STRUCT *SetupRipple(sint32 x, sint32 y, sint32 z, sint32 size, sint32 flags);
void ControlColouredLights(sint16 item_number);
void TriggerDartSmoke(sint32 x, sint32 y, sint32 z, sint32 xv, sint32 zv, sint32 hit);
void TriggerRocketSmoke(sint32 x, sint32 y, sint32 z, sint32 col);
void TriggerExplosionSmoke(sint32 x, sint32 y, sint32 z, sint32 uw);
void TriggerExplosionSmokeEnd(sint32 x, sint32 y, sint32 z, sint32 uw);

extern sint32 KillEverythingFlag;
extern DYNAMIC dynamics[MAX_DYNAMICS * 2];
extern sint32 number_dynamics;
extern RIPPLE_STRUCT ripples[16];
extern SPLASH_SETUP splash_setup;
extern SPARKS spark[256];

#endif
