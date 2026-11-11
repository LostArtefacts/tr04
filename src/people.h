#ifndef PEOPLE_H
#define PEOPLE_H

#include "box.h"

sint16 GunShot(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number);
sint16 GunMiss(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number);
sint32 TargetVisible(ITEM_INFO *item, AI_INFO *info);
sint32 Targetable(ITEM_INFO *item, AI_INFO *info);
sint16 GunHit(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number);

#endif
