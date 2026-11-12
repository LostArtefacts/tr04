#include "delstuff.h"
#include "anim.h"
#include "control.h"
#include "draw.h"
#include "lara_states.h"
#include "lara.h"
#include "room.h"
#include "specific/3dmath.h"
#include "specific/specificfx.h"
#include "specific/output.h"
#include "specific/input.h"
#include "specific/function_stubs.h"
#include "hair.h"
#include "objnums.h"
#include "gameflow.h"

extern uint8 NodesToStashFromScratch[15][4];
extern uint8 NodesToStashToScratch[14][2];
extern uint8 SkinUseMatrix[14][2];
extern uint8 lara_underwater_skin_sweetness_table[15];
extern sint32 in_joints;

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

void DrawLara(ITEM_INFO *item, sint32 mirror)
{
    OBJECT_INFO *obj;
    PHD_VECTOR v0;
    PHD_VECTOR v1;
    sint16 **meshpp;
    sint32 *bone;
    sint32 top, bottom, left, right, dx, dy, dz, dist, stash, xRot, i, j;
    sint16 cos, sin;
    static sint16 a = 255;

    left = phd_left;
    right = phd_right;
    top = phd_top;
    bottom = phd_bottom;
    phd_top = 0;
    phd_left = 0;
    phd_bottom = phd_winymax;
    phd_right = phd_winxmax;
    phd_PushMatrix();
    obj = &objects[item->object_number];
    if (lara.vehicle == NO_ITEM)
        S_PrintShadow(obj->shadow_size, GLaraShadowframe, item, 0);
    if (input & IN_LOOK) {
        dx = lara_item->pos.x_pos - CamPos.x;
        dy = lara_item->pos.y_pos - CamPos.y - 512;
        dz = lara_item->pos.z_pos - CamPos.z;
        dist = phd_sqrt(SQUARE(dx) + SQUARE(dy) + SQUARE(dz));
        a = dist >> 2;
        if (a < 0)
            a = 0;
        if (a > 255)
            a = 255;
        GlobalAlpha = a << 24;
    } else {
        if (a < 255) {
            a += 8;
            if (a > 255)
                a = 255;
        }
        GlobalAlpha = a << 24;
    }
    if (!mirror)
        CalculateObjectLightingLara();
    for (i = 0; i < 15; i++) {
        phd_mxptr[M00] = lara_matrices[i * 12 + M00];
        phd_mxptr[M01] = lara_matrices[i * 12 + M01];
        phd_mxptr[M02] = lara_matrices[i * 12 + M02];
        phd_mxptr[M03] = lara_matrices[i * 12 + M03];
        phd_mxptr[M10] = lara_matrices[i * 12 + M10];
        phd_mxptr[M11] = lara_matrices[i * 12 + M11];
        phd_mxptr[M12] = lara_matrices[i * 12 + M12];
        phd_mxptr[M13] = lara_matrices[i * 12 + M13];
        phd_mxptr[M20] = lara_matrices[i * 12 + M20];
        phd_mxptr[M21] = lara_matrices[i * 12 + M21];
        phd_mxptr[M22] = lara_matrices[i * 12 + M22];
        phd_mxptr[M23] = lara_matrices[i * 12 + M23];
        if (LaraNodeUnderwater[i])
            bLaraUnderWater = i;
        else
            bLaraUnderWater = -1;
        phd_PutPolygons(lara.mesh_ptrs[lara_mesh_sweetness_table[i]], -1);
        for (j = 0; j < 4; j++) {
            stash = NodesToStashFromScratch[i][j];
            if (stash == 255)
                break;
            StashSkinVertices(stash);
        }
    }
    in_joints = 1;
    phd_PopMatrix();
    bLaraUnderWater = LaraNodeUnderwater[8] != 0 ? 8 : -1;
    DrawHair();
    phd_PushMatrix();
    obj = &objects[LARA_SKIN_JOINTS];
    meshpp = &meshes[obj->mesh_index];
    meshpp += 2;
    for (i = 0; i < 14; i++) {
        SkinVerticesToScratch(NodesToStashToScratch[i][0]);
        SkinVerticesToScratch(NodesToStashToScratch[i][1]);
        bLaraUnderWater = lara_underwater_skin_sweetness_table[i];
        if (!LaraNodeUnderwater[bLaraUnderWater])
            bLaraUnderWater = -1;
        if (SkinUseMatrix[i][0] < 255) {
            phd_mxptr[M00] = lara_matrices[SkinUseMatrix[i][1] * 12 + M00];
            phd_mxptr[M01] = lara_matrices[SkinUseMatrix[i][1] * 12 + M01];
            phd_mxptr[M02] = lara_matrices[SkinUseMatrix[i][1] * 12 + M02];
            phd_mxptr[M03] = lara_matrices[SkinUseMatrix[i][1] * 12 + M03];
            phd_mxptr[M10] = lara_matrices[SkinUseMatrix[i][1] * 12 + M10];
            phd_mxptr[M11] = lara_matrices[SkinUseMatrix[i][1] * 12 + M11];
            phd_mxptr[M12] = lara_matrices[SkinUseMatrix[i][1] * 12 + M12];
            phd_mxptr[M13] = lara_matrices[SkinUseMatrix[i][1] * 12 + M13];
            phd_mxptr[M20] = lara_matrices[SkinUseMatrix[i][1] * 12 + M20];
            phd_mxptr[M21] = lara_matrices[SkinUseMatrix[i][1] * 12 + M21];
            phd_mxptr[M22] = lara_matrices[SkinUseMatrix[i][1] * 12 + M22];
            phd_mxptr[M23] = lara_matrices[SkinUseMatrix[i][1] * 12 + M23];
            phd_PushMatrix();
            v0.x = lara_matrices[12 * SkinUseMatrix[i][0] + M01];
            v0.y = lara_matrices[12 * SkinUseMatrix[i][0] + M11];
            v0.z = lara_matrices[12 * SkinUseMatrix[i][0] + M21];
            v1.x = lara_matrices[12 * SkinUseMatrix[i][1] + M01];
            v1.y = lara_matrices[12 * SkinUseMatrix[i][1] + M11];
            v1.z = lara_matrices[12 * SkinUseMatrix[i][1] + M21];
            cos = ((v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z)) >> W2V_SHIFT;
            sin = phd_sqrt(16777216 - SQUARE(cos));
            if (i == 1 || i == 4)
                xRot = -phd_atan(cos, sin);
            else
                xRot = phd_atan(cos, sin);
            phd_RotX(-(sint16)xRot >> 1);
            phd_PutPolygons(*meshpp, -1);
            phd_PopMatrix();
        } else
            phd_PutPolygons(*meshpp, -1);
        meshpp += 2;
    }
    in_joints = 0;
    bLaraUnderWater = LaraNodeUnderwater[0] ? 0 : -1;
    if (!(gfLevelFlags & GF_YOUNGLARA)) {
        obj = &objects[lara.holster];
        meshpp = &meshes[obj->mesh_index];
        meshpp += 8;
        phd_mxptr[M00] = lara_matrices[1 * 12 + M00];
        phd_mxptr[M01] = lara_matrices[1 * 12 + M01];
        phd_mxptr[M02] = lara_matrices[1 * 12 + M02];
        phd_mxptr[M03] = lara_matrices[1 * 12 + M03];
        phd_mxptr[M10] = lara_matrices[1 * 12 + M10];
        phd_mxptr[M11] = lara_matrices[1 * 12 + M11];
        phd_mxptr[M12] = lara_matrices[1 * 12 + M12];
        phd_mxptr[M13] = lara_matrices[1 * 12 + M13];
        phd_mxptr[M20] = lara_matrices[1 * 12 + M20];
        phd_mxptr[M21] = lara_matrices[1 * 12 + M21];
        phd_mxptr[M22] = lara_matrices[1 * 12 + M22];
        phd_mxptr[M23] = lara_matrices[1 * 12 + M23];
        phd_PutPolygons(*meshpp, -1);
        meshpp += 8;
        phd_mxptr[M00] = lara_matrices[4 * 12 + M00];
        phd_mxptr[M01] = lara_matrices[4 * 12 + M01];
        phd_mxptr[M02] = lara_matrices[4 * 12 + M02];
        phd_mxptr[M03] = lara_matrices[4 * 12 + M03];
        phd_mxptr[M10] = lara_matrices[4 * 12 + M10];
        phd_mxptr[M11] = lara_matrices[4 * 12 + M11];
        phd_mxptr[M12] = lara_matrices[4 * 12 + M12];
        phd_mxptr[M13] = lara_matrices[4 * 12 + M13];
        phd_mxptr[M20] = lara_matrices[4 * 12 + M20];
        phd_mxptr[M21] = lara_matrices[4 * 12 + M21];
        phd_mxptr[M22] = lara_matrices[4 * 12 + M22];
        phd_mxptr[M23] = lara_matrices[4 * 12 + M23];
        phd_PutPolygons(*meshpp, -1);
        if (lara.back_gun) {
            sint16 *rot;

            phd_PushMatrix();
            phd_mxptr[M00] = lara_matrices[84 + M00];
            phd_mxptr[M01] = lara_matrices[84 + M01];
            phd_mxptr[M02] = lara_matrices[84 + M02];
            phd_mxptr[M03] = lara_matrices[84 + M03];
            phd_mxptr[M10] = lara_matrices[84 + M10];
            phd_mxptr[M11] = lara_matrices[84 + M11];
            phd_mxptr[M12] = lara_matrices[84 + M12];
            phd_mxptr[M13] = lara_matrices[84 + M13];
            phd_mxptr[M20] = lara_matrices[84 + M20];
            phd_mxptr[M21] = lara_matrices[84 + M21];
            phd_mxptr[M22] = lara_matrices[84 + M22];
            phd_mxptr[M23] = lara_matrices[84 + M23];
            bone = &bones[objects[lara.back_gun].bone_index];
            phd_TranslateRel(bone[53], bone[54], bone[55]);
            rot = objects[lara.back_gun].frame_base + 9;
            gar_RotYXZsuperpack(&rot, 14);
            phd_PutPolygons(meshes[objects[lara.back_gun].mesh_index + 28], -1);
            phd_PopMatrix();
        }
    }
    bLaraUnderWater = 0;
    phd_PopMatrix();
    phd_left = left;
    phd_right = right;
    phd_top = top;
    phd_bottom = bottom;
    GlobalAlpha = 0xFF000000;
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
