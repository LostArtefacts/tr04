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

sint32 IsVolumetric(void)
{
    return App.Volumetric;
}
