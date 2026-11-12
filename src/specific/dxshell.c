#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "dxshell.h"
#include "winmain.h"
#include "function_stubs.h"

sint32 DXAttempt(HRESULT r)
{
    if (SUCCEEDED(r))
        return DD_OK;
    Log(1, "ERROR : %s", DXGetErrorString(r));
    return DD_FALSE;
}

void *AddStruct(void *p, sint32 num, sint32 size)
{
    void *ptr;

    if (num)
        ptr = realloc(p, size * (num + 1));
    else
        ptr = malloc(size);
    memset((char *)ptr + size * num, 0, size);
    return ptr;
}

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

void DXMove(sint32 x, sint32 y)
{
    Log(2, "DXMove : x %d y %d", x, y);
    if (G_dxptr && !(G_dxptr->Flags & 1))
        SetRect(&G_dxptr->rScreen, x, y, x + G_dxptr->dwRenderWidth, y + G_dxptr->dwRenderHeight);
}

void DXReadKeyboard(char *KeyMap)
{
    HRESULT state;

    state = IDirectInputDevice2_GetDeviceState(G_dxptr->Keyboard, 256, KeyMap);
    if (FAILED(state)) {
        if (state == DIERR_INPUTLOST)
            IDirectInputDevice2_Acquire(G_dxptr->Keyboard);
        IDirectInputDevice2_GetDeviceState(G_dxptr->Keyboard, 256, KeyMap);
    }
}
