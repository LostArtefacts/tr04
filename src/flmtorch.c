#include "flmtorch.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "sound.h"
#include "lara.h"
#include "laraflar.h"
#include "delstuff.h"
#include "specific/function_stubs.h"

void TriggerTorchFlame(sint16 item_number, sint32 node)
{
    SPARKS *sptr;
    sint32 size;

    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 255;
    sptr->sG = (GetRandomControl() & 0x1F) + 48;
    sptr->sB = 48;
    sptr->dR = (GetRandomControl() & 0x3F) + 192;
    sptr->dG = (GetRandomControl() & 0x3F) + 128;
    sptr->dB = 32;
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 24;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0xF) - 8;
    sptr->y = 0;
    sptr->z = (GetRandomControl() & 0xF) - 8;
    sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Yvel = -16 - (GetRandomControl() & 0xF);
    sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Friction = 5;
    sptr->Flags = 4762;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
    else
        sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
    sptr->MaxYvel = -16 - (GetRandomControl() & 7);
    sptr->FxObj = (uint8)item_number;
    sptr->NodeNumber = (uint8)node;
    sptr->Scalar = 1;
    size = (GetRandomControl() & 0x1F) + 80;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 3;
    SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &items[item_number].pos, 0);
}

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
