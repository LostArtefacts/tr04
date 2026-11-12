#include "flmtorch.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "lara.h"
#include "laraflar.h"
#include "delstuff.h"

void GetFlameTorch(void)
{
    if (lara.gun_type == WEAPON_FLARE)
        CreateFlare(FLARE_ITEM, 0);
    lara.request_gun_type = WEAPON_TORCH;
    lara.gun_type = WEAPON_TORCH;
    lara.flare_control_left = 1;
    lara.left_arm.anim_number = objects[TORCH_ANIM].anim_index;
    lara.gun_status = LG_READY;
    lara.left_arm.lock = 0;
    lara.left_arm.frame_number = 0;
    lara.left_arm.frame_base = anims[objects[TORCH_ANIM].anim_index].frame_ptr;
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[TORCH_ANIM].mesh_index + LM_LHAND * 2];
}
