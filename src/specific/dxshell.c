#include <windows.h>
#include <ddraw.h>

#include "dxshell.h"

void DXBitMask2ShiftCnt(uint32 mask, uint8 *shift, uint8 *count)
{
    uint8 i;

    for (i = 0; !(mask & 1); i++)
        mask >>= 1;
    *shift = i;
    for (i = 0; mask & 1; i++)
        mask >>= 1;
    *count = i;
}

sint32 BPPToDDBD(sint32 BPP)
{
    switch (BPP) {
    case 1:
        return DDBD_1;
    case 2:
        return DDBD_2;
    case 4:
        return DDBD_4;
    case 8:
        return DDBD_8;
    case 16:
        return DDBD_16;
    case 24:
        return DDBD_24;
    case 32:
        return DDBD_32;
    default:
        return 0;
    }
}
