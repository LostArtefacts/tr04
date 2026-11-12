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

sint32 IsVolumetric(void)
{
    return App.Volumetric;
}
