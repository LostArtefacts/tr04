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

sint32 DXGetInfo(DXINFO *dxinfo, HWND hwnd)
{
    Log(2, "DXInitialise");
    G_hwnd = hwnd;
    Log(5, "Enumerating DirectDraw Devices");
    DXAttempt(DirectDrawEnumerate(DXEnumDirectDraw, dxinfo));
    DXAttempt(DirectSoundEnumerate(DXEnumDirectSound, dxinfo));
    G_dxinfo = dxinfo;
    return 1;
}

BOOL CALLBACK DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
    DXINFO *dxinfo;
    DXDIRECTSOUNDINFO *DSInfo;
    sint32 nDSInfo;

    Log(2, "DXEnumDirectSound");
    dxinfo = (DXINFO *)lpContext;
    nDSInfo = dxinfo->nDSInfo;
    dxinfo->DSInfo = (DXDIRECTSOUNDINFO *)AddStruct(dxinfo->DSInfo, nDSInfo, sizeof(DXDIRECTSOUNDINFO));
    DSInfo = &dxinfo->DSInfo[nDSInfo];
    if (!lpGuid) {
        DSInfo->lpGuid = 0;
    } else {
        DSInfo->lpGuid = &DSInfo->Guid;
        DSInfo->Guid = *lpGuid;
    }
    lstrcpy(DSInfo->About, lpcstrDescription);
    lstrcpy(DSInfo->Name, lpcstrModule);
    Log(5, "Found - %s %s", lpcstrDescription, lpcstrModule);
    dxinfo->nDSInfo++;
    return DDENUMRET_OK;
}

void DXFreeInfo(DXINFO *dxinfo)
{
    sint32 i;
    sint32 j;

    Log(2, "DXFreeInfo");
    for (i = 0; i < dxinfo->nDDInfo; i++) {
        for (j = 0; j < dxinfo->DDInfo[i].nD3DDevices; j++) {
            free(dxinfo->DDInfo[i].D3DDevices[j].DisplayModes);
            free(dxinfo->DDInfo[i].D3DDevices[j].TextureInfos);
            free(dxinfo->DDInfo[i].D3DDevices[j].ZBufferInfos);
        }
        free(dxinfo->DDInfo[i].D3DDevices);
        free(dxinfo->DDInfo[i].DisplayModes);
    }
    free(dxinfo->DDInfo);
    free(dxinfo->DSInfo);
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

sint32 DXDDCreate(LPGUID pGuid, void **pDD4)
{
    LPDIRECTDRAW pDD;

    Log(2, "DXDDCreate");
    if (DXAttempt(DirectDrawCreate(pGuid, &pDD, 0)) == DD_OK) {
        DXAttempt(IDirectDraw_QueryInterface(pDD, &IID_IDirectDraw4, pDD4));
        if (pDD) {
            Log(4, "Released %s @ %x - RefCnt = %d", "DirectDraw", pDD, IDirectDraw_Release(pDD));
            pDD = 0;
        } else {
            Log(1, "%s Attempt To Release NULL Ptr", "DirectDraw");
        }
        Log(5, "DXDDCreate Successful");
        return 1;
    }
    Log(1, "DXDDCreate Failed");
    return 0;
}

sint32 DXD3DCreate(LPDIRECTDRAW4 pDD4, void **pD3D)
{
    Log(2, "DXD3DCreate");
    if (DXAttempt(IDirectDraw4_QueryInterface(pDD4, &IID_IDirect3D3, pD3D)) == DD_OK) {
        Log(5, "DXD3DCreate Successful");
        return 1;
    }
    Log(1, "DXD3DCreate Failed");
    return 0;
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

HRESULT CALLBACK DXEnumDisplayModes(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext)
{
    DXDIRECTDRAWINFO *DDInfo;
    DXDISPLAYMODE *DM;
    sint32 nDisplayModes;

    DDInfo = (DXDIRECTDRAWINFO *)lpContext;
    nDisplayModes = DDInfo->nDisplayModes;
    DDInfo->DisplayModes = (DXDISPLAYMODE *)AddStruct(DDInfo->DisplayModes, nDisplayModes, sizeof(DXDISPLAYMODE));
    DM = &DDInfo->DisplayModes[nDisplayModes];
    DM->w = lpDDSurfaceDesc2->dwWidth;
    DM->h = lpDDSurfaceDesc2->dwHeight;
    DM->bpp = lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount;
    DM->bPalette = (lpDDSurfaceDesc2->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) >> 5;
    DM->RefreshRate = lpDDSurfaceDesc2->dwRefreshRate;
    memcpy(&DM->ddsd, lpDDSurfaceDesc2, sizeof(DM->ddsd));
    if (!DM->bPalette) {
        DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwRBitMask, &DM->rshift, &DM->rbpp);
        DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwGBitMask, &DM->gshift, &DM->gbpp);
        DXBitMask2ShiftCnt(lpDDSurfaceDesc2->ddpfPixelFormat.dwBBitMask, &DM->bshift, &DM->bbpp);
        Log(3, "%d x %d - %d Bit - %d%d%d", DM->w, DM->h, DM->bpp, DM->rbpp, DM->gbpp, DM->bbpp);
    } else {
        Log(3, "%d x %d - %d Bit - Palette", DM->w, DM->h, DM->bpp);
    }
    DDInfo->nDisplayModes++;
    return DDENUMRET_OK;
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

HRESULT CALLBACK DXEnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
    DXD3DDEVICE *d3d;
    DXZBUFFERINFO *zbuffer;
    sint32 nZBufferInfos;

    d3d = (DXD3DDEVICE *)lpContext;
    nZBufferInfos = d3d->nZBufferInfos;
    d3d->ZBufferInfos = (DXZBUFFERINFO *)AddStruct(d3d->ZBufferInfos, nZBufferInfos, sizeof(DXZBUFFERINFO));
    zbuffer = &d3d->ZBufferInfos[nZBufferInfos];
    memcpy(&zbuffer->ddpf, lpDDPixFmt, sizeof(DDPIXELFORMAT));
    zbuffer->bpp = lpDDPixFmt->dwRGBBitCount;
    Log(3, "%d Bit", zbuffer->bpp);
    d3d->nZBufferInfos++;
    return D3DENUMRET_OK;
}

sint32 DXCreateSurface(LPDIRECTDRAW4 dd, LPDDSURFACEDESC2 desc, LPDIRECTDRAWSURFACE4 *surf)
{
    Log(2, "DXCreateSurface");
    if (DXAttempt(IDirectDraw4_CreateSurface(dd, desc, surf, 0)) == DD_OK)
        return 1;
    Log(1, "DXCreateSurface Failed");
    return 0;
}

sint32 DXSetVideoMode(LPDIRECTDRAW4 dd, sint32 dwWidth, sint32 dwHeight, sint32 dwBPP)
{
    Log(2, "DXSetVideoMode");
    Log(5, "SetDisplayMode - %dx%dx%d", dwWidth, dwHeight, dwBPP);
    if (DXAttempt(IDirectDraw4_SetDisplayMode(dd, dwWidth, dwHeight, dwBPP, 0, 0)) != DD_OK)
        return 0;
    return 1;
}

sint32 DXCreateD3DDevice(LPDIRECT3D3 d3d, GUID guid, LPDIRECTDRAWSURFACE4 surf, LPDIRECT3DDEVICE3 *device)
{
    Log(2, "DXCreateD3DDevice");
    if (DXAttempt(IDirect3D3_CreateDevice(d3d, &guid, surf, device, 0)) == DD_OK) {
        Log(2, "DXCreateD3DDevice Successful");
        return 1;
    }
    Log(1, "DXCreateD3DDevice Failed");
    return 0;
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
