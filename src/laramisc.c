#include "laramisc.h"
#include "laraswim.h"
#include "anim.h"
#include "lara_states.h"
#include "specific/input.h"
#include "gameflow.h"
#include "objnums.h"
#include "room.h"

void LaraCheatGetStuff(void)
{
    lara.num_flares = -1;
    lara.num_small_medipack = -1;
    lara.num_large_medipack = -1;
    if (objects[CROWBAR_ITEM].loaded)
        lara.crowbar = 1;
    lara.uzis_type_carried = 9;
    lara.lasersight = 1;
    lara.shotgun_type_carried = 9;
    lara.sixshooter_type_carried = 9;
    lara.num_uzi_ammo = -1;
    lara.num_revolver_ammo = -1;
    lara.num_shotgun_ammo1 = -1;
}

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

void InitialiseLaraLoad(sint16 item_number)
{
    lara.item_number = item_number;
    lara_item = &items[item_number];
}

void InitialiseLaraAnims(ITEM_INFO *item)
{
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        lara.water_status = LW_UNDERWATER;
        item->fallspeed = 0;
        item->current_anim_state = item->goal_anim_state = AS_TREAD;
        item->anim_number = ANIM_TREAD;
        item->frame_number = anims[ANIM_TREAD].frame_base;
    } else {
        lara.water_status = LW_ABOVE_WATER;
        item->current_anim_state = item->goal_anim_state = AS_STOP;
        item->anim_number = ANIM_STOP;
        item->frame_number = anims[ANIM_STOP].frame_base;
    }
}
