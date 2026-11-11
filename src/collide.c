#include "collide.h"
#include "room.h"
#include "specific/3dmath.h"
#include "control.h"
#include "lara.h"
#include "sphere.h"

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
