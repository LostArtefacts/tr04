#ifndef TOMB4FX_H
#define TOMB4FX_H

#include "types.h"
#include "3dglodef.h"

void TriggerShatterSmoke(sint32 x, sint32 y, sint32 z);
void TriggerBlood(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 num);
void CreateBubble(PHD_3DPOS *pos, sint16 room_number, sint32 size, sint32 biggest);
sint32 ExplodingDeath2(sint16 item_number, sint32 mesh_bits, sint16 Flags);

#endif
