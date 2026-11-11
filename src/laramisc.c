#include "laramisc.h"
#include "laraswim.h"
#include "anim.h"
#include "lara_states.h"
#include "specific/input.h"
#include "gameflow.h"

void LaraCheat(ITEM_INFO *item, COLL_INFO *coll)
{
    lara_item->hit_points = 1000;
    LaraUnderWater(item, coll);
    if ((input & IN_WALK) && !(input & IN_LOOK)) {
        lara.water_status = LW_ABOVE_WATER;
        item->frame_number = anims[ANIM_STOP].frame_base;
        item->anim_number = ANIM_STOP;
        item->pos.x_rot = item->pos.z_rot = 0;
        lara.torso_x_rot = lara.torso_y_rot = 0;
        lara.head_x_rot = lara.head_y_rot = 0;
        lara.gun_status = LG_NO_ARMS;
        LaraInitialiseMeshes();
        lara.mesh_effects = 0;
        lara_item->hit_points = cheat_hit_points;
    }
}

void LaraCheatyBits(void)
{
    if (!Gameflow->CheatEnabled)
        return;
    if (input & IN_D) {
        LaraCheatGetStuff();
        lara_item->hit_points = 1000;
    }
    if (input & IN_CHEAT) {
        lara_item->pos.y_pos -= 128;
        if (lara.water_status != LW_FLYCHEAT) {
            lara.water_status = LW_FLYCHEAT;
            lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
            lara_item->anim_number = ANIM_SWIMCHEAT;
            lara_item->current_anim_state = AS_SWIM;
            lara_item->goal_anim_state = AS_SWIM;
            lara_item->gravity_status = 0;
            lara_item->pos.x_rot = 5460;
            lara_item->fallspeed = 30;
            lara.air = 1800;
            lara.death_count = 0;
            lara.torso_y_rot = 0;
            lara.torso_x_rot = 0;
            lara.head_y_rot = 0;
            lara.head_x_rot = 0;
            cheat_hit_points = lara_item->hit_points;
        }
    }
}
