#include "jeep.h"
#include "control.h"
#include "lara.h"
#include "lara1gun.h"
#include "lara_states.h"
#include "anim.h"
#include "sound.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/function_stubs.h"
#include "specific/specificfx.h"


void DrawJeepExtras(ITEM_INFO *item)
{
    JEEPINFO *jeep;

    if (lara.vehicle == NO_ITEM)
        return;
    jeep = (JEEPINFO *)item->data;
    DrawJeepSpeedo(phd_winwidth - 64, phd_winheight - 16, jeep->velocity, 0x6000, 0x8000, 32, jeep->gear);
}

sint32 DoShift(ITEM_INFO *item, PHD_VECTOR *pos, PHD_VECTOR *old)
{
    sint32 x, z, x_old, z_old, shift_x, shift_z;
    sint16 room_number;
    FLOOR_INFO *floor;
    sint32 height;

    x = pos->x >> WALL_SHIFT;
    z = pos->z >> WALL_SHIFT;
    x_old = old->x >> WALL_SHIFT;
    z_old = old->z >> WALL_SHIFT;
    shift_x = pos->x & (WALL_L - 1);
    shift_z = pos->z & (WALL_L - 1);
    if (x == x_old) {
        if (z == z_old) {
            item->pos.z_pos += (old->z - pos->z);
            item->pos.x_pos += (old->x - pos->x);
        } else if (z > z_old) {
            item->pos.z_pos -= shift_z + 1;
            return (pos->x - item->pos.x_pos);
        } else {
            item->pos.z_pos += WALL_L + 1 - shift_z;
            return (item->pos.x_pos - pos->x);
        }
    } else if (z == z_old) {
        if (x > x_old) {
            item->pos.x_pos -= shift_x + 1;
            return (item->pos.z_pos - pos->z);
        } else {
            item->pos.x_pos += WALL_L + 1 - shift_x;
            return (pos->z - item->pos.z_pos);
        }
    } else {
        x = z = 0;
        room_number = item->room_number;
        floor = GetFloor(old->x, pos->y, pos->z, &room_number);
        height = GetHeight(floor, old->x, pos->y, pos->z);
        if (height < old->y - STEP_L) {
            if (pos->z > old->z)
                z = -shift_z - 1;
            else
                z = WALL_L + 1 - shift_z;
        }
        room_number = item->room_number;
        floor = GetFloor(pos->x, pos->y, old->z, &room_number);
        height = GetHeight(floor, pos->x, pos->y, old->z);
        if (height < old->y - STEP_L) {
            if (pos->x > old->x)
                x = -shift_x - 1;
            else
                x = WALL_L + 1 - shift_x;
        }
        if (x && z) {
            item->pos.z_pos += z;
            item->pos.x_pos += x;
        } else if (z) {
            item->pos.z_pos += z;
            if (z > 0)
                return (item->pos.x_pos - pos->x);
            else
                return (pos->x - item->pos.x_pos);
        } else if (x) {
            item->pos.x_pos += x;
            if (x > 0)
                return (pos->z - item->pos.z_pos);
            else
                return (item->pos.z_pos - pos->z);
        } else {
            item->pos.z_pos += (old->z - pos->z);
            item->pos.x_pos += (old->x - pos->x);
        }
    }
    return 0;
}
