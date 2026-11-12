#include "moveblok.h"
#include "box.h"
#include "control.h"
#include "room.h"

void InitialiseMovingBlock(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    ClearMovableBlockSplitters(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
}

void ClearMovableBlockSplitters(sint32 x, sint32 y, sint32 z, sint16 room_number)
{
    sint16 height, base_room_number;
    FLOOR_INFO *floor;

    floor = GetFloor(x, y, z, &room_number);
    boxes[floor->box].overlap_index &= ~BLOCKED;
    height = boxes[floor->box].height;
    base_room_number = room_number;
    floor = GetFloor(x + WALL_L, y, z, &room_number);
    if (floor->box != NO_BOX) {
        if (boxes[floor->box].height == height && (boxes[floor->box].overlap_index & BLOCKABLE) && (boxes[floor->box].overlap_index & BLOCKED))
            ClearMovableBlockSplitters(x + WALL_L, y, z, room_number);
    }
    room_number = base_room_number;
    floor = GetFloor(x - WALL_L, y, z, &room_number);
    if (floor->box != NO_BOX) {
        if (boxes[floor->box].height == height && (boxes[floor->box].overlap_index & BLOCKABLE) && (boxes[floor->box].overlap_index & BLOCKED))
            ClearMovableBlockSplitters(x - WALL_L, y, z, room_number);
    }
    room_number = base_room_number;
    floor = GetFloor(x, y, z + WALL_L, &room_number);
    if (floor->box != NO_BOX) {
        if (boxes[floor->box].height == height && (boxes[floor->box].overlap_index & BLOCKABLE) && (boxes[floor->box].overlap_index & BLOCKED))
            ClearMovableBlockSplitters(x, y, z + WALL_L, room_number);
    }
    room_number = base_room_number;
    floor = GetFloor(x, y, z - WALL_L, &room_number);
    if (floor->box != NO_BOX) {
        if (boxes[floor->box].height == height && (boxes[floor->box].overlap_index & BLOCKABLE) && (boxes[floor->box].overlap_index & BLOCKED))
            ClearMovableBlockSplitters(x, y, z - WALL_L, room_number);
    }
}
