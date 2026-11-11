#ifndef DOOR_H
#define DOOR_H

#include "room.h"

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

void ShutThatDoor(DOORPOS_DATA *d);
void OpenThatDoor(DOORPOS_DATA *d);
void PushPullKickDoorControl(sint16 item_number);

#endif
