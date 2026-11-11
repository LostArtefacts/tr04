#include "laraclmb.h"
#include "camera.h"
#include "lara.h"
#include "lara_states.h"
#include "specific/input.h"

sint32 LaraTestClimbPos(ITEM_INFO *item, sint32 front, sint32 right, sint32 origin, sint32 height, sint32 *shift)
{
    sint32 angle, zfront, xfront, z, x;

    xfront = 0;
    zfront = 0;
    angle = (uint16)(item->pos.y_rot + 0x2000) / 0x4000;
    switch (angle) {
    case NORTH:
        z = front + item->pos.z_pos;
        x = right + item->pos.x_pos;
        zfront = 256;
        break;
    case EAST:
        x = front + item->pos.x_pos;
        z = item->pos.z_pos - right;
        xfront = 256;
        break;
    case SOUTH:
        z = item->pos.z_pos - front;
        x = item->pos.x_pos - right;
        zfront = -256;
        break;
    default:
        x = item->pos.x_pos - front;
        z = item->pos.z_pos + right;
        xfront = -256;
        break;
    }
    return LaraTestClimb(x, origin + item->pos.y_pos, z, xfront, zfront, height, item->room_number, shift);
}
