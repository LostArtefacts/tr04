#include "larasurf.h"
#include "lara_states.h"
#include "anim.h"
#include "control.h"
#include "specific/input.h"

void lara_as_surfswim(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    lara.dive_count = 0;
    if (input & IN_LEFT)
        item->pos.y_rot -= LARA_SLOW_TURN;
    else if (input & IN_RIGHT)
        item->pos.y_rot += LARA_SLOW_TURN;
    if (!(input & IN_FORWARD))
        item->goal_anim_state = AS_SURFTREAD;
    if (input & IN_JUMP)
        item->goal_anim_state = AS_SURFTREAD;
    item->fallspeed += 2 * 4;
    if (item->fallspeed > SURF_MAXSPEED)
        item->fallspeed = SURF_MAXSPEED;
}

void lara_as_surfback(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    lara.dive_count = 0;
    if (input & IN_LEFT)
        item->pos.y_rot -= LARA_SLOW_TURN / 2;
    else if (input & IN_RIGHT)
        item->pos.y_rot += LARA_SLOW_TURN / 2;
    if (!(input & IN_BACK))
        item->goal_anim_state = AS_SURFTREAD;
    item->fallspeed += 2 * 4;
    if (item->fallspeed > SURF_MAXSPEED)
        item->fallspeed = SURF_MAXSPEED;
}

void lara_as_surfleft(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    lara.dive_count = 0;
    if (input & IN_LEFT)
        item->pos.y_rot -= LARA_SLOW_TURN / 2;
    else if (input & IN_RIGHT)
        item->pos.y_rot += LARA_SLOW_TURN / 2;
    if (!(input & IN_LSTEP))
        item->goal_anim_state = AS_SURFTREAD;
    item->fallspeed += 2 * 4;
    if (item->fallspeed > SURF_MAXSPEED)
        item->fallspeed = SURF_MAXSPEED;
}

void lara_as_surfright(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    lara.dive_count = 0;
    if (input & IN_LEFT)
        item->pos.y_rot -= LARA_SLOW_TURN / 2;
    else if (input & IN_RIGHT)
        item->pos.y_rot += LARA_SLOW_TURN / 2;
    if (!(input & IN_RSTEP))
        item->goal_anim_state = AS_SURFTREAD;
    item->fallspeed += 2 * 4;
    if (item->fallspeed > SURF_MAXSPEED)
        item->fallspeed = SURF_MAXSPEED;
}

void lara_as_surftread(ITEM_INFO *item, COLL_INFO *coll)
{
    item->fallspeed -= 4;
    if (item->fallspeed < 0)
        item->fallspeed = 0;
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    if (input & IN_LOOK) {
        LookUpDown();
        return;
    }
    if (input & IN_LEFT)
        item->pos.y_rot -= LARA_SLOW_TURN;
    else if (input & IN_RIGHT)
        item->pos.y_rot += LARA_SLOW_TURN;
    if (input & IN_FORWARD)
        item->goal_anim_state = AS_SURFSWIM;
    else if (input & IN_BACK)
        item->goal_anim_state = AS_SURFBACK;
    if (input & IN_LSTEP)
        item->goal_anim_state = AS_SURFLEFT;
    else if (input & IN_RSTEP)
        item->goal_anim_state = AS_SURFRIGHT;
    if (input & IN_JUMP) {
        lara.dive_count++;
        if (lara.dive_count == DIVE_COUNT)
            item->goal_anim_state = AS_SWIM;
    } else {
        lara.dive_count = 0;
    }
}

void lara_col_surfswim(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->bad_neg = -STEPUP_HEIGHT;
    lara.move_angle = item->pos.y_rot;
    LaraSurfaceCollision(item, coll);
    LaraTestWaterClimbOut(item, coll);
}

sint32 LaraTestWaterClimbOut(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 hdif;
    PHD_ANGLE angle;

    if (coll->coll_type != COLL_FRONT || !(input & IN_ACTION) || ABS(coll->left_floor2 - coll->right_floor2) >= 60)
        return 0;
    if (lara.gun_status != LG_NO_ARMS && !(lara.gun_status == LG_READY && lara.gun_type == WEAPON_FLARE))
        return 0;
    if (coll->front_ceiling > 0 || coll->mid_ceiling > -STEPUP_HEIGHT)
        return 0;
    hdif = coll->front_floor + 700;
    if (hdif <= -(STEP_L * 2) || hdif > 700 - STEPUP_HEIGHT)
        return 0;
    angle = item->pos.y_rot;
    if (angle >= -(35 * ONE_DEGREE) && angle <= 35 * ONE_DEGREE)
        angle = 0;
    else if (angle >= 16384 - 35 * ONE_DEGREE && angle <= 16384 + 35 * ONE_DEGREE)
        angle = 16384;
    else if (angle >= 32767 - 35 * ONE_DEGREE || angle <= -32767 + 35 * ONE_DEGREE)
        angle = -32768;
    else if (angle >= -16384 - 35 * ONE_DEGREE && angle <= -16384 + 35 * ONE_DEGREE)
        angle = -16384;
    if (angle & 0x3fff)
        return 0;
    item->pos.y_pos += hdif - 5;
    UpdateLaraRoom(item, -LARA_HITE / 2);
    switch (angle) {
    case 0:
        item->pos.z_pos = (item->pos.z_pos & -WALL_L) + WALL_L + LARA_RAD;
        break;
    case 0x4000:
        item->pos.x_pos = (item->pos.x_pos & -WALL_L) + WALL_L + LARA_RAD;
        break;
    case -0x8000:
        item->pos.z_pos = (item->pos.z_pos & -WALL_L) - LARA_RAD;
        break;
    case -0x4000:
        item->pos.x_pos = (item->pos.x_pos & -WALL_L) - LARA_RAD;
        break;
    }
    if (hdif < -128) {
        item->anim_number = ANIM_SURFCLIMB;
        item->frame_number = anims[ANIM_SURFCLIMB].frame_base;
    } else if (hdif < 128) {
        item->anim_number = ANIM_SURF2STND;
        item->frame_number = anims[ANIM_SURF2STND].frame_base;
    } else {
        item->anim_number = ANIM_SURF2QSTND;
        item->frame_number = anims[ANIM_SURF2QSTND].frame_base;
    }
    item->current_anim_state = AS_WATEROUT;
    item->goal_anim_state = AS_STOP;
    item->pos.y_rot = angle;
    lara.gun_status = LG_HANDS_BUSY;
    item->pos.x_rot = item->pos.z_rot = 0;
    item->gravity_status = 0;
    item->speed = 0;
    item->fallspeed = 0;
    lara.water_status = LW_ABOVE_WATER;
    return 1;
}

void LaraSurfaceCollision(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 wh;

    coll->facing = lara.move_angle;
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos + 700, item->pos.z_pos, item->room_number, 800);
    ShiftItem(item, coll);
    if ((coll->coll_type & (COLL_FRONT | COLL_TOP | COLL_CLAMP | COLL_TOPFRONT))
        || (coll->mid_floor < 0 && (coll->mid_type == 2 || coll->mid_type == 3))) {
        item->fallspeed = 0;
        item->pos.x_pos = coll->old.x;
        item->pos.y_pos = coll->old.y;
        item->pos.z_pos = coll->old.z;
    } else if (coll->coll_type == COLL_LEFT) {
        item->pos.y_rot += 5 * ONE_DEGREE;
    } else if (coll->coll_type == COLL_RIGHT) {
        item->pos.y_rot -= 5 * ONE_DEGREE;
    }
    wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
    if (wh - item->pos.y_pos <= -100) {
        item->goal_anim_state = AS_SWIM;
        item->current_anim_state = AS_DIVE;
        item->anim_number = ANIM_SURFDIVE;
        item->frame_number = anims[ANIM_SURFDIVE].frame_base;
        item->pos.x_rot = -45 * ONE_DEGREE;
        item->fallspeed = 20 * 4;
        lara.water_status = LW_UNDERWATER;
        return;
    }
    LaraTestWaterStepOut(item, coll);
}

sint32 LaraTestWaterStepOut(ITEM_INFO *item, COLL_INFO *coll)
{
    if (coll->coll_type == COLL_FRONT || coll->mid_type == 2 || coll->mid_type == 3 || coll->mid_floor >= 0)
        return 0;
    if (coll->mid_floor < -(STEP_L / 2)) {
        item->anim_number = ANIM_SURF2WADE1;
        item->frame_number = anims[ANIM_SURF2WADE1].frame_base;
        item->current_anim_state = AS_WATEROUT;
        item->goal_anim_state = AS_STOP;
    } else if (item->goal_anim_state == AS_SURFLEFT)
        item->goal_anim_state = AS_STEPLEFT;
    else if (item->goal_anim_state == AS_SURFRIGHT)
        item->goal_anim_state = AS_STEPRIGHT;
    else {
        item->goal_anim_state = AS_WADE;
        item->current_anim_state = AS_WADE;
        item->anim_number = ANIM_WADE;
        item->frame_number = anims[ANIM_WADE].frame_base;
    }
    item->pos.y_pos += coll->front_floor + 700 - 5;
    UpdateLaraRoom(item, -LARA_HITE / 2);
    item->pos.x_rot = item->pos.z_rot = 0;
    item->gravity_status = 0;
    item->speed = 0;
    item->fallspeed = 0;
    lara.water_status = LW_WADE;
    return 1;
}

void lara_col_surfback(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot - 32768;
    LaraSurfaceCollision(item, coll);
}

void lara_col_surfleft(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot - 16384;
    LaraSurfaceCollision(item, coll);
}

void lara_col_surfright(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.move_angle = item->pos.y_rot + 16384;
    LaraSurfaceCollision(item, coll);
}

void lara_col_surftread(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->goal_anim_state == AS_SWIM) {
        item->current_anim_state = AS_DIVE;
        item->anim_number = ANIM_SURFDIVE;
        item->frame_number = anims[ANIM_SURFDIVE].frame_base;
        item->pos.x_rot = -45 * ONE_DEGREE;
        item->fallspeed = 20 * 4;
        lara.water_status = LW_UNDERWATER;
    }
    lara.move_angle = item->pos.y_rot;
    LaraSurfaceCollision(item, coll);
}
