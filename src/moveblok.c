#include "moveblok.h"
#include "anim.h"
#include "box.h"
#include "control.h"
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
