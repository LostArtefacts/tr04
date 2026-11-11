#include "laraflar.h"
#include "lara.h"
#include "anim.h"
#include "objnums.h"
#include "delstuff.h"

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

void draw_flare_meshes(void)
{
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[FLARE_ANIM].mesh_index + LM_LHAND * 2];
}

void ready_flare(void)
{
    lara.gun_status = LG_NO_ARMS;
    lara.left_arm.x_rot = lara.left_arm.y_rot = lara.left_arm.z_rot = 0;
    lara.right_arm.x_rot = lara.right_arm.y_rot = lara.right_arm.z_rot = 0;
    lara.left_arm.lock = lara.right_arm.lock = 0;
    lara.target = NULL;
}
