#ifndef EFFECTS_H
#define EFFECTS_H

#include "items.h"

typedef struct fx_info {
    PHD_3DPOS pos;
    sint16 room_number;
    sint16 object_number;
    sint16 next_fx;
    sint16 next_active;
    sint16 speed;
    sint16 fallspeed;
    sint16 frame_number;
    sint16 counter;
    sint16 shade;
    sint16 flag1;
    sint16 flag2;
} FX_INFO;

extern sint32 GlobalFogOff;
extern sint32 FogTableColor[28];

extern OBJECT_VECTOR *sound_effects;
extern sint32 number_sound_effects;
extern void (*effect_routines[47])(ITEM_INFO *item);

#define UNFLIPFLAG 0x80
#define FLIPFLAG 0x40

void turn180_effect(ITEM_INFO *item);
void Richochet(GAME_VECTOR *pos);
void ExplosionFX(ITEM_INFO *item);
void lara_hands_free(ITEM_INFO *item);
sint16 DoBloodSplat(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 direction, sint16 room_number);
void shoot_right_gun(ITEM_INFO *item);
void shoot_left_gun(ITEM_INFO *item);
void reset_hair(ITEM_INFO *item);
void draw_right_gun(ITEM_INFO *item);
void draw_left_gun(ITEM_INFO *item);
void DoLotsOfBlood(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 direction, sint16 room_num, sint32 number);
sint32 ItemNearLara(PHD_3DPOS *pos, sint32 distance);
void invisibility_on(ITEM_INFO *item);
void invisibility_off(ITEM_INFO *item);
void floor_shake_effect(ITEM_INFO *item);
void Splash(ITEM_INFO *item);
void finish_level_effect(ITEM_INFO *item);
void PoseidonSFX(ITEM_INFO *item);
void ActivateCamera(ITEM_INFO *item);
void ActivateKey(ITEM_INFO *item);
void LaraLocation(ITEM_INFO *item);
void LaraLocationPad(ITEM_INFO *item);
void SoundFlipEffect(ITEM_INFO *item);
void SwapCrowbar(ITEM_INFO *item);
void MeshSwapToPour(ITEM_INFO *item);
void MeshSwapFromPour(ITEM_INFO *item);
void ClearScarabsPatch(ITEM_INFO *item);
void swap_meshes_with_meshswap1(ITEM_INFO *item);
void swap_meshes_with_meshswap2(ITEM_INFO *item);
void swap_meshes_with_meshswap3(ITEM_INFO *item);
void RubbleFX(ITEM_INFO *item);
void GhostTrap(ITEM_INFO *item);
void SetFog(ITEM_INFO *item);
void WaterFall(sint16 item_number);
void SoundEffects(void);
void WadeSplash(ITEM_INFO *item, sint32 water, sint32 depth);

extern FX_INFO *effects;
extern sint16 next_fx_active;

#endif
