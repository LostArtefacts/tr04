#include <stdlib.h>
#include "polyinsert.h"
#include "winmain.h"
#include "../camera.h"
#include "../tomb4fx.h"
#include "function_stubs.h"
#include "function_table.h"
#include "clipping.h"
#include "drawroom.h"
#include "3dmath.h"
#include "output.h"
#include "../text.h"
#include "../gameflow.h"
#include "../newinv.h"

static __forceinline void HWR_DrawSortList_inl(D3DTLBUMPVERTEX *info, sint32 num_verts, sint32 texture, sint32 type)
{
    switch (type) {
    case 0:
        if (App.dx.lpZBuffer)
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[texture].tex));
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
        break;
    case 1:
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[texture].tex));
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
        break;
    case 2:
        if (App.dx.lpZBuffer)
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SPECULARENABLE, 0);
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[texture].tex));
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SPECULARENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        break;
    case 3:
        if (App.dx.lpZBuffer)
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[texture].tex));
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        break;
    case 7:
        if (App.dx.lpZBuffer)
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[texture].tex));
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        break;
    case 4:
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[texture].tex));
        if (App.dx.lpZBuffer) {
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZENABLE, 0);
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 0);
        }
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
        if (App.dx.lpZBuffer) {
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 1);
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZENABLE, 1);
        }
        break;
    case 6:
        if (App.dx.lpZBuffer)
            IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, 0));
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_LINELIST, FVF, info, num_verts, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
        break;
    }
    DrawPrimitiveCnt++;
}

void HWR_DrawSortList(D3DTLBUMPVERTEX *info, sint32 num_verts, sint32 texture, sint32 type)
{
    HWR_DrawSortList_inl(info, num_verts, texture, type);
}

void DrawSortList(void)
{
    D3DTLBUMPVERTEX *vtx;
    D3DTLBUMPVERTEX *bVtx;
    D3DTLBUMPVERTEX *bVtxbak;
    sint32 nVtx, tpage, drawtype;
    sint32 num, i;
    SORTLIST *pSort;

    nVtx = 0;
    if (!SortCount) {
        return;
    }
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 1);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
    if (App.dx.lpZBuffer) {
        for (num = 0; num < SortCount; num++) {
            pSort = SortList[num];
            if (pSort->drawtype == 1 || pSort->drawtype == 4 || pSort->drawtype == 0) {
                break;
            }
        }
        bVtxbak = Bucket[0].vtx;
        bVtx = bVtxbak;
        tpage = pSort->tpage;
        drawtype = pSort->drawtype;
        for (; num < SortCount; num++) {
            pSort = SortList[num];
            if (pSort->drawtype == 1 || pSort->drawtype == 4 || pSort->drawtype == 0) {
                if (pSort->tpage == tpage && pSort->drawtype == drawtype) {
                    vtx = (D3DTLBUMPVERTEX *)(pSort + 1);
                    for (i = 0; i < pSort->nVtx; i++, vtx++, bVtx++) {
                        bVtx->sx = vtx->sx;
                        bVtx->sy = vtx->sy;
                        bVtx->sz = vtx->sz;
                        bVtx->rhw = vtx->rhw;
                        bVtx->color = vtx->color;
                        bVtx->specular = vtx->specular;
                        bVtx->tu = vtx->tu;
                        bVtx->tv = vtx->tv;
                        nVtx++;
                    }
                } else {
                    HWR_DrawSortList_inl(bVtxbak, nVtx, tpage, drawtype);
                    tpage = pSort->tpage;
                    drawtype = pSort->drawtype;
                    bVtx = bVtxbak;
                    nVtx = 0;
                    num--;
                }
            }
        }
        if (nVtx) {
            HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
        }
        for (num = SortCount - 1; num >= 0; num--) {
            pSort = SortList[num];
            if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 6 || pSort->drawtype == 7) {
                break;
            }
        }
        tpage = pSort->tpage;
        drawtype = pSort->drawtype;
        bVtx = bVtxbak;
        nVtx = 0;
        for (; num >= 0; num--) {
            pSort = SortList[num];
            if (pSort->drawtype == 2 || pSort->drawtype == 3 || pSort->drawtype == 6 || pSort->drawtype == 7) {
                if (pSort->tpage == tpage && pSort->drawtype == drawtype) {
                    vtx = (D3DTLBUMPVERTEX *)(pSort + 1);
                    for (i = 0; i < pSort->nVtx; i++, vtx++, bVtx++) {
                        bVtx->sx = vtx->sx;
                        bVtx->sy = vtx->sy;
                        bVtx->sz = vtx->sz;
                        bVtx->rhw = vtx->rhw;
                        bVtx->color = vtx->color;
                        bVtx->specular = vtx->specular;
                        bVtx->tu = vtx->tu;
                        bVtx->tv = vtx->tv;
                        nVtx++;
                    }
                } else {
                    HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
                    tpage = pSort->tpage;
                    nVtx = 0;
                    drawtype = pSort->drawtype;
                    bVtx = bVtxbak;
                    num++;
                }
            }
        }
        if (nVtx) {
            HWR_DrawSortList(bVtxbak, nVtx, tpage, drawtype);
        }
    } else {
        for (num = 0; num < SortCount; num++) {
            pSort = SortList[num];
            vtx = (D3DTLBUMPVERTEX *)(pSort + 1);
            if (pSort->polytype == 4) {
                IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_TEXTUREPERSPECTIVE, 0);
            }
            HWR_DrawSortList(vtx, pSort->nVtx, pSort->tpage, pSort->drawtype);
            if (pSort->polytype == 4) {
                IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);
            }
        }
    }
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 1);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
    InitBuckets();
}

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

void mD3DTransform(FVECTOR *vec, D3DMATRIX *mx)
{
    FVECTOR t;

    t.x = mx->_11 * vec->x + mx->_21 * vec->y + mx->_31 * vec->z + mx->_41;
    t.y = mx->_22 * vec->y + (mx->_12 * vec->x + mx->_32 * vec->z) + mx->_42;
    t.z = mx->_23 * vec->y + (mx->_13 * vec->x + mx->_33 * vec->z) + mx->_43;
    *vec = t;
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

void OmniFog(D3DTLVERTEX *v)
{
    FOGBULB_STRUCT *FogBulb;
    FVECTOR pos, dP, dV, d;
    float val, val2, inv;
    sint32 i, s, r, g, b, lVal;

    if (InventoryActive || nPolyType == 6 || gfLevelFlags & GF_TRAIN)
        return;
    s = v->specular >> 24;
    if (gfCurrentLevel != 5 && gfCurrentLevel != 6 && s != 255) {
        v->specular |= 0xFF000000;
        r = (((v->color >> 8) & 0xFF00) * s) >> 16;
        g = ((v->color >> 8 & 0xFF) * s) >> 8;
        b = (CLRB(v->color) * s) >> 8;
        v->color = RGBA(r, g, b, v->color >> 24);
        if (!(v->color & 0xFFFFFF))
            return;
    }
    OmniEffect(v);
    if (NumFogBulbsInRange && NumActiveFogBulbs) {
        inv = 1.0F / v->sz;
        for (i = 0; i < NumActiveFogBulbs; i++) {
            FogBulb = ActiveFogBulbs[i];
            if (FogBulb->inRange) {
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
                            lVal = (v->specular >> 24) + (*(sint32 *)&val2 & 0x3FFFFF) - FogBulb->density;
                            if (lVal < 0)
                                lVal = 0;
                            v->specular = v->specular & 0xFFFFFF | lVal << 24;
                        }
                    }
                }
            }
        }
    }
}

void AddQuadClippedZBuffer(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, sint16 v3, TEXTURESTRUCT *tex, sint32 double_sided)
{
    D3DTLBUMPVERTEX *p;
    D3DTLVERTEX *vtx;
    TEXTURESTRUCT tex2;
    sint32 *nVtx;

    if (clipflags[v0] & clipflags[v1] & clipflags[v2] & clipflags[v3])
        return;
    if ((clipflags[v0] | clipflags[v1] | clipflags[v2] | clipflags[v3]) < 0) {
        AddTriClippedZBuffer(v, v0, v1, v2, tex, double_sided);
        tex2.drawtype = tex->drawtype;
        tex2.flag = tex->flag;
        tex2.tpage = tex->tpage;
        tex2.u1 = tex->u1;
        tex2.v1 = tex->v1;
        tex2.u2 = tex->u3;
        tex2.v2 = tex->v3;
        tex2.u3 = tex->u4;
        tex2.v3 = tex->v4;
        AddTriClippedZBuffer(v, v0, v2, v3, &tex2, double_sided);
        return;
    }
    if (IsVisible(&v[v0], &v[v1], &v[v2])) {
        if (!double_sided)
            return;
        v0 ^= v2;
        v2 ^= v0;
        v0 ^= v2;
        tex2.drawtype = tex->drawtype;
        tex2.flag = tex->flag;
        tex2.tpage = tex->tpage;
        tex2.u1 = tex->u3;
        tex2.v1 = tex->v3;
        tex2.u2 = tex->u2;
        tex2.v2 = tex->v2;
        tex2.u3 = tex->u1;
        tex2.v3 = tex->v1;
        tex2.u4 = tex->u4;
        tex2.v4 = tex->v4;
        tex = &tex2;
    }
    if (clipflags[v0] | clipflags[v1] | clipflags[v2] | clipflags[v3]) {
        AddTriClippedZBuffer(v, v0, v1, v2, tex, double_sided);
        tex2.drawtype = tex->drawtype;
        tex2.flag = tex->flag;
        tex2.tpage = tex->tpage;
        tex2.u1 = tex->u1;
        tex2.v1 = tex->v1;
        tex2.u2 = tex->u3;
        tex2.v2 = tex->v3;
        tex2.u3 = tex->u4;
        tex2.v3 = tex->v4;
        AddTriClippedZBuffer(v, v0, v2, v3, &tex2, double_sided);
        return;
    }
    {
        sint32 cb0, cb1, cb2, cb3;
        sint32 sb0, sb1, sb2, sb3;

        FindBucket(tex->tpage, &p, &nVtx);
        *nVtx += 6;
        sb0 = v[v0].specular;
        sb1 = v[v1].specular;
        sb2 = v[v2].specular;
        sb3 = v[v3].specular;
        cb0 = v[v0].color;
        cb1 = v[v1].color;
        cb2 = v[v2].color;
        cb3 = v[v3].color;
        if (App.Volumetric) {
            OmniFog(&v[v0]);
            OmniFog(&v[v1]);
            OmniFog(&v[v2]);
            OmniFog(&v[v3]);
        }
        vtx = &v[v0];
        p->sx = vtx->sx;
        p->sy = vtx->sy;
        p->sz = f_a - f_boo * vtx->rhw;
        p->rhw = vtx->rhw;
        p->color = vtx->color;
        p->specular = vtx->specular;
        p->tu = tex->u1;
        p->tv = tex->v1;
        p[3].sx = p->sx;
        p[3].sy = p->sy;
        p[3].sz = p->sz;
        p[3].rhw = p->rhw;
        p[3].color = p->color;
        p[3].specular = p->specular;
        p[3].tu = p->tu;
        p[3].tv = p->tv;
        p++;
        vtx = &v[v1];
        p->sx = vtx->sx;
        p->sy = vtx->sy;
        p->sz = f_a - f_boo * vtx->rhw;
        p->rhw = vtx->rhw;
        p->color = vtx->color;
        p->specular = vtx->specular;
        p->tu = tex->u2;
        p->tv = tex->v2;
        p++;
        vtx = &v[v2];
        p->sx = vtx->sx;
        p->sy = vtx->sy;
        p->sz = f_a - f_boo * vtx->rhw;
        p->rhw = vtx->rhw;
        p->color = vtx->color;
        p->specular = vtx->specular;
        p->tu = tex->u3;
        p->tv = tex->v3;
        p[2].sx = p->sx;
        p[2].sy = p->sy;
        p[2].sz = p->sz;
        p[2].rhw = p->rhw;
        p[2].color = p->color;
        p[2].specular = p->specular;
        p[2].tu = p->tu;
        p[2].tv = p->tv;
        p += 3;
        vtx = &v[v3];
        p->sx = vtx->sx;
        p->sy = vtx->sy;
        p->sz = f_a - f_boo * vtx->rhw;
        p->rhw = vtx->rhw;
        p->color = vtx->color;
        p->specular = vtx->specular;
        p->tu = tex->u4;
        p->tv = tex->v4;
        nPolys += 2;
        v[v0].specular = sb0;
        v[v1].specular = sb1;
        v[v2].specular = sb2;
        v[v3].specular = sb3;
        v[v0].color = cb0;
        v[v1].color = cb1;
        v[v2].color = cb2;
        v[v3].color = cb3;
    }
}

void AddTriClippedZBuffer(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, TEXTURESTRUCT *tex, sint32 double_sided)
{
    D3DTLVERTEX *vtx;
    D3DTLBUMPVERTEX *p;
    D3DTLBUMPVERTEX *bp;
    TEXTURESTRUCT tex2;
    sint32 *nVtx;
    sint32 nPoints, i;
    sint32 cb0, cb1, cb2;
    sint32 sb0, sb1, sb2;
    sint8 clip, clipZ;

    clipZ = 0;
    if (clipflags[v0] & clipflags[v1] & clipflags[v2])
        return;
    if ((clipflags[v0] | clipflags[v1] | clipflags[v2]) < 0) {
        if (!visible_zclip(&v[v0], &v[v1], &v[v2])) {
            if (!double_sided)
                return;
            v1 ^= v2;
            v2 ^= v1;
            v1 ^= v2;
            if (!visible_zclip(&v[v0], &v[v1], &v[v2]))
                return;
            tex2.drawtype = tex->drawtype;
            tex2.flag = tex->flag;
            tex2.tpage = tex->tpage;
            tex2.u1 = tex->u1;
            tex2.v1 = tex->v1;
            tex2.u2 = tex->u3;
            tex2.v2 = tex->v3;
            tex2.u3 = tex->u2;
            tex2.v3 = tex->v2;
            tex = &tex2;
        }
        clipZ = 1;
        p = zClipperBuffer;
    } else {
        if (IsVisible(&v[v0], &v[v1], &v[v2])) {
            if (!double_sided)
                return;
            v1 ^= v2;
            v2 ^= v1;
            v1 ^= v2;
            tex2.drawtype = tex->drawtype;
            tex2.flag = tex->flag;
            tex2.tpage = tex->tpage;
            tex2.u1 = tex->u1;
            tex2.v1 = tex->v1;
            tex2.u2 = tex->u3;
            tex2.v2 = tex->v3;
            tex2.u3 = tex->u2;
            tex2.v3 = tex->v2;
            tex = &tex2;
        }
        if (!(clipflags[v0] | clipflags[v1] | clipflags[v2])) {
            FindBucket(tex->tpage, &p, &nVtx);
            clip = 0;
            *nVtx += 3;
            goto backup;
        }
        p = XYUVClipperBuffer;
    }
    clip = 1;
backup:
    sb0 = v[v0].specular;
    sb1 = v[v1].specular;
    sb2 = v[v2].specular;
    cb0 = v[v0].color;
    cb1 = v[v1].color;
    cb2 = v[v2].color;
    if (App.Volumetric) {
        OmniFog(&v[v0]);
        OmniFog(&v[v1]);
        OmniFog(&v[v2]);
    }
    bp = p;
    vtx = &v[v0];
    p->sx = vtx->sx;
    p->sy = vtx->sy;
    p->sz = vtx->sz;
    p->rhw = vtx->rhw;
    p->color = vtx->color;
    p->specular = vtx->specular;
    p->tu = tex->u1;
    p->tv = tex->v1;
    p->tx = vtx->tu;
    p->ty = vtx->tv;
    p++;
    vtx = &v[v1];
    p->sx = vtx->sx;
    p->sy = vtx->sy;
    p->sz = vtx->sz;
    p->rhw = vtx->rhw;
    p->color = vtx->color;
    p->specular = vtx->specular;
    p->tu = tex->u2;
    p->tv = tex->v2;
    p->tx = vtx->tu;
    p->ty = vtx->tv;
    p++;
    vtx = &v[v2];
    p->sx = vtx->sx;
    p->sy = vtx->sy;
    p->sz = vtx->sz;
    p->rhw = vtx->rhw;
    p->color = vtx->color;
    p->specular = vtx->specular;
    p->tu = tex->u3;
    p->tv = tex->v3;
    p->tx = vtx->tu;
    p->ty = vtx->tv;
    if (clip) {
        nPoints = 3;
        if (clipZ) {
            nPoints = ZClipper(3, zClipperBuffer, XYUVClipperBuffer);
            if (!nPoints) {
                v[v0].specular = sb0;
                v[v1].specular = sb1;
                v[v2].specular = sb2;
                v[v0].color = cb0;
                v[v1].color = cb1;
                v[v2].color = cb2;
                return;
            }
        } else {
            p = XYUVClipperBuffer;
            for (i = 3; i > 0; i--, p++) {
                p->tu = p->rhw * p->tu;
                p->tv = p->rhw * p->tv;
            }
        }
        nPoints = XYUVGClipper(nPoints, XYUVClipperBuffer);
        if (nPoints) {
            FindBucket(tex->tpage, &p, &nVtx);
            *nVtx += 3 * nPoints - 6;
            AddClippedPoly(p, nPoints, XYUVClipperBuffer, tex);
        }
    } else {
        p = bp;
        p->sz = f_a - f_boo * p->rhw;
        p++;
        p->sz = f_a - f_boo * p->rhw;
        p++;
        p->sz = f_a - f_boo * p->rhw;
        nPolys++;
    }
    v[v0].specular = sb0;
    v[v1].specular = sb1;
    v[v2].specular = sb2;
    v[v0].color = cb0;
    v[v1].color = cb1;
    v[v2].color = cb2;
}

void AddClippedPoly(D3DTLBUMPVERTEX *dest, sint32 nPoints, D3DTLBUMPVERTEX *v, TEXTURESTRUCT *pTex)
{
    D3DTLBUMPVERTEX *first;
    D3DTLBUMPVERTEX *q;
    float z;
    sint32 i;

    first = dest;
    for (i = 0; i < 3; i++, v++, dest++) {
        dest->sx = v->sx;
        dest->sy = v->sy;
        dest->sz = f_a - f_boo * v->rhw;
        dest->rhw = v->rhw;
        dest->color = v->color;
        dest->specular = v->specular;
        z = 1.0F / v->rhw;
        dest->tu = z * v->tu;
        dest->tv = z * v->tv;
    }
    nClippedPolys++;
    v--;
    for (i = nPoints - 3; i > 0; i--) {
        v++;
        dest->sx = first->sx;
        dest->sy = first->sy;
        dest->sz = first->sz;
        dest->rhw = first->rhw;
        dest->color = first->color;
        dest->specular = first->specular;
        dest->tu = first->tu;
        dest->tv = first->tv;
        q = dest - 1;
        dest++;
        dest->sx = q->sx;
        dest->sy = q->sy;
        dest->sz = q->sz;
        dest->rhw = q->rhw;
        dest->color = q->color;
        dest->specular = q->specular;
        dest->tu = q->tu;
        dest->tv = q->tv;
        dest++;
        dest->sx = v->sx;
        dest->sy = v->sy;
        dest->sz = f_a - f_boo * v->rhw;
        dest->rhw = v->rhw;
        dest->color = v->color;
        dest->specular = v->specular;
        z = 1.0F / v->rhw;
        dest->tu = z * v->tu;
        dest->tv = z * v->tv;
        dest++;
        nClippedPolys++;
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

void AddTriClippedSorted(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, TEXTURESTRUCT *tex, sint32 double_sided)
{
    D3DTLBUMPVERTEX *p;
    D3DTLBUMPVERTEX *bp;
    D3DTLVERTEX *pV;
    SORTLIST *sl;
    TEXTURESTRUCT tex2;
    float z;
    sint32 cb0, cb1, cb2;
    sint32 sb0, sb1, sb2;
    sint32 num, i;
    sint8 clip, clipZ;

    clipZ = 0;
    if (clipflags[v0] & clipflags[v1] & clipflags[v2])
        return;
    if ((clipflags[v0] | clipflags[v1] | clipflags[v2]) < 0) {
        if (!visible_zclip(&v[v0], &v[v1], &v[v2])) {
            if (!double_sided)
                return;
            v1 ^= v2;
            v2 ^= v1;
            v1 ^= v2;
            if (!visible_zclip(&v[v0], &v[v1], &v[v2]))
                return;
            tex2.drawtype = tex->drawtype;
            tex2.flag = tex->flag;
            tex2.tpage = tex->tpage;
            tex2.u1 = tex->u1;
            tex2.v1 = tex->v1;
            tex2.u2 = tex->u3;
            tex2.v2 = tex->v3;
            tex2.u3 = tex->u2;
            tex2.v3 = tex->v2;
            tex = &tex2;
        }
        clipZ = 1;
        p = zClipperBuffer;
    } else {
        if (IsVisible(&v[v0], &v[v1], &v[v2])) {
            if (!double_sided)
                return;
            v1 ^= v2;
            v2 ^= v1;
            v1 ^= v2;
            tex2.drawtype = tex->drawtype;
            tex2.flag = tex->flag;
            tex2.tpage = tex->tpage;
            tex2.u1 = tex->u1;
            tex2.v1 = tex->v1;
            tex2.u2 = tex->u3;
            tex2.v2 = tex->v3;
            tex2.u3 = tex->u2;
            tex2.v3 = tex->v2;
            tex = &tex2;
        }
        if (!(clipflags[v0] | clipflags[v1] | clipflags[v2])) {
            p = (D3DTLBUMPVERTEX *)(sizeof(SORTLIST) + pSortBuffer);
            sl = (SORTLIST *)pSortBuffer;
            clip = 0;
            sl->tpage = tex->tpage;
            sl->drawtype = tex->drawtype;
            sl->nVtx = 3;
            sl->polytype = (sint16)nPolyType;
            pSortBuffer += sl->nVtx * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
            *pSortList = sl;
            pSortList++;
            goto backup;
        }
        p = XYUVClipperBuffer;
    }
    clip = 1;
backup:
    z = 0;
    sb0 = v[v0].specular;
    sb1 = v[v1].specular;
    sb2 = v[v2].specular;
    cb0 = v[v0].color;
    cb1 = v[v1].color;
    cb2 = v[v2].color;
    if (App.Volumetric && tex->drawtype != 2) {
        OmniFog(&v[v0]);
        OmniFog(&v[v1]);
        OmniFog(&v[v2]);
    }
    bp = p;
    pV = &v[v0];
    p->sx = pV->sx;
    p->sy = pV->sy;
    p->sz = pV->sz;
    p->rhw = pV->rhw;
    p->color = pV->color;
    p->specular = pV->specular;
    p->tu = tex->u1;
    p->tv = tex->v1;
    p->tx = pV->tu;
    p->ty = pV->tv;
    if (nPolyType)
        z = pV->sz;
    else if (pV->sz > z)
        z = pV->sz;
    p++;
    pV = &v[v1];
    p->sx = pV->sx;
    p->sy = pV->sy;
    p->sz = pV->sz;
    p->rhw = pV->rhw;
    p->color = pV->color;
    p->specular = pV->specular;
    p->tu = tex->u2;
    p->tv = tex->v2;
    p->tx = pV->tu;
    p->ty = pV->tv;
    if (nPolyType)
        z += pV->sz;
    else if (pV->sz > z)
        z = pV->sz;
    p++;
    pV = &v[v2];
    p->sx = pV->sx;
    p->sy = pV->sy;
    p->sz = pV->sz;
    p->rhw = pV->rhw;
    p->color = pV->color;
    p->specular = pV->specular;
    p->tu = tex->u3;
    p->tv = tex->v3;
    p->tx = pV->tu;
    p->ty = pV->tv;
    if (nPolyType)
        z += pV->sz;
    else if (pV->sz > z)
        z = pV->sz;
    if (clip) {
        num = 3;
        if (clipZ) {
            num = ZClipper(3, zClipperBuffer, XYUVClipperBuffer);
            if (!num) {
                v[v0].specular = sb0;
                v[v1].specular = sb1;
                v[v2].specular = sb2;
                v[v0].color = cb0;
                v[v1].color = cb1;
                v[v2].color = cb2;
                return;
            }
        } else {
            p = XYUVClipperBuffer;
            for (i = 0; i < 3; i++, p++) {
                p->tu *= p->rhw;
                p->tv *= p->rhw;
            }
        }
        num = XYUVGClipper(num, XYUVClipperBuffer);
        if (num) {
            p = (D3DTLBUMPVERTEX *)(sizeof(SORTLIST) + pSortBuffer);
            sl = (SORTLIST *)pSortBuffer;
            sl->drawtype = tex->drawtype;
            sl->nVtx = (sint16)(3 * num - 6);
            if (nPolyType)
                sl->zVal = z * 0.333333F;
            else
                sl->zVal = z;
            sl->tpage = tex->tpage;
            sl->polytype = (sint16)nPolyType;
            pSortBuffer += sl->nVtx * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
            *pSortList = sl;
            pSortList++;
            SortCount++;
            AddClippedPoly(p, num, XYUVClipperBuffer, tex);
        }
        v[v0].color = cb0;
        v[v1].color = cb1;
        v[v2].color = cb2;
        v[v0].specular = sb0;
        v[v1].specular = sb1;
        v[v2].specular = sb2;
        return;
    } else {
        p = bp;
        p->sz = f_a - f_boo * p->rhw;
        p++;
        p->sz = f_a - f_boo * p->rhw;
        p++;
        p->sz = f_a - f_boo * p->rhw;
        if (nPolyType)
            sl->zVal = z * 0.333333F;
        else
            sl->zVal = z;
        SortCount++;
        nPolys++;
    }
    v[v0].color = cb0;
    v[v1].color = cb1;
    v[v2].color = cb2;
    v[v0].specular = sb0;
    v[v1].specular = sb1;
    v[v2].specular = sb2;
}

void AddQuadClippedSorted(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, sint16 v3, TEXTURESTRUCT *tex, sint32 double_sided)
{
    D3DTLBUMPVERTEX *p;
    D3DTLVERTEX *pV;
    SORTLIST *sl;
    TEXTURESTRUCT tex2;
    float z;
    sint32 cb0, cb1, cb2, cb3;
    sint32 sb0, sb1, sb2, sb3;

    z = 0;
    if (clipflags[v0] & clipflags[v1] & clipflags[v2] & clipflags[v3])
        return;
    if ((clipflags[v0] | clipflags[v1] | clipflags[v2] | clipflags[v3]) < 0) {
        AddTriClippedSorted(v, v0, v1, v2, tex, double_sided);
        tex2.drawtype = tex->drawtype;
        tex2.flag = tex->flag;
        tex2.tpage = tex->tpage;
        tex2.u1 = tex->u1;
        tex2.v1 = tex->v1;
        tex2.u2 = tex->u3;
        tex2.v2 = tex->v3;
        tex2.u3 = tex->u4;
        tex2.v3 = tex->v4;
        AddTriClippedSorted(v, v0, v2, v3, &tex2, double_sided);
        return;
    }
    if (IsVisible(&v[v0], &v[v1], &v[v2])) {
        if (!double_sided)
            return;
        v0 ^= v2;
        v2 ^= v0;
        v0 ^= v2;
        tex2.drawtype = tex->drawtype;
        tex2.flag = tex->flag;
        tex2.tpage = tex->tpage;
        tex2.u1 = tex->u3;
        tex2.v1 = tex->v3;
        tex2.u2 = tex->u2;
        tex2.v2 = tex->v2;
        tex2.u3 = tex->u1;
        tex2.v3 = tex->v1;
        tex2.u4 = tex->u4;
        tex2.v4 = tex->v4;
        tex = &tex2;
    }
    if (clipflags[v0] | clipflags[v1] | clipflags[v2] | clipflags[v3]) {
        AddTriClippedSorted(v, v0, v1, v2, tex, double_sided);
        tex2.drawtype = tex->drawtype;
        tex2.flag = tex->flag;
        tex2.tpage = tex->tpage;
        tex2.u1 = tex->u1;
        tex2.v1 = tex->v1;
        tex2.u2 = tex->u3;
        tex2.v2 = tex->v3;
        tex2.u3 = tex->u4;
        tex2.v3 = tex->v4;
        AddTriClippedSorted(v, v0, v2, v3, &tex2, double_sided);
        return;
    }
    p = (D3DTLBUMPVERTEX *)(sizeof(SORTLIST) + pSortBuffer);
    sl = (SORTLIST *)pSortBuffer;
    sl->tpage = tex->tpage;
    sl->drawtype = tex->drawtype;
    sl->nVtx = 6;
    sl->polytype = (sint16)nPolyType;
    pSortBuffer += sl->nVtx * sizeof(D3DTLBUMPVERTEX) + sizeof(SORTLIST);
    *pSortList = sl;
    pSortList++;
    SortCount++;
    sb0 = v[v0].specular;
    sb1 = v[v1].specular;
    sb2 = v[v2].specular;
    sb3 = v[v3].specular;
    cb0 = v[v0].color;
    cb1 = v[v1].color;
    cb2 = v[v2].color;
    cb3 = v[v3].color;
    if (App.Volumetric && tex->drawtype != 2) {
        OmniFog(&v[v0]);
        OmniFog(&v[v1]);
        OmniFog(&v[v2]);
        OmniFog(&v[v3]);
    }
    pV = &v[v0];
    p->sx = pV->sx;
    p->sy = pV->sy;
    p->sz = f_a - f_boo * pV->rhw;
    p->rhw = pV->rhw;
    p->color = pV->color;
    p->specular = pV->specular;
    p->tu = tex->u1;
    p->tv = tex->v1;
    if (nPolyType)
        z = pV->sz;
    else if (pV->sz > z)
        z = pV->sz;
    p[3].sx = p->sx;
    p[3].sy = p->sy;
    p[3].sz = p->sz;
    p[3].rhw = p->rhw;
    p[3].color = p->color;
    p[3].specular = p->specular;
    p[3].tu = p->tu;
    p[3].tv = p->tv;
    p++;
    pV = &v[v1];
    p->sx = pV->sx;
    p->sy = pV->sy;
    p->sz = f_a - f_boo * pV->rhw;
    p->rhw = pV->rhw;
    p->color = pV->color;
    p->specular = pV->specular;
    p->tu = tex->u2;
    p->tv = tex->v2;
    if (nPolyType)
        z += pV->sz;
    else if (pV->sz > z)
        z = pV->sz;
    p++;
    pV = &v[v2];
    p->sx = pV->sx;
    p->sy = pV->sy;
    p->sz = f_a - f_boo * pV->rhw;
    p->rhw = pV->rhw;
    p->color = pV->color;
    p->specular = pV->specular;
    p->tu = tex->u3;
    p->tv = tex->v3;
    if (nPolyType)
        z += pV->sz;
    else if (pV->sz > z)
        z = pV->sz;
    p[2].sx = p->sx;
    p[2].sy = p->sy;
    p[2].sz = p->sz;
    p[2].rhw = p->rhw;
    p[2].color = p->color;
    p[2].specular = p->specular;
    p[2].tu = p->tu;
    p[2].tv = p->tv;
    p += 3;
    pV = &v[v3];
    p->sx = pV->sx;
    p->sy = pV->sy;
    p->sz = f_a - f_boo * pV->rhw;
    p->rhw = pV->rhw;
    p->color = pV->color;
    p->specular = pV->specular;
    p->tu = tex->u4;
    p->tv = tex->v4;
    if (nPolyType)
        z += pV->sz;
    else if (pV->sz > z)
        z = pV->sz;
    if (nPolyType)
        z *= 0.25F;
    sl->zVal = z;
    nPolys += 2;
    v[v0].color = cb0;
    v[v1].color = cb1;
    v[v2].color = cb2;
    v[v3].color = cb3;
    v[v0].specular = sb0;
    v[v1].specular = sb1;
    v[v2].specular = sb2;
    v[v3].specular = sb3;
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
