#include <string.h>

#include "draw.h"
#include "specific/3dmath.h"
#include "specific/output.h"
#include "camera.h"
#include "delstuff.h"
#include "deltapak.h"
#include "health.h"
#include "anim.h"

#define S_LongMemCpy(d, s, n) memcpy(d, s, (n) * sizeof(sint32))

sint32 DrawPhaseGame(void)
{
    CalcLaraMatrices(0);
    phd_PushUnitMatrix();
    CalcLaraMatrices(1);
    phd_PopMatrix();
    if (GLOBAL_playing_cutseq)
        frigup_lara();
    SetLaraUnderwaterNodes();
    DrawRooms(camera.pos.room_number);
    DrawGameInfo(1);
    S_OutputPolyList();
    camera.number_frames = S_DumpScreen();
    S_AnimateTextures(camera.number_frames);
    return camera.number_frames;
}

void S_InsertRoom(sint16 room_number)
{
    ROOM_INFO *r;

    current_room = room_number;
    r = &room[room_number];
    phd_TranslateAbs(0, 0, 0);
    phd_left = r->left;
    phd_right = r->right;
    phd_top = r->top;
    phd_bottom = r->bottom;
    _InsertRoom(r, 1);
}

void GetRoomBounds(void)
{
    ROOM_INFO *r;
    sint16 *door;
    sint32 rn, drn;

    while (room_list_start != room_list_end) {
        rn = draw_room_list[room_list_start % 128];
        room_list_start++;
        r = &room[rn];
        r->bound_active -= 2;
        if (r->test_left < r->left)
            r->left = r->test_left;
        if (r->test_top < r->top)
            r->top = r->test_top;
        if (r->test_right > r->right)
            r->right = r->test_right;
        if (r->test_bottom > r->bottom)
            r->bottom = r->test_bottom;
        if (!(r->bound_active & 1)) {
            draw_rooms[number_draw_rooms++] = (sint16)rn;
            r->bound_active |= 1;
            if (r->flags & ROOM_OUTSIDE)
                outside = ROOM_OUTSIDE;
        }
        if (r->flags & ROOM_OUTSIDE) {
            if (r->left < outside_left)
                outside_left = r->left;
            if (r->right > outside_right)
                outside_right = r->right;
            if (r->top < outside_top)
                outside_top = r->top;
            if (r->bottom > outside_bottom)
                outside_bottom = r->bottom;
        }
        phd_PushMatrix();
        phd_TranslateAbs(r->x, r->y, r->z);
        door = r->door;
        if (door) {
            for (drn = *door++; drn > 0; drn--) {
                rn = *door++;
                if (door[0] * (r->x + door[3] - w2v_matrix[M03]) + door[1] * (r->y + door[4] - w2v_matrix[M13]) + door[2] * (r->z + door[5] - w2v_matrix[M23]) < 0)
                    SetRoomBounds(door, rn, r);
                door += 15;
            }
        }
        phd_PopMatrix();
    }
}

void InitInterpolate(sint32 frac, sint32 rate)
{
    IM_frac = frac;
    IM_rate = rate;
    IMptr = IMstack;
    S_LongMemCpy(IMstack, phd_mxptr, 12);
}

void phd_PopMatrix_I(void)
{
    phd_PopMatrix();
    IMptr -= 12;
}

void phd_PushMatrix_I(void)
{
    phd_PushMatrix();
    S_LongMemCpy(IMptr + 12, IMptr, 12);
    IMptr += 12;
}

void phd_RotY_I(sint16 ang)
{
    sint32 *oldm;

    phd_RotY(ang);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotY(ang);
    phd_mxptr = oldm;
}

void phd_RotX_I(sint16 ang)
{
    sint32 *oldm;

    phd_RotX(ang);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotX(ang);
    phd_mxptr = oldm;
}

void phd_RotZ_I(sint16 ang)
{
    sint32 *oldm;

    phd_RotZ(ang);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotZ(ang);
    phd_mxptr = oldm;
}

void phd_TranslateRel_I(sint32 x, sint32 y, sint32 z)
{
    sint32 *oldm;

    phd_TranslateRel(x, y, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_TranslateRel(x, y, z);
    phd_mxptr = oldm;
}

void phd_TranslateRel_ID(sint32 x, sint32 y, sint32 z, sint32 x2, sint32 y2, sint32 z2)
{
    sint32 *oldm;

    phd_TranslateRel(x, y, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_TranslateRel(x2, y2, z2);
    phd_mxptr = oldm;
}

void phd_RotYXZ_I(sint16 y, sint16 x, sint16 z)
{
    sint32 *oldm;

    phd_RotYXZ(y, x, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotYXZ(y, x, z);
    phd_mxptr = oldm;
}

void gar_RotYXZsuperpack_I(sint16 **pprot1, sint16 **pprot2, sint32 skip)
{
    sint32 *oldm;

    gar_RotYXZsuperpack(pprot1, skip);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    gar_RotYXZsuperpack(pprot2, skip);
    phd_mxptr = oldm;
}

void gar_RotYXZsuperpack(sint16 **pprot, sint32 skip)
{
    uint16 *prot;
    sint32 packed;

    while (skip) {
        prot = *pprot;
        if (*prot & (3 << 14))
            (*pprot) += 1;
        else
            (*pprot) += 2;
        skip--;
    }
    prot = *pprot;
    switch (*prot >> 14) {
    case 0:
        packed = (*prot << 16) + *(prot + 1);
        phd_RotYXZpack(packed);
        (*pprot) += 2;
        return;
    case 1:
        phd_RotX((sint16)((*prot & 0xFFF) << 4));
        break;
    case 2:
        phd_RotY((sint16)((*prot & 0xFFF) << 4));
        break;
    default:
        phd_RotZ((sint16)((*prot & 0xFFF) << 4));
        break;
    }
    (*pprot) += 1;
}

void phd_PutPolygons_I(sint16 *ptr, sint32 clip)
{
    phd_PushMatrix();
    InterpolateMatrix();
    phd_PutPolygons(ptr, clip);
    phd_PopMatrix();
}

sint32 GetFrames(ITEM_INFO *item, sint16 *frm[], sint32 *rate)
{
    ANIM_STRUCT *anim;
    sint32 frame;
    sint32 first, second;
    sint32 frame_size;
    sint32 interp, rat;

    frame = item->frame_number;
    anim = &anims[item->anim_number];
    frm[0] = frm[1] = anim->frame_ptr;
    rat = *rate = anim->interpolation & 0xff;
    frame_size = anim->interpolation >> 8;
    frame -= anim->frame_base;
    first = frame / rat;
    interp = frame % rat;
    frm[0] += first * frame_size;
    frm[1] = frm[0] + frame_size;
    if (interp == 0)
        return 0;
    second = (first + 1) * rat;
    if (second > anim->frame_end)
        *rate = anim->frame_end - second + rat;
    return interp;
}

sint16 *GetBoundsAccurate(ITEM_INFO *item)
{
    sint32 i, frac, rate;
    sint16 *bptr, *frmptr[2];

    frac = GetFrames(item, frmptr, &rate);
    if (frac == 0)
        return frmptr[0];
    bptr = interpolated_bounds;
    for (i = 0; i < 6; i++, bptr++, frmptr[0]++, frmptr[1]++)
        *bptr = *frmptr[0] + ((*frmptr[1] - *frmptr[0]) * frac) / rate;
    return interpolated_bounds;
}

sint16 *GetBestFrame(ITEM_INFO *item)
{
    sint32 frac;
    sint32 rate;
    sint16 *frmptr[2];

    frac = GetFrames(item, frmptr, &rate);
    if (frac <= (rate >> 1))
        return frmptr[0];
    else
        return frmptr[1];
}
