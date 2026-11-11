#include "lara.h"
#include "lara_states.h"
#include "anim.h"
#include "room.h"
#include "control.h"
#include "camera.h"
#include "sound.h"
#include "specific/input.h"

#define DAMAGE_START 140
#define DAMAGE_LENGTH 14
#define FASTFALL_FRICTION item->speed = (item->speed * 95) / 100
#define HEAD_TURN (2 * ONE_DEGREE)
extern sint32 jump_ok;

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

void MonkeySwingSnap(ITEM_INFO *item, COLL_INFO *coll)
{
    sint16 room_num;
    FLOOR_INFO *floor;

    room_num = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
    item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + (768 - 64);
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

void lara_col_turn_l(ITEM_INFO *item, COLL_INFO *coll)
{
    lara_col_turn_r(item, coll);
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
