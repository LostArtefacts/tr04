#include "laramisc.h"
#include "laraswim.h"
#include "anim.h"
#include "lara_states.h"
#include "specific/input.h"

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
