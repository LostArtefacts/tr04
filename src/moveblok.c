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
#include "lara_states.h"
#include "delstuff.h"
#include "specific/input.h"
#include "sound.h"
#include "sphere.h"
#include "tomb4fx.h"
#include "text.h"
#include "deltapak.h"
#include "specific/function_stubs.h"

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

void MovableBlock(sint16 item_number)
{
    ITEM_INFO *item, *l;
    PHD_VECTOR pos;
    sint32 offset;
    uint16 quadrant;
    sint32 frame, base;
    static sint8 sfx = 0;

    item = &items[item_number];
    l = lara_item;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    quadrant = (uint16)(l->pos.y_rot + 0x2000) / 0x4000;
    if (l->anim_number == ANIM_PUSH) {
        base = anims[ANIM_PUSH].frame_base;
        frame = l->frame_number;
        if ((frame < base + 30 || frame > base + 67) && (frame < base + 78 || frame > base + 125) && (frame < base + 140 || frame > base + 160)) {
            if (sfx) {
                SoundEffect(SFX_PUSH_BLOCK_END, &item->pos, SFX_ALWAYS);
                sfx = 0;
            }
        } else {
            SoundEffect(SFX_PUSHABLE_SOUND, &item->pos, SFX_ALWAYS);
            sfx = 1;
        }
        GetLaraJointPos(&pos, 14);
        switch (quadrant) {
        case NORTH:
            offset = pos.z + *(sint32 *)&item->item_flags[2] - *(sint32 *)&l->item_flags[2];
            if (ABS(item->pos.z_pos - offset) < 512 && item->pos.z_pos < offset)
                item->pos.z_pos = offset;
            break;
        case SOUTH:
            offset = pos.z + *(sint32 *)&item->item_flags[2] - *(sint32 *)&l->item_flags[2];
            if (ABS(item->pos.z_pos - offset) < 512 && item->pos.z_pos > offset)
                item->pos.z_pos = offset;
            break;
        case EAST:
            offset = pos.x + *(sint32 *)item->item_flags - *(sint32 *)l->item_flags;
            if (ABS(item->pos.x_pos - offset) < 512 && item->pos.x_pos < offset)
                item->pos.x_pos = offset;
            break;
        case WEST:
            offset = pos.x + *(sint32 *)item->item_flags - *(sint32 *)l->item_flags;
            if (ABS(item->pos.x_pos - offset) < 512 && item->pos.x_pos > offset)
                item->pos.x_pos = offset;
            break;
        }
        if (l->frame_number == anims[l->anim_number].frame_end - 1) {
            if (!(input & IN_ACTION))
                l->goal_anim_state = AS_STOP;
            else if (!TestBlockPush(item, 1024, quadrant))
                l->goal_anim_state = AS_STOP;
        }
    } else if (l->anim_number == ANIM_PULL) {
        base = anims[ANIM_PULL].frame_base;
        frame = l->frame_number;
        if ((frame < base + 40 || frame > base + 122) && (frame < base + 130 || frame > base + 170)) {
            if (sfx) {
                SoundEffect(SFX_PUSH_BLOCK_END, &item->pos, SFX_ALWAYS);
                sfx = 0;
            }
        } else {
            SoundEffect(SFX_PUSHABLE_SOUND, &item->pos, SFX_ALWAYS);
            sfx = 1;
        }
        GetLaraJointPos(&pos, 14);
        switch (quadrant) {
        case NORTH:
            offset = pos.z + *(sint32 *)&item->item_flags[2] - *(sint32 *)&l->item_flags[2];
            if (ABS(item->pos.z_pos - offset) < 512 && item->pos.z_pos > offset)
                item->pos.z_pos = offset;
            break;
        case SOUTH:
            offset = pos.z + *(sint32 *)&item->item_flags[2] - *(sint32 *)&l->item_flags[2];
            if (ABS(item->pos.z_pos - offset) < 512 && item->pos.z_pos < offset)
                item->pos.z_pos = offset;
            break;
        case EAST:
            offset = pos.x + *(sint32 *)item->item_flags - *(sint32 *)l->item_flags;
            if (ABS(item->pos.x_pos - offset) < 512 && item->pos.x_pos > offset)
                item->pos.x_pos = offset;
            break;
        case WEST:
            offset = pos.x + *(sint32 *)item->item_flags - *(sint32 *)l->item_flags;
            if (ABS(item->pos.x_pos - offset) < 512 && item->pos.x_pos < offset)
                item->pos.x_pos = offset;
            break;
        }
        if (l->frame_number == anims[l->anim_number].frame_end - 1) {
            if (!(input & IN_ACTION))
                l->goal_anim_state = AS_STOP;
            else if (!TestBlockPull(item, 1024, quadrant))
                l->goal_anim_state = AS_STOP;
        }
    } else if (l->anim_number == 417 || l->anim_number == 418) {
        if (l->frame_number == anims[417].frame_base || l->frame_number == anims[418].frame_base) {
            item->pos.x_pos = (item->pos.x_pos & -512) | 512;
            item->pos.z_pos = (item->pos.z_pos & -512) | 512;
        }
        if (l->frame_number == anims[l->anim_number].frame_end) {
            FLOOR_INFO *floor;
            sint16 room_number;

            room_number = item->room_number;
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos - 256, item->pos.z_pos, &room_number);
            GetHeight(floor, item->pos.x_pos, item->pos.y_pos - 256, item->pos.z_pos);
            TestTriggers(trigger_index, 1, item->flags & IFL_CODEBITS);
            RemoveActiveItem(item_number);
            item->status = ITEM_INACTIVE;
        }
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

void MovableBlockCollision(sint16 item_number, ITEM_INFO *laraitem, COLL_INFO *coll)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    PHD_VECTOR pos;
    sint16 *bounds;
    sint16 room_number, yrot, quadrant;

    item = &items[item_number];
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos - 256, item->pos.z_pos, &room_number);
    item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (item->room_number != room_number)
        ItemNewRoom(item_number, room_number);
    if (input & IN_ACTION && laraitem->current_anim_state == AS_STOP && laraitem->anim_number == ANIM_BREATH &&
        !laraitem->gravity_status && lara.gun_status == LG_NO_ARMS && item->status == ITEM_INACTIVE &&
        item->trigger_flags >= 0 || (lara.IsMoving && lara.GeneralPtr == (void *)item_number)) {
        room_number = laraitem->room_number;
        GetFloor(item->pos.x_pos, item->pos.y_pos - 256, item->pos.z_pos, &room_number);
        if (room_number == item->room_number) {
            bounds = GetBoundsAccurate(item);
            MovingBlockBounds[0] = bounds[0] - 100;
            MovingBlockBounds[1] = bounds[1] + 100;
            MovingBlockBounds[4] = bounds[4] - 200;
            MovingBlockBounds[5] = 0;
            yrot = item->pos.y_rot;
            item->pos.y_rot = (laraitem->pos.y_rot + 0x2000) & 0xC000;
            if (TestLaraPosition(MovingBlockBounds, item, laraitem)) {
                if (((uint16)(yrot + 0x2000) / 0x4000) + ((uint16)item->pos.y_rot / 0x4000) & 1)
                    MovingBlockPos.z = bounds[0] - 35;
                else
                    MovingBlockPos.z = bounds[4] - 35;
                if (MoveLaraPosition(&MovingBlockPos, item, laraitem)) {
                    laraitem->anim_number = ANIM_PPREADY;
                    laraitem->frame_number = anims[ANIM_PPREADY].frame_base;
                    laraitem->current_anim_state = AS_PPREADY;
                    laraitem->goal_anim_state = AS_PPREADY;
                    lara.IsMoving = 0;
                    lara.gun_status = LG_HANDS_BUSY;
                    lara.CornerX = (sint32)item;
                } else
                    lara.GeneralPtr = (void *)item_number;
            } else if (lara.IsMoving && lara.GeneralPtr == (void *)item_number) {
                lara.IsMoving = 0;
                lara.gun_status = LG_NO_ARMS;
            }
            item->pos.y_rot = yrot;
        }
    } else if (laraitem->current_anim_state == AS_PPREADY && laraitem->frame_number == anims[ANIM_PPREADY].frame_base + 19 &&
        (ITEM_INFO *)lara.CornerX == item) {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
        quadrant = (uint16)(laraitem->pos.y_rot + 8192) >> 14;
        if (input & IN_FORWARD) {
            if (!TestBlockPush(item, 1024, quadrant))
                return;
            laraitem->goal_anim_state = AS_PUSHBLOCK;
        } else if (input & IN_BACK) {
            if (!TestBlockPull(item, 1024, quadrant))
                return;
            laraitem->goal_anim_state = AS_PULLBLOCK;
        } else
            return;
        AddActiveItem(item_number);
        item->status = ITEM_ACTIVE;
        lara.head_x_rot = 0;
        lara.head_y_rot = 0;
        lara.torso_x_rot = 0;
        lara.torso_y_rot = 0;
        GetLaraJointPos(&pos, 14);
        *(sint32 *)&laraitem->item_flags[0] = pos.x;
        *(sint32 *)&laraitem->item_flags[2] = pos.z;
        *(sint32 *)&item->item_flags[0] = item->pos.x_pos;
        *(sint32 *)&item->item_flags[2] = item->pos.z_pos;
    } else
        ObjectCollision(item_number, laraitem, coll);
}

void InitialisePlanetEffect(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    char *pifl;
    uint8 others[4];
    sint32 i, j;

    item = &items[item_number];
    item->mesh_bits = 0;
    item2 = items;
    for (i = 0; i < level_items; i++, item2++) {
        if (item2->object_number >= PUSHABLE_OBJECT1 && item2->object_number <= PUSHABLE_OBJECT5 && item2->trigger_flags == item->trigger_flags) {
            item->item_flags[0] = i;
            break;
        }
    }
    if (item->trigger_flags == 1) {
        item2 = items;
        for (i = 0, j = 0; i < level_items; i++, item2++) {
            if (item2->object_number == PLANET_EFFECT && item_number != i) {
                others[j] = i;
                j++;
            }
        }
        pifl = (char *)&item->item_flags[2];
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                item2 = &items[others[j]];
                if (item2->trigger_flags == i + 2) {
                    *pifl++ = others[j];
                    break;
                }
            }
        }
    }
}

void ControlPlanetEffect(sint16 item_number)
{
    ITEM_INFO *item, *item2;
    PHD_VECTOR pos, pos2;
    uint8 *pifl;
    sint32 b, g, i;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (item->item_flags[0] > 0) {
        items[item->item_flags[0]].trigger_flags = -items[item->item_flags[0]].trigger_flags;
        item->item_flags[0] = NO_ITEM;
    }
    item->mesh_bits = 255;
    AnimateItem(item);
    if (item->trigger_flags == 1) {
        if ((items[((uint8 *)&item->item_flags[2])[0]].flags & IFL_CODEBITS) == IFL_CODEBITS &&
            (items[((uint8 *)&item->item_flags[2])[1]].flags & IFL_CODEBITS) == IFL_CODEBITS &&
            (items[((uint8 *)&item->item_flags[3])[0]].flags & IFL_CODEBITS) == IFL_CODEBITS &&
            (items[((uint8 *)&item->item_flags[3])[1]].flags & IFL_CODEBITS) == IFL_CODEBITS) {
            pos.x = pos.y = pos.z = 0;
            GetJointAbsPosition(item, &pos, 0);
            item2 = find_a_fucking_item(ANIMATING4);
            pos2.x = pos2.y = pos2.z = 0;
            GetJointAbsPosition(item2, &pos2, 0);
            b = (GetRandomControl() & 0x1F) + 224;
            g = b - (GetRandomControl() & 0x3F);
            TriggerLightningGlow(pos.x, pos.y, pos.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 48));
            TriggerLightningGlow(pos2.x, pos2.y, pos2.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 64));
            if (!(GlobalCounter & 3))
                TriggerLightning(&pos, &pos2, (GetRandomControl() & 0x1F) + 32, RGBA(0, g, b, 24), 1, 32, 5);
            pifl = (uint8 *)&item->item_flags[2];
            for (i = 0; i < 4; i++) {
                pos2.x = pos2.y = pos2.z = 0;
                GetJointAbsPosition(&items[pifl[i]], &pos2, 0);
                if (!(GlobalCounter & 3))
                    TriggerLightning(&pos2, &pos, (GetRandomControl() & 0x1F) + 32, RGBA(0, g, b, 24), 1, 32, 5);
                TriggerLightningGlow(pos.x, pos.y, pos.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 48));
                SoundEffect(SFX_ELEC_ARCING_LOOP, (PHD_3DPOS *)&pos, SFX_DEFAULT);
                pos = pos2;
            }
            TriggerLightningGlow(pos2.x, pos2.y, pos2.z, RGBA(0, g, b, (GetRandomControl() & 0x1F) + 48));
        }
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
