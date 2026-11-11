#ifndef TOMB4FX_H
#define TOMB4FX_H

#include "types.h"
#include "3dglodef.h"

void TriggerShatterSmoke(sint32 x, sint32 y, sint32 z);
void TriggerBlood(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 num);
void CreateBubble(PHD_3DPOS *pos, sint16 room_number, sint32 size, sint32 biggest);
sint32 ExplodingDeath2(sint16 item_number, sint32 mesh_bits, sint16 Flags);
void AddFire(sint32 x, sint32 y, sint32 z, sint32 size, sint16 room_number, sint16 fade);
void TriggerShockwave(PHD_VECTOR *pos, sint32 InnerOuterRads, sint32 speed, sint32 rgb, sint32 XRotFlags);

extern sint16 FlashFadeR;
extern sint16 FlashFadeG;
extern sint16 FlashFadeB;
extern sint16 FlashFader;

#endif
