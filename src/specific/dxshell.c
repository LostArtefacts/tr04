#include <stdlib.h>
#include <stdio.h>
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

BOOL CALLBACK DXEnumDirectDraw(GUID *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
{
    DXINFO *dxinfo;
    DXDIRECTDRAWINFO *DDInfo;
    sint32 nDDInfo;

    Log(2, "DXEnumDirectDraw");
    dxinfo = (DXINFO *)lpContext;
    nDDInfo = dxinfo->nDDInfo;
    dxinfo->DDInfo = (DXDIRECTDRAWINFO *)AddStruct(dxinfo->DDInfo, nDDInfo, sizeof(DXDIRECTDRAWINFO));
    DDInfo = &dxinfo->DDInfo[nDDInfo];
    if (!lpGUID) {
        DDInfo->lpGuid = 0;
    } else {
        DDInfo->lpGuid = &DDInfo->Guid;
        DDInfo->Guid = *lpGUID;
    }
    lstrcpy(DDInfo->About, lpDriverDescription);
    lstrcpy(DDInfo->Name, lpDriverName);
    Log(5, "Obtaining Information For %s", lpDriverDescription);
    if (DXDDCreate(lpGUID, (void **)&G_ddraw)) {
        DXAttempt(IDirectDraw4_GetDeviceIdentifier(G_ddraw, &DDInfo->DDIdentifier, 0));
        Log(5, "Found - %s\nDriver %s Version %d.%d.%d.%d", DDInfo->DDIdentifier.szDescription, DDInfo->DDIdentifier.szDriver,
            HIWORD(DDInfo->DDIdentifier.liDriverVersion.HighPart), LOWORD(DDInfo->DDIdentifier.liDriverVersion.HighPart),
            HIWORD(DDInfo->DDIdentifier.liDriverVersion.LowPart), LOWORD(DDInfo->DDIdentifier.liDriverVersion.LowPart));
        memset(&DDInfo->DDCaps, 0, sizeof(DDInfo->DDCaps));
        DDInfo->DDCaps.dwSize = sizeof(DDCAPS);
        Log(5, "Getting Device Capabilities");
        DXAttempt(IDirectDraw4_GetCaps(G_ddraw, &DDInfo->DDCaps, 0));
        Log(5, "Enumerating Display Modes");
        DXSetCooperativeLevel(G_ddraw, 0, DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES | DDSCL_NORMAL | DDSCL_ALLOWMODEX);
        Log(2, "DXEnumDisplayModes");
        DXAttempt(IDirectDraw4_EnumDisplayModes(G_ddraw, 0, 0, (LPVOID)DDInfo, DXEnumDisplayModes));
        if (DXD3DCreate(G_ddraw, (void **)&G_d3d)) {
            Log(2, "DXEnumDirect3D");
            DXAttempt(IDirect3D3_EnumDevices(G_d3d, DXEnumDirect3D, (LPVOID)DDInfo));
            if (G_d3d) {
                Log(4, "Released %s @ %x - RefCnt = %d", "Direct3D", G_d3d, IDirect3D3_Release(G_d3d));
                G_d3d = 0;
            } else {
                Log(1, "%s Attempt To Release NULL Ptr", "Direct3D");
            }
        }
        DXSetCooperativeLevel(G_ddraw, 0, DDSCL_NORMAL);
        if (G_ddraw) {
            Log(4, "Released %s @ %x - RefCnt = %d", "DirectDraw", G_ddraw, IDirectDraw4_Release(G_ddraw));
            G_ddraw = 0;
        } else {
            Log(1, "%s Attempt To Release NULL Ptr", "DirectDraw");
        }
    }
    dxinfo->nDDInfo++;
    return DDENUMRET_OK;
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

HRESULT CALLBACK DXEnumTextureFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
    DXD3DDEVICE *d3d;
    DXTEXTUREINFO *tex;
    sint32 nTextureInfos;

    if (!(lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS) || !(lpDDPixFmt->dwFlags & DDPF_RGB))
        return DDENUMRET_OK;
    d3d = (DXD3DDEVICE *)lpContext;
    nTextureInfos = d3d->nTextureInfos;
    d3d->TextureInfos = (DXTEXTUREINFO *)AddStruct(d3d->TextureInfos, nTextureInfos, sizeof(DXTEXTUREINFO));
    tex = &d3d->TextureInfos[nTextureInfos];
    memcpy(&tex->ddpf, lpDDPixFmt, sizeof(DDPIXELFORMAT));
    if (lpDDPixFmt->dwFlags & DDPF_PALETTEINDEXED8) {
        tex->bPalette = 1;
        tex->bpp = 8;
        Log(3, "8 Bit");
    } else if (lpDDPixFmt->dwFlags & DDPF_PALETTEINDEXED4) {
        tex->bPalette = 1;
        tex->bpp = 4;
        Log(3, "4 Bit");
    } else {
        tex->bPalette = 0;
        tex->bpp = lpDDPixFmt->dwRGBBitCount;
        if (lpDDPixFmt->dwFlags & DDPF_RGB) {
            if (lpDDPixFmt->dwFlags & DDPF_ALPHAPIXELS) {
                DXBitMask2ShiftCnt(lpDDPixFmt->dwRBitMask, &tex->rshift, &tex->rbpp);
                DXBitMask2ShiftCnt(lpDDPixFmt->dwGBitMask, &tex->gshift, &tex->gbpp);
                DXBitMask2ShiftCnt(lpDDPixFmt->dwBBitMask, &tex->bshift, &tex->bbpp);
                DXBitMask2ShiftCnt(lpDDPixFmt->dwRGBAlphaBitMask, &tex->ashift, &tex->abpp);
                tex->bAlpha = 1;
                Log(3, "%d Bit %d%d%d%d RGBA", tex->bpp, tex->rbpp, tex->gbpp, tex->bbpp, tex->abpp);
            } else {
                DXBitMask2ShiftCnt(lpDDPixFmt->dwRBitMask, &tex->rshift, &tex->rbpp);
                DXBitMask2ShiftCnt(lpDDPixFmt->dwGBitMask, &tex->gshift, &tex->gbpp);
                DXBitMask2ShiftCnt(lpDDPixFmt->dwBBitMask, &tex->bshift, &tex->bbpp);
                Log(3, "%d Bit %d%d%d RGB", tex->bpp, tex->rbpp, tex->gbpp, tex->bbpp);
            }
        }
    }
    d3d->nTextureInfos++;
    return D3DENUMRET_OK;
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

sint32 DXChangeVideoMode(void)
{
    sint32 val;

    Log(2, "DXChangeVideoMode");
    G_dxptr->Flags |= 0x40;
    IDirect3D3_EvictManagedTextures(G_dxptr->lpD3D);
    val = DXCreate(0, 0, 0, G_dxptr->Flags, G_dxptr, G_dxptr->hWnd, G_dxptr->WindowStyle);
    G_dxptr->Flags ^= 0x40;
    Log(2, "Exited DXChangeVideoMode %d", val);
    return val;
}

sint32 DXToggleFullScreen(void)
{
    DXDISPLAYMODE *dm;

    Log(2, "DXToggleFullScreen");
    if (G_dxptr->Flags & 2) {
        Log(5, "Switching To Full Screen");
        G_dxptr->Flags ^= 2;
        G_dxptr->Flags |= 65;
        G_dxptr->Flags |= 64;
        IDirect3D3_EvictManagedTextures(G_dxptr->lpD3D);
        dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
        DXCreate(dm->w, dm->h, dm->bpp, G_dxptr->Flags, G_dxptr, G_dxptr->hWnd, G_dxptr->WindowStyle);
        G_dxptr->Flags ^= 64;
        return 1;
    }
    Log(5, "Switching To A Window");
    G_dxptr->Flags ^= 1;
    G_dxptr->Flags |= 66;
    IDirect3D3_EvictManagedTextures(G_dxptr->lpD3D);
    dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
    DXCreate(dm->w, dm->h, dm->bpp, G_dxptr->Flags, G_dxptr, G_dxptr->hWnd, G_dxptr->WindowStyle);
    G_dxptr->Flags ^= 64;
    return 1;
}

sint32 DXCreateViewport(LPDIRECT3D3 d3d, LPDIRECT3DDEVICE3 device, sint32 w, sint32 h, LPDIRECT3DVIEWPORT3 *viewport)
{
    D3DVIEWPORT2 vp2;

    Log(2, "DXCreateViewport");
    if (DXAttempt(IDirect3D3_CreateViewport(d3d, viewport, 0)) != DD_OK)
        return 0;
    if (DXAttempt(IDirect3DDevice3_AddViewport(device, *viewport)) != DD_OK)
        return 0;
    memset(&vp2, 0, sizeof(D3DVIEWPORT2));
    vp2.dwSize = sizeof(D3DVIEWPORT2);
    vp2.dvClipWidth = (float)w;
    vp2.dvClipHeight = (float)h;
    vp2.dwX = 0;
    vp2.dwY = 0;
    vp2.dvClipX = 0;
    vp2.dvClipY = 0;
    vp2.dvMinZ = 0;
    vp2.dwWidth = w;
    vp2.dvMaxZ = 1;
    vp2.dwHeight = h;
    if (DXAttempt(IDirect3DViewport3_SetViewport2(*viewport, &vp2)) != DD_OK)
        return 0;
    DXAttempt(IDirect3DDevice3_SetCurrentViewport(device, *viewport));
    return 1;
}

HRESULT DXShowFrame(void)
{
    if (IDirectDrawSurface4_IsLost(G_dxptr->lpPrimaryBuffer)) {
        Log(3, "Restored Primary Buffer");
        DXAttempt(IDirectDrawSurface4_Restore(G_dxptr->lpPrimaryBuffer));
    }
    if (IDirectDrawSurface4_IsLost(G_dxptr->lpBackBuffer)) {
        Log(3, "Restored Back Buffer");
        DXAttempt(IDirectDrawSurface4_Restore(G_dxptr->lpBackBuffer));
    }
    if (!(App.dx.Flags & 0x82))
        return 0;
    if (G_dxptr->Flags & 2)
        return DXAttempt(IDirectDrawSurface4_Blt(G_dxptr->lpPrimaryBuffer, &G_dxptr->rScreen, G_dxptr->lpBackBuffer, &G_dxptr->rViewport, DDBLT_WAIT, 0));
    else
        return DXAttempt(IDirectDrawSurface4_Flip(G_dxptr->lpPrimaryBuffer, 0, DDFLIP_WAIT));
}

void DXMove(sint32 x, sint32 y)
{
    Log(2, "DXMove : x %d y %d", x, y);
    if (G_dxptr && !(G_dxptr->Flags & 1))
        SetRect(&G_dxptr->rScreen, x, y, x + G_dxptr->dwRenderWidth, y + G_dxptr->dwRenderHeight);
}

void DXSaveScreen(LPDIRECTDRAWSURFACE4 surf, const char *name)
{
    FILE *file;
    DDSURFACEDESC2 desc;
    sint16 *pSurf;
    sint16 *pDest;
    char *pM;
    uint32 val;
    static sint32 num = 0;
    sint32 r;
    sint32 g;
    sint32 b;
    char buf[16];
    uint32 h;
    uint32 w;

    memset(&desc, 0, sizeof(DDSURFACEDESC2));
    desc.dwSize = sizeof(DDSURFACEDESC2);
    DXAttempt(IDirectDrawSurface4_GetSurfaceDesc(surf, &desc));
    DXAttempt(IDirectDrawSurface4_Lock(surf, 0, &desc, DDLOCK_WAIT, 0));
    pSurf = (sint16 *)desc.lpSurface;
    sprintf(buf, "%s%04d.tga", name, num++);
    file = fopen(buf, "wb");
    if (file) {
        *(sint16 *)&tga_header[12] = (sint16)desc.dwWidth;
        *(sint16 *)&tga_header[14] = (sint16)desc.dwHeight;
        fwrite(tga_header, sizeof(tga_header), 1, file);
        pM = (char *)malloc(2 * desc.dwWidth * desc.dwHeight);
        pDest = (sint16 *)pM;
        pSurf += desc.dwHeight * (desc.lPitch / 2);
        for (h = 0; h < desc.dwHeight; h++) {
            for (w = 0; w < desc.dwWidth; w++) {
                val = pSurf[w];
                if (desc.ddpfPixelFormat.dwRBitMask == 0xF800) {
                    r = (val >> 11) & 0x1F;
                    g = (val >> 6) & 0x1F;
                    b = val & 0x1F;
                    *pDest++ = (sint16)((r << 10) + (g << 5) + b);
                } else {
                    *pDest++ = (sint16)val;
                }
            }
            pSurf -= desc.lPitch / 2;
        }
        fwrite(pM, 2 * desc.dwWidth * desc.dwHeight, 1, file);
        fclose(file);
        free(pM);
        buf[7]++;
        if ((uint8)buf[7] > '9') {
            buf[7] = '0';
            buf[6]++;
        }
    }
    DXAttempt(IDirectDrawSurface4_Unlock(surf, 0));
}

void DXInitKeyboard(HWND hwnd, HINSTANCE hinstance)
{
    LPDIRECTINPUT dinput;
    LPDIRECTINPUTDEVICE Keyboard;

    DXAttempt(DirectInputCreate(hinstance, 0x500, &dinput, 0));
    IDirectInput_QueryInterface(dinput, &IID_IDirectInput2, (void **)&G_dxptr->lpDirectInput);
    if (dinput) {
        Log(4, "Released %s @ %x - RefCnt = %d", "DirectInput", dinput, IDirectInput_Release(dinput));
        dinput = 0;
    } else {
        Log(1, "%s Attempt To Release NULL Ptr", "DirectInput");
    }
    DXAttempt(IDirectInput2_CreateDevice(G_dxptr->lpDirectInput, &GUID_SysKeyboard, &Keyboard, 0));
    IDirectInputDevice_QueryInterface(Keyboard, &IID_IDirectInputDevice2, (void **)&G_dxptr->Keyboard);
    if (Keyboard) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Keyboard", Keyboard, IDirectInputDevice_Release(Keyboard));
        Keyboard = 0;
    } else {
        Log(1, "%s Attempt To Release NULL Ptr", "Keyboard");
    }
    DXAttempt(IDirectInputDevice2_SetCooperativeLevel(G_dxptr->Keyboard, hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND));
    DXAttempt(IDirectInputDevice2_SetDataFormat(G_dxptr->Keyboard, &c_dfDIKeyboard));
    DXAttempt(IDirectInputDevice2_Acquire(G_dxptr->Keyboard));
    memset(keymap, 0, sizeof(keymap));
    memset(keymap2, 0, sizeof(keymap2));
    memset(DXUnk754124, 0, sizeof(DXUnk754124));
    DXUnk7541ac = 0;
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
