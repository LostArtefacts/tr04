#include "laraflar.h"
#include "lara.h"
#include "anim.h"
#include "objnums.h"
#include "delstuff.h"
#include "larafire.h"
#include "lara_states.h"

void set_flare_arm(sint32 frame)
{
    sint32 anim_base;

    anim_base = objects[FLARE_ANIM].anim_index;
    if (frame >= 1) {
        if (frame < 33)
            anim_base += 1;
        else if (frame < 72)
            anim_base += 2;
        else if (frame < 95)
            anim_base += 3;
        else
            anim_base += 4;
    }
    lara.left_arm.anim_number = anim_base;
    lara.left_arm.frame_base = anims[lara.left_arm.anim_number].frame_ptr;
}

void undraw_flare(void)
{
    sint16 ani, ani2;

    ani = lara.left_arm.frame_number;
    ani2 = lara.flare_frame;
    lara.flare_control_left = 1;
    if (lara_item->goal_anim_state == AS_STOP && lara.vehicle == NO_ITEM) {
        if (lara_item->anim_number == ANIM_BREATH) {
            lara_item->anim_number = ANIM_THROWFLARE;
            lara_item->frame_number = lara.flare_frame = ani2 = anims[ANIM_THROWFLARE].frame_base + ani;
        }
        if (lara_item->anim_number == ANIM_THROWFLARE) {
            lara.flare_control_left = 0;
            if (ani2 >= anims[ANIM_THROWFLARE].frame_base + 31) {
                ani2 = 0;
                lara.gun_type = lara.request_gun_type = lara.last_gun_type;
                lara.gun_status = LG_NO_ARMS;
                InitialiseNewWeapon();
                lara.target = NULL;
                lara.left_arm.lock = lara.right_arm.lock = 0;
                lara_item->anim_number = ANIM_STOP;
                lara_item->frame_number = lara.flare_frame = anims[ANIM_STOP].frame_base;
                lara_item->current_anim_state = AS_STOP;
                lara_item->goal_anim_state = AS_STOP;
                return;
            }
            ani2++;
            lara.flare_frame = ani2;
        }
    } else {
        if (lara_item->current_anim_state == AS_STOP && lara.vehicle == NO_ITEM) {
            lara_item->anim_number = ANIM_STOP;
            lara_item->frame_number = anims[ANIM_STOP].frame_base;
        }
    }
    if (ani >= 33 && ani < 72)
        ani = 1;
    if (ani == 0) {
        ani = 1;
    } else if (ani >= 72 && ani < 72 + 23) {
        ani++;
        if (ani == 72 + 23 - 1)
            ani = 1;
    } else if (ani >= 1 && ani < 33) {
        ani++;
        if (ani == 21) {
            CreateFlare(FLARE_ITEM, 1);
            undraw_flare_meshes();
            lara.flare_age = 0;
        } else if (ani == 33) {
            ani = 0;
            lara.gun_type = lara.request_gun_type = lara.last_gun_type;
            lara.gun_status = LG_NO_ARMS;
            InitialiseNewWeapon();
            lara.target = NULL;
            lara.left_arm.lock = lara.right_arm.lock = 0;
            lara.flare_control_left = 0;
            lara.flare_frame = 0;
        }
    } else if (ani >= 95 && ani < 110) {
        ani++;
        if (ani == 110)
            ani = 1;
    }
    if (ani >= 1 && ani < 21)
        DoFlareInHand(lara.flare_age);
    lara.left_arm.frame_number = ani;
    set_flare_arm(lara.left_arm.frame_number);
}

void draw_flare_meshes(void)
{
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[FLARE_ANIM].mesh_index + LM_LHAND * 2];
}

void undraw_flare_meshes(void)
{
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
}

void ready_flare(void)
{
    lara.gun_status = LG_NO_ARMS;
    lara.left_arm.x_rot = lara.left_arm.y_rot = lara.left_arm.z_rot = 0;
    lara.right_arm.x_rot = lara.right_arm.y_rot = lara.right_arm.z_rot = 0;
    lara.left_arm.lock = lara.right_arm.lock = 0;
    lara.target = NULL;
}
