#include "lara.h"
#include "lara_states.h"
#include "anim.h"
#include "room.h"
#include "control.h"
#include "draw.h"
#include "specific/3dmath.h"
#include "camera.h"
#include "sound.h"
#include "specific/input.h"

#define DAMAGE_START 140
#define DAMAGE_LENGTH 14
#define FASTFALL_FRICTION item->speed = (item->speed * 95) / 100
#define HEAD_TURN (2 * ONE_DEGREE)
#define CAM_A_HANG 0
#define CAM_E_HANG (-60 * ONE_DEGREE)
#define LARA_DEF_ADD_EDGE (5 * ONE_DEGREE)
#define HANG_HEIGHT 600
extern sint32 jump_ok;

sint16 LaraCeilingFront(ITEM_INFO *item, sint16 ang, sint32 dist, sint32 h)
{
    sint16 room_num;
    FLOOR_INFO *floor;
    sint32 x, y, z, height;

    x = item->pos.x_pos + (((rcossin_tbl[(ang >> 3) & 0x1FFE] << 2) * dist) >> W2V_SHIFT);
    y = item->pos.y_pos - h;
    z = item->pos.z_pos + (((rcossin_tbl[((ang >> 3) & 0x1FFE) + 1] << 2) * dist) >> W2V_SHIFT);
    room_num = item->room_number;
    floor = GetFloor(x, y, z, &room_num);
    height = GetCeiling(floor, x, y, z);
    if (height != NO_HEIGHT)
        height += h - item->pos.y_pos;
    return (sint16)height;
}

void lara_as_duckl(ITEM_INFO *item, COLL_INFO *coll)
{
    if ((input & (IN_DUCK | IN_LEFT)) != (IN_DUCK | IN_LEFT) || item->hit_points <= 0)
        item->goal_anim_state = AS_DUCK;
    item->pos.y_rot -= 273;
}

void lara_as_duckr(ITEM_INFO *item, COLL_INFO *coll)
{
    if ((input & (IN_DUCK | IN_LEFT)) != (IN_DUCK | IN_LEFT) || item->hit_points <= 0)
        item->goal_anim_state = AS_DUCK;
    item->pos.y_rot += 273;
}

sint32 LaraFallen(ITEM_INFO *item, COLL_INFO *coll)
{
    if (lara.water_status != LW_WADE && coll->mid_floor > STEPUP_HEIGHT) {
        item->anim_number = ANIM_FALLDOWN;
        item->frame_number = anims[ANIM_FALLDOWN].frame_base;
        item->current_anim_state = AS_FORWARDJUMP;
        item->goal_anim_state = AS_FORWARDJUMP;
        item->fallspeed = 0;
        item->gravity_status = 1;
        return 1;
    }
    return 0;
}

void lara_as_duck(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 1;
    coll->enable_baddie_push = 1;
    lara.IsDucked = 1;
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_ALL4S;
        return;
    }
    if (input & IN_LOOK)
        LookUpDown();
    if (input & (IN_FORWARD | IN_BACK) && (input & IN_DUCK || lara.keep_ducked) && lara.gun_status == LG_NO_ARMS &&
        lara.water_status != LW_WADE && (item->anim_number == ANIM_DUCKBREATHE || item->anim_number == 245) &&
        !(input & (IN_FLARE | IN_DRAW)) && (lara.gun_type != WEAPON_FLARE || lara.flare_age < 900 && lara.flare_age)) {
        lara.torso_y_rot = 0;
        lara.torso_x_rot = 0;
        item->goal_anim_state = AS_ALL4S;
    }
}

void lara_as_all4s(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_DEATH;
        return;
    }
    if (input & IN_LOOK)
        LookUpDown();
    lara.torso_x_rot = lara.torso_y_rot = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 1;
    if (item->anim_number == ANIM_DUCK_TO_ALL4S)
        lara.gun_status = LG_HANDS_BUSY;
    camera.target_elevation = -23 * ONE_DEGREE;
}

sint16 LaraFloorFront(ITEM_INFO *item, sint16 ang, sint32 dist)
{
    sint16 room_num;
    FLOOR_INFO *floor;
    sint32 x, y, z, height;

    x = item->pos.x_pos + (((rcossin_tbl[(ang >> 3) & 0x1FFE] << 2) * dist) >> W2V_SHIFT);
    y = item->pos.y_pos - LARA_HITE;
    z = item->pos.z_pos + (((rcossin_tbl[((ang >> 3) & 0x1FFE) + 1] << 2) * dist) >> W2V_SHIFT);
    room_num = item->room_number;
    floor = GetFloor(x, y, z, &room_num);
    height = GetHeight(floor, x, y, z);
    if (height != NO_HEIGHT)
        height -= item->pos.y_pos;
    return (sint16)height;
}

void lara_as_crawl(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_ALL4S;
        return;
    }
    if (input & IN_LOOK)
        LookUpDown();
    lara.torso_x_rot = lara.torso_y_rot = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 1;
    camera.target_elevation = -23 * ONE_DEGREE;
    if ((input & IN_FORWARD) && ((input & IN_DUCK) || lara.keep_ducked) && lara.water_status != LW_WADE) {
        if (input & IN_LEFT) {
            lara.turn_rate -= LARA_TURN_RATE;
            if (lara.turn_rate < -LARA_JUMP_TURN)
                lara.turn_rate = -LARA_JUMP_TURN;
        } else if (input & IN_RIGHT) {
            lara.turn_rate += LARA_TURN_RATE;
            if (lara.turn_rate > LARA_JUMP_TURN)
                lara.turn_rate = LARA_JUMP_TURN;
        }
    } else
        item->goal_anim_state = AS_ALL4S;
}

void lara_col_crawl(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    lara.move_angle = item->pos.y_rot;
    coll->radius = LARA_RAD * 2;
    coll->bad_pos = STEP_L - 1;
    coll->bad_neg = -(STEP_L - 1);
    coll->bad_ceiling = 400;
    coll->slopes_are_walls = 1;
    coll->slopes_are_pits = 1;
    coll->facing = lara.move_angle;
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);
    if (LaraDeflectEdgeDuck(item, coll)) {
        item->current_anim_state = AS_ALL4S;
        item->goal_anim_state = AS_ALL4S;
        if (item->anim_number != ANIM_ALL4S) {
            item->anim_number = ANIM_ALL4S;
            item->frame_number = anims[ANIM_ALL4S].frame_base;
        }
        return;
    }
    if (LaraFallen(item, coll)) {
        lara.gun_status = LG_NO_ARMS;
        return;
    }
    if (TestLaraSlide(item, coll))
        return;
    ShiftItem(item, coll);
    if (coll->mid_floor != NO_HEIGHT && coll->mid_floor > -STEP_L)
        item->pos.y_pos += coll->mid_floor;
}

sint32 LaraDeflectEdgeDuck(ITEM_INFO *item, COLL_INFO *coll)
{
    if (coll->coll_type == COLL_FRONT || coll->coll_type == COLL_TOPFRONT) {
        ShiftItem(item, coll);
        item->gravity_status = 0;
        item->speed = 0;
        return 1;
    } else if (coll->coll_type == COLL_LEFT) {
        ShiftItem(item, coll);
        item->pos.y_rot += LARA_DUCK_DEFLECT;
    } else if (coll->coll_type == COLL_RIGHT) {
        ShiftItem(item, coll);
        item->pos.y_rot -= LARA_DUCK_DEFLECT;
    }
    return 0;
}

void lara_col_all4turnlr(ITEM_INFO *item, COLL_INFO *coll)
{
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);
    if (!TestLaraSlide(item, coll) && coll->mid_floor != NO_HEIGHT && coll->mid_floor > -256)
        item->pos.y_pos += coll->mid_floor;
}

void lara_as_crawlb(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0 || lara.water_status == LW_WADE) {
        item->goal_anim_state = AS_ALL4S;
        return;
    }
    if (input & IN_LOOK)
        LookUpDown();
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 1;
    lara.torso_x_rot = lara.torso_y_rot = 0;
    camera.target_elevation = -23 * ONE_DEGREE;
    if (input & IN_BACK) {
        if (input & IN_RIGHT) {
            lara.turn_rate -= LARA_TURN_RATE;
            if (lara.turn_rate < -LARA_JUMP_TURN)
                lara.turn_rate = -LARA_JUMP_TURN;
        } else if (input & IN_LEFT) {
            lara.turn_rate += LARA_TURN_RATE;
            if (lara.turn_rate > LARA_JUMP_TURN)
                lara.turn_rate = LARA_JUMP_TURN;
        }
    } else
        item->goal_anim_state = AS_ALL4S;
}

void lara_col_crawl2hang(ITEM_INFO *item, COLL_INFO *coll)
{
    sint16 *bounds;
    sint32 edge, edge_catch;
    sint16 angle;

    camera.target_angle = 0;
    camera.target_elevation = -10920;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    if (item->anim_number == ANIM_CRAWL_TO_HANG) {
        item->fallspeed = 512;
        item->pos.y_pos += 255;
        coll->bad_pos = -NO_HEIGHT;
        coll->bad_neg = -STEPUP_HEIGHT;
        coll->bad_ceiling = 192;
        coll->facing = lara.move_angle = item->pos.y_rot;
        GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 870);
        if (!(edge_catch = LaraTestEdgeCatch(item, coll, &edge)))
            return;
        if (edge_catch < 0 && !LaraTestHangOnClimbWall(item, coll))
            return;
        angle = item->pos.y_rot;
        if (angle >= -6370 && angle <= 6370)
            angle = 0;
        else if (angle >= 10014 && angle <= 22754)
            angle = 16384;
        else if (angle >= 26397 || angle <= -26397)
            angle = -32768;
        else if (angle >= -22754 && angle <= -10014)
            angle = -16384;
        if (angle & 0x3FFF)
            return;
        if (TestHangSwingIn(item, angle)) {
            lara.head_x_rot = lara.head_y_rot = 0;
            lara.torso_x_rot = lara.torso_y_rot = 0;
            item->anim_number = ANIM_HANG2STOP;
            item->frame_number = anims[ANIM_HANG2STOP].frame_base;
            item->current_anim_state = AS_HANG2;
            item->goal_anim_state = AS_HANG2;
        } else {
            item->anim_number = ANIM_GRABLEDGE;
            item->frame_number = anims[ANIM_GRABLEDGE].frame_base;
            item->current_anim_state = AS_HANG;
            item->goal_anim_state = AS_HANG;
        }
        bounds = GetBoundsAccurate(item);
        if (edge_catch > 0) {
            item->pos.y_pos += coll->front_floor - bounds[2];
            item->pos.x_pos += coll->shift.x;
            item->pos.z_pos += coll->shift.z;
        } else
            item->pos.y_pos = edge - bounds[2];
        item->pos.y_rot = angle;
        item->gravity_status = 1;
        item->speed = 2;
        item->fallspeed = 1;
        lara.gun_status = LG_HANDS_BUSY;
    }
}

sint32 LaraTestEdgeCatch(ITEM_INFO *item, COLL_INFO *coll, sint32 *edge)
{
    sint32 hdif;
    sint16 *bounds;

    bounds = GetBoundsAccurate(item);
    hdif = coll->front_floor - bounds[2];
    if ((hdif < 0 && hdif + item->fallspeed < 0) || (hdif > 0 && hdif + item->fallspeed > 0)) {
        hdif = item->pos.y_pos + bounds[2];
        if ((hdif >> (WALL_SHIFT - 2)) != ((hdif + item->fallspeed) >> (WALL_SHIFT - 2))) {
            if (item->fallspeed > 0)
                *edge = (hdif + item->fallspeed) & ~(STEP_L - 1);
            else
                *edge = hdif & ~(STEP_L - 1);
            return -1;
        }
        return 0;
    }
    if (ABS(coll->left_floor2 - coll->right_floor2) >= 60)
        return 0;
    return 1;
}

sint32 LaraTestHangOnClimbWall(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 shift, result;
    sint16 *bounds;

    if (!lara.climb_status || item->fallspeed < 0)
        return 0;
    switch ((uint16)(item->pos.y_rot + 0x2000) / 0x4000) {
    case NORTH:
    case SOUTH:
        item->pos.z_pos += coll->shift.z;
        break;
    case EAST:
    case WEST:
        item->pos.x_pos += coll->shift.x;
        break;
    }
    bounds = GetBoundsAccurate(item);
    if (lara.move_angle != item->pos.y_rot) {
        sint16 l, r;
        r = LaraCeilingFront(item, item->pos.y_rot, 0, 0);
        l = LaraCeilingFront(item, lara.move_angle, 128, 0);
        if (ABS(r - l) > 60)
            return 0;
    }
    if (!LaraTestClimbPos(item, coll->radius, coll->radius, bounds[2], bounds[3] - bounds[2], &shift) || !LaraTestClimbPos(item, coll->radius, -coll->radius, bounds[2], bounds[3] - bounds[2], &shift))
        return 0;
    result = LaraTestClimbPos(item, coll->radius, 0, bounds[2], bounds[3] - bounds[2], &shift);
    if (!result)
        return 0;
    else if (result == 1)
        return 1;
    item->pos.y_pos += shift;
    return 1;
}

sint32 TestHangSwingIn(ITEM_INFO *item, PHD_ANGLE angle)
{
    sint32 x, y, z;
    sint32 h, c;
    FLOOR_INFO *floor;
    sint16 room_num;

    x = item->pos.x_pos;
    y = item->pos.y_pos;
    z = item->pos.z_pos;
    room_num = item->room_number;
    switch (angle) {
    case 0:
        z += 256;
        break;
    case 16384:
        x += 256;
        break;
    case -16384:
        x -= 256;
        break;
    case -32768:
        z -= 256;
        break;
    }
    floor = GetFloor(x, y, z, &room_num);
    h = GetHeight(floor, x, y, z);
    c = GetCeiling(floor, x, y, z);
    if (h != NO_HEIGHT) {
        if (h - y > 0 && c - y < -400 && y - 819 - c > -72)
            return 1;
    }
    return 0;
}

void lara_as_dash(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0 || !DashTimer || !(input & IN_SPRINT) || lara.water_status == LW_WADE) {
        item->goal_anim_state = AS_RUN;
        return;
    }
    DashTimer--;
    if ((input & IN_DUCK) && (lara.gun_status == LG_NO_ARMS || lara.gun_type == WEAPON_NONE || lara.gun_type == WEAPON_PISTOLS || lara.gun_type == WEAPON_REVOLVER || lara.gun_type == WEAPON_UZI || lara.gun_type == WEAPON_FLARE)) {
        item->goal_anim_state = AS_DUCK;
        return;
    }
    if (input & IN_LEFT) {
        lara.turn_rate -= 409;
        if (lara.turn_rate < -728)
            lara.turn_rate = -728;
        item->pos.z_rot -= 273;
        if (item->pos.z_rot < -2912)
            item->pos.z_rot = -2912;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += 409;
        if (lara.turn_rate > 728)
            lara.turn_rate = 728;
        item->pos.z_rot += 273;
        if (item->pos.z_rot > 2912)
            item->pos.z_rot = 2912;
    }
    if ((input & IN_JUMP) && !item->gravity_status)
        item->goal_anim_state = AS_DASHDIVE;
    else if (input & IN_FORWARD) {
        if (input & IN_WALK)
            item->goal_anim_state = AS_WALK;
        else
            item->goal_anim_state = AS_DASH;
    } else if (!(input & (IN_LEFT | IN_RIGHT)))
        item->goal_anim_state = AS_STOP;
}

void lara_col_dash(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (TestLaraVault(item, coll))
        return;
    if (LaraDeflectEdge(item, coll)) {
        item->pos.z_rot = 0;
        if (TestWall(item, 256, 0, -640)) {
            item->current_anim_state = AS_SPLAT;
            item->anim_number = ANIM_HITWALLLEFT;
            item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
            return;
        }
        LaraCollideStop(item, coll);
    }
    if (LaraFallen(item, coll))
        return;
    if (coll->mid_floor >= -STEPUP_HEIGHT && coll->mid_floor < -STEP_L / 2) {
        if (item->frame_number >= 3 && item->frame_number <= 14) {
            item->anim_number = ANIM_RUNSTEPUP_LEFT;
            item->frame_number = anims[ANIM_RUNSTEPUP_LEFT].frame_base;
        } else {
            item->anim_number = ANIM_RUNSTEPUP_RIGHT;
            item->frame_number = anims[ANIM_RUNSTEPUP_RIGHT].frame_base;
        }
    }
    if (TestLaraSlide(item, coll))
        return;
    if (coll->mid_floor >= 50)
        item->pos.y_pos += 50;
    else if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void GetLaraCollisionInfo(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->facing = lara.move_angle;
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, LARA_HITE);
}

sint32 LaraHitCeiling(ITEM_INFO *item, COLL_INFO *coll)
{
    if (coll->coll_type != COLL_TOP && coll->coll_type != COLL_CLAMP)
        return 0;
    item->pos.x_pos = coll->old.x;
    item->pos.y_pos = coll->old.y;
    item->pos.z_pos = coll->old.z;
    item->goal_anim_state = AS_STOP;
    item->current_anim_state = AS_STOP;
    item->frame_number = anims[ANIM_STOP].frame_base;
    item->anim_number = ANIM_STOP;
    item->speed = 0;
    item->fallspeed = 0;
    item->gravity_status = 0;
    return 1;
}

sint32 LaraDeflectEdge(ITEM_INFO *item, COLL_INFO *coll)
{
    if (coll->coll_type == COLL_FRONT || coll->coll_type == COLL_TOPFRONT) {
        ShiftItem(item, coll);
        item->goal_anim_state = AS_STOP;
        item->speed = 0;
        item->gravity_status = 0;
        return 1;
    } else if (coll->coll_type == COLL_LEFT) {
        ShiftItem(item, coll);
        item->pos.y_rot += LARA_DEF_ADD_EDGE;
    } else if (coll->coll_type == COLL_RIGHT) {
        ShiftItem(item, coll);
        item->pos.y_rot -= LARA_DEF_ADD_EDGE;
    }
    return 0;
}

sint32 LaraTestClimbStance(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 shift_l, result_r, shift_r, result_l;

    result_r = LaraTestClimbPos(item, coll->radius, coll->radius + 120, -700, STEP_L * 2, &shift_r);
    if (result_r != 1)
        return 0;
    result_l = LaraTestClimbPos(item, coll->radius, -(coll->radius + 120), -700, STEP_L * 2, &shift_l);
    if (result_l != 1)
        return 0;
    if (shift_r) {
        if (shift_l) {
            if ((shift_l < 0) ^ (shift_r < 0))
                return 0;
            if (shift_r < 0 && shift_l < shift_r)
                shift_r = shift_l;
            else if (shift_r > 0 && shift_l > shift_r)
                shift_r = shift_l;
        }
        item->pos.y_pos += shift_r;
    } else if (shift_l) {
        item->pos.y_pos += shift_l;
    }
    return 1;
}

sint32 TestWall(ITEM_INFO *item, sint32 front, sint32 right, sint32 down)
{
    sint32 x, y, z;
    sint32 h, c;
    FLOOR_INFO *floor;
    sint16 room_num;
    PHD_ANGLE angle;

    x = item->pos.x_pos;
    y = item->pos.y_pos + down;
    z = item->pos.z_pos;
    room_num = item->room_number;
    angle = (uint16)(item->pos.y_rot + 0x2000) / 0x4000;
    switch (angle) {
    case 0:
        x -= right;
        break;
    case 3:
        z += right;
        break;
    case 2:
        x += right;
        break;
    case 1:
        z -= right;
        break;
    }
    floor = GetFloor(x, y, z, &room_num);
    switch (angle) {
    case 0:
        z += front;
        break;
    case 3:
        x -= front;
        break;
    case 2:
        z -= front;
        break;
    case 1:
        x += front;
        break;
    }
    floor = GetFloor(x, y, z, &room_num);
    h = GetHeight(floor, x, y, z);
    c = GetCeiling(floor, x, y, z);
    if (h != NO_HEIGHT) {
        if (h - y > 0 && c - y < 0)
            return 0;
        return 2;
    }
    return 1;
}

void LaraCollideStop(ITEM_INFO *item, COLL_INFO *coll)
{
    switch (coll->old_anim_state) {
    case AS_STOP:
    case AS_TURN_L:
    case AS_TURN_R:
    case AS_FASTTURN:
        item->current_anim_state = coll->old_anim_state;
        item->anim_number = coll->old_anim_number;
        item->frame_number = coll->old_frame_number;
        if (input & IN_LEFT)
            item->goal_anim_state = AS_TURN_L;
        else if (input & IN_RIGHT)
            item->goal_anim_state = AS_TURN_R;
        else
            item->goal_anim_state = AS_STOP;
        AnimateLara(item);
        break;
    default:
        item->anim_number = ANIM_STOP;
        item->frame_number = anims[ANIM_STOP].frame_base;
    }
}

void lara_as_dashdive(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->goal_anim_state != AS_DEATH && item->goal_anim_state != AS_STOP && item->goal_anim_state != AS_RUN) {
        if (item->fallspeed > LARA_FASTFALL_SPEED)
            item->goal_anim_state = AS_FASTFALL;
    }
}

void lara_col_dashdive(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->speed >= 0)
        lara.move_angle = item->pos.y_rot;
    else
        lara.move_angle = item->pos.y_rot - 0x8000;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEP_L;
    coll->bad_ceiling = 192;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    LaraDeflectEdgeJump(item, coll);
    if (LaraFallen(item, coll))
        return;
    if (item->speed < 0)
        lara.move_angle = item->pos.y_rot;
    if (coll->mid_floor <= 0 && item->fallspeed > 0) {
        if (LaraLandedBad(item, coll))
            item->goal_anim_state = AS_DEATH;
        else if (lara.water_status != LW_WADE && (input & IN_FORWARD) && !(input & IN_WALK))
            item->goal_anim_state = AS_RUN;
        else
            item->goal_anim_state = AS_STOP;
        item->fallspeed = 0;
        item->pos.y_pos += coll->mid_floor;
        item->gravity_status = 0;
        item->speed = 0;
        AnimateLara(item);
    }
    ShiftItem(item, coll);
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

sint32 LaraLandedBad(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 land_speed;

    land_speed = item->fallspeed - 140;
    if (land_speed <= 0)
        return 0;
    if (land_speed > 14)
        item->hit_points = -1;
    else
        item->hit_points += (sint16)(-1000 * (land_speed * land_speed) / 196);
    return item->hit_points <= 0;
}

void MonkeySwingFall(ITEM_INFO *item)
{
    item->goal_anim_state = AS_UPJUMP;
    item->current_anim_state = AS_UPJUMP;
    item->anim_number = ANIM_STOPHANG;
    item->frame_number = anims[ANIM_STOPHANG].frame_base + 9;
    item->gravity_status = 1;
    item->speed = 2;
    item->fallspeed = 1;
    item->pos.y_pos += 256;
    lara.gun_status = LG_NO_ARMS;
}

void lara_col_hang2(ITEM_INFO *item, COLL_INFO *coll)
{
    item->fallspeed = 0;
    item->gravity_status = 0;
    if (lara.CanMonkeySwing) {
        coll->bad_pos = -NO_HEIGHT;
        coll->bad_neg = NO_HEIGHT;
        coll->bad_ceiling = 0;
        coll->facing = lara.move_angle = item->pos.y_rot;
        coll->slopes_are_walls = 0;
        coll->radius = LARA_RAD;
        GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, HANG_HEIGHT);
        if ((input & IN_FORWARD) && coll->coll_type != COLL_FRONT && ABS(coll->mid_ceiling - coll->front_ceiling) < 50)
            item->goal_anim_state = AS_MONKEYSWING;
        else if ((input & IN_LSTEP) && TestMonkeyLeft(item, coll))
            item->goal_anim_state = AS_MONKEYL;
        else if ((input & IN_RSTEP) && TestMonkeyRight(item, coll))
            item->goal_anim_state = AS_MONKEYR;
        else if (input & IN_LEFT)
            item->goal_anim_state = AS_HANGTURNL;
        else if (input & IN_RIGHT)
            item->goal_anim_state = AS_HANGTURNR;
        MonkeySwingSnap(item, coll);
    } else {
        LaraHangTest(item, coll);
        if (item->goal_anim_state == AS_HANG2) {
            if ((input & IN_FORWARD) && coll->front_floor > -850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= 0 && coll->left_floor2 - coll->left_ceiling2 >= 0 && coll->right_floor2 - coll->right_ceiling2 >= 0 && !coll->hit_static) {
                if (input & IN_WALK)
                    item->goal_anim_state = AS_GYMNAST;
                else
                    item->goal_anim_state = AS_NULL;
                return;
            }
            if ((input & (IN_DUCK | IN_FORWARD)) && coll->front_floor > -850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= -STEP_L && coll->left_floor2 - coll->left_ceiling2 >= -STEP_L && coll->right_floor2 - coll->right_ceiling2 >= -STEP_L && !coll->hit_static) {
                item->goal_anim_state = AS_HANG2DUCK;
                item->required_anim_state = AS_DUCK;
            } else if ((input & IN_LEFT) || (input & IN_LSTEP))
                item->goal_anim_state = AS_HANGLEFT;
            else if ((input & IN_RIGHT) || (input & IN_RSTEP))
                item->goal_anim_state = AS_HANGRIGHT;
        }
    }
}

void MonkeySwingSnap(ITEM_INFO *item, COLL_INFO *coll)
{
    sint16 room_num;
    FLOOR_INFO *floor;

    room_num = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
    item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + (768 - 64);
}

sint16 TestMonkeyLeft(ITEM_INFO *item, COLL_INFO *coll)
{
    sint16 oct;

    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = NO_HEIGHT;
    coll->bad_ceiling = 0;
    coll->facing = lara.move_angle = item->pos.y_rot - 16384;
    coll->radius = LARA_RAD;
    coll->slopes_are_walls = 0;
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, HANG_HEIGHT);
    if (ABS(coll->mid_ceiling - coll->front_ceiling) > 50)
        return 0;
    if (coll->coll_type) {
        oct = GetDirOctant(item->pos.y_rot);
        if ((oct == 0 && coll->coll_type == COLL_FRONT) || (oct == 0 && coll->coll_type == COLL_LEFT) || (oct == 1 && coll->coll_type == COLL_RIGHT) || (oct == 1 && coll->coll_type == COLL_LEFT))
            return 0;
    }
    return 1;
}

sint16 GetDirOctant(sint32 rot)
{
    if (ABS(rot) < 8192 || ABS(rot) > 24576)
        return 0;
    else
        return 1;
}

sint16 TestMonkeyRight(ITEM_INFO *item, COLL_INFO *coll)
{
    sint16 oct;

    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->facing = lara.move_angle = item->pos.y_rot + 16384;
    coll->radius = LARA_RAD;
    coll->slopes_are_walls = 0;
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, HANG_HEIGHT);
    if (ABS(coll->mid_ceiling - coll->front_ceiling) > 50)
        return 0;
    if (coll->coll_type) {
        oct = GetDirOctant(item->pos.y_rot);
        if ((oct == 0 && coll->coll_type == COLL_FRONT) || (oct == 1 && coll->coll_type == COLL_FRONT) || (oct == 1 && coll->coll_type == COLL_RIGHT) || (oct == 1 && coll->coll_type == COLL_LEFT))
            return 0;
    }
    return 1;
}

void lara_as_monkeyswing(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_HANG2;
        return;
    }
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    lara.torso_x_rot = lara.torso_y_rot = 0;
    if (input & IN_LOOK)
        LookUpDown();
    if (input & IN_FORWARD)
        item->goal_anim_state = AS_MONKEYSWING;
    else
        item->goal_anim_state = AS_HANG2;
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_JUMP_TURN)
            lara.turn_rate = -LARA_JUMP_TURN;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_JUMP_TURN)
            lara.turn_rate = LARA_JUMP_TURN;
    }
}

void lara_col_monkeyswing(ITEM_INFO *item, COLL_INFO *coll)
{
    if (!(input & IN_ACTION) || !lara.CanMonkeySwing) {
        MonkeySwingFall(item);
        return;
    }
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = NO_HEIGHT;
    coll->bad_ceiling = 0;
    coll->facing = lara.move_angle = item->pos.y_rot;
    coll->slopes_are_walls = 0;
    coll->radius = 100;
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);
    if (coll->coll_type != COLL_FRONT && ABS(coll->mid_ceiling - coll->front_ceiling) <= 50) {
        if (ABS(coll->mid_ceiling - coll->left_ceiling2) > 50) {
            ShiftItem(item, coll);
            item->pos.y_rot += 910;
        } else if (ABS(coll->mid_ceiling - coll->right_ceiling2) > 50) {
            ShiftItem(item, coll);
            item->pos.y_rot -= 910;
        }
        camera.target_elevation = 1820;
        MonkeySwingSnap(item, coll);
    } else {
        item->anim_number = ANIM_MONKEYHANG;
        item->current_anim_state = AS_HANG2;
        item->goal_anim_state = AS_HANG2;
        item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
    }
}

void lara_as_monkeyl(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_HANG2;
        return;
    }
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    lara.torso_x_rot = lara.torso_y_rot = 0;
    if (input & IN_LSTEP)
        item->goal_anim_state = AS_MONKEYL;
    else
        item->goal_anim_state = AS_HANG2;
    camera.target_elevation = 10 * ONE_DEGREE;
}

void lara_col_monkeyl(ITEM_INFO *item, COLL_INFO *coll)
{
    if (input & IN_ACTION && lara.CanMonkeySwing) {
        if (!TestMonkeyLeft(item, coll)) {
            item->anim_number = ANIM_MONKEYHANG;
            item->current_anim_state = AS_HANG2;
            item->goal_anim_state = AS_HANG2;
            item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
        } else
            MonkeySwingSnap(item, coll);
    } else
        MonkeySwingFall(item);
}

void lara_as_monkeyr(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_HANG2;
        return;
    }
    lara.torso_x_rot = lara.torso_y_rot = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    if (input & IN_RSTEP)
        item->goal_anim_state = AS_MONKEYR;
    else
        item->goal_anim_state = AS_HANG2;
    camera.target_elevation = 10 * ONE_DEGREE;
}

void lara_col_monkeyr(ITEM_INFO *item, COLL_INFO *coll)
{
    if (input & IN_ACTION && lara.CanMonkeySwing) {
        if (!TestMonkeyRight(item, coll)) {
            item->anim_number = ANIM_MONKEYHANG;
            item->current_anim_state = AS_HANG2;
            item->goal_anim_state = AS_HANG2;
            item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
        } else
            MonkeySwingSnap(item, coll);
    } else
        MonkeySwingFall(item);
}

void lara_as_hangturnl(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_HANG2;
        return;
    }
    camera.target_elevation = 10 * ONE_DEGREE;
    lara.torso_x_rot = lara.torso_y_rot = 0;
    item->pos.y_rot -= ONE_DEGREE + ONE_DEGREE / 2;
    if (!(input & IN_LEFT))
        item->goal_anim_state = AS_HANG2;
}

void lara_col_hangturnlr(ITEM_INFO *item, COLL_INFO *coll)
{
    if ((input & IN_ACTION) && lara.CanMonkeySwing) {
        coll->bad_pos = -NO_HEIGHT;
        coll->bad_neg = -STEPUP_HEIGHT;
        coll->bad_ceiling = 0;
        coll->facing = lara.move_angle = item->pos.y_rot;
        coll->radius = LARA_RAD;
        coll->slopes_are_walls = 1;
        GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);
        MonkeySwingSnap(item, coll);
    } else
        MonkeySwingFall(item);
}

void lara_as_hangturnr(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_HANG2;
        return;
    }
    lara.torso_x_rot = lara.torso_y_rot = 0;
    camera.target_elevation = 10 * ONE_DEGREE;
    item->pos.y_rot += ONE_DEGREE + ONE_DEGREE / 2;
    if (!(input & IN_RIGHT))
        item->goal_anim_state = AS_HANG2;
}

void lara_as_monkey180(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    item->goal_anim_state = AS_HANG2;
}

void lara_col_monkey180(ITEM_INFO *item, COLL_INFO *coll)
{
    lara_col_monkeyswing(item, coll);
}

void lara_as_poleleft(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    if (((input & (IN_LEFT | IN_ACTION)) != (IN_LEFT | IN_ACTION)) || input & (IN_FORWARD | IN_BACK) || item->hit_points <= 0)
        item->goal_anim_state = AS_POLESTAT;
    else
        item->pos.y_rot += 256;
}

void lara_as_poleright(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    if (((input & (IN_RIGHT | IN_ACTION)) != (IN_RIGHT | IN_ACTION)) || input & (IN_FORWARD | IN_BACK) || item->hit_points <= 0)
        item->goal_anim_state = AS_POLESTAT;
    else
        item->pos.y_rot -= 256;
}

void lara_col_stop(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (LaraFallen(item, coll))
        return;
    if (TestLaraSlide(item, coll))
        return;
    ShiftItem(item, coll);
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_as_upjump(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
}

void lara_as_forwardjump(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->goal_anim_state == AS_SWANDIVE || item->goal_anim_state == AS_REACH)
        item->goal_anim_state = AS_FORWARDJUMP;
    if (item->goal_anim_state != AS_DEATH && item->goal_anim_state != AS_STOP && item->goal_anim_state != AS_RUN) {
        if ((input & IN_ACTION) && lara.gun_status == LG_NO_ARMS)
            item->goal_anim_state = AS_REACH;
        if (input & (IN_ROLL | IN_BACK))
            item->goal_anim_state = AS_TWIST;
        if ((input & IN_WALK) && lara.gun_status == LG_NO_ARMS)
            item->goal_anim_state = AS_SWANDIVE;
        if (item->fallspeed > LARA_FASTFALL_SPEED)
            item->goal_anim_state = AS_FASTFALL;
    }
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_JUMP_TURN)
            lara.turn_rate = -LARA_JUMP_TURN;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_JUMP_TURN)
            lara.turn_rate = LARA_JUMP_TURN;
    }
}

void lara_col_forwardjump(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->speed >= 0)
        lara.move_angle = item->pos.y_rot;
    else
        lara.move_angle = item->pos.y_rot - 0x8000;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 192;
    GetLaraCollisionInfo(item, coll);
    LaraDeflectEdgeJump(item, coll);
    if (item->speed < 0)
        lara.move_angle = item->pos.y_rot;
    if (coll->mid_floor <= 0 && item->fallspeed > 0) {
        if (LaraLandedBad(item, coll))
            item->goal_anim_state = AS_DEATH;
        else if (lara.water_status != LW_WADE && (input & IN_FORWARD) && !(input & IN_WALK))
            item->goal_anim_state = AS_RUN;
        else
            item->goal_anim_state = AS_STOP;
        item->fallspeed = 0;
        item->gravity_status = 0;
        item->speed = 0;
        if (coll->mid_floor != NO_HEIGHT)
            item->pos.y_pos += coll->mid_floor;
        AnimateLara(item);
    }
}

void lara_as_reach(ITEM_INFO *item, COLL_INFO *coll)
{
    camera.target_angle = 85 * ONE_DEGREE;
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
}

void LaraSlideEdgeJump(ITEM_INFO *item, COLL_INFO *coll)
{
    ShiftItem(item, coll);
    switch (coll->coll_type) {
    case COLL_LEFT:
        item->pos.y_rot += LARA_DEF_ADD_EDGE;
        break;
    case COLL_RIGHT:
        item->pos.y_rot -= LARA_DEF_ADD_EDGE;
        break;
    case COLL_TOP:
    case COLL_TOPFRONT:
        if (item->fallspeed <= 0)
            item->fallspeed = 1;
        break;
    case COLL_CLAMP:
        item->pos.z_pos -= (phd_cos(coll->facing) * 100) >> W2V_SHIFT;
        item->pos.x_pos -= (phd_sin(coll->facing) * 100) >> W2V_SHIFT;
        item->speed = 0;
        coll->mid_floor = 0;
        if (item->fallspeed <= 0)
            item->fallspeed = 16;
        break;
    default:
        break;
    }
}

void lara_as_walk(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    if (lara.IsMoving)
        return;
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_SLOW_TURN)
            lara.turn_rate = -LARA_SLOW_TURN;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_SLOW_TURN)
            lara.turn_rate = LARA_SLOW_TURN;
    }
    if (input & IN_FORWARD) {
        if (lara.water_status == LW_WADE)
            item->goal_anim_state = AS_WADE;
        else if (!(input & IN_WALK))
            item->goal_anim_state = AS_RUN;
        else
            item->goal_anim_state = AS_WALK;
    } else
        item->goal_anim_state = AS_STOP;
}

void lara_as_run(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_DEATH;
        return;
    }
    if (input & IN_ROLL) {
        item->anim_number = ANIM_ROLL;
        item->frame_number = anims[ANIM_ROLL].frame_base + 2;
        item->current_anim_state = AS_ROLL;
        item->goal_anim_state = AS_STOP;
        return;
    }
    if ((input & IN_SPRINT) && DashTimer) {
        item->goal_anim_state = AS_DASH;
        return;
    }
    if ((input & IN_DUCK) && lara.water_status != LW_WADE
        && (lara.gun_status == LG_NO_ARMS || lara.gun_type == WEAPON_NONE || lara.gun_type == WEAPON_PISTOLS
            || lara.gun_type == WEAPON_REVOLVER || lara.gun_type == WEAPON_UZI || lara.gun_type == WEAPON_FLARE)) {
        item->goal_anim_state = AS_DUCK;
        return;
    }
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_FAST_TURN)
            lara.turn_rate = -LARA_FAST_TURN;
        item->pos.z_rot -= LARA_LEAN_RATE;
        if (item->pos.z_rot < -LARA_LEAN_MAX)
            item->pos.z_rot = -LARA_LEAN_MAX;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_FAST_TURN)
            lara.turn_rate = LARA_FAST_TURN;
        item->pos.z_rot += LARA_LEAN_RATE;
        if (item->pos.z_rot > LARA_LEAN_MAX)
            item->pos.z_rot = LARA_LEAN_MAX;
    }
    if (item->anim_number == ANIM_STARTRUN)
        jump_ok = 0;
    else if (item->anim_number != ANIM_RUN || item->frame_number == 4)
        jump_ok = 1;
    if ((input & IN_JUMP) && jump_ok && !item->gravity_status)
        item->goal_anim_state = AS_FORWARDJUMP;
    else if (input & IN_FORWARD) {
        if (lara.water_status == LW_WADE)
            item->goal_anim_state = AS_WADE;
        else if (input & IN_WALK)
            item->goal_anim_state = AS_WALK;
        else
            item->goal_anim_state = AS_RUN;
    } else
        item->goal_anim_state = AS_STOP;
}

void lara_as_fastback(ITEM_INFO *item, COLL_INFO *coll)
{
    item->goal_anim_state = AS_STOP;
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_MED_TURN)
            lara.turn_rate = -LARA_MED_TURN;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_MED_TURN)
            lara.turn_rate = LARA_MED_TURN;
    }
}

void lara_as_turn_r(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    lara.turn_rate += LARA_TURN_RATE;
    if (lara.gun_status == LG_READY && lara.water_status != LW_WADE)
        item->goal_anim_state = AS_FASTTURN;
    else if (lara.turn_rate > LARA_SLOW_TURN) {
        if (!(input & IN_WALK) && lara.water_status != LW_WADE)
            item->goal_anim_state = AS_FASTTURN;
        else
            lara.turn_rate = LARA_SLOW_TURN;
    }
    if (input & IN_FORWARD) {
        if (lara.water_status == LW_WADE)
            item->goal_anim_state = AS_WADE;
        else if (input & IN_WALK)
            item->goal_anim_state = AS_WALK;
        else
            item->goal_anim_state = AS_RUN;
    } else if (!(input & IN_RIGHT))
        item->goal_anim_state = AS_STOP;
}

void lara_as_turn_l(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    lara.turn_rate -= LARA_TURN_RATE;
    if (lara.gun_status == LG_READY && lara.water_status != LW_WADE)
        item->goal_anim_state = AS_FASTTURN;
    else if (lara.turn_rate < -LARA_SLOW_TURN) {
        if (!(input & IN_WALK) && lara.water_status != LW_WADE)
            item->goal_anim_state = AS_FASTTURN;
        else
            lara.turn_rate = -LARA_SLOW_TURN;
    }
    if (input & IN_FORWARD) {
        if (lara.water_status == LW_WADE)
            item->goal_anim_state = AS_WADE;
        else if (input & IN_WALK)
            item->goal_anim_state = AS_WALK;
        else
            item->goal_anim_state = AS_RUN;
    } else if (!(input & IN_LEFT))
        item->goal_anim_state = AS_STOP;
}

void lara_as_fastfall(ITEM_INFO *item, COLL_INFO *coll)
{
    FASTFALL_FRICTION;
    if (item->fallspeed == DAMAGE_START + DAMAGE_LENGTH)
        SoundEffect(30, &item->pos, 0);
}

void lara_as_hang(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.IsClimbing = 0;
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    if (input & IN_LOOK)
        LookUpDown();
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    camera.target_angle = 0;
    camera.target_elevation = -10920;
}

void lara_as_hangleft(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.target_angle = CAM_A_HANG;
    camera.target_elevation = CAM_E_HANG;
    if (!(input & (IN_LEFT | IN_LSTEP)))
        item->goal_anim_state = AS_HANG;
}

void lara_col_hangleft(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot - 16384;
    coll->radius = 102;
    LaraHangTest(item, coll);
    lara.move_angle = item->pos.y_rot - 16384;
}

void lara_as_hangright(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.target_angle = CAM_A_HANG;
    camera.target_elevation = CAM_E_HANG;
    if (!(input & (IN_RIGHT | IN_RSTEP)))
        item->goal_anim_state = AS_HANG;
}

void lara_col_hangright(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot + 16384;
    coll->radius = 102;
    LaraHangTest(item, coll);
    lara.move_angle = item->pos.y_rot + 16384;
}

void lara_as_splat(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
}

void lara_as_compress(ITEM_INFO *item, COLL_INFO *coll)
{
    if (lara.water_status != LW_WADE) {
        if ((input & IN_FORWARD) && LaraFloorFront(item, item->pos.y_rot, 256) >= -STEPUP_HEIGHT) {
            item->goal_anim_state = AS_FORWARDJUMP;
            lara.move_angle = item->pos.y_rot;
        } else if ((input & IN_LEFT) && LaraFloorFront(item, item->pos.y_rot - 16384, 256) >= -STEPUP_HEIGHT) {
            item->goal_anim_state = AS_LEFTJUMP;
            lara.move_angle = item->pos.y_rot - 16384;
        } else if ((input & IN_RIGHT) && LaraFloorFront(item, item->pos.y_rot + 16384, 256) >= -STEPUP_HEIGHT) {
            item->goal_anim_state = AS_RIGHTJUMP;
            lara.move_angle = item->pos.y_rot + 16384;
        } else if ((input & IN_BACK) && LaraFloorFront(item, item->pos.y_rot - 32768, 256) >= -STEPUP_HEIGHT) {
            item->goal_anim_state = AS_BACKJUMP;
            lara.move_angle = item->pos.y_rot - 32768;
        }
    }
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
}

void lara_as_back(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    if (lara.IsMoving)
        return;
    if (input & IN_BACK && (input & IN_WALK || lara.water_status == LW_WADE))
        item->goal_anim_state = AS_BACK;
    else
        item->goal_anim_state = AS_STOP;
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_SLOW_TURN)
            lara.turn_rate = -LARA_SLOW_TURN;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_SLOW_TURN)
            lara.turn_rate = LARA_SLOW_TURN;
    }
}

void lara_as_null(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
}

void lara_as_fastturn(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    if (lara.turn_rate < 0) {
        lara.turn_rate = -LARA_FAST_TURN;
        if (!(input & IN_LEFT))
            item->goal_anim_state = AS_STOP;
    } else {
        lara.turn_rate = LARA_FAST_TURN;
        if (!(input & IN_RIGHT))
            item->goal_anim_state = AS_STOP;
    }
}

void lara_as_stepright(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    if (lara.IsMoving)
        return;
    if (!(input & IN_RSTEP))
        item->goal_anim_state = AS_STOP;
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_SLOW_TURN)
            lara.turn_rate = -LARA_SLOW_TURN;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_SLOW_TURN)
            lara.turn_rate = LARA_SLOW_TURN;
    }
}

void lara_as_stepleft(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_STOP;
        return;
    }
    if (lara.IsMoving)
        return;
    if (!(input & IN_LSTEP))
        item->goal_anim_state = AS_STOP;
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_SLOW_TURN)
            lara.turn_rate = -LARA_SLOW_TURN;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_SLOW_TURN)
            lara.turn_rate = LARA_SLOW_TURN;
    }
}

void lara_as_slide(ITEM_INFO *item, COLL_INFO *coll)
{
    camera.target_elevation = -45 * ONE_DEGREE;
    if ((input & IN_JUMP) && !(input & IN_BACK))
        item->goal_anim_state = AS_FORWARDJUMP;
}

void lara_as_backjump(ITEM_INFO *item, COLL_INFO *coll)
{
    camera.target_angle = ONE_DEGREE * 135;
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
    else if (item->goal_anim_state == AS_RUN)
        item->goal_anim_state = AS_STOP;
    else if ((input & IN_FORWARD || input & IN_ROLL) && item->goal_anim_state != AS_STOP)
        item->goal_anim_state = AS_TWIST;
}

void lara_as_rightjump(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
    else if (input & IN_LEFT && item->goal_anim_state != AS_STOP)
        item->goal_anim_state = AS_TWIST;
}

void lara_as_leftjump(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
    else if (input & IN_RIGHT && item->goal_anim_state != AS_STOP)
        item->goal_anim_state = AS_TWIST;
}

void lara_as_fallback(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
    if ((input & IN_ACTION) && lara.gun_status == LG_NO_ARMS)
        item->goal_anim_state = AS_REACH;
}

void lara_as_slideback(ITEM_INFO *item, COLL_INFO *coll)
{
    if ((input & IN_JUMP) && !(input & IN_FORWARD))
        item->goal_anim_state = AS_BACKJUMP;
}

void lara_as_pushblock(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.flags = FOLLOW_CENTRE;
    camera.target_angle = 90 * ONE_DEGREE;
    camera.target_elevation = -25 * ONE_DEGREE;
}

void lara_as_ppready(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.target_angle = 75 * ONE_DEGREE;
    if (!(input & IN_ACTION))
        item->goal_anim_state = AS_STOP;
}

void lara_as_pickup(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.target_angle = -130 * ONE_DEGREE;
    camera.target_elevation = -15 * ONE_DEGREE;
    camera.target_distance = WALL_L;
}

void lara_as_pickupflare(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;
    camera.target_angle = 23660;
    camera.target_elevation = -2730;
    camera.target_distance = 1024;
    if (item->frame_number == anims[item->anim_number].frame_end - 1)
        lara.gun_status = LG_NO_ARMS;
}

void lara_as_switchon(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.target_angle = 80 * ONE_DEGREE;
    camera.target_elevation = -25 * ONE_DEGREE;
    camera.target_distance = WALL_L;
    camera.speed = 6;
}

void lara_as_usekey(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.target_angle = -80 * ONE_DEGREE;
    camera.target_elevation = -25 * ONE_DEGREE;
    camera.target_distance = WALL_L;
}

void lara_as_special(ITEM_INFO *item, COLL_INFO *coll)
{
    camera.flags = FOLLOW_CENTRE;
    camera.target_angle = 170 * ONE_DEGREE;
    camera.target_elevation = -25 * ONE_DEGREE;
}

void lara_as_swandive(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 1;
    if (item->fallspeed > LARA_FASTFALL_SPEED && item->goal_anim_state != AS_DIVE)
        item->goal_anim_state = AS_FASTDIVE;
}

void lara_as_fastdive(ITEM_INFO *item, COLL_INFO *coll)
{
    if ((input & IN_ROLL) && item->goal_anim_state == AS_FASTDIVE)
        item->goal_anim_state = AS_TWIST;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 1;
    FASTFALL_FRICTION;
}

void lara_as_waterout(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    camera.flags = FOLLOW_CENTRE;
}

void lara_as_deathslide(ITEM_INFO *item, COLL_INFO *coll)
{
    sint16 room_number;
    FLOOR_INFO *floor;

    camera.target_angle = ONE_DEGREE * 70;
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    coll->trigger = trigger_index;
    if (!(input & IN_ACTION)) {
        item->goal_anim_state = AS_FORWARDJUMP;
        AnimateLara(item);
        lara_item->gravity_status = 1;
        lara_item->speed = 100;
        lara_item->fallspeed = 40;
        lara.move_angle = item->pos.y_rot;
    }
}

void lara_col_run(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (TestLaraVault(item, coll))
        return;
    if (LaraDeflectEdge(item, coll)) {
        item->pos.z_rot = 0;
        if (item->anim_number != ANIM_STARTRUN && TestWall(item, 256, 0, -640)) {
            item->current_anim_state = AS_SPLAT;
            if (item->frame_number >= 0 && item->frame_number <= 9) {
                item->anim_number = ANIM_HITWALLLEFT;
                item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
                return;
            }
            if (item->frame_number >= 10 && item->frame_number <= 21) {
                item->anim_number = ANIM_HITWALLRIGHT;
                item->frame_number = anims[ANIM_HITWALLRIGHT].frame_base;
                return;
            }
        }
        LaraCollideStop(item, coll);
    }
    if (LaraFallen(item, coll))
        return;
    if (coll->mid_floor >= -STEPUP_HEIGHT && coll->mid_floor < -STEP_L / 2) {
        if (coll->front_floor == NO_HEIGHT || coll->front_floor < -STEPUP_HEIGHT || coll->front_floor >= -STEP_L / 2)
            coll->mid_floor = 0;
        else if (item->frame_number >= 3 && item->frame_number <= 14) {
            item->anim_number = ANIM_RUNSTEPUP_LEFT;
            item->frame_number = anims[ANIM_RUNSTEPUP_LEFT].frame_base;
        } else {
            item->anim_number = ANIM_RUNSTEPUP_RIGHT;
            item->frame_number = anims[ANIM_RUNSTEPUP_RIGHT].frame_base;
        }
    }
    if (TestLaraSlide(item, coll))
        return;
    if (coll->mid_floor >= 50)
        item->pos.y_pos += 50;
    else if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_fastback(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    lara.move_angle = item->pos.y_rot - 32768;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 0;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (coll->mid_floor > 200) {
        item->anim_number = ANIM_FALLBACK;
        item->frame_number = anims[ANIM_FALLBACK].frame_base;
        item->current_anim_state = AS_FALLBACK;
        item->goal_anim_state = AS_FALLBACK;
        item->fallspeed = 0;
        item->gravity_status = 1;
        return;
    }
    if (TestLaraSlide(item, coll))
        return;
    if (LaraDeflectEdge(item, coll))
        LaraCollideStop(item, coll);
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_turn_r(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (coll->mid_floor > 100) {
        item->anim_number = ANIM_FALLDOWN;
        item->frame_number = anims[ANIM_FALLDOWN].frame_base;
        item->current_anim_state = AS_FORWARDJUMP;
        item->goal_anim_state = AS_FORWARDJUMP;
        item->fallspeed = 0;
        item->gravity_status = 1;
        return;
    }
    if (TestLaraSlide(item, coll))
        return;
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_turn_l(ITEM_INFO *item, COLL_INFO *coll)
{
    lara_col_turn_r(item, coll);
}

void lara_col_death(ITEM_INFO *item, COLL_INFO *coll)
{
    StopSoundEffect(30);
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->radius = LARA_RAD * 4;
    GetLaraCollisionInfo(item, coll);
    ShiftItem(item, coll);
    item->hit_points = -1;
    lara.air = -1;
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_fastfall(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = 1;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 192;
    GetLaraCollisionInfo(item, coll);
    LaraSlideEdgeJump(item, coll);
    if (coll->mid_floor <= 0) {
        if (LaraLandedBad(item, coll)) {
            item->goal_anim_state = AS_DEATH;
        } else {
            item->goal_anim_state = AS_STOP;
            item->current_anim_state = AS_STOP;
            item->frame_number = anims[ANIM_LANDFAR].frame_base;
            item->anim_number = ANIM_LANDFAR;
        }
        StopSoundEffect(30);
        item->fallspeed = 0;
        item->gravity_status = 0;
        if (coll->mid_floor != NO_HEIGHT)
            item->pos.y_pos += coll->mid_floor;
    }
}

void lara_col_splat(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    ShiftItem(item, coll);
    if (coll->mid_floor >= -STEP_L && coll->mid_floor <= STEP_L)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_compress(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = NO_HEIGHT;
    coll->bad_ceiling = 0;
    GetLaraCollisionInfo(item, coll);
    if (LaraFallen(item, coll))
        return;
    if (coll->mid_ceiling > -100) {
        item->goal_anim_state = AS_STOP;
        item->current_anim_state = AS_STOP;
        item->anim_number = ANIM_STOP;
        item->frame_number = anims[ANIM_STOP].frame_base;
        item->speed = 0;
        item->fallspeed = 0;
        item->gravity_status = 0;
        item->pos.x_pos = coll->old.x;
        item->pos.y_pos = coll->old.y;
        item->pos.z_pos = coll->old.z;
    }
    if (coll->mid_floor > -STEP_L && coll->mid_floor < STEP_L)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_back(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    lara.move_angle = item->pos.y_rot - 32768;
    if (lara.water_status == LW_WADE)
        coll->bad_pos = -NO_HEIGHT;
    else
        coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (LaraDeflectEdge(item, coll))
        LaraCollideStop(item, coll);
    if (LaraFallen(item, coll))
        return;
    if (coll->mid_floor > STEP_L / 2 && coll->mid_floor < (STEP_L * 3) / 2) {
        if (item->frame_number >= 964 && item->frame_number <= 993) {
            item->anim_number = ANIM_BACKSTEPD_RIGHT;
            item->frame_number = anims[ANIM_BACKSTEPD_RIGHT].frame_base;
        } else {
            item->anim_number = ANIM_BACKSTEPD_LEFT;
            item->frame_number = anims[ANIM_BACKSTEPD_LEFT].frame_base;
        }
    }
    if (TestLaraSlide(item, coll))
        return;
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_stepright(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->current_anim_state == AS_STEPRIGHT)
        lara.move_angle = item->pos.y_rot + 16384;
    else
        lara.move_angle = item->pos.y_rot - 16384;
    item->gravity_status = item->fallspeed = 0;
    if (lara.water_status == LW_WADE)
        coll->bad_pos = -NO_HEIGHT;
    else
        coll->bad_pos = STEP_L / 2;
    coll->bad_neg = -STEP_L / 2;
    coll->bad_ceiling = 0;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (LaraDeflectEdge(item, coll))
        LaraCollideStop(item, coll);
    if (LaraFallen(item, coll))
        return;
    if (TestLaraSlide(item, coll))
        return;
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_stepleft(ITEM_INFO *item, COLL_INFO *coll)
{
    lara_col_stepright(item, coll);
}

void lara_col_slide(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot;
    lara_slide_slope(item, coll);
}

void lara_slide_slope(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -512;
    coll->bad_ceiling = 0;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    LaraDeflectEdge(item, coll);
    if (coll->mid_floor > 200) {
        if (item->current_anim_state == AS_SLIDE) {
            item->anim_number = ANIM_FALLDOWN;
            item->frame_number = anims[ANIM_FALLDOWN].frame_base;
            item->current_anim_state = AS_FORWARDJUMP;
            item->goal_anim_state = AS_FORWARDJUMP;
        } else {
            item->anim_number = ANIM_FALLBACK;
            item->frame_number = anims[ANIM_FALLBACK].frame_base;
            item->current_anim_state = AS_FALLBACK;
            item->goal_anim_state = AS_FALLBACK;
        }
        StopSoundEffect(3);
        item->gravity_status = 1;
        item->fallspeed = 0;
        return;
    }
    TestLaraSlide(item, coll);
    item->pos.y_pos += coll->mid_floor;
    if (ABS(coll->tilt_x) <= 2 && ABS(coll->tilt_z) <= 2) {
        item->goal_anim_state = AS_STOP;
        StopSoundEffect(3);
    }
}

void lara_col_backjump(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot - 32768;
    lara_col_jumper(item, coll);
}

void lara_col_rightjump(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot + 16384;
    lara_col_jumper(item, coll);
}

void lara_col_leftjump(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot - 16384;
    lara_col_jumper(item, coll);
}

void lara_col_fallback(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot - 32768;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 192;
    GetLaraCollisionInfo(item, coll);
    LaraDeflectEdgeJump(item, coll);
    if (coll->mid_floor <= 0 && item->fallspeed > 0) {
        if (LaraLandedBad(item, coll))
            item->goal_anim_state = AS_DEATH;
        else
            item->goal_anim_state = AS_STOP;
        item->fallspeed = 0;
        item->gravity_status = 0;
        if (coll->mid_floor != NO_HEIGHT)
            item->pos.y_pos += coll->mid_floor;
    }
}

void lara_col_slideback(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot - 32768;
    lara_slide_slope(item, coll);
}

void lara_col_roll(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (TestLaraSlide(item, coll))
        return;
    if (LaraFallen(item, coll))
        return;
    ShiftItem(item, coll);
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_roll2(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity_status = item->fallspeed = 0;
    lara.move_angle = item->pos.y_rot - 32768;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
    if (LaraHitCeiling(item, coll))
        return;
    if (TestLaraSlide(item, coll))
        return;
    if (coll->mid_floor > 200) {
        item->anim_number = ANIM_FALLBACK;
        item->frame_number = anims[ANIM_FALLBACK].frame_base;
        item->current_anim_state = AS_FALLBACK;
        item->goal_anim_state = AS_FALLBACK;
        item->fallspeed = 0;
        item->gravity_status = 1;
        return;
    }
    ShiftItem(item, coll);
    if (coll->mid_floor != NO_HEIGHT)
        item->pos.y_pos += coll->mid_floor;
}

void lara_col_swandive(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 192;
    GetLaraCollisionInfo(item, coll);
    LaraDeflectEdgeJump(item, coll);
    if (coll->mid_floor <= 0 && item->fallspeed > 0) {
        item->goal_anim_state = AS_STOP;
        item->fallspeed = 0;
        item->gravity_status = 0;
        if (coll->mid_floor != NO_HEIGHT)
            item->pos.y_pos += coll->mid_floor;
    }
}

void lara_col_fastdive(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 192;
    GetLaraCollisionInfo(item, coll);
    LaraDeflectEdgeJump(item, coll);
    if (coll->mid_floor <= 0 && item->fallspeed > 0) {
        if (item->fallspeed > 133)
            item->goal_anim_state = AS_DEATH;
        else
            item->goal_anim_state = AS_STOP;
        item->fallspeed = 0;
        item->gravity_status = 0;
        if (coll->mid_floor != NO_HEIGHT)
            item->pos.y_pos += coll->mid_floor;
    }
}

void lara_default_col(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    GetLaraCollisionInfo(item, coll);
}

void lara_col_jumper(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->bad_pos = -NO_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 192;
    GetLaraCollisionInfo(item, coll);
    LaraDeflectEdgeJump(item, coll);
    if (item->fallspeed > 0) {
        if (coll->mid_floor <= 0) {
            if (LaraLandedBad(item, coll))
                item->goal_anim_state = AS_DEATH;
            else
                item->goal_anim_state = AS_STOP;
            item->fallspeed = 0;
            item->gravity_status = 0;
            if (coll->mid_floor != NO_HEIGHT)
                item->pos.y_pos += coll->mid_floor;
        }
    }
}

void ResetLook(void)
{
    if (camera.type == LOOK_CAMERA)
        return;
    if (lara.head_x_rot > -HEAD_TURN && lara.head_x_rot < HEAD_TURN)
        lara.head_x_rot = 0;
    else
        lara.head_x_rot -= lara.head_x_rot / 8;
    if (lara.head_y_rot > -HEAD_TURN && lara.head_y_rot < HEAD_TURN)
        lara.head_y_rot = 0;
    else
        lara.head_y_rot -= lara.head_y_rot / 8;
    if (lara.gun_status != LG_HANDS_BUSY && lara.vehicle == NO_ITEM) {
        lara.torso_x_rot = lara.head_x_rot;
        lara.torso_y_rot = lara.head_y_rot;
    }
}

void LookUpDown(void)
{
    if (lara.vehicle == NO_ITEM) {
        camera.type = LOOK_CAMERA;
        if (input & IN_FORWARD) {
            input -= IN_FORWARD;
            if (lara.head_x_rot > -6370) {
                if (BinocularRange)
                    lara.head_x_rot -= (sint16)(364 * (1792 - BinocularRange) / 3072);
                else
                    lara.head_x_rot -= 364;
            }
        } else if (input & IN_BACK) {
            input -= IN_BACK;
            if (lara.head_x_rot < 5460) {
                if (BinocularRange)
                    lara.head_x_rot += (sint16)(364 * (1792 - BinocularRange) / 3072);
                else
                    lara.head_x_rot += 364;
            }
        }
        if (lara.gun_status != LG_HANDS_BUSY && !lara.left_arm.lock && !lara.right_arm.lock)
            lara.torso_x_rot = lara.head_x_rot;
    }
}

void LookLeftRight(void)
{
    if (lara.vehicle == NO_ITEM) {
        camera.type = LOOK_CAMERA;
        if (input & IN_LEFT) {
            input -= IN_LEFT;
            if (lara.head_y_rot > -8008) {
                if (BinocularRange)
                    lara.head_y_rot -= (sint16)(364 * (1792 - BinocularRange) / 1536);
                else
                    lara.head_y_rot -= 364;
            }
        } else if (input & IN_RIGHT) {
            input -= IN_RIGHT;
            if (lara.head_y_rot < 8008) {
                if (BinocularRange)
                    lara.head_y_rot += (sint16)(364 * (1792 - BinocularRange) / 1536);
                else
                    lara.head_y_rot += 364;
            }
        }
        if (lara.gun_status != LG_HANDS_BUSY && !lara.left_arm.lock && !lara.right_arm.lock)
            lara.torso_y_rot = lara.head_y_rot;
    }
}
