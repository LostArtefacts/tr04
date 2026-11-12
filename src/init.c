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
