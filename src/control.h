#ifndef CONTROL_H
#define CONTROL_H

#include "items.h"
#include "room.h"

#define NO_HEIGHT (-32512)

enum floor_types {
    FLOOR_TYPE,
    DOOR_TYPE,
    TILT_TYPE,
    ROOF_TYPE,
    TRIGGER_TYPE,
    LAVA_TYPE,
    CLIMB_TYPE,
    SPLIT1,
    SPLIT2,
    SPLIT3,
    SPLIT4,
    NOCOLF1T,
    NOCOLF1B,
    NOCOLF2T,
    NOCOLF2B,
    NOCOLC1T,
    NOCOLC1B,
    NOCOLC2T,
    NOCOLC2B,
    MONKEY_TYPE,
    TRIGTRIGGER_TYPE,
    MINER_TYPE
};

FLOOR_INFO *GetFloor(sint32 x, sint32 y, sint32 z, sint16 *room_number);
sint32 GetHeight(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z);
sint32 GetCeiling(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z);
void AnimateItem(ITEM_INFO *item);

extern char PoisonFlag;
extern sint16 *trigger_index;

#endif
