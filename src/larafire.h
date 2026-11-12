#ifndef LARAFIRE_H
#define LARAFIRE_H

#include "lara.h"

typedef struct weapon_info {
    sint16 lock_angles[4];
    sint16 left_angles[4];
    sint16 right_angles[4];
    sint16 aim_speed;
    sint16 shot_accuracy;
    sint16 gun_height;
    sint16 target_dist;
    sint8 damage;
    uint8 recoil_frame;
    uint8 flash_time;
    uint8 draw_frame;
    sint16 sample_num;
} WEAPON_INFO;

sint32 WeaponObject(sint32 weapon_type);
sint32 CheckForHoldingState(sint32 state);
void InitialiseNewWeapon(void);
sint32 WeaponObjectMesh(sint32 weapon_type);
sint16 *get_current_ammo_pointer(sint32 weapon_type);
void find_target_point(ITEM_INFO *item, GAME_VECTOR *target);
void HitTarget(ITEM_INFO *item, GAME_VECTOR *hitpos, sint32 damage, sint32 grenade);
sint32 FireWeapon(sint32 weapon_type, ITEM_INFO *target, ITEM_INFO *src, sint16 *angles);
void DoProperDetection(sint16 item_number, sint32 x, sint32 y, sint32 z, sint32 xv, sint32 yv, sint32 zv);
void LaraTargetInfo(WEAPON_INFO *winfo);

extern WEAPON_INFO weapons[9];
extern sint16 HoldStates[18];

#endif
