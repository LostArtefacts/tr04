#include <stdlib.h>
#include "polyinsert.h"
#include "winmain.h"
#include "../camera.h"
#include "../tomb4fx.h"
#include "function_stubs.h"

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
