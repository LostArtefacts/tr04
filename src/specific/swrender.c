#include <windows.h>
#include <string.h>
#include "polyinsert.h"
#include "swrender.h"

typedef struct sw_device {
    void *lpVtbl;
    sint32 refcount;
    void *viewport;
    void *target;
    HGLOBAL hmem;
    void *buffer;
    sint32 width;
    sint32 height;
} SW_DEVICE;

extern float SW_ZLimit;
extern LPDIRECT3DDEVICE3 SW_RealD3DDevice;

HRESULT __stdcall SW_D3D_QueryInterface(SW_DEVICE *dev)
{
    return 0x887602C1;
}

HRESULT __stdcall SW_D3D_GetCaps(SW_DEVICE *dev, D3DDEVICEDESC *hal, D3DDEVICEDESC *hel)
{
    memset(hel, 0, sizeof(D3DDEVICEDESC));
    memset(hal, 0, sizeof(D3DDEVICEDESC));
    hal->dwSize = sizeof(D3DDEVICEDESC);
    hel->dwSize = sizeof(D3DDEVICEDESC);
    hel->dwFlags = 0xC3;
    hel->dwDeviceRenderBitDepth = DDBD_16 | DDBD_32;
    hel->dcmColorModel = D3DCOLOR_RGB;
    hel->dwTextureOpCaps = 0x1A;
    hel->dpcTriCaps.dwRasterCaps = 0x80;
    return 0x887602C1;
}

ULONG __stdcall SW_D3D_AddRef(SW_DEVICE *dev)
{
    return ++dev->refcount;
}

ULONG __stdcall SW_D3D_Release(SW_DEVICE *dev)
{
    dev->refcount--;
    if (dev->hmem) {
        GlobalFree(dev->hmem);
        dev->hmem = 0;
    }
    if (!dev->refcount)
        SW_FreeBuffer();
    return dev->refcount;
}

HRESULT __stdcall SW_D3D_SetCurrentViewport(SW_DEVICE *dev, void *viewport)
{
    dev->viewport = viewport;
    return 0;
}

HRESULT __stdcall SW_D3D_GetCurrentViewport(SW_DEVICE *dev, void **viewport)
{
    *viewport = dev->viewport;
    return 0;
}

HRESULT __stdcall SW_D3D_ViewportStub(void *iface, void *arg)
{
    return 0;
}

HRESULT __stdcall SW_D3D_SetRenderTarget(SW_DEVICE *dev, void *target, DWORD flags)
{
    dev->target = target;
    return 0;
}

HRESULT __stdcall SW_D3D_GetRenderTarget(SW_DEVICE *dev, void **target)
{
    *target = dev->target;
    return 0;
}

HRESULT __stdcall SW_D3D_SetLightState(SW_DEVICE *dev, D3DLIGHTSTATETYPE type, DWORD state)
{
    return IDirect3DDevice3_SetLightState(SW_RealD3DDevice, type, state);
}

sint32 SW_SetZLimit(float limit)
{
    SW_ZLimit = limit;
    return 0;
}

LPVOID DBLog_MapView(HANDLE hMap)
{
    return MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0x100000);
}

HANDLE DBLog_OpenMapping(LPCSTR name)
{
    return OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, name);
}
