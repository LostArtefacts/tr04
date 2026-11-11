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

void DartEmitterControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *dart;
    sint32 x, z, xLimit, zLimit, xv, zv, rand, i;
    sint16 num;

    item = &items[item_number];
    if (item->active) {
        if (item->timer <= 0) {
            item->timer = 24;
        } else {
            item->timer--;
            return;
        }
    }
    num = CreateItem();
    if (num != NO_ITEM) {
        x = 0;
        z = 0;
        dart = &items[num];
        dart->object_number = DARTS;
        dart->room_number = item->room_number;
        switch (item->pos.y_rot) {
        case 0:
            z = 512;
            break;
        case 0x4000:
            x = 512;
            break;
        case -0x4000:
            x = -512;
            break;
        case -0x8000:
            z = -512;
            break;
        }
        dart->pos.x_pos = x + item->pos.x_pos;
        dart->pos.y_pos = item->pos.y_pos - 512;
        dart->pos.z_pos = z + item->pos.z_pos;
        InitialiseItem(num);
        dart->pos.x_rot = 0;
        dart->pos.y_rot = item->pos.y_rot + 0x8000;
        dart->speed = 256;
        xLimit = 0;
        zLimit = 0;
        if (x)
            xLimit = abs(x << 1) - 1;
        else
            zLimit = abs(z << 1) - 1;
        for (i = 0; i < 5; i++) {
            rand = -GetRandomControl();
            if (z < 0)
                zv = -(zLimit & rand);
            else
                zv = zLimit & rand;
            if (x < 0)
                xv = -(xLimit & rand);
            else
                xv = xLimit & rand;
            TriggerDartSmoke(dart->pos.x_pos, dart->pos.y_pos, dart->pos.z_pos, xv, zv, 0);
        }
        AddActiveItem(num);
        dart->status = ITEM_ACTIVE;
        SoundEffect(SFX_DART_SPITT, &dart->pos, SFX_DEFAULT);
    }
}

void DartsControl(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 x, z, speed, i;
    sint16 room_num;

    item = &items[item_number];
    if (item->touch_bits) {
        lara_item->hit_points -= 25;
        lara_item->hit_status = 1;
        lara.poisoned += 160;
        DoBloodSplat(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, (GetRandomControl() & 3) + 4, lara_item->pos.y_rot, lara_item->room_number);
        KillItem(item_number);
    } else {
        x = item->pos.x_pos;
        z = item->pos.z_pos;
        speed = ((rcossin_tbl[((item->pos.x_rot >> 3) & 0x1FFE) + 1] << 2) * item->speed) >> 14;
        item->pos.z_pos += ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * speed) >> 14;
        item->pos.x_pos += ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * speed) >> 14;
        item->pos.y_pos -= ((rcossin_tbl[(item->pos.x_rot >> 3) & 0x1FFE] << 2) * item->speed) >> 14;
        room_num = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
        if (item->room_number != room_num)
            ItemNewRoom(item_number, room_num);
        item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        if (item->pos.y_pos >= item->floor) {
            for (i = 0; i < 4; i++)
                TriggerDartSmoke(x, item->pos.y_pos, z, 0, 0, 1);
            KillItem(item_number);
        }
    }
}

void FlameEmitter2Control(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 r, g;
    sint16 room_number;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (item->trigger_flags >= 0) {
        if (item->trigger_flags != 2) {
            if (item->trigger_flags == 123)
                AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 1, item->room_number, item->item_flags[3]);
            else
                AddFire(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 1 - item->trigger_flags, item->room_number, item->item_flags[3]);
        }
        if (!item->trigger_flags || item->trigger_flags == 2) {
            r = (GetRandomControl() & 0x3F) + 192;
            g = (GetRandomControl() & 0x1F) + 96;
            if (item->item_flags[3]) {
                r = (r * item->item_flags[3]) >> 8;
                g = (g * item->item_flags[3]) >> 8;
            }
            TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 10, r, g, 0);
        }
        if (item->trigger_flags == 2) {
            item->pos.x_pos += phd_sin(item->pos.y_rot + 0x8000) >> 11;
            item->pos.z_pos += phd_cos(item->pos.y_rot + 0x8000) >> 11;
            room_number = item->room_number;
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            if (room[room_number].flags & ROOM_UNDERWATER) {
                FlashFadeR = 255;
                FlashFadeG = 128;
                FlashFadeB = 0;
                FlashFader = 32;
                KillItem(item_number);
                return;
            }
            if (item->room_number != room_number)
                ItemNewRoom(item_number, room_number);
            item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            if (wibble & 7)
                TriggerFireFlame(item->pos.x_pos, item->pos.y_pos - 32, item->pos.z_pos, -1, 1);
        }
        SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
    } else if (!item->item_flags[0]) {
        FlipMap(-item->trigger_flags);
        flipmap[-item->trigger_flags] ^= IFL_CODEBITS;
        item->item_flags[0] = 1;
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

sint32 TestBoundsCollideTeethSpikes(ITEM_INFO *item)
{
    sint16 *bounds;
    sint32 x, y, z, rad, xMin, xMax, zMin, zMax;

    if (item->trigger_flags & 8) {
        x = item->pos.x_pos & ~0x3FF | 0x200;
        z = (item->pos.z_pos + SPxzoffs[item->trigger_flags & 7]) & ~0x3FF | 0x200;
    } else {
        z = item->pos.z_pos & ~0x3FF | 0x200;
        x = (item->pos.x_pos - SPxzoffs[item->trigger_flags & 7]) & ~0x3FF | 0x200;
    }
    if (item->trigger_flags & 1)
        rad = 300;
    else
        rad = 480;
    y = item->pos.y_pos + SPDETyoffs[item->trigger_flags & 7];
    bounds = GetBestFrame(lara_item);
    if (lara_item->pos.y_pos + bounds[2] > y || lara_item->pos.y_pos + bounds[3] < y - 900)
        return 0;
    xMin = lara_item->pos.x_pos + bounds[0];
    xMax = lara_item->pos.x_pos + bounds[1];
    zMin = lara_item->pos.z_pos + bounds[4];
    zMax = lara_item->pos.z_pos + bounds[5];
    return xMin <= x + rad && xMax >= x - rad && zMin <= z + rad && zMax >= z - rad;
}

void ControlRaisingBlock(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        if (!item->item_flags[2]) {
            AlterFloorHeight(item, -1024 << (item->object_number == RAISING_BLOCK2));
            item->item_flags[2] = 1;
        }
        if (item->item_flags[1] < 4096) {
            SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
            item->item_flags[1] += 64;
            if (item->trigger_flags && abs(item->pos.x_pos - lara_item->pos.x_pos) < 10240 && abs(item->pos.y_pos - lara_item->pos.y_pos) < 10240 && abs(item->pos.z_pos - lara_item->pos.z_pos) < 10240) {
                if (item->item_flags[1] == 64 || item->item_flags[1] == 4096)
                    camera.bounce = -32;
                else
                    camera.bounce = -16;
            }
        }
    } else if (item->item_flags[1] > 0) {
        SoundEffect(SFX_RUMBLE_NEXTDOOR, &item->pos, SFX_DEFAULT);
        if (item->trigger_flags && abs(item->pos.x_pos - lara_item->pos.x_pos) < 10240 && abs(item->pos.y_pos - lara_item->pos.y_pos) < 10240 && abs(item->pos.z_pos - lara_item->pos.z_pos) < 10240) {
            if (item->item_flags[1] == 64 || item->item_flags[1] == 4096)
                camera.bounce = -32;
            else
                camera.bounce = -16;
        }
        item->item_flags[1] -= 64;
    } else if (item->item_flags[2]) {
        AlterFloorHeight(item, 1024 << (item->object_number == RAISING_BLOCK2));
        item->item_flags[2] = 0;
    }
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

void ControlHammer(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    ROOM_INFO *r;
    sint32 hammered, frame, target_item;

    item = &items[item_number];
    frame = item->frame_number - anims[item->anim_number].frame_base;
    item->item_flags[3] = 150;
    if (TriggerActive(item)) {
        hammered = 0;
        if (item->trigger_flags) {
            if (item->current_anim_state == 1 && item->goal_anim_state == 1) {
                if (item->item_flags[2]) {
                    if (item->trigger_flags == 3) {
                        item->flags &= ~IFL_CODEBITS;
                        item->item_flags[2] = 0;
                    } else if (item->trigger_flags == 4)
                        item->item_flags[2]--;
                    else
                        item->item_flags[2] = 0;
                } else {
                    item->anim_number = objects[HAMMER].anim_index + 1;
                    item->frame_number = anims[item->anim_number].frame_base;
                    item->current_anim_state = item->goal_anim_state = 2;
                    item->item_flags[2] = 60;
                }
            } else {
                item->goal_anim_state = 1;
                *(sint32 *)&item->item_flags[0] = frame < 52 ? 0x7E0 : 0;
                if (frame == 8) {
                r = &room[item->room_number];
                    if (item->trigger_flags != 2) {
                        for (target_item = r->item_number; target_item != NO_ITEM; target_item = items[target_item].next_item) {
                            item2 = &items[target_item];
                            if (item2->object_number >= PUSHABLE_OBJECT1 && item2->object_number <= PUSHABLE_OBJECT4 && item2->pos.x_pos == item->pos.x_pos && item2->pos.z_pos == item->pos.z_pos) {
                                ExplodeItemNode(item2, 0, 0, 128);
                                KillItem(target_item);
                                hammered = 1;
                            }
                        }
                        if (hammered) {
                            for (target_item = r->item_number; target_item != NO_ITEM; target_item = items[target_item].next_item) {
                                item2 = &items[target_item];
                                if (item2->object_number == PUZZLE_ITEM4_COMBO1 ||
                                    item2->object_number == PUZZLE_ITEM4_COMBO2 ||
                                    item2->object_number == PUZZLE_ITEM5) {
                                    if (item2->pos.x_pos == item->pos.x_pos && item2->pos.z_pos == item->pos.z_pos)
                                        item2->status = ITEM_INACTIVE;
                                }
                            }
                        }
                    } else {
                        for (target_item = r->item_number; target_item != NO_ITEM; target_item = items[target_item].next_item) {
                            item2 = &items[target_item];
                            if (item2->object_number == OBELISK && item2->pos.y_rot == -0x4000 && items[item2->item_flags[0]].pos.y_rot == 0x4000 && !items[item2->item_flags[1]].pos.y_rot) {
                                item2->flags |= IFL_CODEBITS;
                                items[item2->item_flags[0]].flags |= IFL_CODEBITS;
                                items[item2->item_flags[1]].flags |= IFL_CODEBITS;
                                break;
                            }
                        }
                        SoundEffect(SFX_DOOR_GEN_THUD, &item->pos, SFX_DEFAULT);
                        SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
                    }
                } else if (frame > 52 && item->trigger_flags == 2)
                    item->flags &= ~IFL_CODEBITS;
            }
        } else
            *(sint32 *)&item->item_flags[0] = frame < 52 ? 0xE0 : 0;
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

void FallingSquishyBlockCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 i;

    item = &items[item_number];
    if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l)) {
        if (item->frame_number - anims[item->anim_number].frame_base > 8) {
            if (l->hit_points > 0)
                ItemPushLara(item, l, coll, 0, 1);
        } else {
            item->frame_number += 2;
            l->hit_points = 0;
            l->current_anim_state = AS_DEATH;
            l->goal_anim_state = AS_DEATH;
            l->anim_number = ANIM_FBLOCK_DEATH;
            l->frame_number = anims[ANIM_FBLOCK_DEATH].frame_base + 50;
            l->fallspeed = 0;
            l->speed = 0;
            for (i = 0; i < 12; i++)
                TriggerBlood(l->pos.x_pos, l->pos.y_pos - 128, l->pos.z_pos, GetRandomControl() << 1, 3);
        }
    }
}

void MineCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    ITEM_INFO *mines;
    sint32 j;
    sint16 i;

    item = &items[item_number];
    if (item->trigger_flags || item->item_flags[3])
        return;
    if (l->anim_number == ANIM_MINEDETECT && l->frame_number >= anims[ANIM_MINEDETECT].frame_base + 57) {
        mines = items;
        for (i = 0; i < level_items; i++, mines++) {
            if (mines->object_number != MINE || mines->status == ITEM_INVISIBLE || mines->trigger_flags)
                continue;
            TriggerExplosionSparks(mines->pos.x_pos, mines->pos.y_pos, mines->pos.z_pos, 3, -2, 0, mines->room_number);
            for (j = 0; j < 2; j++)
                TriggerExplosionSparks(mines->pos.x_pos, mines->pos.y_pos, mines->pos.z_pos, 3, -1, 0, mines->room_number);
            mines->mesh_bits = 1;
            ExplodeItemNode(mines, 0, 0, -32);
            KillItem(i);
            if (!(GetRandomControl() & 3))
                SoundEffect(SFX_MINE_EXP_OVERLAY, &mines->pos, SFX_DEFAULT);
            mines->status = ITEM_INVISIBLE;
        }
    } else if (TestBoundsCollide(item, l, 512)) {
        TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);
        for (i = 0; i < 2; i++)
            TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
        item->mesh_bits = 1;
        ExplodeItemNode(item, 0, 0, 128);
        KillItem(item_number);
        l->anim_number = ANIM_MINEDEATH;
        l->frame_number = anims[ANIM_MINEDEATH].frame_base;
        l->current_anim_state = AS_DEATH;
        l->speed = 0;
        SoundEffect(SFX_MINE_EXP_OVERLAY, &item->pos, SFX_DEFAULT);
    }
}
