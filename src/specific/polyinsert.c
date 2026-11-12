#include "polyinsert.h"
#include "winmain.h"

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
