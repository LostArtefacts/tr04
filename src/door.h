#ifndef DOOR_H
#define DOOR_H

#include "room.h"
#include "items.h"
#include "collide.h"

typedef struct doorpos_data {
    FLOOR_INFO *floor;
    FLOOR_INFO data;
    sint16 block;
} DOORPOS_DATA;

typedef struct door_data {
    DOORPOS_DATA d1;
    DOORPOS_DATA d1flip;
    DOORPOS_DATA d2;
    DOORPOS_DATA d2flip;
    sint16 Opened;
} DOOR_DATA;

extern PHD_VECTOR DoubleDoorPos;
extern PHD_VECTOR PullDoorPos;
extern PHD_VECTOR PushDoorPos;
extern PHD_VECTOR KickDoorPos;
extern PHD_VECTOR UnderwaterDoorPos;
extern PHD_VECTOR CrowbarDoorPos;
extern sint16 PushPullKickDoorBounds[12];
extern sint16 UnderwaterDoorBounds[12];
extern sint16 CrowbarDoorBounds[12];

void ShutThatDoor(DOORPOS_DATA *d);
void OpenThatDoor(DOORPOS_DATA *d);
void PushPullKickDoorControl(sint16 item_number);
void DoorControl(sint16 item_number);
void DoubleDoorCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll);
void UnderwaterDoorCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll);

#endif
