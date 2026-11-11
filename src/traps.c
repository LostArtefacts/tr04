#include "traps.h"
#include "effects.h"
#include "items.h"
#include "lara.h"
#include "objnums.h"
#include "control.h"
#include "sound.h"
#include "tomb4fx.h"
#include "collide.h"
#include "specific/function_stubs.h"

void TrapDoorControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        if (!item->current_anim_state && item->trigger_flags >= 0)
            item->goal_anim_state = 1;
    } else if (item->current_anim_state == 1)
        item->goal_anim_state = 0;
    AnimateItem(item);
    if (item->current_anim_state == 1 && item->item_flags[2])
        OpenTrapDoor(item);
    else if (!item->current_anim_state && !item->item_flags[2])
        CloseTrapDoor(item);
}

void FallingBlock(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->item_flags[0] > 1) {
        item->pos.z_rot = (GetRandomControl() & 0x3FF) - 512;
        item->pos.x_rot = (GetRandomControl() & 0x3FF) - 512;
        item->item_flags[0]--;
    } else {
        item->mesh_bits = -2;
        ExplodingDeath2(item_number, -1, 2465);
        KillItem(item_number);
    }
}

void FallingBlockFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 tx, tz;

    tx = x ^ item->pos.x_pos;
    tz = z ^ item->pos.z_pos;
    if (tx & ~1023 || tz & ~1023)
        return;
    if (y <= item->pos.y_pos) {
        *height = item->pos.y_pos;
        height_type = WALL;
        OnObject = 1;
    }
}

void FallingBlockCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 tx, tz;

    tx = x ^ item->pos.x_pos;
    tz = z ^ item->pos.z_pos;
    if (tx & ~1023 || tz & ~1023)
        return;
    if (y > item->pos.y_pos)
        *height = item->pos.y_pos + 256;
}

void ControlFallingBlock2(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (lara_item->pos.y_pos == item->pos.y_pos && OnTwoBlockPlatform(item, lara_item->pos.x_pos, lara_item->pos.z_pos) && lara.vehicle != NO_ITEM) {
        SoundEffect(SFX_BIKE_HIT_OBJECTS, &item->pos, SFX_DEFAULT);
        item->mesh_bits = -2;
        ExplodingDeath2(item_number, -1, 417);
        KillItem(item_number);
    }
}

void FallingCeiling(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint16 room_number;

    item = &items[item_number];
    if (!item->current_anim_state) {
        item->gravity_status = 1;
        item->goal_anim_state = 1;
    } else if (item->current_anim_state == 1 && item->touch_bits) {
        lara_item->hit_points -= 300;
        lara_item->hit_status = 1;
    }
    AnimateItem(item);
    if (item->status == ITEM_DEACTIVATED) {
        RemoveActiveItem(item_number);
        return;
    }
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (room_number != item->room_number)
        ItemNewRoom(item_number, room_number);
    if (item->current_anim_state == 1 && item->pos.y_pos >= item->floor) {
        item->pos.y_pos = item->floor;
        item->goal_anim_state = 2;
        item->fallspeed = 0;
        item->gravity_status = 0;
    }
}

void LaraBurn(void)
{
    sint16 fx_number;

    if (!lara.burn && !lara.BurnGreen) {
        fx_number = CreateEffect(lara_item->room_number);
        if (fx_number != NO_ITEM) {
            effects[fx_number].object_number = FLAME;
            lara.burn = 1;
        }
    }
}

void LavaBurn(ITEM_INFO *item)
{
    FLOOR_INFO *floor;
    sint16 room_number;

    if (item->hit_points >= 0 && lara.water_status != LW_FLYCHEAT) {
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, 32000, item->pos.z_pos, &room_number);
        if (item->floor == GetHeight(floor, item->pos.x_pos, 32000, item->pos.z_pos)) {
            item->hit_status = 1;
            item->hit_points = -1;
            LaraBurn();
        }
    }
}
