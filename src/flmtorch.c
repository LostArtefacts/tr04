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
#include "specific/3dmath.h"
#include "draw.h"
#include "debris.h"
#include "laramisc.h"
#include "larafire.h"

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

void FlameTorchControl(sint16 item_number)
{
    ITEM_INFO **itemlist;
    MESH_INFO **meshlist;
    ITEM_INFO *item;
    STATIC_INFO *sinfo;
    PHD_3DPOS pos;
    sint32 x, y, z, xv, yv, zv;

    item = &items[item_number];
    if (item->fallspeed)
        item->pos.z_rot += 910;
    if (!item->fallspeed && !item->speed) {
        item->pos.x_rot = 0;
        item->pos.z_rot = 0;
    }
    xv = (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * item->speed >> W2V_SHIFT;
    x = item->pos.x_pos;
    item->pos.x_pos += xv;
    y = item->pos.y_pos;
    zv = (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * item->speed >> W2V_SHIFT;
    z = item->pos.z_pos;
    item->pos.z_pos += zv;
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        item->fallspeed += (5 - item->fallspeed) / 2;
        item->speed += (5 - item->speed) / 2;
        if (item->item_flags[3])
            item->item_flags[3] = 0;
    } else
        item->fallspeed += 6;
    yv = item->fallspeed;
    item->pos.y_pos += yv;
    DoProperDetection(item_number, x, y, z, xv, yv, zv);
    itemlist = (ITEM_INFO**)&tsv_buffer[0];
    meshlist = (MESH_INFO**)&tsv_buffer[1024];
    if (GetCollidedObjects(item, 0, 1, itemlist, meshlist, 0)) {
        mycoll.enable_baddie_push = 1;
        if (itemlist[0]) {
            if (!objects[itemlist[0]->object_number].intelligent)
                ObjectCollision(itemlist[0] - items, item, &mycoll);
        } else {
            sinfo = &static_objects[meshlist[0]->static_number];
            pos.x_pos = meshlist[0]->x;
            pos.y_pos = meshlist[0]->y;
            pos.z_pos = meshlist[0]->z;
            pos.y_rot = meshlist[0]->y_rot;
            ItemPushLaraStatic(item, (sint16 *)&sinfo->x_minc, &pos, &mycoll);
        }
        item->speed >>= 1;
    }
    if (item->item_flags[3]) {
        TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 12 - (GetRandomControl() & 1),
            (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 96, 0);
        if (!(wibble & 7))
            TriggerTorchFlame(item_number, 1);
        TorchItem = item;
    }
}
