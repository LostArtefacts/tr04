#include "polyinsert.h"
#include "winmain.h"

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
