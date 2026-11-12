#include "moveblok.h"
#include "lara.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "collide.h"
#include "debris.h"
#include "objnums.h"
#include "draw.h"
#include "specific/3dmath.h"
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

sint32 TestBlockPush(ITEM_INFO *item, sint32 height, uint16 quadrant)
{
    ITEM_INFO **itemlist;
    FLOOR_INFO *floor;
    ROOM_INFO *r;
    sint32 x, y, z, rx, rz;
    sint16 room_number;

    x = item->pos.x_pos;
    y = item->pos.y_pos;
    z = item->pos.z_pos;
    itemlist = (ITEM_INFO **)&tsv_buffer[0];
    switch (quadrant) {
    case NORTH:
        z += 1024;
        break;
    case EAST:
        x += 1024;
        break;
    case SOUTH:
        z -= 1024;
        break;
    case WEST:
        x -= 1024;
        break;
    }
    room_number = item->room_number;
    floor = GetFloor(x, y - 256, z, &room_number);
    r = &room[room_number];
    rx = (x - r->x) >> 10;
    rz = (z - r->z) >> 10;
    if (r->floor[rx * r->x_size + rz].stopper)
        return 0;
    if (GetHeight(floor, x, y - 256, z) != y)
        return 0;
    GetHeight(floor, x, y, z);
    if (height_type != WALL)
        return 0;
    floor = GetFloor(x, y - height + 100, z, &room_number);
    if (GetCeiling(floor, x, y - height + 100, z) > y - height + 100)
        return 0;
    rx = item->pos.x_pos;
    rz = item->pos.z_pos;
    item->pos.x_pos = x;
    item->pos.z_pos = z;
    GetCollidedObjects(item, 256, 1, itemlist, 0, 0);
    item->pos.x_pos = rx;
    item->pos.z_pos = rz;
    if (*itemlist) {
        for (; *itemlist; itemlist++) {
            if ((*itemlist)->object_number == TWOBLOCK_PLATFORM || (*itemlist)->object_number == HAMMER)
                return 1;
        }
        return 0;
    }
    return 1;
}

sint32 TestBlockPull(ITEM_INFO *item, sint32 height, uint16 quadrant)
{
    ITEM_INFO **itemlist;
    ITEM_INFO *collided;
    FLOOR_INFO *floor;
    ROOM_INFO *r;
    sint32 x, y, z, destx, destz, rx, rz, ignore, i;
    sint16 room_number;

    itemlist = (ITEM_INFO **)&tsv_buffer[0];
    destx = 0;
    destz = 0;
    switch (quadrant) {
    case NORTH:
        destz = -1024;
        break;
    case EAST:
        destx = -1024;
        break;
    case SOUTH:
        destz = 1024;
        break;
    case WEST:
        destx = 1024;
        break;
    }
    x = item->pos.x_pos + destx;
    y = item->pos.y_pos;
    z = item->pos.z_pos + destz;
    room_number = item->room_number;
    floor = GetFloor(x, y - 256, z, &room_number);
    r = &room[room_number];
    rx = (x - r->x) >> 10;
    rz = (z - r->z) >> 10;
    if (r->floor[rx * r->x_size + rz].stopper)
        return 0;
    if (GetHeight(floor, x, y - 256, z) != y)
        return 0;
    floor = GetFloor(x, y - height, z, &room_number);
    if (floor->ceiling << 8 > y - height)
        return 0;
    rx = item->pos.x_pos;
    rz = item->pos.z_pos;
    GetCollidedObjects(item, 256, 1, itemlist, 0, 0);
    item->pos.x_pos = rx;
    item->pos.z_pos = rz;
    if (itemlist[0]) {
        ignore = 0;
        for (i = 0; itemlist[0] != 0; i++, itemlist++) {
            collided = itemlist[0];
            if (collided->object_number == TWOBLOCK_PLATFORM || collided->object_number == HAMMER) {
                ignore = 1;
                break;
            }
        }
        if (!ignore)
            return 0;
    }
    x += destx;
    z += destz;
    room_number = item->room_number;
    floor = GetFloor(x, y - 256, z, &room_number);
    if (GetHeight(floor, x, y - 256, z) != y)
        return 0;
    floor = GetFloor(x, y - 762, z, &room_number);
    if (floor->ceiling << 8 > y - 762)
        return 0;
    x = lara_item->pos.x_pos + destx;
    y = lara_item->pos.y_pos;
    z = lara_item->pos.z_pos + destz;
    room_number = lara_item->room_number;
    GetFloor(x, y, z, &room_number);
    r = &room[room_number];
    rx = (x - r->x) >> 10;
    rz = (z - r->z) >> 10;
    if (r->floor[rx * r->x_size + rz].stopper)
        return 0;
    rx = lara_item->pos.x_pos;
    rz = lara_item->pos.z_pos;
    lara_item->pos.x_pos = x;
    lara_item->pos.z_pos = z;
    GetCollidedObjects(lara_item, 256, 1, itemlist, 0, 0);
    lara_item->pos.x_pos = rx;
    lara_item->pos.z_pos = rz;
    if (itemlist[0]) {
        for (i = 0; itemlist[0] != 0; i++, itemlist++) {
            collided = itemlist[0];
            if (collided == item || collided->object_number == TWOBLOCK_PLATFORM || collided->object_number == HAMMER)
                return 1;
        }
        return 0;
    }
    return 1;
}

void DrawPlanetEffect(ITEM_INFO *item)
{
    OBJECT_INFO *obj;
    sint16 **meshpp;
    sint32 *bone, i;
    sint16 *frm[2];
    sint16 *rot;
    sint32 poppush, pp;

    if (!item->mesh_bits)
        return;
    GetFrames(item, frm, &poppush);
    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    ((void (*)(sint32, sint32, sint32))phd_RotYXZ)(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    CalculateObjectLighting(item, frm[0]);
    obj = &objects[item->object_number];
    meshpp = &meshes[obj->mesh_index];
    bone = &bones[obj->bone_index];
    phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
    rot = frm[0] + 9;
    gar_RotYXZsuperpack(&rot, 0);
    phd_PutPolygons(*meshpp, -1);
    phd_PutPolygons(*meshpp, -1);
    meshpp += 2;
    for (i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2) {
        pp = bone[0];
        if (pp & 1)
            phd_PopMatrix();
        if (pp & 2)
            phd_PushMatrix();
        phd_TranslateRel(bone[1], bone[2], bone[3]);
        gar_RotYXZsuperpack(&rot, 0);
        phd_PutPolygons(*meshpp, -1);
    }
    phd_PopMatrix();
}
