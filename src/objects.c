#include "objects.h"
#include "room.h"
#include "control.h"
#include "lara.h"
#include "box.h"
#include "anim.h"
#include "sound.h"
#include "effects.h"
#include "tomb4fx.h"
#include "sphere.h"
#include "specific/3dmath.h"

void SmashObject(sint16 item_number)
{
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    ITEM_INFO *item;

    item = &items[item_number];
    r = &room[item->room_number];
    floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + ((item->pos.x_pos - r->x) >> WALL_SHIFT) * r->x_size];
    if (boxes[floor->box].overlap_index & 0x8000)
        boxes[floor->box].overlap_index &= ~0x4000;
    SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_DEFAULT);
    SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
    item->mesh_bits = 0xFFFE;
    item->collidable = 0;
    ExplodingDeath2(item_number, -1, 256);
    item->flags |= IFL_INVISIBLE;
    if (item->status == ITEM_ACTIVE)
        RemoveActiveItem(item_number);
    item->status = ITEM_DEACTIVATED;
}

void BridgeFlatFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    if (y <= item->pos.y_pos) {
        *height = item->pos.y_pos;
        height_type = WALL;
        OnObject = 1;
    }
}

void BridgeFlatCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    if (y > item->pos.y_pos)
        *height = item->pos.y_pos + STEP_L;
}

sint32 GetOffset(ITEM_INFO *item, sint32 x, sint32 z)
{
    if (item->pos.y_rot == 0)
        return (WALL_L - x) & (WALL_L - 1);
    else if (item->pos.y_rot == -0x8000)
        return x & (WALL_L - 1);
    else if (item->pos.y_rot == 0x4000)
        return z & (WALL_L - 1);
    return (WALL_L - z) & (WALL_L - 1);
}

void BridgeTilt1Floor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);
    if (y <= level) {
        *height = level;
        height_type = WALL;
        OnObject = 1;
    }
}

void BridgeTilt1Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);
    if (y > level)
        *height = level + STEP_L;
}

void BridgeTilt2Floor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);
    if (y <= level) {
        *height = level;
        height_type = WALL;
        OnObject = 1;
    }
}

void BridgeTilt2Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);
    if (y > level)
        *height = level + STEP_L;
}

