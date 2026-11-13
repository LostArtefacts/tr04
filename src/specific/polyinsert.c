#include <stdlib.h>
#include "polyinsert.h"
#include "winmain.h"
#include "../camera.h"
#include "../tomb4fx.h"
#include "function_stubs.h"
#include "drawroom.h"
#include "3dmath.h"
#include "output.h"
#include "../text.h"

void DoSort(sint32 left, sint32 right, SORTLIST **list)
{
    SORTLIST *swap;
    float z;
    sint32 l, r;

    l = left;
    r = right;
    z = list[(left + right) / 2]->zVal;
    do {
        while (list[l]->zVal > z && l < right) {
            l++;
        }
        while (z > list[r]->zVal && r > left) {
            r--;
        }
        if (l <= r) {
            swap = list[l];
            list[l] = list[r];
            list[r] = swap;
            l++;
            r--;
        }
    } while (l <= r);
    if (left < r) {
        DoSort(left, r, list);
    }
    if (l < right) {
        DoSort(l, right, list);
    }
}

void SortPolyList(sint32 count, SORTLIST **list)
{
    sint32 i;

    if (!count) {
        return;
    }
    for (i = 0; i < count; i++) {
        list[i]->zVal -= (float)i * 0.1F;
    }
    DoSort(0, count - 1, list);
}

void InitialiseSortList(void)
{
    pSortBuffer = SortBuffer;
    pSortList = SortList;
    SortCount = 0;
}

void j_nullsub_10(void)
{
    nullsub_10();
}

void nullsub_10(void)
{
}

void ClearFXFogBulbs(void)
{
    sint32 i;

    for (i = 0; i < 5; i++) {
        FXFogBulbs[i].active = 0;
    }
    NumFXFogBulbs = 0;
}

void ControlFXBulb(FOGBULB_STRUCT *FogBulb)
{
    if (FogBulb->timer > 0) {
        FogBulb->timer--;
        FogBulb->rad += (GetRandomDraw() - 0x100) & 0x1FF;
        if (FogBulb->rad > FogBulb->FXRad) {
            FogBulb->rad = (float)(FogBulb->FXRad + (GetRandomDraw() & 0xFF));
        }
    } else {
        FogBulb->timer--;
        if (FogBulb->timer < -30) {
            FogBulb->timer = 0;
        }
        FogBulb->rad -= 255;
        if (FogBulb->rad < 0) {
            FogBulb->rad = 0;
            FogBulb->active = 0;
        }
    }
    TriggerFlashSmoke((sint32)FogBulb->WorldPos.x, (sint32)FogBulb->WorldPos.y, (sint32)FogBulb->WorldPos.z, (sint16)FogBulb->room_number);
    TriggerFlashSmoke((sint32)FogBulb->WorldPos.x, (sint32)FogBulb->WorldPos.y, (sint32)FogBulb->WorldPos.z, (sint16)FogBulb->room_number);
    FogBulb->sqrad = FogBulb->rad * FogBulb->rad;
    FogBulb->inv_sqrad = 1.0F / FogBulb->sqrad;
}

void CreateFXBulbs(void)
{
    FOGBULB_STRUCT *FogBulb;
    sint32 i;

    NumFXFogBulbs = 0;
    for (i = 0; i < 5; i++) {
        FogBulb = &FXFogBulbs[i];
        if (FogBulb->active) {
            CreateFogPos(FogBulb);
            ControlFXBulb(FogBulb);
            NumFXFogBulbs++;
        }
    }
}

void TriggerFXFogBulb(sint32 x, sint32 y, sint32 z, sint32 FXRad, sint32 density, sint32 r, sint32 g, sint32 b, sint32 room_number)
{
    sint32 num;

    for (num = 0; num < 5; num++) {
        if (!FXFogBulbs[num].active) {
            FXFogBulbs[num].inRange = 1;
            FXFogBulbs[num].density = density;
            FXFogBulbs[num].WorldPos.x = (float)x;
            FXFogBulbs[num].WorldPos.y = (float)y;
            FXFogBulbs[num].WorldPos.z = (float)z;
            FXFogBulbs[num].rad = 0;
            FXFogBulbs[num].sqrad = 0;
            FXFogBulbs[num].inv_sqrad = 1 / FXFogBulbs[num].sqrad;
            FXFogBulbs[num].timer = 50;
            FXFogBulbs[num].active = 1;
            FXFogBulbs[num].FXRad = FXRad;
            FXFogBulbs[num].r = r;
            FXFogBulbs[num].g = g;
            FXFogBulbs[num].b = b;
            FXFogBulbs[num].room_number = room_number;
            CreateFogPos(&FXFogBulbs[num]);
            NumFXFogBulbs++;
            return;
        }
    }
}

sint32 IsVolumetric(void)
{
    return App.Volumetric;
}

int DistCompare(const void *a, const void *b)
{
    FOGBULB_STRUCT *bulbA;
    FOGBULB_STRUCT *bulbB;
    FVECTOR dA;
    FVECTOR dB;
    float distA, distB;

    bulbA = (FOGBULB_STRUCT *)a;
    bulbB = (FOGBULB_STRUCT *)b;
    dA.x = bulbA->WorldPos.x - camera.pos.x;
    dA.y = bulbA->WorldPos.y - camera.pos.y;
    dA.z = bulbA->WorldPos.z - camera.pos.z;
    dB.x = bulbB->WorldPos.x - camera.pos.x;
    dB.y = bulbB->WorldPos.y - camera.pos.y;
    dB.z = bulbB->WorldPos.z - camera.pos.z;
    distA = dA.x * dA.x + dA.y * dA.y + dA.z * dA.z;
    distB = dB.x * dB.x + dB.y * dB.y + dB.z * dB.z;
    bulbA->dist = distA;
    bulbB->dist = distB;
    if (distA > distB) {
        return 1;
    }
    if (distA < distB) {
        return -1;
    }
    return 0;
}

void InitialiseFogBulbs(void)
{
    FOGBULB_STRUCT *FogBulb;
    sint32 i;

    NumActiveFogBulbs = 0;
    qsort(&FogBulbs, NumLevelFogBulbs, sizeof(FOGBULB_STRUCT), DistCompare);
    for (i = 0; i < NumLevelFogBulbs; i++) {
        FogBulb = &FogBulbs[i];
        CreateFogPos(FogBulb);
        if (FogBulb->inRange) {
            ActiveFogBulbs[NumActiveFogBulbs] = FogBulb;
            NumActiveFogBulbs++;
            if (NumActiveFogBulbs >= 5) {
                return;
            }
        }
    }
}

void OmniEffect(D3DTLVERTEX *v)
{
    FOGBULB_STRUCT *FogBulb;
    FVECTOR pos, dP, dV, d;
    float val, val2, inv;
    sint32 i, r, g, b, lVal;

    inv = 1.0F / v->sz;
    for (i = 0; i < 5; i++) {
        FogBulb = &FXFogBulbs[i];
        if (FogBulb->active && FogBulb->inRange) {
            pos.x = v->tu;
            pos.y = v->tv;
            pos.z = v->sz;
            if (FogBulb->pos.z < pos.z) {
                val = FogBulb->dist * inv;
                pos.x *= val;
                pos.y *= val;
                pos.z *= val;
            }
            dP.x = pos.x - FogBulb->pos.x;
            dP.y = pos.y - FogBulb->pos.y;
            dP.z = pos.z - FogBulb->pos.z;
            d = dP;
            dV.x = FogBulb->vec.x - FogBulb->pos.x;
            dV.y = FogBulb->vec.y - FogBulb->pos.y;
            dV.z = FogBulb->vec.z - FogBulb->pos.z;
            val = dV.z * dV.z + dV.y * dV.y + dV.x * dV.x;
            if (val != 0) {
                val2 = (dV.z * d.z + dV.y * d.y + dV.x * d.x) / val;
                if (val2 >= -1.0F) {
                    if (val2 < 0)
                        val = dP.z * dP.z + dP.y * dP.y + dP.x * dP.x;
                    else {
                        dP.x = d.x - val2 * dV.x;
                        dP.y = d.y - val2 * dV.y;
                        dP.z = d.z - val2 * dV.z;
                        val = dP.z * dP.z + dP.y * dP.y + dP.x * dP.x;
                    }
                    if (val != 0 && val < FogBulb->sqrad) {
                        val2 = FogBulb->density * FogBulb->inv_sqrad * val + 12582912.0F;
                        lVal = *(sint32 *)&val2 & 0x3FFFFF;
                        r = CLRR(v->specular) + (((FogBulb->density - lVal) * FogBulb->r) >> 8);
                        g = CLRG(v->specular) + (((FogBulb->density - lVal) * FogBulb->g) >> 8);
                        b = CLRB(v->specular) + (((FogBulb->density - lVal) * FogBulb->b) >> 8);
                        if (r > 255)
                            r = 255;
                        if (g > 255)
                            g = 255;
                        if (b > 255)
                            b = 255;
                        v->specular = b | v->specular & 0xFF000000 | ((g | ((r | (r << 8)) << 8)) << 8);
                    }
                }
            }
        }
    }
}

void SubdivideEdge(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v, sint16 *c, float tu1, float tv1, float tu2, float tv2, float *tu, float *tv)
{
    float zv;
    sint16 cf, r0, g0, b0, a0, r1, g1, b1, a1;

    cf = 0;
    *tu = (tu1 + tu2) * 0.5F;
    *tv = (tv1 + tv2) * 0.5F;
    v->sx = (v0->tu + v1->tu) * 0.5F;
    v->sy = (v0->tv + v1->tv) * 0.5F;
    v->sz = (v0->sz + v1->sz) * 0.5F;
    v->tu = v->sx;
    v->tv = v->sy;
    if (v->sz >= f_mznear) {
        zv = f_mpersp / v->sz;
        v->sx = zv * v->sx + f_centerx;
        v->sy = zv * v->sy + f_centery;
        v->rhw = f_moneopersp * zv;
        if (v->sx < f_left)
            cf++;
        else if (v->sx > f_right)
            cf += 2;
        if (v->sy < f_top)
            cf += 4;
        else if (v->sy > f_bottom)
            cf += 8;
    } else
        cf = -128;
    *c = cf;
    a0 = (v0->color >> 24) & 0xFF;
    r0 = (v0->color >> 16) & 0xFF;
    g0 = (v0->color >> 8) & 0xFF;
    b0 = v0->color & 0xFF;
    a1 = (v1->color >> 24) & 0xFF;
    r1 = (v1->color >> 16) & 0xFF;
    g1 = (v1->color >> 8) & 0xFF;
    b1 = v1->color & 0xFF;
    a1 = (a0 + a1) >> 1;
    r1 = (r0 + r1) >> 1;
    g1 = (g0 + g1) >> 1;
    b1 = (b0 + b1) >> 1;
    v->color = RGBA(r1, g1, b1, a1);
    a0 = (v0->specular >> 24) & 0xFF;
    r0 = (v0->specular >> 16) & 0xFF;
    g0 = (v0->specular >> 8) & 0xFF;
    b0 = v0->specular & 0xFF;
    a1 = (v1->specular >> 24) & 0xFF;
    r1 = (v1->specular >> 16) & 0xFF;
    g1 = (v1->specular >> 8) & 0xFF;
    b1 = v1->specular & 0xFF;
    a1 = (a0 + a1) >> 1;
    r1 = (r0 + r1) >> 1;
    g1 = (g0 + g1) >> 1;
    b1 = (b0 + b1) >> 1;
    v->specular = RGBA(r1, g1, b1, a1);
}

void SubdivideTri(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2, TEXTURESTRUCT *tex, sint32 double_sided, sint32 steps, sint16 *c)
{
    D3DTLVERTEX v[3];
    D3DTLVERTEX vb[3];
    TEXTURESTRUCT tex2;
    float uv[6];
    sint16 bclip[4];
    sint16 aclip[3];
    sint16 c0, c1, c2;

    if (steps) {
        tex2.drawtype = tex->drawtype;
        tex2.tpage = tex->tpage;
        tex2.flag = tex->flag;
        SubdivideEdge(v0, v1, &v[0], &aclip[0], tex->u1, tex->v1, tex->u2, tex->v2, &uv[0], &uv[1]);
        SubdivideEdge(v1, v2, &v[1], &aclip[1], tex->u2, tex->v2, tex->u3, tex->v3, &uv[2], &uv[3]);
        SubdivideEdge(v2, v0, &v[2], &aclip[2], tex->u3, tex->v3, tex->u1, tex->v1, &uv[4], &uv[5]);
        tex2.u1 = tex->u1;
        tex2.v1 = tex->v1;
        tex2.u2 = uv[0];
        tex2.v2 = uv[1];
        tex2.u3 = uv[4];
        tex2.v3 = uv[5];
        bclip[0] = c[0];
        bclip[1] = aclip[0];
        bclip[2] = aclip[2];
        SubdivideTri(v0, &v[0], &v[2], &tex2, double_sided, steps - 1, bclip);
        tex2.u1 = uv[0];
        tex2.v1 = uv[1];
        tex2.u2 = tex->u2;
        tex2.v2 = tex->v2;
        tex2.u3 = uv[2];
        tex2.v3 = uv[3];
        bclip[0] = aclip[0];
        bclip[1] = c[1];
        bclip[2] = aclip[1];
        SubdivideTri(&v[0], v1, &v[1], &tex2, double_sided, steps - 1, bclip);
        bclip[0] = aclip[2];
        bclip[1] = aclip[0];
        bclip[2] = aclip[1];
        bclip[3] = c[2];
        tex2.u1 = uv[4];
        tex2.v1 = uv[5];
        tex2.u2 = uv[0];
        tex2.v2 = uv[1];
        tex2.u3 = uv[2];
        tex2.v3 = uv[3];
        tex2.u4 = tex->u3;
        tex2.v4 = tex->v3;
        SubdivideQuad(&v[2], v, &v[1], v2, &tex2, double_sided, steps - 1, bclip);
    } else {
        c0 = clipflags[0];
        c1 = clipflags[1];
        c2 = clipflags[2];
        clipflags[0] = c[0];
        clipflags[1] = c[1];
        clipflags[2] = c[2];
        vb[0] = *v0;
        vb[1] = *v1;
        vb[2] = *v2;
        AddTriClippedSorted(vb, 0, 1, 2, tex, double_sided);
        clipflags[0] = c0;
        clipflags[1] = c1;
        clipflags[2] = c2;
    }
}

void SubdivideQuad(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2, D3DTLVERTEX *v3, TEXTURESTRUCT *tex, sint32 double_sided, sint32 steps, sint16 *c)
{
    D3DTLVERTEX v[5];
    D3DTLVERTEX vb[4];
    TEXTURESTRUCT tex2;
    float uv[10];
    sint16 aclip[5];
    sint16 bclip[4];
    sint16 c0, c1, c2, c3;

    if (steps) {
        tex2.drawtype = tex->drawtype;
        tex2.tpage = tex->tpage;
        tex2.flag = tex->flag;
        SubdivideEdge(v0, v1, &v[0], &aclip[0], tex->u1, tex->v1, tex->u2, tex->v2, &uv[0], &uv[1]);
        SubdivideEdge(v2, v3, &v[1], &aclip[1], tex->u3, tex->v3, tex->u4, tex->v4, &uv[2], &uv[3]);
        SubdivideEdge(v0, v3, &v[2], &aclip[2], tex->u1, tex->v1, tex->u4, tex->v4, &uv[4], &uv[5]);
        SubdivideEdge(v1, v2, &v[3], &aclip[3], tex->u2, tex->v2, tex->u3, tex->v3, &uv[6], &uv[7]);
        SubdivideEdge(v0, v2, &v[4], &aclip[4], tex->u1, tex->v1, tex->u3, tex->v3, &uv[8], &uv[9]);
        tex2.u1 = tex->u1;
        tex2.v1 = tex->v1;
        tex2.u2 = uv[0];
        tex2.v2 = uv[1];
        tex2.u3 = uv[8];
        tex2.v3 = uv[9];
        tex2.u4 = uv[4];
        tex2.v4 = uv[5];
        bclip[0] = c[0];
        bclip[1] = aclip[0];
        bclip[2] = aclip[4];
        bclip[3] = aclip[2];
        SubdivideQuad(v0, &v[0], &v[4], &v[2], &tex2, double_sided, steps - 1, bclip);
        tex2.u1 = uv[0];
        tex2.v1 = uv[1];
        tex2.u2 = tex->u2;
        tex2.v2 = tex->v2;
        tex2.u3 = uv[6];
        tex2.v3 = uv[7];
        tex2.u4 = uv[8];
        tex2.v4 = uv[9];
        bclip[0] = aclip[0];
        bclip[1] = c[1];
        bclip[2] = aclip[3];
        bclip[3] = aclip[4];
        SubdivideQuad(&v[0], v1, &v[3], &v[4], &tex2, double_sided, steps - 1, bclip);
        tex2.u1 = uv[8];
        tex2.v1 = uv[9];
        tex2.u2 = uv[6];
        tex2.v2 = uv[7];
        tex2.u3 = tex->u3;
        tex2.v3 = tex->v3;
        tex2.u4 = uv[2];
        tex2.v4 = uv[3];
        bclip[0] = aclip[4];
        bclip[1] = aclip[3];
        bclip[2] = c[2];
        bclip[3] = aclip[1];
        SubdivideQuad(&v[4], &v[3], v2, &v[1], &tex2, double_sided, steps - 1, bclip);
        tex2.u1 = uv[4];
        tex2.v1 = uv[5];
        tex2.u2 = uv[8];
        tex2.v2 = uv[9];
        tex2.u3 = uv[2];
        tex2.v3 = uv[3];
        tex2.u4 = tex->u4;
        tex2.v4 = tex->v4;
        bclip[0] = aclip[2];
        bclip[1] = aclip[4];
        bclip[2] = aclip[1];
        bclip[3] = c[3];
        SubdivideQuad(&v[2], &v[4], &v[1], v3, &tex2, double_sided, steps - 1, bclip);
    } else {
        c0 = clipflags[0];
        c1 = clipflags[1];
        c2 = clipflags[2];
        c3 = clipflags[3];
        clipflags[0] = c[0];
        clipflags[1] = c[1];
        clipflags[2] = c[2];
        clipflags[3] = c[3];
        vb[0] = *v0;
        vb[1] = *v1;
        vb[2] = *v2;
        vb[3] = *v3;
        AddQuadClippedSorted(vb, 0, 1, 2, 3, tex, double_sided);
        clipflags[0] = c0;
        clipflags[1] = c1;
        clipflags[2] = c2;
        clipflags[3] = c3;
    }
}

void AddTriSubdivide(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, TEXTURESTRUCT *tex, sint32 double_sided)
{
    sint32 steps;
    sint16 c[4];
    float z;

    if (!nPolyType) {
        z = v[v2].sz;
        if (z < 3500) {
            steps = 1;
            if (z < 2000) {
                steps = 2;
            }
            c[0] = clipflags[v0];
            c[1] = clipflags[v1];
            c[2] = clipflags[v2];
            SubdivideTri(&v[v0], &v[v1], &v[v2], tex, double_sided, steps, c);
        } else {
            AddTriClippedSorted(v, v0, v1, v2, tex, double_sided);
        }
    } else {
        AddTriClippedSorted(v, v0, v1, v2, tex, double_sided);
    }
}

void AddQuadSubdivide(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, sint16 v3, TEXTURESTRUCT *tex, sint32 double_sided)
{
    sint32 steps;
    sint16 c[4];
    float z;

    if (!nPolyType) {
        z = v[v3].sz;
        if (z < 3500) {
            steps = 1;
            if (z < 2000) {
                steps = 2;
            }
            c[0] = clipflags[v0];
            c[1] = clipflags[v1];
            c[2] = clipflags[v2];
            c[3] = clipflags[v3];
            SubdivideQuad(&v[v0], &v[v1], &v[v2], &v[v3], tex, double_sided, steps, c);
        } else {
            AddQuadClippedSorted(v, v0, v1, v2, v3, tex, double_sided);
        }
    } else {
        AddQuadClippedSorted(v, v0, v1, v2, v3, tex, double_sided);
    }
}

void AddLineClippedSorted(D3DTLVERTEX *v0, D3DTLVERTEX *v1, sint16 drawtype)
{
    D3DTLBUMPVERTEX *v;
    SORTLIST *sl;

    v = (D3DTLBUMPVERTEX *)(sizeof(SORTLIST) + pSortBuffer);
    sl = (SORTLIST *)pSortBuffer;
    sl->tpage = 0;
    sl->drawtype = drawtype;
    sl->nVtx = 2;
    pSortBuffer += sl->nVtx * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
    *pSortList = sl;
    pSortList++;
    sl->zVal = v0->sz;
    SortCount++;
    v->sx = v0->sx;
    v->sy = v0->sy;
    v->sz = f_a - f_boo * v0->rhw;
    v->rhw = v0->rhw;
    v->color = v0->color;
    v->specular = v0->specular;
    v++;
    v->sx = v1->sx;
    v->sy = v1->sy;
    v->sz = f_a - f_boo * v1->rhw;
    v->rhw = v1->rhw;
    v->color = v1->color;
    v->specular = v1->specular;
}

void CalcColorSplitMMX(D3DCOLOR s, D3DCOLOR *d)
{
    __asm {
        xor eax, eax
        mov ecx, s
        movd mm3, rgbmask
        movd mm0, ecx
        movd mm1, rgb80h
        psubusb mm0, mm1
        movd mm2, eax
        movd mm5, zero
        punpcklbw mm5, mm0
        psrlw mm5, 9
        packuswb mm0, mm5
        psrlq mm0, 0x20
        movd ebx, mm0
        pcmpgtb mm0, mm2
        movd eax, mm0
        pandn mm0, mm3
        movd mm1, ecx
        pand mm1, mm0
        paddusb mm1, mm1
        movd ecx, mm1
        mov edx, d
        or ecx, eax
        mov [edx], ecx
        mov ecx, [edx + 4]
        and ebx, 0xFFFFFF
        or ecx, ebx
        mov [edx + 4], ecx
    }
    d[0] &= 0xFFFFFF;
    d[0] |= GlobalAlpha;
}

void AddPrelitMMX(sint32 prelight, D3DCOLOR *color)
{
    sint32 c;

    c = color[0];
    __asm {
        xor eax, eax
        movd mm3, rgbmask
        movd mm4, prelight
        movd mm0, c
        paddusb mm0, mm4
        movd ecx, mm0
        movd mm1, rgb80h
        psubusb mm0, mm1
        movd mm2, eax
        movd mm5, zero
        punpcklbw mm5, mm0
        psrlw mm5, 9
        packuswb mm0, mm5
        psrlq mm0, 0x20
        movd ebx, mm0
        pcmpgtb mm0, mm2
        movd eax, mm0
        pandn mm0, mm3
        movd mm1, ecx
        pand mm1, mm0
        paddusb mm1, mm1
        movd ecx, mm1
        mov edx, color
        or ecx, eax
        mov [edx], ecx
        mov ecx, [edx + 4]
        and ebx, 0xFFFFFF
        or ecx, ebx
        mov [edx + 4], ecx
    }
}

void CalcColorSplit(D3DCOLOR s, D3DCOLOR *d)
{
    sint32 b, sb, sg, r, g, sr;

    sb = 0;
    sg = 0;
    sr = 0;
    b = CLRB(s);
    r = CLRR(s);
    g = CLRG(s);
    r -= 128;
    g -= 128;
    b -= 128;
    if (r > 0) {
        sr = r;
        r = 255;
    } else {
        r = CLRR(s) << 1;
    }
    if (g > 0) {
        sg = g;
        g = 255;
    } else {
        g = CLRG(s) << 1;
    }
    if (b > 0) {
        sb = b;
        b = 255;
    } else {
        b = CLRB(s) << 1;
    }
    if (r > 255) {
        r = 255;
    }
    if (g > 255) {
        g = 255;
    }
    if (b > 255) {
        b = 255;
    }
    d[0] = (d[0] & 0xFF000000) | RGBONLY(r, g, b);
    sr >>= 1;
    sg >>= 1;
    sb >>= 1;
    d[1] = (d[1] & 0xFF000000) | RGBONLY(sr, sg, sb);
    d[0] &= 0xFFFFFF;
    d[0] |= GlobalAlpha;
}
