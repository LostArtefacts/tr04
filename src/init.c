#include <string.h>
#include "init.h"
#include "items.h"
#include "objects.h"
#include "objnums.h"
#include "anim.h"
#include "room.h"
#include "control.h"
#include "door.h"
#include "traps.h"
#include "draw.h"
#include "rope.h"
#include "scarab.h"
#include "debris.h"
#include "box.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "pickup.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseTrapDoor(sint16 item_number)
{
    CloseTrapDoor(&items[item_number]);
}

void InitialiseFallingBlock2(sint16 item_number)
{
    items[item_number].mesh_bits = 1;
}

void InitialiseFlameEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->trigger_flags < 0) {
        item->item_flags[0] = (GetRandomControl() & 0x3F) + 90;
        item->item_flags[2] = 256;
        if ((-item->trigger_flags & 7) == 7) {
            switch (item->pos.y_rot) {
            case 0:
                item->pos.z_pos += 512;
                break;
            case 0x4000:
                item->pos.x_pos += 512;
                break;
            case -0x8000:
                item->pos.z_pos -= 512;
                break;
            case -0x4000:
                item->pos.x_pos -= 512;
                break;
            }
        }
    }
}

void InitialiseFlameEmitter2(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->pos.y_pos -= 64;
    if (item->trigger_flags != 123) {
        switch (item->pos.y_rot) {
        case 0:
            if (item->trigger_flags == 2)
                item->pos.z_pos += 80;
            else
                item->pos.z_pos += 256;
            break;
        case 0x4000:
            if (item->trigger_flags == 2)
                item->pos.x_pos += 80;
            else
                item->pos.x_pos += 256;
            break;
        case -0x8000:
            if (item->trigger_flags == 2)
                item->pos.z_pos -= 80;
            else
                item->pos.z_pos -= 256;
            break;
        case -0x4000:
            if (item->trigger_flags == 2)
                item->pos.x_pos -= 80;
            else
                item->pos.x_pos -= 256;
            break;
        }
    }
}

void InitialiseFlameEmitter3(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint32 i;
    sint16 tf;

    item = &items[item_number];
    tf = item->trigger_flags;
    if (tf < 3)
        return;
    item2 = items;
    for (i = 0; i < level_items; i++, item2++) {
        if (item2->object_number == ANIMATING3) {
            if (item2->trigger_flags == tf)
                item->item_flags[2] = i;
            else if (!item2->trigger_flags)
                item->item_flags[3] = i;
        }
    }
}

void InitialiseJobySpike(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 h, c;
    sint16 room_number;

    item = &items[item_number];
    item->pos.y_rot = (sint16)(GetRandomControl() << 10);
    item->item_flags[2] = GetRandomControl() & 1;
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    item->item_flags[3] = (sint16)((4096 * (h - c)) / 3328);
}

void InitialiseTwoBlockPlatform(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->item_flags[0] = (sint16)item->pos.y_pos;
    item->item_flags[1] = 1;
}

void InitialiseSlicerDicer(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->pos.z_pos += phd_cos(item->pos.y_rot + 0x4000) >> 5;
    item->pos.x_pos += phd_sin(item->pos.y_rot + 0x4000) >> 5;
    item->item_flags[0] = (sint16)(item->pos.x_pos >> 8);
    item->item_flags[1] = (sint16)((item->pos.y_pos - 4608) >> 8);
    item->item_flags[3] = 50;
    item->item_flags[2] = (sint16)(item->pos.z_pos >> 8);
}

void InitialiseScaledSpike(sint16 item_number)
{
    sint16 xzrots[8];
    ITEM_INFO *item;

    xzrots[0] = -0x8000;
    xzrots[1] = -0x6000;
    xzrots[2] = -0x4000;
    xzrots[3] = -0x2000;
    xzrots[4] = 0;
    xzrots[5] = 0x2000;
    xzrots[6] = 0x4000;
    xzrots[7] = 0x6000;
    item = &items[item_number];
    item->status = ITEM_INVISIBLE;
    if (item->trigger_flags & 8) {
        item->pos.x_rot = xzrots[item->trigger_flags & 7];
        item->pos.z_pos -= SPxzoffs[item->trigger_flags & 7];
        item->pos.y_rot = 0x4000;
    } else {
        item->pos.z_rot = xzrots[item->trigger_flags & 7];
        item->pos.x_pos += SPxzoffs[item->trigger_flags & 7];
    }
    item->pos.y_pos += SPyoffs[item->trigger_flags & 7];
    item->item_flags[0] = 1024;
    item->item_flags[2] = 0;
}

void InitialiseRaisingBlock(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint16 room_num;

    item = &items[item_number];
    room_num = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
    boxes[floor->box].overlap_index &= 0xBFFF;
    if (item->object_number == EXPANDING_PLATFORM) {
        switch (item->pos.y_rot) {
        case 0:
            item->pos.z_pos += 511;
            break;
        case 0x4000:
            item->pos.x_pos += 511;
            break;
        case -0x8000:
            item->pos.z_pos -= 511;
            break;
        case -0x4000:
            item->pos.x_pos -= 511;
            break;
        }
    }
    if (item->trigger_flags == 2) {
        item->flags |= IFL_CODEBITS;
        AddActiveItem(item_number);
        item->status = ITEM_ACTIVE;
    }
}

void InitialiseBurningFloor(sint16 item_number)
{
    items[item_number].required_anim_state = 127;
}

void InitialiseSethBlade(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->anim_number = objects[SETH_BLADE].anim_index + 1;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 2;
    item->item_flags[2] = abs(item->trigger_flags);
}

void InitialiseObelisk(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint16 *ifl;
    sint32 i;

    item = &items[item_number];
    item->anim_number = objects[item->object_number].anim_index + 3;
    item->frame_number = anims[item->anim_number].frame_base;
    AddActiveItem(item_number);
    item->status = ITEM_ACTIVE;
    if (item->trigger_flags == 2) {
        ifl = item->item_flags;
        item2 = items;
        for (i = 0; i < level_items; i++, item2++) {
            if (item2->object_number == OBELISK && i != item_number)
                *ifl++ = i;
            if (item2->object_number == ANIMATING3)
                item->item_flags[2] = i;
        }
    }
}

void InitialiseMineHelicopter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!item->trigger_flags)
        item->mesh_bits = 0;
}

void InitialiseSmashObject(sint16 item_number)
{
    ITEM_INFO *item;
    ROOM_INFO *rinfo;
    FLOOR_INFO *floor;

    item = &items[item_number];
    item->flags = 0;
    item->mesh_bits = 1;
    rinfo = &room[item->room_number];
    floor = &rinfo->floor[((item->pos.z_pos - rinfo->z) >> 10) + ((item->pos.x_pos - rinfo->x) >> 10) * rinfo->x_size];
    if (boxes[floor->box].overlap_index & 0x8000)
        boxes[floor->box].overlap_index |= 0x4000;
}

void InitialiseMapper(sint16 item_number)
{
    items[item_number].mesh_bits = 0xFFFFFFFD;    
}

void InitialiseLightningConductor(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint32 pack, i;

    item = &items[item_number];
    if (item->trigger_flags == 2) {
        pack = 0;
        item2 = items;
        for (i = 0; i < level_items; i++, item2++) {
            if (item2->object_number == ANIMATING8) {
                item->item_flags[2] |= i << (pack != 0 ? 8 : 0);
                pack++;
            }
        }
    }
}

void InitialiseStatuePlinth(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!item->trigger_flags)
        item->mesh_bits = 1;
}

void InitialiseEffects(void)
{
    sint32 i;

    memset(spark, 0, sizeof(spark));
    memset(fire_spark, 0, sizeof(fire_spark));
    memset(smoke_spark, 0, sizeof(smoke_spark));
    memset(Gunshells, 0, sizeof(Gunshells));
    memset(Gunflashes, 0, sizeof(Gunflashes));
    memset(debris, 0, sizeof(debris));
    memset(blood, 0, sizeof(blood));
    memset(splashes, 0, sizeof(splashes));
    memset(ripples, 0, sizeof(ripples));
    memset(Bubbles, 0, sizeof(Bubbles));
    memset(Drips, 0, sizeof(Drips));
    memset(ShockWaves, 0, sizeof(ShockWaves));
    for (i = 0; i < 256; i++)
        spark[i].Dynamic = -1;
    next_fire_spark = 1;
    next_smoke_spark = 0;
    next_gunshell = 0;
    next_bubble = 0;
    next_drip = 0;
    next_debris = 0;
    next_blood = 0;
    ClearScarabs();
    Func403620();
}

void InitialiseSmokeEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->object_number != STEAM_EMITTER)
        return;
    if (item->trigger_flags & 8) {
        item->item_flags[0] = item->trigger_flags >> 4;
        switch (item->pos.y_rot) {
        case 0:
            item->pos.z_pos += 320;
            break;
        case 0x4000:
            item->pos.x_pos += 320;
            break;
        case -0x4000:
            item->pos.x_pos -= 320;
            break;
        case -0x8000:
            item->pos.z_pos -= 320;
            break;
        }
    } else if (room[item->room_number].flags & ROOM_UNDERWATER && item->trigger_flags == 1) {
        item->item_flags[0] = 20;
        item->item_flags[1] = 1;
    }
}

void InitialiseDoor(sint16 item_number)
{
    ITEM_INFO *item;
    ROOM_INFO *r;
    ROOM_INFO *b;
    DOOR_DATA *door;
    sint32 dx, dy;
    sint16 two_room, box_number, room_number;

    item = &items[item_number];
    door = (DOOR_DATA *)game_malloc(sizeof(DOOR_DATA));
    item->data = door;
    door->Opened = 0;
    dx = 0;
    dy = 0;
    if (!item->pos.y_rot)
        dx = -1;
    else if (item->pos.y_rot == -32768)
        dx = 1;
    else if (item->pos.y_rot == 16384)
        dy = -1;
    else
        dy = 1;
    r = &room[item->room_number];
    door->d1.floor = &r->floor[(((item->pos.z_pos - r->z) >> 10) + dx) + (((item->pos.x_pos - r->x) >> 10) + dy) * r->x_size];
    room_number = GetDoor(door->d1.floor);
    if (room_number == 255)
        box_number = door->d1.floor->box;
    else {
        b = &room[room_number];
        box_number = b->floor[(((item->pos.z_pos - b->z) >> 10) + dx) + (((item->pos.x_pos - b->x) >> 10) + dy) * b->x_size].box;
    }
    door->d1.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
    memcpy(&door->d1.data, door->d1.floor, sizeof(FLOOR_INFO));
    if (r->flipped_room != -1) {
        r = &room[r->flipped_room];
        door->d1flip.floor = &r->floor[(((item->pos.z_pos - r->z) >> 10) + dx) + (((item->pos.x_pos - r->x) >> 10) + dy) * r->x_size];
        room_number = GetDoor(door->d1flip.floor);
        if (room_number == 255)
            box_number = door->d1flip.floor->box;
        else {
            b = &room[room_number];
            box_number = b->floor[(((item->pos.z_pos - b->z) >> 10) + dx) + (((item->pos.x_pos - b->x) >> 10) + dy) * b->x_size].box;
        }
        door->d1flip.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
        memcpy(&door->d1flip.data, door->d1flip.floor, sizeof(FLOOR_INFO));
    } else
        door->d1flip.floor = 0;
    two_room = GetDoor(door->d1.floor);
    ShutThatDoor(&door->d1);
    ShutThatDoor(&door->d1flip);
    if (two_room == 255) {
        door->d2.floor = 0;
        door->d2flip.floor = 0;
    } else {
        r = &room[two_room];
        door->d2.floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + ((item->pos.x_pos - r->x) >> 10) * r->x_size];
        room_number = GetDoor(door->d2.floor);
        if (room_number == 255)
            box_number = door->d2.floor->box;
        else {
            b = &room[room_number];
            box_number = b->floor[((item->pos.z_pos - b->z) >> 10) + ((item->pos.x_pos - b->x) >> 10) * b->x_size].box;
        }
        door->d2.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
        memcpy(&door->d2.data, door->d2.floor, sizeof(FLOOR_INFO));
        if (r->flipped_room != -1) {
            r = &room[r->flipped_room];
            door->d2flip.floor = &r->floor[((item->pos.z_pos - r->z) >> 10) + ((item->pos.x_pos - r->x) >> 10) * r->x_size];
            room_number = GetDoor(door->d2flip.floor);
            if (room_number == 255)
                box_number = door->d2flip.floor->box;
            else {
                b = &room[room_number];
                box_number = b->floor[((item->pos.z_pos - b->z) >> 10) + ((item->pos.x_pos - b->x) >> 10) * b->x_size].box;
            }
            door->d2flip.block = (boxes[box_number].overlap_index & 0x8000) ? box_number : 2047;
            memcpy(&door->d2flip.data, door->d2flip.floor, sizeof(FLOOR_INFO));
        } else
            door->d2flip.floor = 0;
        ShutThatDoor(&door->d2);
        ShutThatDoor(&door->d2flip);
        room_number = item->room_number;
        ItemNewRoom(item_number, two_room);
        item->room_number = room_number;
    }
}

void InitialisePulley(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->item_flags[3] = item->trigger_flags;
    item->trigger_flags = abs(item->trigger_flags);
    if (item->status == ITEM_INVISIBLE) {
        item->item_flags[1] = 1;
        item->status = ITEM_INACTIVE;
    }
}

void InitialisePickUp(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 *bounds;

    item = &items[item_number];
    if (!(item->trigger_flags & 0x3F) || (item->trigger_flags & 0x3F) == 3 || (item->trigger_flags & 0x3F) == 4) {
        bounds = GetBoundsAccurate(item);
        item->pos.y_pos -= bounds[3];
    }
    if (item->trigger_flags & 128)
        RPickups[NumRPickups++] = (uint8)item_number;
    if ((sint16)(item->trigger_flags & 256))
        item->mesh_bits = 0;
    if (item->status == ITEM_INVISIBLE)
        item->flags |= IFL_TRIGGERED;
}

void init_all_ropes(void)
{
    sint32 i;

    for (i = 0; i < 5; i++)
        RopeList[i].Active = 0;
    nRope = 0;
}

void CreateRope(ROPE_STRUCT *rope, PHD_VECTOR *pos, PHD_VECTOR *dir, sint32 slength)
{
    sint32 n, l;

    rope->Position.x = pos->x;
    rope->Position.y = pos->y;
    rope->Position.z = pos->z;
    l = slength << 16;
    rope->SegmentLength = l;
    dir->x <<= 16;
    dir->y <<= 16;
    dir->z <<= 16;
    Normalise(dir);
    for (n = 0; n < 24; n++) {
        rope->Segment[n].x = (__int64)(l * n) * dir->x >> 16;
        rope->Segment[n].y = (__int64)(l * n) * dir->y >> 16;
        rope->Segment[n].z = (__int64)(l * n) * dir->z >> 16;
        rope->Velocity[n].x = 0;
        rope->Velocity[n].y = 0;
        rope->Velocity[n].z = 0;
    }
    rope->Active = 0;
}

void InitialiseRope(sint16 item_number)
{
    PHD_VECTOR RopePos, RopeDir;
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint16 room_number;

    item = &items[item_number];
    room_number = item->room_number;
    RopePos.x = item->pos.x_pos;
    RopePos.z = item->pos.z_pos;
    RopePos.y = item->pos.y_pos;
    floor = GetFloor(RopePos.x, RopePos.y, RopePos.z, &room_number);
    RopePos.y = GetCeiling(floor, RopePos.x, RopePos.y, RopePos.z);
    RopeDir.x = 0;
    RopeDir.y = 0x4000;
    RopeDir.z = 0;
    CreateRope(&RopeList[nRope], &RopePos, &RopeDir, 128);
    item->trigger_flags = (sint16)nRope;
    AddActiveItem(item_number);
    nRope++;
}
