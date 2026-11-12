#include "laramisc.h"
#include "laraswim.h"
#include "anim.h"
#include "lara_states.h"
#include "specific/input.h"
#include "gameflow.h"
#include "objnums.h"
#include "room.h"
#include "control.h"
#include "effects.h"
#include "sound.h"
#include "rope.h"
#include "specific/3dmath.h"

void LaraInitialiseMeshes(void)
{
    sint32 i;

    for (i = 0; i < 15; i++) {
        meshes[objects[LARA].mesh_index + i * 2] = meshes[objects[LARA_SKIN].mesh_index + i * 2];
        lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + i * 2];
    }
    if (lara.gun_type == WEAPON_GRENADE)
        lara.back_gun = GRENADE_GUN_ANIM;
    else if (lara.shotgun_type_carried)
        lara.back_gun = SHOTGUN_ANIM;
    else if (lara.grenade_type_carried)
        lara.back_gun = GRENADE_GUN_ANIM;
    lara.gun_status = LG_NO_ARMS;
    lara.left_arm.frame_number = 0;
    lara.right_arm.frame_number = 0;
    lara.target = 0;
    lara.right_arm.lock = 0;
    lara.left_arm.lock = 0;
}

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

void AnimateLara(ITEM_INFO *item)
{
    sint32 i;
    ANIM_STRUCT *anim;
    sint16 *command;
    sint32 speed;
    uint16 type;

    item->frame_number++;
    anim = &anims[item->anim_number];
    if (anim->number_changes > 0) {
        if (GetChange(item, anim)) {
            anim = &anims[item->anim_number];
            item->current_anim_state = anim->current_anim_state;
        }
    }
    if (item->frame_number > anim->frame_end) {
        if (anim->number_commands > 0) {
            command = &commands[anim->command_index];
            for (i = anim->number_commands; i > 0; i--) {
                switch (*(command++)) {
                case 1:
                    TranslateItem(item, *command, *(command + 1), *(command + 2));
                    UpdateLaraRoom(item, -381);
                    command += 3;
                    break;
                case 2:
                    item->fallspeed = *(command++);
                    item->speed = *(command++);
                    item->gravity_status = 1;
                    if (lara.calc_fallspeed) {
                        item->fallspeed = lara.calc_fallspeed;
                        lara.calc_fallspeed = 0;
                    }
                    break;
                case 3:
                    if (lara.gun_status != LG_FLARE)
                        lara.gun_status = LG_NO_ARMS;
                    break;
                case 4:
                    break;
                case 5:
                case 6:
                    command += 2;
                    break;
                }
            }
        }
        item->anim_number = anim->jump_anim_num;
        item->frame_number = anim->jump_frame_num;
        anim = &anims[item->anim_number];
        item->current_anim_state = anim->current_anim_state;
    }
    if (anim->number_commands > 0) {
        command = &commands[anim->command_index];
        for (i = anim->number_commands; i > 0; i--) {
            switch (*(command++)) {
            case 1:
                command += 3;
                break;
            case 2:
                command += 2;
                break;
            case 3:
            case 4:
                break;
            case 5:
                if (item->frame_number == *command) {
                    type = *(command + 1) & 0xC000;
                    if (type == 0 || (type == 0x4000 && (lara.water_surface_dist >= 0 || lara.water_surface_dist == NO_HEIGHT)) ||
                        (type == 0x8000 && lara.water_surface_dist < 0 && lara.water_surface_dist != NO_HEIGHT))
                        SoundEffect(*(command + 1) & 0x3FFF, &item->pos, 2);
                }
                command += 2;
                break;
            case 6:
                if (item->frame_number == *command) {
                    FXType = *(command + 1) & 0xC000;
                    effect_routines[*(command + 1) & 0x3FFF](item);
                }
                command += 2;
                break;
            }
        }
    }
    if (!item->gravity_status) {
        speed = anim->velocity;
        if (anim->acceleration)
            speed += anim->acceleration * (item->frame_number - anim->frame_base);
        item->speed = (sint16)(speed >> 16);
    } else {
        speed = anim->velocity + anim->acceleration * (item->frame_number - anim->frame_base - 1);
        item->speed -= (sint16)(speed >> 16);
        speed += anim->acceleration;
        item->speed += (sint16)(speed >> 16);
        item->fallspeed += (item->fallspeed < FASTFALL_SPEED) ? 6 : 1;
        item->pos.y_pos += item->fallspeed;
    }
    if (lara.RopePtr != -1)
        AlignLaraToRope(item);
    if (!lara.IsMoving) {
        item->pos.x_pos += (rcossin_tbl[(lara.move_angle >> 3) & 0x1FFE] << 2) * item->speed >> W2V_SHIFT;
        item->pos.z_pos += (rcossin_tbl[((lara.move_angle >> 3) & 0x1FFE) + 1] << 2) * item->speed >> W2V_SHIFT;
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
