#include "people.h"
#include "items.h"
#include "lara.h"
#include "effects.h"
#include "specific/function_stubs.h"
#include "room.h"

sint16 GunShot(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number)
{
    return NO_ITEM;
}

sint16 GunMiss(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number)
{
    GAME_VECTOR pos;

    pos.x = lara_item->pos.x_pos + (GetRandomControl() - 0x4000) * (WALL_L / 2) / 0x7fff;
    pos.y = lara_item->floor;
    pos.z = lara_item->pos.z_pos + (GetRandomControl() - 0x4000) * (WALL_L / 2) / 0x7fff;
    pos.room_number = lara_item->room_number;
    Richochet(&pos);
    return GunShot(x, y, z, speed, yrot, room_number);
}
