#include "lara.h"
#include "lara_states.h"
#include "camera.h"
#include "sound.h"
#include "specific/input.h"

#define DAMAGE_START 140
#define DAMAGE_LENGTH 14
#define FASTFALL_FRICTION item->speed = (item->speed * 95) / 100
#define HEAD_TURN (2 * ONE_DEGREE)

void GetLaraCollisionInfo(ITEM_INFO *item, COLL_INFO *coll)
{
    coll->facing = lara.move_angle;
    GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, LARA_HITE);
}

void lara_as_dashdive(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->goal_anim_state != AS_DEATH && item->goal_anim_state != AS_STOP && item->goal_anim_state != AS_RUN) {
        if (item->fallspeed > LARA_FASTFALL_SPEED)
            item->goal_anim_state = AS_FASTFALL;
    }
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

void lara_col_monkey180(ITEM_INFO *item, COLL_INFO *coll)
{
    lara_col_monkeyswing(item, coll);
}

void lara_as_upjump(ITEM_INFO *item, COLL_INFO *coll)
{
    if (item->fallspeed > LARA_FASTFALL_SPEED)
        item->goal_anim_state = AS_FASTFALL;
}
