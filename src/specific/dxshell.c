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
