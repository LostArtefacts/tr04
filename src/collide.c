#include <stdlib.h>
#include <string.h>

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
#include "objnums.h"
#include "sound.h"

void Func488650(void);

void GetCollisionInfo(COLL_INFO *coll, sint32 x, sint32 y, sint32 z, sint16 room_number, sint32 hite)
{
    sint16 tilt;
    static sint32 xfront, zfront;
    FLOOR_INFO *floor;
    sint32 fspeed, zright, xright, xleft, xright2, xleft2, zleft2, zleft, ang, zright2;
    sint16 room_num2, room_num;
    sint32 tz, yT, tx, h, c, yh;

    fspeed = lara_item->fallspeed;
    coll->coll_type = COLL_NONE;
    coll->shift.x = coll->shift.y = coll->shift.z = 0;
    coll->quadrant = (uint16)(coll->facing + 0x2000) / 0x4000;
    ang = abs(lara_item->pos.y_rot - coll->facing) > 0x7000 ? 0x3000 : 0x4000;
    xleft2 = (250 * phd_sin(coll->facing - ang)) >> W2V_SHIFT;
    zleft2 = (250 * phd_cos(coll->facing - ang)) >> W2V_SHIFT;
    xright2 = (250 * phd_sin(coll->facing + ang)) >> W2V_SHIFT;
    zright2 = (250 * phd_cos(coll->facing + ang)) >> W2V_SHIFT;
    yh = y - hite;
    yT = yh - 160;
    room_num = room_number;
    floor = GetFloor(x, yT, z, &room_num);
    h = GetHeight(floor, x, yT, z);
    if (h != NO_HEIGHT)
        h -= y;
    c = GetCeiling(floor, x, yT - fspeed, z);
    if (c != NO_HEIGHT)
        c -= yh;
    coll->mid_floor = h;
    coll->mid_ceiling = c;
    coll->mid_type = height_type;
    coll->trigger = trigger_index;
    tilt = GetTiltType(floor, x, lara_item->pos.y_pos, z);
    coll->tilt_x = (sint8)tilt;
    coll->tilt_z = tilt >> 8;
    switch (coll->quadrant) {
    case NORTH:
        xfront = (coll->radius * phd_sin(coll->facing)) >> W2V_SHIFT;
        zfront = coll->radius;
        xright = coll->radius;
        zright = coll->radius;
        zleft = coll->radius;
        xleft = -coll->radius;
        break;
    case EAST:
        xfront = coll->radius;
        zfront = (coll->radius * phd_cos(coll->facing)) >> W2V_SHIFT;
        xright = coll->radius;
        zright = -coll->radius;
        zleft = coll->radius;
        xleft = coll->radius;
        break;
    case SOUTH:
        xfront = (coll->radius * phd_sin(coll->facing)) >> W2V_SHIFT;
        zfront = -coll->radius;
        xright = -coll->radius;
        zright = -coll->radius;
        zleft = -coll->radius;
        xleft = coll->radius;
        break;
    case WEST:
        xfront = -coll->radius;
        zfront = (coll->radius * phd_cos(coll->facing)) >> W2V_SHIFT;
        xright = -coll->radius;
        zright = coll->radius;
        zleft = -coll->radius;
        xleft = -coll->radius;
        break;
    default:
        xfront = 0;
        zfront = 0;
        xright = 0;
        zright = 0;
        zleft = 0;
        xleft = 0;
        break;
    }
    tx = x + xfront;
    tz = z + zfront;
    floor = GetFloor(tx, yT, tz, &room_num);
    h = GetHeight(floor, tx, yT, tz);
    if (h != NO_HEIGHT)
        h -= y;
    c = GetCeiling(floor, tx, yT - fspeed, tz);
    if (c != NO_HEIGHT)
        c -= yh;
    coll->front_ceiling = c;
    coll->front_floor = h;
    coll->front_type = height_type;
    tx += xfront;
    tz += zfront;
    floor = GetFloor(tx, yT, tz, &room_num);
    h = GetHeight(floor, tx, yT, tz);
    if (h != NO_HEIGHT)
        h -= y;
    if (coll->slopes_are_walls && (coll->front_type == BIG_SLOPE || coll->front_type == DIAGONAL) && coll->front_floor < coll->mid_floor && h < coll->front_floor && coll->front_floor < 0)
        coll->front_floor = -32767;
    else if (coll->slopes_are_pits && (coll->front_type == BIG_SLOPE || coll->front_type == DIAGONAL) && coll->front_floor > coll->mid_floor)
        coll->front_floor = 512;
    else if (coll->lava_is_pit && coll->front_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
        coll->front_floor = 512;
    room_num2 = room_number;
    tx = x + xleft;
    tz = z + zleft;
    floor = GetFloor(tx, yT, tz, &room_num2);
    h = GetHeight(floor, tx, yT, tz);
    if (h != NO_HEIGHT)
        h -= y;
    c = GetCeiling(floor, tx, yT - fspeed, tz);
    if (c != NO_HEIGHT)
        c -= yh;
    coll->left_ceiling = c;
    coll->left_floor = h;
    coll->left_type = height_type;
    if (coll->slopes_are_walls == 1 && (coll->left_type == BIG_SLOPE || coll->left_type == DIAGONAL) && coll->left_floor < 0)
        coll->left_floor = -32767;
    else if (coll->slopes_are_pits && (coll->left_type == BIG_SLOPE || coll->left_type == DIAGONAL) && coll->left_floor > 0)
        coll->left_floor = 512;
    else if (coll->lava_is_pit && coll->left_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
        coll->left_floor = 512;
    floor = GetFloor(tx, yT, tz, &room_num);
    h = GetHeight(floor, tx, yT, tz);
    if (h != NO_HEIGHT)
        h -= y;
    c = GetCeiling(floor, tx, yT - fspeed, tz);
    if (c != NO_HEIGHT)
        c -= yh;
    coll->left_ceiling2 = c;
    coll->left_floor2 = h;
    coll->left_type2 = height_type;
    if (coll->slopes_are_walls == 1 && (coll->left_type2 == BIG_SLOPE || coll->left_type2 == DIAGONAL) && coll->left_floor2 < 0)
        coll->left_floor2 = -32767;
    else if (coll->slopes_are_pits && (coll->left_type2 == BIG_SLOPE || coll->left_type2 == DIAGONAL) && coll->left_floor2 > 0)
        coll->left_floor2 = 512;
    else if (coll->lava_is_pit && coll->left_floor2 > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
        coll->left_floor2 = 512;
    room_num2 = room_number;
    tx = x + xright;
    tz = z + zright;
    floor = GetFloor(tx, yT, tz, &room_num2);
    h = GetHeight(floor, tx, yT, tz);
    if (h != NO_HEIGHT)
        h -= y;
    c = GetCeiling(floor, tx, yT - fspeed, tz);
    if (c != NO_HEIGHT)
        c -= yh;
    coll->right_ceiling = c;
    coll->right_floor = h;
    coll->right_type = height_type;
    if (coll->slopes_are_walls == 1 && (coll->right_type == BIG_SLOPE || coll->right_type == DIAGONAL) && coll->right_floor < 0)
        coll->right_floor = -32767;
    else if (coll->slopes_are_pits && (coll->right_type == BIG_SLOPE || coll->right_type == DIAGONAL) && coll->right_floor > 0)
        coll->right_floor = 512;
    else if (coll->lava_is_pit && coll->right_floor > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
        coll->right_floor = 512;
    floor = GetFloor(tx, yT, tz, &room_num);
    h = GetHeight(floor, tx, yT, tz);
    if (h != NO_HEIGHT)
        h -= y;
    c = GetCeiling(floor, tx, yT - fspeed, tz);
    if (c != NO_HEIGHT)
        c -= yh;
    coll->right_ceiling2 = c;
    coll->right_floor2 = h;
    coll->right_type2 = height_type;
    if (coll->slopes_are_walls == 1 && (coll->right_type2 == BIG_SLOPE || coll->right_type2 == DIAGONAL) && coll->right_floor2 < 0)
        coll->right_floor2 = -32767;
    else if (coll->slopes_are_pits && (coll->right_type2 == BIG_SLOPE || coll->right_type2 == DIAGONAL) && coll->right_floor2 > 0)
        coll->right_floor2 = 512;
    else if (coll->lava_is_pit && coll->right_floor2 > 0 && trigger_index && (trigger_index[0] & 0x1F) == LAVA_TYPE)
        coll->right_floor2 = 512;
    room_num2 = room_number;
    tx = x + xleft2;
    tz = z + zleft2;
    floor = GetFloor(tx, yT, tz, &room_num2);
    GetHeight(floor, tx, yT, tz);
    GetCeiling(floor, tx, yT - fspeed, tz);
    room_num2 = room_number;
    tx = x + xright2;
    tz = z + zright2;
    floor = GetFloor(tx, yT, tz, &room_num2);
    GetHeight(floor, tx, yT, tz);
    GetCeiling(floor, tx, yT - fspeed, tz);
    CollideStaticObjects(coll, x, y, z, room_number, hite);
    if (coll->mid_floor == NO_HEIGHT) {
        coll->shift.x = coll->old.x - x;
        coll->shift.y = coll->old.y - y;
        coll->shift.z = coll->old.z - z;
        coll->coll_type = COLL_FRONT;
        return;
    }
    if (coll->mid_floor - coll->mid_ceiling <= 0) {
        coll->shift.x = coll->old.x - x;
        coll->shift.y = coll->old.y - y;
        coll->shift.z = coll->old.z - z;
        coll->coll_type = COLL_CLAMP;
        return;
    }
    if (coll->mid_ceiling >= 0) {
        coll->hit_ceiling = 1;
        coll->shift.y = coll->mid_ceiling;
        coll->coll_type = COLL_TOP;
    }
    if (coll->front_floor > coll->bad_pos || coll->front_floor < coll->bad_neg || coll->front_ceiling > coll->bad_ceiling) {
        if (coll->front_type == DIAGONAL || coll->front_type == SPLIT_TRI) {
            coll->shift.x = coll->old.x - x;
            coll->shift.z = coll->old.z - z;
        } else {
            switch (coll->quadrant) {
            case NORTH:
            case SOUTH:
                coll->shift.x = coll->old.x - x;
                coll->shift.z = FindGridShift(z + zfront, z);
                break;
            case EAST:
            case WEST:
                coll->shift.x = FindGridShift(x + xfront, x);
                coll->shift.z = coll->old.z - z;
                break;
            }
        }
        coll->coll_type = COLL_FRONT;
        return;
    }
    if (coll->front_ceiling >= coll->bad_ceiling) {
        coll->shift.x = coll->old.x - x;
        coll->shift.y = coll->old.y - y;
        coll->shift.z = coll->old.z - z;
        coll->coll_type = COLL_TOPFRONT;
        return;
    }
    if (coll->left_floor > coll->bad_pos || coll->left_floor < coll->bad_neg || coll->left_ceiling > coll->bad_ceiling) {
        if (coll->left_type == SPLIT_TRI) {
            coll->shift.x = coll->old.x - x;
            coll->shift.z = coll->old.z - z;
        } else {
            switch (coll->quadrant) {
            case NORTH:
            case SOUTH:
                coll->shift.x = FindGridShift(x + xleft, x + xfront);
                break;
            case EAST:
            case WEST:
                coll->shift.z = FindGridShift(z + zleft, z + zfront);
                break;
            }
        }
        coll->coll_type = COLL_LEFT;
        return;
    }
    if (coll->right_floor > coll->bad_pos || coll->right_floor < coll->bad_neg || coll->right_ceiling > coll->bad_ceiling) {
        if (coll->right_type == SPLIT_TRI) {
            coll->shift.x = coll->old.x - x;
            coll->shift.z = coll->old.z - z;
        } else {
            switch (coll->quadrant) {
            case NORTH:
            case SOUTH:
                coll->shift.x = FindGridShift(x + xright, x + xfront);
                break;
            case EAST:
            case WEST:
                coll->shift.z = FindGridShift(z + zright, z + zfront);
                break;
            }
        }
        coll->coll_type = COLL_RIGHT;
        return;
    }
}

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

sint32 ItemPushLara(ITEM_INFO *item, ITEM_INFO *l, COLL_INFO *coll, sint32 spaz, sint32 big_push)
{
    sint16 *bounds;
    sint32 dx, dz, s, c, x, z;
    sint32 xmin, xmax, zmin, zmax, left, top, right, bottom;
    sint16 facing;

    dx = l->pos.x_pos - item->pos.x_pos;
    dz = l->pos.z_pos - item->pos.z_pos;
    c = phd_cos(item->pos.y_rot);
    s = phd_sin(item->pos.y_rot);
    x = (dx * c - dz * s) >> W2V_SHIFT;
    z = (dx * s + dz * c) >> W2V_SHIFT;

    if (big_push & 2)
        bounds = GlobalCollisionBounds;
    else
        bounds = GetBestFrame(item);

    xmin = bounds[0];
    xmax = bounds[1];
    zmin = bounds[4];
    zmax = bounds[5];

    if (big_push & 1) {
        xmin -= coll->radius;
        xmax += coll->radius;
        zmin -= coll->radius;
        zmax += coll->radius;
    }

    if (ABS(dx) > 4608 || ABS(dz) > 4608 || x <= xmin || x >= xmax || z <= zmin || z >= zmax)
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

    l->pos.x_pos = item->pos.x_pos + ((c * x + s * z) >> W2V_SHIFT);
    l->pos.z_pos = item->pos.z_pos + ((c * z - s * x) >> W2V_SHIFT);

    if (spaz && bounds[3] - bounds[2] > 256 && item->object_number != VON_CROY && item->object_number != GUIDE && item->object_number != ENEMY_JEEP) {
        x = (bounds[0] + bounds[1]) / 2;
        z = (bounds[4] + bounds[5]) / 2;
        dx -= (c * x + s * z) >> W2V_SHIFT;
        dz -= (c * z - s * x) >> W2V_SHIFT;
        lara.hit_direction = (uint16)(l->pos.y_rot - phd_atan(dz, dx) - 24576) >> W2V_SHIFT;

        if (!lara.hit_frame)
            SoundEffect(SFX_LARA_INJURY, &l->pos, SFX_DEFAULT);

        lara.hit_frame++;

        if (lara.hit_frame > 34)
            lara.hit_frame = 34;
    }

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

    if (lara.IsMoving && lara.MoveCount > 15) {
        lara.IsMoving = 0;
        lara.gun_status = LG_NO_ARMS;
    }

    return 1;
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

sint32 MoveLaraPosition(PHD_VECTOR *v, ITEM_INFO *item, ITEM_INFO *l)
{
    PHD_3DPOS pos;
    FLOOR_INFO *floor;
    sint32 height;
    sint16 room_number;

    pos.x_rot = item->pos.x_rot;
    pos.y_rot = item->pos.y_rot;
    pos.z_rot = item->pos.z_rot;
    phd_PushUnitMatrix();
    ((void (*)(sint32, sint32, sint32))phd_RotYXZ)(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    pos.x_pos = item->pos.x_pos + ((phd_mxptr[M00] * v->x + phd_mxptr[M01] * v->y + phd_mxptr[M02] * v->z) >> W2V_SHIFT);
    pos.y_pos = item->pos.y_pos + ((phd_mxptr[M10] * v->x + phd_mxptr[M11] * v->y + phd_mxptr[M12] * v->z) >> W2V_SHIFT);
    pos.z_pos = item->pos.z_pos + ((phd_mxptr[M20] * v->x + phd_mxptr[M21] * v->y + phd_mxptr[M22] * v->z) >> W2V_SHIFT);
    phd_PopMatrix();
    if (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM || item->object_number == CLOCKWORK_BEETLE) {
        room_number = l->room_number;
        floor = GetFloor(pos.x_pos, pos.y_pos, pos.z_pos, &room_number);
        height = GetHeight(floor, pos.x_pos, pos.y_pos, pos.z_pos);
        if (ABS(height - l->pos.y_pos) > 512) {
            if (lara.IsMoving) {
                lara.IsMoving = 0;
                lara.gun_status = LG_NO_ARMS;
            }
            return 0;
        }
        if (phd_sqrt(SQUARE(pos.x_pos - l->pos.x_pos) + SQUARE(pos.y_pos - l->pos.y_pos) + SQUARE(pos.z_pos - l->pos.z_pos)) < 128)
            return 1;
    }
    return Move3DPosTo3DPos(&l->pos, &pos, 12, 364);
}

void GenericSphereBoxCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    SPHERE *sptr;
    sint32 TouchBits, DeadlyBits, dx, dy, dz;
    sint16 y_rot;

    item = &items[item_number];
    if (item->status != ITEM_INVISIBLE && TestBoundsCollide(item, l, coll->radius)) {
        TouchBits = TestCollision(item, l);
        if (TouchBits) {
            y_rot = item->pos.y_rot;
            item->pos.y_rot = 0;
            GetSpheres(item, Slist, 1);
            item->pos.y_rot = y_rot;
            DeadlyBits = *(sint32 *)&item->item_flags[0];
            if (item->item_flags[2])
                TouchBits &= ~0x1;
            if (TouchBits) {
                sptr = Slist;
                do {
                    if (TouchBits & 0x1) {
                        GlobalCollisionBounds[0] = (sint16)(sptr->x - item->pos.x_pos - sptr->r);
                        GlobalCollisionBounds[2] = (sint16)(sptr->y - item->pos.y_pos - sptr->r);
                        GlobalCollisionBounds[4] = (sint16)(sptr->z - item->pos.z_pos - sptr->r);
                        GlobalCollisionBounds[1] = (sint16)(sptr->x - item->pos.x_pos + sptr->r);
                        GlobalCollisionBounds[3] = (sint16)(sptr->y - item->pos.y_pos + sptr->r);
                        GlobalCollisionBounds[5] = (sint16)(sptr->z - item->pos.z_pos + sptr->r);
                        dx = lara_item->pos.x_pos;
                        dy = lara_item->pos.y_pos;
                        dz = lara_item->pos.z_pos;
                        if (ItemPushLara(item, l, coll, DeadlyBits & 0x1, 3) && DeadlyBits & 0x1) {
                            lara_item->hit_points -= item->item_flags[3];
                            dx -= lara_item->pos.x_pos;
                            dy -= lara_item->pos.y_pos;
                            dz -= lara_item->pos.z_pos;
                            if ((dx || dy || dz) && TriggerActive(item))
                                DoBloodSplat(l->pos.x_pos + (GetRandomControl() & 0x3F) - 32, sptr->y + (GetRandomControl() & 0x1F) - 16, l->pos.z_pos + (GetRandomControl() & 0x3F) - 32, (item->item_flags[3] >> 5) + (GetRandomControl() & 0x3) + 2, (sint16)(2 * GetRandomControl()), l->room_number);
                            if (!coll->enable_baddie_push) {
                                lara_item->pos.x_pos += dx;
                                lara_item->pos.y_pos += dy;
                                lara_item->pos.z_pos += dz;
                            }
                        }
                    }
                    TouchBits >>= 1;
                    sptr++;
                    DeadlyBits >>= 1;
                } while (TouchBits);
            }
        }
    }
}

void GenericDeadlyBoundingBoxCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 dx, dy, dz;

    item = &items[item_number];
    if (item->status != ITEM_INVISIBLE && item->item_flags[3] && TestBoundsCollide(item, l, coll->radius)) {
        dx = lara_item->pos.x_pos;
        dy = lara_item->pos.y_pos;
        dz = lara_item->pos.z_pos;
        if (ItemPushLara(item, l, coll, 1, 1)) {
            lara_item->hit_points -= item->item_flags[3];
            dx -= lara_item->pos.x_pos;
            dy -= lara_item->pos.y_pos;
            dz -= lara_item->pos.z_pos;
            if ((dx || dy || dz) && TriggerActive(item))
                DoBloodSplat(l->pos.x_pos + (GetRandomControl() & 0x3F) - 32, l->pos.y_pos - (GetRandomControl() & 0x1FF) - 256, l->pos.z_pos + (GetRandomControl() & 0x3F) - 32, (item->item_flags[3] >> 5) + (GetRandomControl() & 0x3) + 2, (sint16)(2 * GetRandomControl()), l->room_number);
            if (!coll->enable_baddie_push) {
                lara_item->pos.x_pos += dx;
                lara_item->pos.y_pos += dy;
                lara_item->pos.z_pos += dz;
            }
        }
    }
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

void StargateCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    SPHERE *sphere;
    sint32 touchedBits, hurtfulBits, x, y, z;
    sint32 i;

    item = &items[item_number];
    if (item->status == ITEM_INVISIBLE)
        return;
    if (!TestBoundsCollide(item, l, coll->radius))
        return;
    for (i = 0; i < 24; i += 6) {
        memcpy(GlobalCollisionBounds, &StarGateBounds[i], 12);
        if (TestBoundsCollide2(item, l, coll->radius))
            ItemPushLara(item, l, coll, 0, 2);
    }
    hurtfulBits = TestCollision(item, l);
    if (!hurtfulBits)
        return;
    touchedBits = *(sint32 *)&item->item_flags[0];
    hurtfulBits &= touchedBits;
    if (!hurtfulBits)
        return;
    sphere = Slist;
    while (hurtfulBits) {
        if (hurtfulBits & 1) {
            GlobalCollisionBounds[0] = (sint16)(sphere->x - item->pos.x_pos - sphere->r);
            GlobalCollisionBounds[2] = (sint16)(sphere->y - item->pos.y_pos - sphere->r);
            GlobalCollisionBounds[4] = (sint16)(sphere->z - item->pos.z_pos - sphere->r);
            GlobalCollisionBounds[1] = (sint16)(sphere->x + sphere->r - item->pos.x_pos);
            GlobalCollisionBounds[3] = (sint16)(sphere->y + sphere->r - item->pos.y_pos);
            GlobalCollisionBounds[5] = (sint16)(sphere->z + sphere->r - item->pos.z_pos);
            x = lara_item->pos.x_pos;
            y = lara_item->pos.y_pos;
            z = lara_item->pos.z_pos;
            if (ItemPushLara(item, l, coll, touchedBits & 1, 2) && touchedBits & 1) {
                x -= lara_item->pos.x_pos;
                y -= lara_item->pos.y_pos;
                z -= lara_item->pos.z_pos;
                if ((x || y || z) && TriggerActive(item)) {
                    DoBloodSplat(l->pos.x_pos + (GetRandomControl() & 0x3F) - 32, sphere->y + (GetRandomControl() & 0x1F) - 16, l->pos.z_pos + (GetRandomControl() & 0x3F) - 32, (GetRandomControl() & 3) + 2, (sint16)(GetRandomControl() << 1), l->room_number);
                    lara_item->hit_points -= 100;
                }
            }
        }
        hurtfulBits >>= 1;
        sphere++;
        touchedBits >>= 1;
    }
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

sint32 GetCollidedObjects(ITEM_INFO *item, sint32 rad, sint32 noInvisible, ITEM_INFO **StoredItems, MESH_INFO **StoredStatics, sint32 StoreLara)
{
    MESH_INFO *mesh;
    ROOM_INFO *r;
    ITEM_INFO *item2;
    sint16 *doors;
    sint16 *bounds;
    sint32 j, sy, cy, dx, dy, dz, num, i;
    sint16 rooms[22];
    sint16 switch_bounds[6];
    sint16 room_count, statics_count, items_count, item_number, next_item;

    rooms[0] = item->room_number;
    r = &room[rooms[0]];
    doors = r->door;
    room_count = 1;
    statics_count = 0;
    items_count = 0;
    if (doors) {
        for (i = *doors++; i > 0; i--, doors += 16) {
            for (j = 0; j < room_count; j++)
                if (rooms[j] == *doors)
                    break;
            if (j == room_count) {
                rooms[room_count] = *doors;
                room_count++;
            }
        }
    }
    if (StoredStatics) {
        for (i = 0; i < room_count; i++) {
            r = &room[rooms[i]];
            mesh = r->mesh;
            for (j = r->num_meshes; j > 0; j--, mesh++) {
                if (mesh->Flags & 1) {
                    bounds = &static_objects[mesh->static_number].x_minc;
                    if (item->pos.y_pos + rad + 128 >= mesh->y + bounds[2] && item->pos.y_pos - rad - 128 <= mesh->y + bounds[3]) {
                        sy = phd_sin(mesh->y_rot);
                        cy = phd_cos(mesh->y_rot);
                        dx = item->pos.x_pos - mesh->x;
                        dz = item->pos.z_pos - mesh->z;
                        num = (dx * cy - sy * dz) >> W2V_SHIFT;
                        if (rad + num + 128 >= bounds[0] && num - rad - 128 <= bounds[1]) {
                            num = (dx * sy + cy * dz) >> W2V_SHIFT;
                            if (rad + num + 128 >= bounds[4] && num - rad - 128 <= bounds[5]) {
                                StoredStatics[statics_count] = mesh;
                                statics_count++;
                                if (!rad) {
                                    StoredItems[0] = 0;
                                    return 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        StoredStatics[statics_count] = 0;
    }
    for (i = 0; i < room_count; i++) {
        item_number = room[rooms[i]].item_number;
        while (item_number != NO_ITEM) {
            item2 = &items[item_number];
            next_item = item2->next_item;
            if (item2 == item) {
                item_number = next_item;
                continue;
            }
            if (!StoreLara && item2 == lara_item) {
                item_number = next_item;
                continue;
            }
            if ((sint16)(item2->flags & IFL_CLEARBODY)) {
                item_number = next_item;
                continue;
            }
            if (item2->object_number == BURNING_FLOOR || !objects[item2->object_number].collision && item2->object_number != LARA) {
                item_number = next_item;
                continue;
            }
            if (!objects[item2->object_number].draw_routine || !item2->mesh_bits) {
                item_number = next_item;
                continue;
            }
            if (noInvisible && item2->status == ITEM_INVISIBLE) {
                item_number = next_item;
                continue;
            }
            dx = item->pos.x_pos - item2->pos.x_pos;
            dy = item->pos.y_pos - item2->pos.y_pos;
            dz = item->pos.z_pos - item2->pos.z_pos;
            if (dx < -2048 || dx > 2048 || dy < -2048 || dy > 2048 || dz < -2048 || dz > 2048) {
                item_number = next_item;
                continue;
            }
            bounds = GetBestFrame(item2);
            if (item->pos.y_pos + rad + 128 < item2->pos.y_pos + bounds[2] || item->pos.y_pos - rad - 128 > item2->pos.y_pos + bounds[3]) {
                item_number = next_item;
                continue;
            }
            sy = phd_sin(item2->pos.y_rot);
            cy = phd_cos(item2->pos.y_rot);
            dx = item->pos.x_pos - item2->pos.x_pos;
            dz = item->pos.z_pos - item2->pos.z_pos;
            num = (dx * cy - sy * dz) >> W2V_SHIFT;
            if (item2->object_number == TURN_SWITCH) {
                switch_bounds[0] = -256;
                switch_bounds[1] = 256;
                switch_bounds[4] = -256;
                switch_bounds[5] = 256;
                bounds = switch_bounds;
            }
            if (rad + num + 128 >= bounds[0] && num - rad - 128 <= bounds[1]) {
                num = (dx * sy + cy * dz) >> W2V_SHIFT;
                if (rad + num + 128 >= bounds[4] && num - rad - 128 <= bounds[5]) {
                    StoredItems[items_count] = item2;
                    items_count++;
                    if (!rad)
                        return 1;
                }
            }
            item_number = next_item;
        }
    }
    StoredItems[items_count] = 0;
    return items_count | statics_count;
}
