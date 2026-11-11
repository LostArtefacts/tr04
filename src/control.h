#ifndef CONTROL_H
#define CONTROL_H

#include "room.h"

#define NO_HEIGHT (-32512)

FLOOR_INFO *GetFloor(sint32 x, sint32 y, sint32 z, sint16 *room_number);
sint32 GetHeight(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z);
sint32 GetCeiling(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z);

extern char PoisonFlag;
extern sint16 *trigger_index;

#endif
