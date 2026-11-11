#include <string.h>

#include "sphere.h"
#include "anim.h"
#include "draw.h"
#include "lara.h"
#include "specific/3dmath.h"

sint32 TestCollision(ITEM_INFO *item, ITEM_INFO *l)
{
    SPHERE *laraSpheres;
    sint32 nItemSpheres, nLaraSpheres, ir, lr, i, j, x, y, z, dx, dy, dz;
    SPHERE *itemSpheres;
    uint32 touch_bits;

    touch_bits = 0;
    nItemSpheres = GetSpheres(item, Slist, 1);
    if (l != lara_item)
        GotLaraSpheres = 0;
    if (!GotLaraSpheres) {
        nLaraSpheres = GetSpheres(l, LaraSpheres, 1);
        NumLaraSpheres = nLaraSpheres;
        if (l == lara_item)
            GotLaraSpheres = 1;
    } else {
        nLaraSpheres = NumLaraSpheres;
    }
    for (i = 0; i < nItemSpheres; i++) {
        itemSpheres = &Slist[i];
        ir = itemSpheres->r;
        if (ir > 0) {
            x = itemSpheres->x;
            y = itemSpheres->y;
            z = itemSpheres->z;
            for (j = 0; j < nLaraSpheres; j++) {
                laraSpheres = &LaraSpheres[j];
                lr = laraSpheres->r;
                if (lr > 0) {
                    dx = laraSpheres->x - x;
                    dy = laraSpheres->y - y;
                    dz = laraSpheres->z - z;
                    lr += ir;
                    if (dx * dx + dy * dy + dz * dz < lr * lr) {
                        touch_bits |= 1 << i;
                        break;
                    }
                }
            }
        }
    }
    item->touch_bits = touch_bits;
    return touch_bits;
}

sint32 GetSpheres(ITEM_INFO *item, SPHERE *ptr, sint32 WorldSpace)
{
    OBJECT_INFO *obj;
    sint16 **meshpp;
    sint32 *bone;
    sint16 *meshp;
    sint16 *frame;
    sint16 *rot;
    sint16 *extra_rot;
    sint32 x, y, z, poppush, i;

    if (!item)
        return 0;
    if (WorldSpace & 1) {
        x = item->pos.x_pos;
        y = item->pos.y_pos;
        z = item->pos.z_pos;
        phd_PushUnitMatrix();
        phd_mxptr[M03] = 0;
        phd_mxptr[M13] = 0;
        phd_mxptr[M23] = 0;
    } else {
        z = 0;
        y = 0;
        x = 0;
        phd_PushMatrix();
        phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    }
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    frame = GetBestFrame(item);
    phd_TranslateRel(frame[6], frame[7], frame[8]);
    rot = frame + 9;
    gar_RotYXZsuperpack(&rot, 0);
    obj = &objects[item->object_number];
    meshpp = &meshes[obj->mesh_index];
    meshp = *meshpp;
    meshpp += 2;
    bone = obj->bone_index + bones;
    phd_PushMatrix();
    if (!(WorldSpace & 2))
        phd_TranslateRel(meshp[0], meshp[1], meshp[2]);
    ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
    ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
    ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
    ptr->r = meshp[3];
    ptr++;
    phd_PopMatrix();
    extra_rot = (sint16 *)item->data;
    for (i = obj->nmeshes - 1; i > 0; i--) {
        poppush = *bone++;
        if (poppush & 1)
            phd_PopMatrix();
        if (poppush & 2)
            phd_PushMatrix();
        phd_TranslateRel(bone[0], bone[1], bone[2]);
        gar_RotYXZsuperpack(&rot, 0);
        if (poppush & 0x1C && extra_rot) {
            if (poppush & 8)
                phd_RotY(*extra_rot++);
            if (poppush & 4)
                phd_RotX(*extra_rot++);
            if (poppush & 0x10)
                phd_RotZ(*extra_rot++);
        }
        meshp = *meshpp;
        phd_PushMatrix();
        if (!(WorldSpace & 2))
            phd_TranslateRel(meshp[0], meshp[1], meshp[2]);
        ptr->x = x + (phd_mxptr[M03] >> W2V_SHIFT);
        ptr->y = y + (phd_mxptr[M13] >> W2V_SHIFT);
        ptr->z = z + (phd_mxptr[M23] >> W2V_SHIFT);
        ptr->r = meshp[3];
        ptr++;
        phd_PopMatrix();
        bone += 3;
        meshpp += 2;
    }
    phd_PopMatrix();
    return obj->nmeshes;
}

void GetJointAbsPosition(ITEM_INFO *item, PHD_VECTOR *pos, sint32 joint)
{
    OBJECT_INFO *obj;
    sint32 *mx;
    sint32 *imx;
    sint32 *bone;
    sint16 *frm[2];
    sint16 *extra_rotation;
    sint16 *rot;
    sint16 *rot2;
    sint32 frac, rate, poppush;

    mx = phd_mxptr;
    imx = IMptr;
    obj = &objects[item->object_number];
    frac = GetFrames(item, frm, &rate);
    phd_PushUnitMatrix();
    phd_mxptr[M03] = 0;
    phd_mxptr[M13] = 0;
    phd_mxptr[M23] = 0;
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    extra_rotation = (sint16 *)item->data;
    if (!extra_rotation)
        extra_rotation = no_rotation;
    bone = &bones[obj->bone_index];
    if (!frac) {
        phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
        rot = frm[0] + 9;
        gar_RotYXZsuperpack(&rot, 0);
        for (; joint > 0; joint--) {
            poppush = *bone;
            if (poppush & 1)
                phd_PopMatrix();
            if (poppush & 2)
                phd_PushMatrix();
            phd_TranslateRel(bone[1], bone[2], bone[3]);
            gar_RotYXZsuperpack(&rot, 0);
            if (poppush & 0x1C) {
                if (poppush & 8)
                    phd_RotY(*extra_rotation++);
                if (poppush & 4)
                    phd_RotX(*extra_rotation++);
                if (poppush & 0x10)
                    phd_RotZ(*extra_rotation++);
            }
            bone += 4;
        }
        phd_TranslateRel(pos->x, pos->y, pos->z);
    } else {
        InitInterpolate2(frac, rate);
        rot = frm[0] + 9;
        rot2 = frm[1] + 9;
        phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
        gar_RotYXZsuperpack_I(&rot, &rot2, 0);
        for (; joint > 0; joint--) {
            poppush = *bone;
            if (poppush & 1)
                phd_PopMatrix_I();
            if (poppush & 2)
                phd_PushMatrix_I();
            phd_TranslateRel_I(bone[1], bone[2], bone[3]);
            gar_RotYXZsuperpack_I(&rot, &rot2, 0);
            if (poppush & 0x1C) {
                if (poppush & 8)
                    phd_RotY_I(*extra_rotation++);
                if (poppush & 4)
                    phd_RotX_I(*extra_rotation++);
                if (poppush & 0x10)
                    phd_RotZ_I(*extra_rotation++);
            }
            bone += 4;
        }
        phd_TranslateRel_I(pos->x, pos->y, pos->z);
        InterpolateMatrix();
    }
    pos->x = item->pos.x_pos + (phd_mxptr[M03] >> W2V_SHIFT);
    pos->y = item->pos.y_pos + (phd_mxptr[M13] >> W2V_SHIFT);
    pos->z = item->pos.z_pos + (phd_mxptr[M23] >> W2V_SHIFT);
    phd_mxptr = mx;
    IMptr = imx;
}

void InitInterpolate2(sint32 frac, sint32 rate)
{
    IM_frac = frac;
    IM_rate = rate;
    IMptr = &IMstack[384];
    memcpy(IMptr, phd_mxptr, 48);
}
