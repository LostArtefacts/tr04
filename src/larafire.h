#ifndef LARAFIRE_H
#define LARAFIRE_H

#include "lara.h"

sint32 WeaponObject(sint32 weapon_type);
sint32 CheckForHoldingState(sint32 state);
void InitialiseNewWeapon(void);
sint32 WeaponObjectMesh(sint32 weapon_type);
sint16 *get_current_ammo_pointer(sint32 weapon_type);
void find_target_point(ITEM_INFO *item, GAME_VECTOR *target);
void HitTarget(ITEM_INFO *item, GAME_VECTOR *hitpos, sint32 damage, sint32 grenade);

extern sint16 HoldStates[18];

#endif
