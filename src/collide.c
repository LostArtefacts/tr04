#include <stdlib.h>

#include "collide.h"
#include "anim.h"
#include "draw.h"
#include "room.h"
#include "specific/3dmath.h"
#include "control.h"
#include "lara.h"
#include "lara_states.h"
#include "sphere.h"
#include "effects.h"

void Func488650(void);

sint32 FindGridShift(sint32 src, sint32 dst)
{
    sint32 srcw;
    sint32 dstw;

    srcw = src >> WALL_SHIFT;
    dstw = dst >> WALL_SHIFT;
    if (srcw == dstw)
        return 0;
    src &= WALL_L - 1;
    if (dstw > srcw)
        return WALL_L - (src - 1);
    else
        return -(src + 1);
}

sint32 CollideStaticObjects(COLL_INFO *coll, sint32 x, sint32 y, sint32 z, sint16 room_number, sint32 hite)
{
    ROOM_INFO *r;
    MESH_INFO *mesh;
    STATIC_INFO *sinfo;
    sint16 *door;
    sint32 lxmin, lxmax, lymin, lymax, lzmin, lzmax;
    sint32 xmin, xmax, ymin, ymax, zmin, zmax;
    sint32 i, j;
    sint16 num_nearby_rooms;
    sint16 nearby_rooms[22];

    coll->hit_static = 0;
    lxmin = x - coll->radius;
    lxmax = x + coll->radius;
    lymin = y - hite;
    lymax = y;
    lzmin = z - coll->radius;
    lzmax = z + coll->radius;
    num_nearby_rooms = 1;
    nearby_rooms[0] = room_number;
    door = room[room_number].door;

    if (door) {
        for (i = *door++; i > 0; i--) {
            for (j = 0; j < num_nearby_rooms; j++) {
                if (nearby_rooms[j] == *door)
                    break;
            }

            if (j == num_nearby_rooms) {
                nearby_rooms[num_nearby_rooms] = *door;
                num_nearby_rooms++;
            }

            door += 16;
        }
    }

    for (i = 0; i < num_nearby_rooms; i++) {
        r = &room[nearby_rooms[i]];
        mesh = r->mesh;

        for (j = r->num_meshes; j > 0; j--, mesh++) {
            sinfo = &static_objects[mesh->static_number];

            if (!(mesh->Flags & 1))
                continue;

            ymin = mesh->y + sinfo->y_minc;
            ymax = mesh->y + sinfo->y_maxc;

            switch (mesh->y_rot) {
            case -0x8000:
                xmin = mesh->x - sinfo->x_maxc;
                xmax = mesh->x - sinfo->x_minc;
                zmin = mesh->z - sinfo->z_maxc;
                zmax = mesh->z - sinfo->z_minc;
                break;
            case -0x4000:
                xmin = mesh->x - sinfo->z_maxc;
                xmax = mesh->x - sinfo->z_minc;
                zmin = mesh->z + sinfo->x_minc;
                zmax = mesh->z + sinfo->x_maxc;
                break;
            case 0x4000:
                xmin = mesh->x + sinfo->z_minc;
                xmax = mesh->x + sinfo->z_maxc;
                zmin = mesh->z - sinfo->x_maxc;
                zmax = mesh->z - sinfo->x_minc;
                break;
            default:
                xmin = mesh->x + sinfo->x_minc;
                xmax = mesh->x + sinfo->x_maxc;
                zmin = mesh->z + sinfo->z_minc;
                zmax = mesh->z + sinfo->z_maxc;
                break;
            }

            if (lxmax <= xmin || lxmin >= xmax || lymax <= ymin || lymin >= ymax || lzmax <= zmin || lzmin >= zmax)
                continue;

            coll->hit_static = 1;
            return 1;
        }
    }

    return 0;
}

void ShiftItem(ITEM_INFO *item, COLL_INFO *coll)
{
    item->pos.x_pos += coll->shift.x;
    item->pos.y_pos += coll->shift.y;
    item->pos.z_pos += coll->shift.z;
    coll->shift.x = coll->shift.y = coll->shift.z = 0;
}

void UpdateLaraRoom(ITEM_INFO *item, sint32 height)
{
    sint16 room_number;
    FLOOR_INFO *floor;
    sint32 x, y, z;

    x = item->pos.x_pos;
    y = item->pos.y_pos + height;
    z = item->pos.z_pos;
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    item->floor = GetHeight(floor, x, y, z);
    if (item->room_number != room_number)
        ItemNewRoom(lara.item_number, room_number);
    Func488650();
}

sint16 GetTiltType(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z)
{
    ROOM_INFO *r;
    sint16 *data;
    sint16 type, tilts, t0, t1, t2, t3, dx, dz, xoff, yoff;

    while (floor->pit_room != NO_ROOM) {
        if (CheckNoColFloorTriangle(floor, x, z) == 1)
            break;
        r = &room[floor->pit_room];
        floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
    }
    if (y + 512 < floor->floor << 8)
        return 0;
    if (floor->index) {
        data = &floor_data[floor->index];
        type = *data & 0x1f;
        if (type == 2)
            return data[1];
        if (type == 7 || type == 8 || type == 11 || type == 13 || type == 12 || type == 14) {
            tilts = data[1];
            t0 = tilts & 15;
            t1 = (tilts >> 4) & 15;
            t2 = (tilts >> 8) & 15;
            t3 = (tilts >> 12) & 15;
            dx = x & 1023;
            dz = z & 1023;
            if ((type & 0x1f) == 7 || (type & 0x1f) == 11 || (type & 0x1f) == 12) {
                if (dx <= 1024 - dz) {
                    xoff = t2 - t1;
                    yoff = t0 - t1;
                } else {
                    xoff = t3 - t0;
                    yoff = t3 - t2;
                }
            } else {
                if (dx <= dz) {
                    xoff = t2 - t1;
                    yoff = t3 - t2;
                } else {
                    xoff = t3 - t0;
                    yoff = t0 - t1;
                }
            }
            return (xoff << 8) | (yoff & 0xff);
        }
    }
    return 0;
}

void LaraBaddieCollision(ITEM_INFO *l, COLL_INFO *coll)
{
    ROOM_INFO *r;
    ITEM_INFO *item;
    MESH_INFO *mesh;
    PHD_3DPOS pos;
    sint16 *door;
    sint16 *bounds;
    sint32 i, j, dx, dy, dz;
    sint16 num_nearby_rooms, item_number, nex, obj_num;
    sint16 nearby_rooms[22];

    l->hit_status = 0;
    lara.hit_direction = -1;

    if (l->hit_points <= 0)
        return;

    num_nearby_rooms = 1;
    nearby_rooms[0] = l->room_number;
    door = room[nearby_rooms[0]].door;

    if (door) {
        for (i = *door++; i > 0; i--) {
            for (j = 0; j < num_nearby_rooms; j++) {
                if (nearby_rooms[j] == *door)
                    break;
            }

            if (j == num_nearby_rooms) {
                nearby_rooms[num_nearby_rooms] = *door;
                num_nearby_rooms++;
            }

            door += 16;
        }
    }

    for (i = 0; i < num_nearby_rooms; i++) {
        r = &room[nearby_rooms[i]];
        item_number = r->item_number;

        while (item_number != NO_ITEM) {
            item = &items[item_number];
            nex = item->next_item;

            obj_num = item->object_number;

            if (item->collidable && item->status != ITEM_INVISIBLE) {
                if (objects[obj_num].collision) {
                    dx = l->pos.x_pos - item->pos.x_pos;
                    dy = l->pos.y_pos - item->pos.y_pos;
                    dz = l->pos.z_pos - item->pos.z_pos;

                    if (dx > -3072 && dx < 3072 && dy > -3072 && dy < 3072 && dz > -3072 && dz < 3072)
                        objects[obj_num].collision(item_number, l, coll);
                }
            }

            item_number = nex;
        }

        if (coll->enable_baddie_push) {
            r = &room[nearby_rooms[i]];
            mesh = r->mesh;

            for (j = r->num_meshes; j > 0; j--, mesh++) {
                if (!(mesh->Flags & 1))
                    continue;

                dx = l->pos.x_pos - mesh->x;
                dy = l->pos.y_pos - mesh->y;
                dz = l->pos.z_pos - mesh->z;

                if (dx > -3072 && dx < 3072 && dy > -3072 && dy < 3072 && dz > -3072 && dz < 3072) {
                    bounds = &static_objects[mesh->static_number].x_minc;
                    pos.x_pos = mesh->x;
                    pos.y_pos = mesh->y;
                    pos.z_pos = mesh->z;
                    pos.y_rot = mesh->y_rot;

                    if (TestBoundsCollideStatic(bounds, &pos, coll->radius))
                        ItemPushLaraStatic(l, bounds, &pos, coll);
                }
            }
        }
    }

    if (lara.hit_direction == -1)
        lara.hit_frame = 0;
}

void CreatureCollision(sint16 item_num, ITEM_INFO *laraitem, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_num];
    if (!TestBoundsCollide(item, laraitem, coll->radius))
        return;
    if (!TestCollision(item, laraitem))
        return;
    if (lara.water_status != LW_UNDERWATER && !lara.water_status != LW_SURFACE) {
        if (coll->enable_baddie_push) {
            ItemPushLara(item, laraitem, coll, coll->enable_spaz, 0);
        } else if (coll->enable_spaz) {
            sint16 *bounds;
            sint32 x, z, rx, rz, c, s;
            x = laraitem->pos.x_pos - item->pos.x_pos;
            z = laraitem->pos.z_pos - item->pos.z_pos;
            c = phd_cos(item->pos.y_rot);
            s = phd_sin(item->pos.y_rot);
            bounds = GetBestFrame(item);
            rx = (bounds[0] + bounds[1]) / 2;
            rz = (bounds[4] + bounds[5]) / 2;
            x -= (c * rx + s * rz) >> W2V_SHIFT;
            z -= (c * rz - s * rx) >> W2V_SHIFT;
            if (bounds[3] - bounds[2] > STEP_L) {
                PHD_ANGLE hitang;
                hitang = laraitem->pos.y_rot - (32768 + phd_atan(z, x));
                lara.hit_direction = (uint16)(hitang + 0x2000) / 0x4000;
                lara.hit_frame++;
                if (lara.hit_frame > 30)
                    lara.hit_frame = 30;
            }
        }
    }
}

void ObjectCollision(sint16 item_num, ITEM_INFO *laraitem, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_num];
    if (!TestBoundsCollide(item, laraitem, coll->radius))
        return;
    if (!TestCollision(item, laraitem))
        return;
    if (coll->enable_baddie_push)
        ItemPushLara(item, laraitem, coll, 0, 1);
}

void ObjectCollisionNoBigPush(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l) && coll->enable_baddie_push)
        ItemPushLara(item, l, coll, 0, 0);
}

void TrapCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->status == ITEM_ACTIVE) {
        if (TestBoundsCollide(item, l, coll->radius))
            ObjectCollision(item_number, l, coll);
    } else if (item->status != ITEM_INVISIBLE)
        ObjectCollision(item_number, l, coll);
}

sint32 TestBoundsCollide(ITEM_INFO *item, ITEM_INFO *l, sint32 rad)
{
    sint16 *bounds;
    sint16 *lbounds;
    sint32 s, c, dx, dz, x, z;

    bounds = GetBestFrame(item);
    lbounds = GetBestFrame(l);
    if (item->pos.y_pos + bounds[3] <= l->pos.y_pos + lbounds[2] || item->pos.y_pos + bounds[2] >= l->pos.y_pos + lbounds[3])
        return 0;
    c = phd_cos(item->pos.y_rot);
    s = phd_sin(item->pos.y_rot);
    dx = l->pos.x_pos - item->pos.x_pos;
    dz = l->pos.z_pos - item->pos.z_pos;
    x = (dx * c - dz * s) >> W2V_SHIFT;
    z = (dx * s + dz * c) >> W2V_SHIFT;
    return x >= bounds[0] - rad && x <= rad + bounds[1] && z >= bounds[4] - rad && z <= rad + bounds[5];
}

sint32 TestBoundsCollideStatic(sint16 *bounds, PHD_3DPOS *pos, sint32 rad)
{
    sint16 *lbounds;
    sint32 s, c, dx, dz, x, z;

    if (!(bounds[0] | bounds[1] | bounds[2] | bounds[3] | bounds[4] | bounds[5]))
        return 0;
    lbounds = GetBestFrame(lara_item);
    if (pos->y_pos + bounds[3] <= lara_item->pos.y_pos + lbounds[2] || pos->y_pos + bounds[2] >= lara_item->pos.y_pos + lbounds[3])
        return 0;
    c = phd_cos(pos->y_rot);
    s = phd_sin(pos->y_rot);
    dx = lara_item->pos.x_pos - pos->x_pos;
    dz = lara_item->pos.z_pos - pos->z_pos;
    x = (dx * c - dz * s) >> W2V_SHIFT;
    z = (dx * s + dz * c) >> W2V_SHIFT;
    return x >= bounds[0] - rad && x <= rad + bounds[1] && z >= bounds[4] - rad && z <= rad + bounds[5];
}

sint32 ItemPushLaraStatic(ITEM_INFO *l, sint16 *bounds, PHD_3DPOS *pos, COLL_INFO *coll)
{
    sint32 dx, dz, s, c, x, z;
    sint32 xmin, xmax, zmin, zmax, left, top, right, bottom;
    sint16 facing;

    dx = l->pos.x_pos - pos->x_pos;
    dz = l->pos.z_pos - pos->z_pos;
    s = phd_sin(pos->y_rot);
    c = phd_cos(pos->y_rot);
    x = (dx * c - dz * s) >> W2V_SHIFT;
    z = (dx * s + dz * c) >> W2V_SHIFT;
    xmin = bounds[0] - coll->radius;
    xmax = bounds[1] + coll->radius;
    zmin = bounds[4] - coll->radius;
    zmax = bounds[5] + coll->radius;

    if (abs(dx) > 4608 || abs(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
        return 0;

    left = x - xmin;
    top = zmax - z;
    right = xmax - x;
    bottom = z - zmin;

    if (left <= right && left <= top && left <= bottom)
        x -= left;
    else if (right <= left && right <= top && right <= bottom)
        x += right;
    else if (top <= left && top <= right && top <= bottom)
        z += top;
    else
        z -= bottom;

    l->pos.x_pos = pos->x_pos + ((c * x + s * z) >> W2V_SHIFT);
    l->pos.z_pos = pos->z_pos + ((c * z - s * x) >> W2V_SHIFT);
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -384;
    coll->bad_ceiling = 0;
    facing = coll->facing;
    coll->facing = (sint16)phd_atan(l->pos.z_pos - coll->old.z, l->pos.x_pos - coll->old.x);
    GetCollisionInfo(coll, l->pos.x_pos, l->pos.y_pos, l->pos.z_pos, l->room_number, 762);
    coll->facing = facing;

    if (coll->coll_type == COLL_NONE) {
        coll->old.x = l->pos.x_pos;
        coll->old.y = l->pos.y_pos;
        coll->old.z = l->pos.z_pos;
        UpdateLaraRoom(l, -10);
    } else {
        l->pos.x_pos = coll->old.x;
        l->pos.z_pos = coll->old.z;
    }

    if (l == lara_item && lara.IsMoving && lara.MoveCount > 15) {
        lara.IsMoving = 0;
        lara.gun_status = LG_NO_ARMS;
    }

    return 1;
}

sint32 TestLaraPosition(sint16 *bounds, ITEM_INFO *item, ITEM_INFO *laraitem)
{
    PHD_ANGLE yrotrel, zrotrel, xrotrel;
    sint32 ry, rz, rx;
    sint32 *mptr;
    sint32 x, z, y;

    xrotrel = laraitem->pos.x_rot - item->pos.x_rot;
    yrotrel = laraitem->pos.y_rot - item->pos.y_rot;
    zrotrel = laraitem->pos.z_rot - item->pos.z_rot;
    if (xrotrel < bounds[6] || xrotrel > bounds[7] || yrotrel < bounds[8] || yrotrel > bounds[9]
        || zrotrel < bounds[10] || zrotrel > bounds[11])
        return 0;
    phd_PushUnitMatrix();
    ((void (*)(sint32, sint32, sint32))phd_RotYXZ)(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    x = laraitem->pos.x_pos - item->pos.x_pos;
    y = laraitem->pos.y_pos - item->pos.y_pos;
    z = laraitem->pos.z_pos - item->pos.z_pos;
    mptr = phd_mxptr;
    rx = (mptr[M00] * x + mptr[M10] * y + mptr[M20] * z) >> W2V_SHIFT;
    ry = (mptr[M01] * x + mptr[M11] * y + mptr[M21] * z) >> W2V_SHIFT;
    rz = (mptr[M02] * x + mptr[M12] * y + mptr[M22] * z) >> W2V_SHIFT;
    phd_PopMatrix();
    if (rx < (sint32)bounds[0] || rx > (sint32)bounds[1] || ry < (sint32)bounds[2] || ry > (sint32)bounds[3]
        || rz < (sint32)bounds[4] || rz > (sint32)bounds[5])
        return 0;
    return 1;
}

void AlignLaraPosition(PHD_VECTOR *pos, ITEM_INFO *item, ITEM_INFO *l)
{
    sint32 *mptr;
    sint32 x, y, z;

    l->pos.x_rot = item->pos.x_rot;
    l->pos.y_rot = item->pos.y_rot;
    l->pos.z_rot = item->pos.z_rot;
    phd_PushUnitMatrix();
    ((void (*)(sint32, sint32, sint32))phd_RotYXZ)(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    mptr = phd_mxptr;
    x = item->pos.x_pos + ((mptr[M00] * pos->x + mptr[M01] * pos->y + mptr[M02] * pos->z) >> W2V_SHIFT);
    y = item->pos.y_pos + ((mptr[M10] * pos->x + mptr[M11] * pos->y + mptr[M12] * pos->z) >> W2V_SHIFT);
    z = item->pos.z_pos + ((mptr[M20] * pos->x + mptr[M21] * pos->y + mptr[M22] * pos->z) >> W2V_SHIFT);
    phd_PopMatrix();
    l->pos.x_pos = x;
    l->pos.y_pos = y;
    l->pos.z_pos = z;
}

sint32 Move3DPosTo3DPos(PHD_3DPOS *pos, PHD_3DPOS *dest, sint32 speed, PHD_ANGLE rotation)
{
    sint32 dx, dy, dz, distance;
    PHD_ANGLE adiff;

    dx = dest->x_pos - pos->x_pos;
    dy = dest->y_pos - pos->y_pos;
    dz = dest->z_pos - pos->z_pos;
    distance = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));

    if (speed >= distance) {
        pos->x_pos = dest->x_pos;
        pos->y_pos = dest->y_pos;
        pos->z_pos = dest->z_pos;
    } else {
        pos->x_pos += speed * dx / distance;
        pos->y_pos += speed * dy / distance;
        pos->z_pos += speed * dz / distance;
    }

    if (!lara.IsMoving) {
        if (lara.water_status != LW_UNDERWATER) {
            switch ((((mGetAngle(dest->x_pos, dest->z_pos, pos->x_pos, pos->z_pos) + 8192) >> 14) - ((uint16)(dest->y_rot + 8192) >> 14)) & 3) {
            case 0:
                lara_item->anim_number = 65;
                lara_item->frame_number = anims[65].frame_base;
                lara_item->current_anim_state = AS_STEPLEFT;
                lara_item->goal_anim_state = AS_STEPLEFT;
                break;

            case 1:
                lara_item->anim_number = 1;
                lara_item->frame_number = anims[1].frame_base;
                lara_item->current_anim_state = AS_WALK;
                lara_item->goal_anim_state = AS_WALK;
                break;

            case 2:
                lara_item->anim_number = 67;
                lara_item->frame_number = anims[67].frame_base;
                lara_item->current_anim_state = AS_STEPRIGHT;
                lara_item->goal_anim_state = AS_STEPRIGHT;
                break;

            default:
                lara_item->anim_number = 40;
                lara_item->frame_number = anims[40].frame_base;
                lara_item->current_anim_state = AS_BACK;
                lara_item->goal_anim_state = AS_BACK;
                break;
            }

            lara.gun_status = LG_HANDS_BUSY;
        }

        lara.IsMoving = 1;
        lara.MoveCount = 0;
    }

    adiff = dest->x_rot - pos->x_rot;

    if (adiff > rotation)
        pos->x_rot += rotation;
    else if (adiff < -rotation)
        pos->x_rot -= rotation;
    else
        pos->x_rot = dest->x_rot;

    adiff = dest->y_rot - pos->y_rot;

    if (adiff > rotation)
        pos->y_rot += rotation;
    else if (adiff < -rotation)
        pos->y_rot -= rotation;
    else
        pos->y_rot = dest->y_rot;

    adiff = dest->z_rot - pos->z_rot;

    if (adiff > rotation)
        pos->z_rot += rotation;
    else if (adiff < -rotation)
        pos->z_rot -= rotation;
    else
        pos->z_rot = dest->z_rot;

    return pos->x_pos == dest->x_pos && pos->y_pos == dest->y_pos && pos->z_pos == dest->z_pos && pos->x_rot == dest->x_rot && pos->y_rot == dest->y_rot && pos->z_rot == dest->z_rot;
}

sint32 TestBoundsCollide2(ITEM_INFO *item, ITEM_INFO *l, sint32 rad)
{
    sint16 *bounds;
    sint32 s, c, dx, dz, x, z;

    bounds = GetBestFrame(l);
    if (item->pos.y_pos + GlobalCollisionBounds[3] <= l->pos.y_pos + bounds[2] ||
        item->pos.y_pos + GlobalCollisionBounds[2] >= l->pos.y_pos + bounds[3])
        return 0;
    c = phd_cos(item->pos.y_rot);
    s = phd_sin(item->pos.y_rot);
    dx = l->pos.x_pos - item->pos.x_pos;
    dz = l->pos.z_pos - item->pos.z_pos;
    x = (c * dx - s * dz) >> W2V_SHIFT;
    z = (c * dz + s * dx) >> W2V_SHIFT;
    return x >= GlobalCollisionBounds[0] - rad && x <= rad + GlobalCollisionBounds[1] && z >= GlobalCollisionBounds[4] - rad && z <= rad + GlobalCollisionBounds[5];
}

void CogCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->status == ITEM_INVISIBLE)
        return;
    if (!TestBoundsCollide(item, l, coll->radius))
        return;
    if (TriggerActive(item)) {
        DoBloodSplat(l->pos.x_pos + (GetRandomControl() & 0x3F) - 32, item->pos.y_pos + (GetRandomControl() & 0x1F) - 16, l->pos.z_pos + (GetRandomControl() & 0x3F) - 32, (GetRandomControl() & 3) + 2, GetRandomControl() << 1, l->room_number);
        lara_item->hit_points -= 10;
    } else if (coll->enable_baddie_push) {
        ItemPushLara(item, l, coll, 0, 0);
    }
}
