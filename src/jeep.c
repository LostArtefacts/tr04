#include "jeep.h"
#include "control.h"
#include "lara.h"
#include "lara1gun.h"
#include "lara_states.h"
#include "anim.h"
#include "objnums.h"
#include "box.h"
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

void JeepExplode(ITEM_INFO *item)
{
    sint32 i;

    if (room[item->room_number].flags & ROOM_UNDERWATER)
        TriggerUnderwaterExplosion(item, 1);
    else {
        TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);
        for (i = 0; i < 3; i++)
            TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
    }
    ExplodingDeath2(lara.vehicle, -1, 256);
    KillItem(lara.vehicle);
    item->status = ITEM_DEACTIVATED;
    SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
    SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
    lara.vehicle = NO_ITEM;
}

sint32 JeepCheckGetOut(void)
{
    ITEM_INFO *item;

    item = lara_item;
    if (item->current_anim_state == 10 && item->frame_number == anims[item->anim_number].frame_end) {
        item->pos.y_rot += 0x4000;
        item->anim_number = ANIM_STOP;
        item->frame_number = anims[ANIM_STOP].frame_base;
        item->current_anim_state = item->goal_anim_state = AS_STOP;
        item->pos.x_pos -= 512 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
        item->pos.z_pos -= 512 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
        item->pos.x_rot = item->pos.z_rot = 0;
        lara.vehicle = NO_ITEM;
        lara.gun_status = LG_NO_ARMS;
        CurrentAtmosphere = 110;
        IsAtmospherePlaying = 1;
        S_CDPlay(110, 1);
    }
    return 1;
}

sint32 DoDynamics_jeep(sint32 height, sint32 fallspeed, sint32 *y, sint32 zero)
{
    sint32 kick;

    if (height > *y) {
        *y += fallspeed;
        if (*y > height - 32) {
            *y = height;
            if (fallspeed > 150)
                lara_item->hit_points += 150 - fallspeed;
            fallspeed = 0;
        } else if (zero)
            fallspeed += zero + (zero >> 1);
        else
            fallspeed += 9;
    } else {
        if (!zero) {
            kick = height - *y;
            if (kick < -80)
                kick = -80;
            fallspeed += (kick - fallspeed) >> 4;
            if (*y > height)
                *y = height;
        } else
            return fallspeed;
    }
    return fallspeed;
}

sint32 GetCollisionAnim(ITEM_INFO *item, PHD_VECTOR *moved, JEEPINFO *jeep)
{
    sint32 c, s, front, side;

    moved->x = item->pos.x_pos - moved->x;
    moved->z = item->pos.z_pos - moved->z;
    if (!moved->x && !moved->z)
        return 0;
    c = phd_cos(item->pos.y_rot);
    s = phd_sin(item->pos.y_rot);
    front = (moved->z * c + moved->x * s) >> W2V_SHIFT;
    side = (moved->x * c - moved->z * s) >> W2V_SHIFT;
    if (ABS(front) > ABS(side)) {
        if (front > 0)
            return 14;
        else
            return 13;
    } else {
        if (side > 0)
            return 11;
        else
            return 12;
    }
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

sint32 CanGetOff_jeep(sint16 num)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 x, y, z, h, c;
    sint16 yrot, room_number;

    item = &items[lara.vehicle];
    yrot = item->pos.y_rot + 0x4000;
    x = item->pos.x_pos - (512 * phd_sin(yrot) >> W2V_SHIFT);
    y = item->pos.y_pos;
    z = item->pos.z_pos - (512 * phd_cos(yrot) >> W2V_SHIFT);
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (height_type != BIG_SLOPE && height_type != DIAGONAL && h != NO_HEIGHT) {
        if (ABS(h - item->pos.y_pos) <= 512) {
            c = GetCeiling(floor, x, y, z);
            if (c - item->pos.y_pos <= -762 && h - c >= 762)
                return 1;
        }
    }
    return 0;
}

void InitialiseEnemyJeep(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[item->object_number].anim_index + 14;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 0;
    item->mesh_bits = ~0x24000;
    item->status += ITEM_ACTIVE;
}

void JeepStart(ITEM_INFO *item, ITEM_INFO *l)
{
    JEEPINFO *jeep;

    jeep = (JEEPINFO *)item->data;
    lara.gun_status = LG_HANDS_BUSY;
    lara.hit_direction = -1;
    l->current_anim_state = l->goal_anim_state = 0;
    l->anim_number = objects[VEHICLE_EXTRA].anim_index + 14;
    l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 14].frame_base;
    item->anim_number = objects[JEEP].anim_index - objects[VEHICLE_EXTRA].anim_index + l->anim_number;
    item->frame_number = anims[item->anim_number].frame_base - anims[l->anim_number].frame_base + l->frame_number;
    item->flags |= IFL_TRIGGERED;
    item->hit_points = 1;
    jeep->unused1 = 0;
    jeep->gear = 0;
}
