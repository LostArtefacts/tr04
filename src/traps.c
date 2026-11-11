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
#include "specific/input.h"
#include "effect2.h"
#include "draw.h"
#include "delstuff.h"
#include "lara_states.h"
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

void OpenTrapDoor(ITEM_INFO *item)
{
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    uint16 pitsky;

    pitsky = item->item_flags[3];
    r = &room[item->room_number];
    floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)];
    if (item->pos.y_pos == r->minfloor) {
        floor->pit_room = pitsky & 0xFF;
        r = &room[floor->pit_room];
        floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)];
        floor->sky_room = pitsky >> 8;
    } else {
        floor->sky_room = pitsky >> 8;
        r = &room[floor->sky_room];
        floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)];
        floor->pit_room = pitsky & 0xFF;
    }
    item->item_flags[2] = 0;
}

void FloorTrapDoorCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 y;

    item = &items[item_number];
    if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH
        && lara.gun_status == LG_NO_ARMS || lara.IsMoving && lara.GeneralPtr == (void *)item_number) {
        if (TestLaraPosition(FloorTrapDoorBounds, item, l)) {
            if (MoveLaraPosition(&FloorTrapDoorPos, item, l)) {
                l->anim_number = ANIM_LIFTTRAP;
                l->frame_number = anims[ANIM_LIFTTRAP].frame_base;
                l->current_anim_state = AS_LIFTTRAP;
                lara.IsMoving = 0;
                lara.head_y_rot = 0;
                lara.head_x_rot = 0;
                lara.torso_y_rot = 0;
                lara.torso_x_rot = 0;
                lara.gun_status = LG_HANDS_BUSY;
                AddActiveItem(item_number);
                item->goal_anim_state = 1;
                item->status = ITEM_ACTIVE;
                UseForcedFixedCamera = 1;
                ForcedFixedCamera.x = item->pos.x_pos - ((2048 * phd_sin(item->pos.y_rot) >> W2V_SHIFT));
                ForcedFixedCamera.z = item->pos.z_pos - ((2048 * phd_cos(item->pos.y_rot) >> W2V_SHIFT));
                y = item->pos.y_pos - 2048;
                if (y < room[item->room_number].maxceiling)
                    y = room[item->room_number].maxceiling;
                ForcedFixedCamera.y = y;
                ForcedFixedCamera.room_number = item->room_number;
            } else
                lara.GeneralPtr = (void *)item_number;
        }
    } else if (item->current_anim_state == 1)
        UseForcedFixedCamera = 0;
}

void CeilingTrapDoorCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (input & IN_ACTION && item->status != ITEM_ACTIVE && l->current_anim_state == AS_UPJUMP && l->gravity_status && lara.gun_status == LG_NO_ARMS && TestLaraPosition(CeilingTrapDoorBounds, item, l)) {
        AlignLaraPosition(&CeilingTrapDoorPos, item, l);
        lara.head_y_rot = 0;
        lara.head_x_rot = 0;
        lara.torso_y_rot = 0;
        lara.torso_x_rot = 0;
        lara.gun_status = LG_HANDS_BUSY;
        l->gravity_status = 0;
        l->fallspeed = 0;
        l->anim_number = ANIM_PULLTRAP;
        l->frame_number = anims[ANIM_PULLTRAP].frame_base;
        l->current_anim_state = AS_PULLTRAP;
        AddActiveItem(item_number);
        item->status = ITEM_ACTIVE;
        item->goal_anim_state = 1;
        UseForcedFixedCamera = 1;
        ForcedFixedCamera.x = item->pos.x_pos - ((1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT);
        ForcedFixedCamera.y = item->pos.y_pos + 1024;
        ForcedFixedCamera.z = item->pos.z_pos - ((1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT);
        ForcedFixedCamera.room_number = item->room_number;
    } else if (item->current_anim_state == 1)
        UseForcedFixedCamera = 0;
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

void RollingBallCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!TestBoundsCollide(item, l, coll->radius) || !TestCollision(item, l))
        return;
    if (TriggerActive(item) && (item->item_flags[0] || item->fallspeed)) {
        lara_item->anim_number = ANIM_RBALL_DEATH;
        lara_item->frame_number = anims[ANIM_RBALL_DEATH].frame_base;
        lara_item->current_anim_state = lara_item->goal_anim_state = AS_DEATH;
        lara_item->gravity_status = 0;
    } else
        ObjectCollision(item_number, l, coll);
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

void ControlTwoBlockPlatform(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 height;
    sint16 room_number;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (item->trigger_flags) {
        if (item->pos.y_pos > item->item_flags[0] - ((item->trigger_flags & ~0xF) << 4))
            item->pos.y_pos -= item->trigger_flags & 0xF;
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        if (room_number != item->room_number)
            ItemNewRoom(item_number, room_number);
    } else {
        OnObject = 0;
        height = lara_item->pos.y_pos + 1;
        TwoBlockPlatformFloor(item, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &height);
        if (!OnObject || lara_item->anim_number == 89)
            item->item_flags[1] = -1;
        else
            item->item_flags[1] = 1;
        if (item->item_flags[1] > 0) {
            if (item->pos.y_pos < item->item_flags[0] + 128) {
                SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
                item->pos.y_pos += 4;
            } else {
                item->item_flags[1] = -1;
            }
        } else if (item->item_flags[1] < 0) {
            if (item->pos.y_pos > item->item_flags[0]) {
                SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
                item->pos.y_pos -= 4;
            } else {
                item->item_flags[1] = 1;
            }
        }
    }
}

void ControlSlicerDicer(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 distance;
    sint16 room_number;

    item = &items[item_number];
    SoundEffect(SFX_METAL_SCRAPE_LOOP, &item->pos, SFX_DEFAULT);
    SoundEffect(SFX_METAL_SCRAPE_LOOP1, &item->pos, SFX_DEFAULT);
    distance = 4608 * phd_cos(item->trigger_flags) >> 14;
    item->pos.x_pos = 256 * item->item_flags[0] + (((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * distance) >> 14);
    item->pos.y_pos = 256 * item->item_flags[1] - (4608 * phd_sin(item->trigger_flags) >> 14);
    item->pos.z_pos = 256 * item->item_flags[2] + (((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * distance) >> 14);
    item->trigger_flags += 170;
    room_number = item->room_number;
    GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (item->room_number != room_number)
        ItemNewRoom(item_number, room_number);
    AnimateItem(item);
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

void ControlSpikeball(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 frame;

    item = &items[item_number];
    frame = item->frame_number - anims[item->anim_number].frame_base;
    if (TriggerActive(item)) {
        if ((frame > 14 && frame < 24) || (frame >= 138 && frame <= 140)) {
            item->item_flags[3] = 150;
            *(sint32 *)&item->item_flags[0] = 0x7FF800;
        } else if (frame >= 141) {
            item->item_flags[3] = 50;
            *(sint32 *)&item->item_flags[0] = 0x7FF800;
        } else {
            *(sint32 *)&item->item_flags[0] = 0;
        }
        AnimateItem(item);
    } else {
        item->frame_number = anims[item->anim_number].frame_base;
        *(sint32 *)&item->item_flags[0] = 0;
    }
}

void Control4xFloorRoofBlade(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 frame;

    item = &items[item_number];
    if (TriggerActive(item)) {
        frame = item->frame_number - anims[item->anim_number].frame_base;
        if (frame <= 5 || frame >= 58 || (frame >= 8 && frame <= 54)) {
            *(sint32 *)&item->item_flags[0] = 0;
        } else if (frame >= 6 && frame <= 7) {
            item->item_flags[3] = 20;
            *(sint32 *)&item->item_flags[0] = 30;
        } else if (frame >= 55 && frame <= 57) {
            item->item_flags[3] = 200;
            *(sint32 *)&item->item_flags[0] = 30;
        }
        AnimateItem(item);
    } else {
        item->frame_number = anims[item->anim_number].frame_base;
        *(sint32 *)&item->item_flags[0] = 0;
    }
}

void ControlBirdBlade(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 frame;

    item = &items[item_number];
    item->item_flags[3] = 100;
    if (TriggerActive(item)) {
        frame = item->frame_number - anims[item->anim_number].frame_base;
        if (frame <= 14 || frame >= 31)
            *(sint32 *)&item->item_flags[0] = 0;
        else
            *(sint32 *)&item->item_flags[0] = 6;
        AnimateItem(item);
    } else {
        item->frame_number = anims[item->anim_number].frame_base;
        *(sint32 *)&item->item_flags[0] = 0;
    }
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

void ControlSethBlade(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 frame;

    item = &items[item_number];
    *(sint32 *)&item->item_flags[0] = 0;
    if (!TriggerActive(item))
        return;
    if (item->current_anim_state == 2) {
        if (item->item_flags[2] > 1)
            item->item_flags[2]--;
        else if (item->item_flags[2] == 1) {
            item->goal_anim_state = 1;
            item->item_flags[2] = 0;
        } else if (!item->item_flags[2] && item->trigger_flags > 0)
            item->item_flags[2] = item->trigger_flags;
    } else {
        frame = item->frame_number - anims[item->anim_number].frame_base;
        if (frame && frame <= 6)
            *(sint32 *)&item->item_flags[0] = -1;
        else if (frame >= 7 && frame <= 15)
            *(sint32 *)&item->item_flags[0] = 448;
        else
            *(sint32 *)&item->item_flags[0] = 0;
        item->item_flags[3] = 1000;
    }
    AnimateItem(item);
}

void ControlFallingSquishyBlock(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        if (item->item_flags[0] < 60) {
            SoundEffect(SFX_EARTHQUAKE_LOOP, &item->pos, SFX_DEFAULT);
            camera.bounce = (item->item_flags[0] - 92) >> 1;
            item->item_flags[0]++;
        } else {
            if (item->frame_number - anims[item->anim_number].frame_base == 8)
                camera.bounce = -96;
            AnimateItem(item);
        }
    }
}
