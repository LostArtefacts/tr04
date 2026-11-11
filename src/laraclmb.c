#include "laraclmb.h"
#include "anim.h"
#include "camera.h"
#include "control.h"
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

void lara_as_climbstnc(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.IsClimbing = 1;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.target_elevation = -3640;
    if (input & IN_LOOK)
        LookUpDown();
    if (input & (IN_LEFT | IN_LSTEP)) {
        item->goal_anim_state = AS_CLIMBLEFT;
        lara.move_angle = item->pos.y_rot - 16384;
    } else if (input & (IN_RIGHT | IN_RSTEP)) {
        item->goal_anim_state = AS_CLIMBRIGHT;
        lara.move_angle = item->pos.y_rot + 16384;
    } else if (input & IN_JUMP) {
        if (item->anim_number == ANIM_CLIMBSTNC) {
            item->goal_anim_state = AS_BACKJUMP;
            lara.move_angle = item->pos.y_rot + 32768;
            lara.gun_status = LG_NO_ARMS;
        }
    }
}

void lara_col_climbstnc(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 result_r, result_l, shift_r, shift_l, ledge_r, ledge_l;

    if (LaraCheckForLetGo(item, coll) || item->anim_number != ANIM_CLIMBSTNC)
        return;
    if (input & IN_FORWARD) {
        if (item->goal_anim_state == AS_NULL)
            return;
        item->goal_anim_state = AS_CLIMBSTNC;
        result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
        result_l = LaraTestClimbUpPos(item, coll->radius, -(coll->radius + 120), &shift_l, &ledge_l);
        if (!result_r || !result_l)
            return;
        if (result_r < 0 || result_l < 0) {
            if (ABS(ledge_l - ledge_r) > 120)
                return;
            item->goal_anim_state = AS_NULL;
            item->pos.y_pos += (ledge_l + ledge_r) / 2 - 256;
            return;
        }
        if (shift_r) {
            if (shift_l) {
                if ((shift_r < 0) ^ (shift_l < 0))
                    return;
                if (shift_r < 0 && shift_r < shift_l)
                    shift_l = shift_r;
                else if (shift_r > 0 && shift_r > shift_l)
                    shift_l = shift_r;
            } else
                shift_l = shift_r;
        }
        item->goal_anim_state = AS_CLIMBING;
        item->pos.y_pos += shift_l;
    } else if (input & IN_BACK) {
        if (item->goal_anim_state == AS_HANG)
            return;
        item->goal_anim_state = AS_CLIMBSTNC;
        item->pos.y_pos += 256;
        result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
        result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift_l);
        item->pos.y_pos -= 256;
        if (!result_r || !result_l)
            return;
        if (shift_r && shift_l) {
            if ((shift_r < 0) ^ (shift_l < 0))
                return;
            if ((shift_r < 0 && shift_r < shift_l) || (shift_r > 0 && shift_r > shift_l))
                shift_l = shift_r;
        }
        if (result_r == 1 && result_l == 1) {
            item->goal_anim_state = AS_CLIMBDOWN;
            item->pos.y_pos += shift_l;
        } else
            item->goal_anim_state = AS_HANG;
    }
}

sint32 LaraCheckForLetGo(ITEM_INFO *item, COLL_INFO *coll)
{
    FLOOR_INFO *floor;
    sint16 room_number;

    item->fallspeed = 0;
    item->gravity_status = 0;
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    coll->trigger = trigger_index;
    if (!(input & IN_ACTION) || item->hit_points <= 0) {
        lara.torso_y_rot = 0;
        lara.torso_x_rot = 0;
        lara.head_y_rot = 0;
        lara.head_x_rot = 0;
        item->current_anim_state = item->goal_anim_state = AS_FORWARDJUMP;
        item->anim_number = ANIM_FALLDOWN;
        item->frame_number = anims[ANIM_FALLDOWN].frame_base;
        item->speed = 2;
        item->gravity_status = 1;
        item->fallspeed = 1;
        lara.gun_status = LG_NO_ARMS;
        return 1;
    }
    return 0;
}

void lara_as_climbleft(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    camera.target_angle = -5460;
    camera.target_elevation = -2730;
    if (!(input & (IN_LEFT | IN_LSTEP)))
        item->goal_anim_state = AS_CLIMBSTNC;
}

void lara_col_climbleft(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 shift, res;

    if (!LaraCheckForLetGo(item, coll)) {
        lara.move_angle = item->pos.y_rot - 16384;
        res = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift);
        LaraDoClimbLeftRight(item, coll, res, shift);
    }
}

void LaraDoClimbLeftRight(ITEM_INFO *item, COLL_INFO *coll, sint32 result, sint32 shift)
{
    sint32 flag;

    if (result == 1) {
        if (input & IN_LEFT)
            item->goal_anim_state = AS_CLIMBLEFT;
        else if (input & IN_RIGHT)
            item->goal_anim_state = AS_CLIMBRIGHT;
        else
            item->goal_anim_state = AS_CLIMBSTNC;
        item->pos.y_pos += shift;
        return;
    } else if (!result) {
        item->pos.x_pos = coll->old.x;
        item->pos.z_pos = coll->old.z;
        item->current_anim_state = item->goal_anim_state = AS_CLIMBSTNC;
        if (coll->old_anim_state != AS_CLIMBSTNC) {
            item->frame_number = anims[ANIM_CLIMBSTNC].frame_base;
            item->anim_number = ANIM_CLIMBSTNC;
            return;
        }
        if (input & IN_LEFT) {
            flag = LaraClimbLeftCornerTest(item, coll);
            if (flag) {
                if (flag > 0) {
                    item->anim_number = ANIM_EXTCLIMBL;
                    item->frame_number = anims[ANIM_EXTCLIMBL].frame_base;
                    item->current_anim_state = item->goal_anim_state = AS_CORNEREXTL;
                } else {
                    item->anim_number = ANIM_INTCLIMBL;
                    item->frame_number = anims[ANIM_INTCLIMBL].frame_base;
                    item->current_anim_state = item->goal_anim_state = AS_CORNERINTL;
                }
                return;
            }
        } else if (input & IN_RIGHT) {
            flag = LaraClimbRightCornerTest(item, coll);
            if (flag) {
                if (flag > 0) {
                    item->anim_number = ANIM_EXTCLIMBR;
                    item->frame_number = anims[ANIM_EXTCLIMBR].frame_base;
                    item->current_anim_state = item->goal_anim_state = AS_CORNEREXTR;
                } else {
                    item->anim_number = ANIM_INTCLIMBR;
                    item->frame_number = anims[ANIM_INTCLIMBR].frame_base;
                    item->current_anim_state = item->goal_anim_state = AS_CORNERINTR;
                }
                return;
            }
        }
        item->frame_number = coll->old_frame_number;
        item->anim_number = coll->old_anim_number;
        AnimateLara(item);
    } else {
        item->goal_anim_state = AS_HANG;
        do {
            AnimateItem(item);
        } while (item->current_anim_state != AS_HANG);
        item->pos.x_pos = coll->old.x;
        item->pos.z_pos = coll->old.z;
    }
}

void lara_as_climbright(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    camera.target_angle = 5460;
    camera.target_elevation = -2730;
    if (!(input & (IN_RIGHT | IN_RSTEP)))
        item->goal_anim_state = AS_CLIMBSTNC;
}

void lara_col_climbright(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 shift, res;

    if (!LaraCheckForLetGo(item, coll)) {
        lara.move_angle = item->pos.y_rot + 16384;
        res = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift);
        LaraDoClimbLeftRight(item, coll, res, shift);
    }
}

void lara_as_climbing(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    camera.target_elevation = 5460;
}

void lara_col_climbing(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 frame, yshift, result_r, result_l, shift_r, shift_l, ledge_l, ledge_r;

    if (LaraCheckForLetGo(item, coll))
        return;
    if (item->anim_number != ANIM_CLIMBING)
        return;
    frame = item->frame_number - anims[ANIM_CLIMBING].frame_base;
    if (!frame)
        yshift = 0;
    else if (frame == 28 || frame == 29)
        yshift = -256;
    else if (frame == 57)
        yshift = -512;
    else
        return;
    item->pos.y_pos += yshift - 256;
    result_r = LaraTestClimbUpPos(item, coll->radius, coll->radius + 120, &shift_r, &ledge_r);
    result_l = LaraTestClimbUpPos(item, coll->radius, -(coll->radius + 120), &shift_l, &ledge_l);
    item->pos.y_pos += 256;
    if (result_r && result_l && input & IN_FORWARD) {
        if (result_r < 0 || result_l < 0) {
            item->goal_anim_state = AS_CLIMBSTNC;
            AnimateLara(item);
            if (ABS(ledge_l - ledge_r) <= 120) {
                item->goal_anim_state = AS_NULL;
                item->pos.y_pos += (ledge_r + ledge_l) / 2 - 256;
            }
        } else {
            item->goal_anim_state = AS_CLIMBING;
            item->pos.y_pos -= yshift;
        }
    } else {
        item->goal_anim_state = AS_CLIMBSTNC;
        if (yshift)
            AnimateLara(item);
    }
}

void lara_as_climbdown(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    camera.target_elevation = -8190;
}

void lara_col_climbdown(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 frame, yshift, result_r, result_l, shift_r, shift_l;

    if (LaraCheckForLetGo(item, coll))
        return;
    if (item->anim_number != ANIM_CLIMBDOWN)
        return;
    frame = item->frame_number - anims[ANIM_CLIMBDOWN].frame_base;
    if (!frame)
        yshift = 0;
    else if (frame == 28 || frame == 29)
        yshift = 256;
    else if (frame == 57)
        yshift = 512;
    else
        return;
    item->pos.y_pos += yshift + 256;
    result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift_r);
    result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -512, 512, &shift_l);
    item->pos.y_pos -= 256;
    if (result_r && result_l && input & IN_BACK) {
        if (shift_r && shift_l) {
            if ((shift_r < 0) ^ (shift_l < 0)) {
                item->goal_anim_state = AS_CLIMBSTNC;
                AnimateLara(item);
                return;
            }
            if (shift_r < 0 && shift_r < shift_l || shift_r > 0 && shift_r > shift_l)
                shift_l = shift_r;
        }
        if (result_r == -1 || result_l == -1) {
            item->current_anim_state = AS_CLIMBSTNC;
            item->frame_number = anims[ANIM_CLIMBSTNC].frame_base;
            item->anim_number = ANIM_CLIMBSTNC;
            item->goal_anim_state = AS_HANG;
            AnimateLara(item);
        } else {
            item->goal_anim_state = AS_CLIMBDOWN;
            item->pos.y_pos -= yshift;
        }
    } else {
        item->goal_anim_state = AS_CLIMBSTNC;
        if (yshift)
            AnimateLara(item);
    }
}

void lara_as_climbend(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    camera.flags = 1;
    camera.target_angle = -8190;
}

sint16 GetClimbTrigger(sint32 x, sint32 y, sint32 z, sint16 room_number)
{
    FLOOR_INFO *floor;
    sint16 *data;

    floor = GetFloor(x, y, z, &room_number);
    GetHeight(floor, x, y, z);
    if (!trigger_index)
        return 0;
    data = trigger_index;
    if ((*data & 0x1F) == LAVA_TYPE) {
        if (*data & 0x8000)
            return 0;
        data++;
    }
    if ((*data & 0x1F) == CLIMB_TYPE)
        return *data;
    return 0;
}

sint32 LaraClimbLeftCornerTest(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 flag, oldZ, x, oldX, z, shift;
    sint16 oldY, angle;

    flag = 0;
    if (item->anim_number != 171)
        return 0;
    oldX = item->pos.x_pos;
    oldZ = item->pos.z_pos;
    oldY = item->pos.y_rot;
    angle = (uint16)(item->pos.y_rot + 0x2000) / 0x4000;
    switch (angle) {
    case NORTH:
    case SOUTH:
        x = (oldX & ~0x3FF) - (oldZ & 0x3FF) + 1024;
        z = (oldZ & ~0x3FF) - (oldX & 0x3FF) + 1024;
        break;
    default:
        x = (oldX & ~0x3FF) | (0x3FF & oldZ);
        z = (oldZ & ~0x3FF) | (oldX & 0x3FF);
        break;
    }
    if (GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & LeftIntRightExtTab[angle]) {
        item->pos.x_pos = x;
        lara.CornerX = x;
        item->pos.z_pos = z;
        lara.CornerZ = z;
        item->pos.y_rot -= 0x4000;
        lara.move_angle = item->pos.y_rot;
        flag = LaraTestClimbPos(item, coll->radius, -120 - coll->radius, -512, 512, &shift);
        item->item_flags[3] = (sint16)flag;
        if (flag)
            flag = -1;
    }
    if (!flag) {
        item->pos.x_pos = oldX;
        lara.move_angle = oldY;
        item->pos.y_rot = oldY;
        item->pos.z_pos = oldZ;
        switch (angle) {
        case NORTH:
            x = (item->pos.x_pos ^ ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF)) - 1024;
            z = ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF) ^ (item->pos.z_pos + 1024);
            break;
        case SOUTH:
            x = ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF) ^ (item->pos.x_pos + 1024);
            z = ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF) ^ (item->pos.z_pos - 1024);
            break;
        case WEST:
            x = (item->pos.x_pos & ~0x3FF) - (item->pos.z_pos & 0x3FF);
            z = (item->pos.z_pos & ~0x3FF) - (0x3FF & item->pos.x_pos);
            break;
        default:
            x = ((1024 + item->pos.x_pos) & ~0x3FF) - (item->pos.z_pos & 0x3FF) + 1024;
            z = ((item->pos.z_pos + 1024) & ~0x3FF) - (item->pos.x_pos & 0x3FF) + 1024;
            break;
        }
        if (GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & LeftExtRightIntTab[angle]) {
            item->pos.x_pos = x;
            lara.CornerX = x;
            item->pos.z_pos = z;
            lara.CornerZ = z;
            item->pos.y_rot += 0x4000;
            lara.move_angle = item->pos.y_rot;
            flag = LaraTestClimbPos(item, coll->radius, -120 - coll->radius, -512, 512, &shift);
            item->item_flags[3] = (sint16)flag;
            if (flag)
                flag = 1;
        }
    }
    item->pos.x_pos = oldX;
    lara.move_angle = oldY;
    item->pos.z_pos = oldZ;
    item->pos.y_rot = oldY;
    item = flag;
    coll = coll;
    coll = item;
    return coll;
}

sint32 LaraClimbRightCornerTest(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 flag, oldZ, x, oldX, z, shift;
    sint16 oldY, angle;

    flag = 0;
    if (item->anim_number != 170)
        return 0;
    oldX = item->pos.x_pos;
    oldZ = item->pos.z_pos;
    oldY = item->pos.y_rot;
    angle = (uint16)(item->pos.y_rot + 0x2000) / 0x4000;
    switch (angle) {
    case NORTH:
    case SOUTH:
        x = (oldX & ~0x3FF) | (0x3FF & oldZ);
        z = (oldZ & ~0x3FF) | (oldX & 0x3FF);
        break;
    default:
        x = (oldX & ~0x3FF) - (oldZ & 0x3FF) + 1024;
        z = (oldZ & ~0x3FF) - (oldX & 0x3FF) + 1024;
        break;
    }
    if (GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & LeftExtRightIntTab[angle]) {
        item->pos.x_pos = x;
        lara.CornerX = x;
        item->pos.z_pos = z;
        lara.CornerZ = z;
        item->pos.y_rot += 0x4000;
        lara.move_angle = item->pos.y_rot;
        flag = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift);
        if (flag)
            flag = -1;
    }
    if (!flag) {
        item->pos.x_pos = oldX;
        lara.move_angle = oldY;
        item->pos.y_rot = oldY;
        item->pos.z_pos = oldZ;
        switch (angle) {
        case NORTH:
            x = ((item->pos.x_pos + 1024) & ~0x3FF) - (item->pos.z_pos & 0x3FF) + 1024;
            z = ((item->pos.z_pos + 1024) & ~0x3FF) - (item->pos.x_pos & 0x3FF) + 1024;
            break;
        case SOUTH:
            x = ((item->pos.x_pos - 1024) & ~0x3FF) - (item->pos.z_pos & 0x3FF) + 1024;
            z = ((item->pos.z_pos - 1024) & ~0x3FF) - (item->pos.x_pos & 0x3FF) + 1024;
            break;
        case WEST:
            x = (item->pos.x_pos ^ ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF)) - 1024;
            z = ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF) ^ (item->pos.z_pos + 1024);
            break;
        default:
            x = ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF) ^ (item->pos.x_pos + 1024);
            z = (item->pos.z_pos ^ ((item->pos.z_pos ^ item->pos.x_pos) & 0x3FF)) - 1024;
            break;
        }
        if (GetClimbTrigger(x, item->pos.y_pos, z, item->room_number) & LeftIntRightExtTab[angle]) {
            item->pos.x_pos = x;
            lara.CornerX = x;
            item->pos.z_pos = z;
            lara.CornerZ = z;
            item->pos.y_rot -= 0x4000;
            lara.move_angle = item->pos.y_rot;
            flag = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -512, 512, &shift);
            if (flag)
                flag = 1;
        }
    }
    item->pos.x_pos = oldX;
    lara.move_angle = oldY;
    item->pos.z_pos = oldZ;
    item->pos.y_rot = oldY;
    return flag;
}
