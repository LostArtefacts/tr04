#include "laraswim.h"
#include "anim.h"
#include "lara_states.h"
#include "control.h"
#include "specific/input.h"

void lara_as_swim(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    if (input & IN_ROLL) {
        item->current_anim_state = AS_WATERROLL;
        item->anim_number = ANIM_WATERROLL;
        item->frame_number = anims[ANIM_WATERROLL].frame_base;
        return;
    }
    SwimTurn(item);
    item->fallspeed += 2 * 4;
    if (item->fallspeed > UW_MAXSPEED)
        item->fallspeed = UW_MAXSPEED;
    if (!(input & IN_JUMP))
        item->goal_anim_state = AS_GLIDE;
}

void SwimTurn(ITEM_INFO *item)
{
    if (input & IN_FORWARD)
        item->pos.x_rot -= 2 * ONE_DEGREE;
    else if (input & IN_BACK)
        item->pos.x_rot += 2 * ONE_DEGREE;
    if (input & IN_LEFT) {
        lara.turn_rate -= LARA_TURN_RATE;
        if (lara.turn_rate < -LARA_MED_TURN)
            lara.turn_rate = -LARA_MED_TURN;
        item->pos.z_rot -= LARA_LEAN_RATE * 2;
    } else if (input & IN_RIGHT) {
        lara.turn_rate += LARA_TURN_RATE;
        if (lara.turn_rate > LARA_MED_TURN)
            lara.turn_rate = LARA_MED_TURN;
        item->pos.z_rot += LARA_LEAN_RATE * 2;
    }
}

void lara_as_glide(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    if (input & IN_ROLL) {
        item->current_anim_state = AS_WATERROLL;
        item->anim_number = ANIM_WATERROLL;
        item->frame_number = anims[ANIM_WATERROLL].frame_base;
        return;
    }
    SwimTurn(item);
    if (input & IN_JUMP)
        item->goal_anim_state = AS_SWIM;
    item->fallspeed -= WATER_FRICTION;
    if (item->fallspeed < 0)
        item->fallspeed = 0;
    if (item->fallspeed <= (UW_MAXSPEED * 2) / 3)
        item->goal_anim_state = AS_TREAD;
}

void lara_as_tread(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = AS_UWDEATH;
        return;
    }
    if (input & IN_ROLL) {
        item->current_anim_state = AS_WATERROLL;
        item->anim_number = ANIM_WATERROLL;
        item->frame_number = anims[ANIM_WATERROLL].frame_base;
        return;
    }
    if (input & IN_LOOK)
        LookUpDown();
    SwimTurn(item);
    if (input & IN_JUMP)
        item->goal_anim_state = AS_SWIM;
    item->fallspeed -= WATER_FRICTION;
    if (item->fallspeed < 0)
        item->fallspeed = 0;
    if (lara.gun_status == LG_HANDS_BUSY)
        lara.gun_status = LG_NO_ARMS;
}

void lara_as_dive(ITEM_INFO *item, COLL_INFO *coll)
{
    if (input & IN_FORWARD)
        item->pos.x_rot -= ONE_DEGREE;
}

void lara_as_uwdeath(ITEM_INFO *item, COLL_INFO *coll)
{
    lara.look = 0;
    item->fallspeed -= 8;
    if (item->fallspeed <= 0)
        item->fallspeed = 0;
    if (item->pos.x_rot >= -2 * ONE_DEGREE && item->pos.x_rot <= 2 * ONE_DEGREE)
        item->pos.x_rot = 0;
    else if (item->pos.x_rot < 0)
        item->pos.x_rot += 2 * ONE_DEGREE;
    else
        item->pos.x_rot -= 2 * ONE_DEGREE;
}

void lara_as_waterroll(ITEM_INFO *item, COLL_INFO *coll)
{
    item->fallspeed = 0;
}

void lara_col_swim(ITEM_INFO *item, COLL_INFO *coll)
{
    LaraSwimCollision(item, coll);
}

void LaraTestWaterDepth(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 wd;
    FLOOR_INFO *floor;
    sint16 room_number;

    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    wd = GetWaterDepth(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);
    if (wd == NO_HEIGHT) {
        item->pos.x_pos = coll->old.x;
        item->pos.y_pos = coll->old.y;
        item->pos.z_pos = coll->old.z;
        item->fallspeed = 0;
    } else if (wd <= STEP_L * 2) {
        item->anim_number = ANIM_SWIM2QSTND;
        item->frame_number = anims[ANIM_SWIM2QSTND].frame_base;
        item->current_anim_state = AS_WATEROUT;
        item->goal_anim_state = AS_STOP;
        item->pos.x_rot = item->pos.z_rot = 0;
        item->gravity_status = 0;
        item->speed = 0;
        item->fallspeed = 0;
        lara.water_status = LW_WADE;
        item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    }
}

void lara_col_glide(ITEM_INFO *item, COLL_INFO *coll)
{
    LaraSwimCollision(item, coll);
}

void lara_col_tread(ITEM_INFO *item, COLL_INFO *coll)
{
    LaraSwimCollision(item, coll);
}

void lara_col_dive(ITEM_INFO *item, COLL_INFO *coll)
{
    LaraSwimCollision(item, coll);
}

void lara_col_uwdeath(ITEM_INFO *item, COLL_INFO *coll)
{
    sint32 wh;

    item->hit_points = -1;
    lara.air = -1;
    lara.gun_status = LG_HANDS_BUSY;
    wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
    if (wh != NO_HEIGHT && wh < item->pos.y_pos - 100)
        item->pos.y_pos -= 5;
    LaraSwimCollision(item, coll);
}

void lara_col_waterroll(ITEM_INFO *item, COLL_INFO *coll)
{
    LaraSwimCollision(item, coll);
}
