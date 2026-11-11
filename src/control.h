#ifndef CONTROL_H
#define CONTROL_H

#include "items.h"
#include "room.h"

#define NO_HEIGHT (-32512)
#define NO_ROOM 255
#define DATA_TYPE 0x1F
#define END_BIT 0x8000

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

extern sint32 OnObject;
extern sint16 *floor_data;
extern sint32 flipeffect;
extern sint32 SetDebounce;

void TriggerNormalCDTrack(sint16 value, sint16 flags, sint16 type);
sint32 LOS(GAME_VECTOR *start, GAME_VECTOR *target);
sint32 xLOS(GAME_VECTOR *start, GAME_VECTOR *target);
sint32 zLOS(GAME_VECTOR *start, GAME_VECTOR *target);
sint32 ClipTarget(GAME_VECTOR *start, GAME_VECTOR *target);
FLOOR_INFO *GetFloor(sint32 x, sint32 y, sint32 z, sint16 *room_number);
sint32 GetHeight(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z);
sint32 GetWaterHeight(sint32 x, sint32 y, sint32 z, sint16 room_number);
sint32 GetCeiling(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z);
void AnimateItem(ITEM_INFO *item);
sint16 GetDoor(FLOOR_INFO *floor);
void TriggerCDTrack(sint16 value, sint16 flags, sint16 type);
void AlterFloorHeight(ITEM_INFO *item, sint32 height);

extern char PoisonFlag;
extern sint16 *trigger_index;

#endif
