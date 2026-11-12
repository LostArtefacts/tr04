#include "delstuff.h"
#include "anim.h"
#include "control.h"
#include "draw.h"
#include "lara_states.h"
#include "lara.h"
#include "room.h"
#include "specific/3dmath.h"

void CalcLaraMatrices(sint32 flag)
{
    ITEM_INFO *l;
    sint32 *bone;
    sint16 *frame;
    sint16 *frmptr[2];
    sint32 rate, frac;
    sint16 jerk;

    l = lara_item;
    bone = &bones[objects[l->object_number].bone_index];
    if (lara.hit_direction < 0) {
        frac = GetFrames(l, frmptr, &rate);
        if (frac) {
            GLaraShadowframe = GetBoundsAccurate(l);
            Rich_CalcLaraMatrices_Interpolated(frmptr[0], frmptr[1], frac, rate, bone, flag);
            return;
        }
    }
    if (lara.hit_direction >= 0) {
        switch (lara.hit_direction) {
        case 0:
            jerk = lara.IsDucked ? ANIM_JERK_DUCKF : ANIM_JERK_FORWARD;
            break;
        case 1:
            jerk = lara.IsDucked ? ANIM_JERK_DUCKR : ANIM_JERK_RIGHT;
            break;
        case 2:
            jerk = lara.IsDucked ? ANIM_JERK_DUCKB : ANIM_JERK_BACK;
            break;
        default:
            jerk = lara.IsDucked ? ANIM_JERK_DUCKL : ANIM_JERK_LEFT;
            break;
        }
        frame = &anims[jerk].frame_ptr[lara.hit_frame * (anims[jerk].interpolation >> 8)];
    } else {
        frame = *frmptr;
    }
    Rich_CalcLaraMatrices_Normal(frame, bone, flag);
}

void GetLaraJointPos(PHD_VECTOR *pos, sint32 node)
{
    phd_PushMatrix();
    phd_mxptr[M00] = lara_joint_matrices[node * 12 + M00];
    phd_mxptr[M01] = lara_joint_matrices[node * 12 + M01];
    phd_mxptr[M02] = lara_joint_matrices[node * 12 + M02];
    phd_mxptr[M03] = lara_joint_matrices[node * 12 + M03];
    phd_mxptr[M10] = lara_joint_matrices[node * 12 + M10];
    phd_mxptr[M11] = lara_joint_matrices[node * 12 + M11];
    phd_mxptr[M12] = lara_joint_matrices[node * 12 + M12];
    phd_mxptr[M13] = lara_joint_matrices[node * 12 + M13];
    phd_mxptr[M20] = lara_joint_matrices[node * 12 + M20];
    phd_mxptr[M21] = lara_joint_matrices[node * 12 + M21];
    phd_mxptr[M22] = lara_joint_matrices[node * 12 + M22];
    phd_mxptr[M23] = lara_joint_matrices[node * 12 + M23];
    phd_TranslateRel(pos->x, pos->y, pos->z);
    pos->x = phd_mxptr[M03] >> W2V_SHIFT;
    pos->y = phd_mxptr[M13] >> W2V_SHIFT;
    pos->z = phd_mxptr[M23] >> W2V_SHIFT;
    pos->x += lara_item->pos.x_pos;
    pos->y += lara_item->pos.y_pos;
    pos->z += lara_item->pos.z_pos;
    phd_PopMatrix();
}

void SetLaraUnderwaterNodes(void)
{
    PHD_VECTOR pos;
    ROOM_INFO *r;
    sint32 bit, i;
    sint16 room_num;

    pos.x = lara_item->pos.x_pos;
    pos.y = lara_item->pos.y_pos;
    pos.z = lara_item->pos.z_pos;
    room_num = lara_item->room_number;
    GetFloor(pos.x, pos.y, pos.z, &room_num);
    bLaraInWater = room[room_num].flags & ROOM_UNDERWATER;
    bit = 0;
    for (i = 14; i >= 0; i--) {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
        GetLaraJointPos(&pos, i);
        if (lara_mesh_sweetness_table[i] == 7)
            pos.y -= 120;
        if (lara_mesh_sweetness_table[i] == 14)
            pos.y -= 60;
        room_num = lara_item->room_number;
        GetFloor(pos.x, pos.y, pos.z, &room_num);
        r = &room[room_num];
        LaraNodeUnderwater[i] = r->flags & ROOM_UNDERWATER;
        if (LaraNodeUnderwater[i]) {
            lara.wet[i] = 252;
            if (!(bit & 1)) {
                LaraNodeAmbient[1] = r->ambient;
                bit |= 1;
            }
        } else if (!(bit & 2)) {
            LaraNodeAmbient[0] = r->ambient;
            bit |= 2;
        }
    }
}
