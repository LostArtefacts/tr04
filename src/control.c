#include "control.h"
#include "objnums.h"
#include "box.h"
#include "anim.h"

sint32 GetChange(ITEM_INFO *item, ANIM_STRUCT *anim)
{
    sint32 i, j;
    CHANGE_STRUCT *change;
    RANGE_STRUCT *range;

    if (item->current_anim_state == item->goal_anim_state)
        return 0;
    change = &changes[anim->change_index];
    for (i = 0; i < anim->number_changes; i++, change++) {
        if (change->goal_anim_state == item->goal_anim_state) {
            range = &ranges[change->range_index];
            for (j = 0; j < change->number_ranges; j++, range++) {
                if (item->frame_number >= range->start_frame && item->frame_number <= range->end_frame) {
                    item->anim_number = range->link_anim_num;
                    item->frame_number = range->link_frame_num;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void AlterFloorHeight(ITEM_INFO *item, sint32 height)
{
    FLOOR_INFO *floor, *ceiling;
    sint16 room_num;

    room_num = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
    ceiling = GetFloor(item->pos.x_pos, item->pos.y_pos + height - WALL_L, item->pos.z_pos, &room_num);
    if (floor->floor == NO_HEIGHT / 256) {
        floor->floor = ceiling->ceiling + height / 256;
    } else {
        floor->floor = height / 256 + floor->floor;
        if (floor->floor == ceiling->ceiling)
            floor->floor = NO_HEIGHT / 256;
    }
    if (boxes[floor->box].overlap_index & BLOCKABLE) {
        if (height < 0)
            boxes[floor->box].overlap_index |= BLOCKED;
        else
            boxes[floor->box].overlap_index &= ~BLOCKED;
    }
}

FLOOR_INFO *GetFloor(sint32 x, sint32 y, sint32 z, sint16 *room_number)
{
    sint16 data;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    sint32 x_floor, y_floor, retval;

    r = &room[*room_number];
    do {
        x_floor = (z - r->z) >> WALL_SHIFT;
        y_floor = (x - r->x) >> WALL_SHIFT;
        if (x_floor <= 0) {
            x_floor = 0;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (x_floor >= r->x_size - 1) {
            x_floor = r->x_size - 1;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (y_floor < 0)
            y_floor = 0;
        else if (y_floor >= r->y_size)
            y_floor = r->y_size - 1;
        floor = &r->floor[x_floor + y_floor * r->x_size];
        data = GetDoor(floor);
        if (data != NO_ROOM) {
            *room_number = data;
            r = &room[data];
        }
    } while (data != NO_ROOM);
    if (y >= floor->floor << 8) {
        do {
            if (floor->pit_room == NO_ROOM)
                return floor;
            if ((retval = CheckNoColFloorTriangle(floor, x, z)) == 1)
                break;
            if (retval == -1) {
                if (y < r->minfloor)
                    break;
            }
            *room_number = floor->pit_room;
            r = &room[floor->pit_room];
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        } while (y >= floor->floor << 8);
    } else if (y < floor->ceiling << 8) {
        do {
            if (floor->sky_room == NO_ROOM)
                return floor;
            if ((retval = CheckNoColCeilingTriangle(floor, x, z)) == 1)
                break;
            if (retval == -1) {
                if (y >= r->maxceiling)
                    break;
            }
            *room_number = floor->sky_room;
            r = &room[floor->sky_room];
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        } while (y < floor->ceiling << 8);
    }
    return floor;
}

sint32 GetWaterHeight(sint32 x, sint32 y, sint32 z, sint16 room_number)
{
    sint32 x_floor, y_floor;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    sint16 data;

    r = &room[room_number];
    do {
        x_floor = (z - r->z) >> WALL_SHIFT;
        y_floor = (x - r->x) >> WALL_SHIFT;
        if (x_floor <= 0) {
            x_floor = 0;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (x_floor >= r->x_size - 1) {
            x_floor = r->x_size - 1;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (y_floor < 0)
            y_floor = 0;
        else if (y_floor >= r->y_size)
            y_floor = r->y_size - 1;
        floor = &r->floor[x_floor + y_floor * r->x_size];
        data = GetDoor(floor);
        if (data != NO_ROOM) {
            room_number = data;
            r = &room[data];
        }
    } while (data != NO_ROOM);
    if (r->flags & ROOM_UNDERWATER) {
        while (floor->sky_room != NO_ROOM) {
            if (CheckNoColCeilingTriangle(floor, x, z) == 1)
                break;
            r = &room[floor->sky_room];
            if (!(r->flags & ROOM_UNDERWATER))
                return r->minfloor;
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        }
        return r->maxceiling;
    } else {
        while (floor->pit_room != NO_ROOM) {
            if (CheckNoColFloorTriangle(floor, x, z) == 1)
                break;
            r = &room[floor->pit_room];
            if (r->flags & ROOM_UNDERWATER)
                return r->maxceiling;
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        }
    }
    return NO_HEIGHT;
}

sint32 TriggerActive(ITEM_INFO *item)
{
    sint32 ok;

    ok = (item->flags & IFL_REVERSE) ? 0 : 1;
    if ((item->flags & IFL_CODEBITS) != IFL_CODEBITS)
        return !ok;
    if (!item->timer)
        return ok;
    if (item->timer > 0) {
        item->timer--;
        if (!item->timer)
            item->timer = -1;
    } else if (item->timer < -1) {
        item->timer++;
        if (item->timer == -1)
            item->timer = 0;
    }
    if (item->timer <= -1)
        return !ok;
    return ok;
}

sint16 GetDoor(FLOOR_INFO *floor)
{
    sint16 *data;
    sint16 type;

    if (!floor->index)
        return NO_ROOM;
    data = &floor_data[floor->index];
    type = *data++;
    if ((type & DATA_TYPE) == TILT_TYPE || (type & DATA_TYPE) == SPLIT1 || (type & DATA_TYPE) == SPLIT2 || (type & DATA_TYPE) == NOCOLF1B || (type & DATA_TYPE) == NOCOLF1T || (type & DATA_TYPE) == NOCOLF2B || (type & DATA_TYPE) == NOCOLF2T) {
        if (type & END_BIT)
            return NO_ROOM;
        data++;
        type = *data++;
    }
    if ((type & DATA_TYPE) == ROOF_TYPE || (type & DATA_TYPE) == SPLIT3 || (type & DATA_TYPE) == SPLIT4 || (type & DATA_TYPE) == NOCOLC1B || (type & DATA_TYPE) == NOCOLC1T || (type & DATA_TYPE) == NOCOLC2B || (type & DATA_TYPE) == NOCOLC2T) {
        if (type & END_BIT)
            return NO_ROOM;
        data++;
        type = *data++;
    }
    if ((type & DATA_TYPE) == DOOR_TYPE)
        return *data;
    return NO_ROOM;
}

sint32 LOS(GAME_VECTOR *start, GAME_VECTOR *target)
{
    sint32 los1, los2;

    target->room_number = start->room_number;
    if (ABS(target->z - start->z) > ABS(target->x - start->x)) {
        los1 = xLOS(start, target);
        los2 = zLOS(start, target);
    } else {
        los1 = zLOS(start, target);
        los2 = xLOS(start, target);
    }
    if (!los2)
        return 0;
    GetFloor(target->x, target->y, target->z, &target->room_number);
    if (!ClipTarget(start, target) || los1 != 1 || los2 != 1)
        return 0;
    return 1;
}

void AddRoomFlipItems(ROOM_INFO *r)
{
    ITEM_INFO *item;
    sint16 item_num;

    for (item_num = r->item_number; item_num != NO_ITEM; item_num = items[item_num].next_item) {
        item = &items[item_num];
        if (item->object_number == RAISING_BLOCK1 && item->item_flags[1])
            AlterFloorHeight(item, -1024);
        if (item->object_number == RAISING_BLOCK2 && item->item_flags[1])
            AlterFloorHeight(item, -2048);
    }
}

void TriggerCDTrack(sint16 value, sint16 flags, sint16 type)
{
    if (value < 128)
        TriggerNormalCDTrack(value, flags, type);
}
