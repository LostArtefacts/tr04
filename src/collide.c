#include "collide.h"
#include "draw.h"
#include "room.h"
#include "specific/3dmath.h"
#include "control.h"
#include "lara.h"
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
