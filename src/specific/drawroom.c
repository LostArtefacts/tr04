#include "drawroom.h"
#include "polyinsert.h"
#include "3dmath.h"
#include "gamemain.h"
#include "../effect2.h"

void ProjectVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip)
{
    float zv, x;
    sint32 i;
    sint16 clip_distance;

    for (i = 0; i < nVerts; i++) {
        v->tu = v->sx;
        v->tv = v->sy;
        clip_distance = 0;
        if (v->sz >= f_mznear) {
            zv = f_mpersp / v->sz;
            if (v->sz > FogEnd) {
                clip_distance = 16;
                v->sz = f_zfar;
            }
            v->sx = zv * v->sx + f_centerx;
            v->sy = zv * v->sy + f_centery;
            v->rhw = f_moneopersp * zv;
            if (bWaterEffect) {
                x = v->sx;
                v->sx += vert_wibble_table[((wibble + (sint32)v->sy) >> 3) & 0x1F];
                v->sy += vert_wibble_table[((wibble + (sint32)x) >> 3) & 0x1F];
            }
            if (v->sx < clip_left)
                clip_distance++;
            else if (v->sx > clip_right)
                clip_distance += 2;
            if (v->sy < clip_top)
                clip_distance += 4;
            else if (v->sy > clip_bottom)
                clip_distance += 8;
        } else
            clip_distance = -128;
        *clip++ = clip_distance;
        v++;
    }
}

void PrelightVertsMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r)
{
    sint32 *prelight;
    sint32 p, c, i;

    if (bWaterEffect && !(r->flags & ROOM_UNDERWATER))
        prelight = r->prelightwater;
    else
        prelight = r->prelight;
    for (i = 0; i < r->nWaterVerts; i++) {
        p = r->prelight[i];
        c = v->color;
        __asm {
            movd mm0, p
            movd mm1, c
            paddusb mm1, mm0
            mov edx, v
            add edx, 0x10
            movd [edx], mm1
        }
        v->specular &= 0xFF000000;
        v++;
    }
    for (i = r->nWaterVerts; i < r->nVerts; i++) {
        AddPrelitMMX(prelight[i], &v->color);
        v++;
    }
    __asm {
        emms
    }
}

void PrelightVertsNonMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r)
{
    sint32 *prelight;
    sint32 p, c, cR, cG, cB, i;

    if (bWaterEffect && !(r->flags & ROOM_UNDERWATER))
        prelight = r->prelightwater;
    else
        prelight = r->prelight;
    for (i = 0; i < r->nWaterVerts; i++) {
        p = r->prelight[i];
        c = v->color;
        cR = (c & 0xFF0000) + (p & 0xFF0000);
        cG = (c & 0xFF00) + (p & 0xFF00);
        cB = (c & 0xFF) + (p & 0xFF);
        if (cR > 0xFF0000)
            cR = 0xFF0000;
        if (cG > 0xFF00)
            cG = 0xFF00;
        if (cB > 0xFF)
            cB = 0xFF;
        v->color = (v->color & 0xFF000000) | cB | cG | cR;
        v->specular &= 0xFF000000;
        v++;
    }
    for (i = r->nWaterVerts; i < r->nVerts; i++) {
        p = prelight[i];
        c = v->color;
        cR = (c & 0xFF0000) + (p & 0xFF0000);
        cG = (c & 0xFF00) + (p & 0xFF00);
        cB = (c & 0xFF) + (p & 0xFF);
        if (cR > 0xFF0000)
            cR = 0xFF0000;
        if (cG > 0xFF00)
            cG = 0xFF00;
        if (cB > 0xFF)
            cB = 0xFF;
        v->color = (c & 0xFF000000) | cB | cG | cR;
        CalcColorSplit(v->color, &v->color);
        v++;
    }
}

void InitBuckets(void)
{
    sint32 n;

    for (n = 0; n < 20; n++) {
        Bucket[n].tpage = -1;
        Bucket[n].nVtx = 0;
    }
}

void FindBucket(sint32 tpage, D3DTLBUMPVERTEX **Vpp, sint32 **nVtxpp)
{
    TEXTUREBUCKET *bucket;
    sint32 i, nVtx, biggest;

    bucket = Bucket;
    for (i = 0; i < 20; i++, bucket++) {
        if (bucket->tpage == tpage && bucket->nVtx < 512) {
            *Vpp = &bucket->vtx[bucket->nVtx];
            *nVtxpp = &bucket->nVtx;
            return;
        }
        if (bucket->nVtx > 512) {
            DrawBucket(&Bucket[i]);
            bucket->tpage = tpage;
            bucket->nVtx = 0;
            *Vpp = bucket->vtx;
            *nVtxpp = &bucket->nVtx;
            return;
        }
    }
    biggest = 0;
    nVtx = 0;
    bucket = Bucket;
    for (i = 0; i < 20; i++, bucket++) {
        if (bucket->tpage == -1) {
            bucket->tpage = tpage;
            *Vpp = bucket->vtx;
            *nVtxpp = &bucket->nVtx;
            return;
        }
        if (bucket->nVtx > nVtx) {
            nVtx = bucket->nVtx;
            biggest = i;
        }
    }
    DrawBucket(&Bucket[biggest]);
    Bucket[biggest].tpage = tpage;
    Bucket[biggest].nVtx = 0;
    *Vpp = Bucket[biggest].vtx;
    *nVtxpp = &Bucket[biggest].nVtx;
}

void CalcTriFaceNormal(D3DVECTOR *p1, D3DVECTOR *p2, D3DVECTOR *p3, D3DVECTOR *N)
{
    float ux, uy, uz, vx, vy, vz;

    ux = p1->x - p2->x;
    uy = p1->y - p2->y;
    uz = p1->z - p2->z;
    vx = p3->x - p2->x;
    vy = p3->y - p2->y;
    vz = p3->z - p2->z;
    N->x = vz * uy - vy * uz;
    N->y = vx * uz - vz * ux;
    N->z = vy * ux - vx * uy;
}
