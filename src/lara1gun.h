#ifndef LARA1GUN_H
#define LARA1GUN_H

#include "lara.h"

void undraw_shotgun_meshes(sint32 weapon_type);
void ready_shotgun(int weapon_type);
void draw_shotgun_meshes(sint32 weapon_type);
void undraw_shotgun(sint32 weapon_type);
void TriggerUnderwaterExplosion(ITEM_INFO *item, sint32 vehicle);
void DoGrenadeDamageOnBaddie(ITEM_INFO *baddie, ITEM_INFO *item);
void draw_shotgun(sint32 weapon_type);
void FireShotgun(void);
void FireGrenade(void);

#endif
