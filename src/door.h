#ifndef DOOR_H
#define DOOR_H

#include "room.h"

typedef struct doorpos_data {
    FLOOR_INFO *floor;
    FLOOR_INFO data;
    sint16 block;
} DOORPOS_DATA;

void ShutThatDoor(DOORPOS_DATA *d);

#endif
