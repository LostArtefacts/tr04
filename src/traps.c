#include "traps.h"
#include "effects.h"
#include "items.h"
#include "lara.h"
#include "objnums.h"
#include "control.h"
#include "sound.h"
#include "tomb4fx.h"
#include "collide.h"
#include "switch.h"
#include "sphere.h"
#include "anim.h"
#include "camera.h"
#include "specific/3dmath.h"
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

void FallingBlockCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 x, z, tx, tz;

    item = &items[item_number];
    x = l->pos.x_pos;
    z = l->pos.z_pos;
    tx = item->pos.x_pos;
    tz = item->pos.z_pos;
    if (!item->item_flags[0] && !item->trigger_flags && item->pos.y_pos == l->pos.y_pos && !((tx ^ x) & ~1023) && !((z ^ tz) & ~1023)) {
        SoundEffect(SFX_ROCK_FALL_CRUMBLE, &item->pos, SFX_DEFAULT);
        AddActiveItem(item_number);
        item->item_flags[0] = 60;
        item->status = ITEM_ACTIVE;
        item->flags |= IFL_CODEBITS;
    }
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

void ControlSmashableBikeWall(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!TriggerActive(item) || lara.vehicle == NO_ITEM)
        return;
    if (TestBoundsCollide(item, &items[lara.vehicle], 1024)) {
        SoundEffect(SFX_BIKE_HIT_OBJECTS, &item->pos, SFX_DEFAULT);
        item->mesh_bits = -2;
        ExplodingDeath2(item_number, -1, 2305);
        item->mesh_bits = 0;
        TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
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

void TwoBlockPlatformFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    if (OnTwoBlockPlatform(item, x, z)) {
        if (y <= item->pos.y_pos + 32 && item->pos.y_pos < *height) {
            *height = item->pos.y_pos;
            OnObject = 1;
            height_type = 0;
        }
    }
}

void TwoBlockPlatformCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    if (OnTwoBlockPlatform(item, x, z)) {
        if (y > item->pos.y_pos + 32 && item->pos.y_pos > *height)
            *height = item->pos.y_pos + 256;
    }
}

sint32 OnTwoBlockPlatform(ITEM_INFO *item, sint32 x, sint32 z)
{
    sint32 tx, tz;

    if (!item->mesh_bits)
        return 0;
    x >>= 10;
    z >>= 10;
    tx = item->pos.x_pos >> 10;
    tz = item->pos.z_pos >> 10;
    if (!item->pos.y_rot && (x == tx || x == tx - 1) && (z == tz || z == tz + 1))
        return 1;
    if (item->pos.y_rot == -0x8000 && (x == tx || x == tx + 1) && (z == tz || z == tz - 1))
        return 1;
    if (item->pos.y_rot == 0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx + 1))
        return 1;
    if (item->pos.y_rot == -0x4000 && (z == tz || z == tz - 1) && (x == tx || x == tx - 1))
        return 1;
    return 0;
}

void ControlChain(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->trigger_flags) {
        item->item_flags[2] = 1;
        item->item_flags[3] = 75;
        if (TriggerActive(item)) {
            *(sint32 *)&item->item_flags[0] = 0x787E;
            AnimateItem(item);
            return;
        }
    } else {
        item->item_flags[3] = 25;
        if (TriggerActive(item)) {
            *(sint32 *)&item->item_flags[0] = 0x780;
            AnimateItem(item);
            return;
        }
    }
    *(sint32 *)&item->item_flags[0] = 0;
}

void ControlPlough(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->item_flags[3] = 50;
    if (TriggerActive(item)) {
        *(sint32 *)&item->item_flags[0] = 0x3F000;
        AnimateItem(item);
    } else
        *(sint32 *)&item->item_flags[0] = 0;
}

void ControlStargate(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->item_flags[3] = 50;
    if (TriggerActive(item)) {
        SoundEffect(SFX_STARGATE_SWIRL, &item->pos, SFX_DEFAULT);
        *(sint32 *)&item->item_flags[0] = 0x36DB600;
        AnimateItem(item);
    } else
        *(sint32 *)&item->item_flags[0] = 0;
}

void ControlCatwalkBlade(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        if (item->frame_number == anims[item->anim_number].frame_end || item->frame_number - anims[item->anim_number].frame_base < 38)
            item->item_flags[3] = 0;
        else
            item->item_flags[3] = 100;
        AnimateItem(item);
    } else
        item->frame_number = anims[item->anim_number].frame_base;
}

void ControlMovingBlade(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        item->item_flags[3] = 50;
        AnimateItem(item);
    } else
        item->frame_number = anims[item->anim_number].frame_base;
}

void ControlPlinthBlade(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        if (item->frame_number == anims[item->anim_number].frame_end)
            item->item_flags[3] = 0;
        else
            item->item_flags[3] = 200;
        AnimateItem(item);
    } else
        item->frame_number = anims[item->anim_number].frame_base;
}
