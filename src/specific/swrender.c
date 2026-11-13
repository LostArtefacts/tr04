#include <windows.h>
#include <string.h>
#include "polyinsert.h"
#include "swrender.h"
#include "function_stubs.h"
#include "dxshell.h"
#include "winmain.h"

typedef struct sw_device {
    IDirect3DDevice3Vtbl *lpVtbl;
    sint32 refcount;
    void *viewport;
    void *target;
    HGLOBAL hmem;
    void *buffer;
    sint32 width;
    sint32 height;
    sint32 in_scene;
} SW_DEVICE;

extern float SW_ZLimit;
extern LPDIRECT3DDEVICE3 SW_RealD3DDevice;
extern HGLOBAL SW_SpanBufferHandle;
extern void *SW_SpanBuffer;
extern void *SW_Surface;
extern sint32 SW_SurfacePitch;
extern sint32 SW_FrameState;
extern sint32 SW_ColorOpScale;
extern DDSURFACEDESC2 SW_SurfDesc;
extern sint32 SW_NumLocked;
extern LPDIRECTDRAWSURFACE4 SW_LockedSurfaces[256];

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

HRESULT __stdcall SW_D3D_SetRenderState(SW_DEVICE *dev, D3DRENDERSTATETYPE type, DWORD state)
{
    if (type == D3DRENDERSTATE_FOGENABLE)
        IDirect3DDevice3_SetRenderState(SW_RealD3DDevice, D3DRENDERSTATE_FOGENABLE, state);
    return SW_SetRenderStateFlags(type, state);
}

HRESULT __stdcall SW_D3D_DrawPrimitive(SW_DEVICE *dev, D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags)
{
    return j_SW_DrawPrimitives(type, fvf, verts, count, flags);
}

HRESULT __stdcall SW_D3D_SetTextureStageState(SW_DEVICE *dev, DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
{
    switch (type) {
    case D3DTSS_COLOROP:
        switch (value) {
        case D3DTOP_MODULATE:
            SW_SetColorOpScale(1);
            return 0;
        case D3DTOP_MODULATE2X:
            SW_SetColorOpScale(2);
            return 0;
        case D3DTOP_MODULATE4X:
            SW_SetColorOpScale(4);
            return 0;
        }
        break;
    case D3DTSS_MAGFILTER:
        if (value == D3DTFG_LINEAR) {
            SW_SetRenderStateFlags(D3DRENDERSTATE_TEXTUREMAG, 2);
            return 0;
        }
        SW_SetRenderStateFlags(D3DRENDERSTATE_TEXTUREMAG, 1);
        return 0;
    }
    return 0;
}

HRESULT __stdcall SW_D3D_BeginScene(SW_DEVICE *dev)
{
    if (dev->in_scene) {
        other_log("D3DFAKE: Allready in scene!!\n");
        IDirect3DDevice3_EndScene((LPDIRECT3DDEVICE3)dev);
    }
    dev->in_scene = 1;
    if (SW_NumLocked)
        other_log("D3DFAKE: %i still locked!\n", SW_NumLocked);
    SW_NumLocked = 0;
    SW_SetSurface(dev->buffer, dev->width << 2);
    return SW_BeginFrame();
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

HRESULT __stdcall SW_D3D_EnumTextureFormats(SW_DEVICE *dev, LPD3DENUMPIXELFORMATSCALLBACK callback, LPVOID ctx)
{
    DDPIXELFORMAT pf;

    memset(&pf, 0, sizeof(DDPIXELFORMAT));
    pf.dwSize = sizeof(DDPIXELFORMAT);
    pf.dwRGBBitCount = 32;
    pf.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
    pf.dwRGBAlphaBitMask = 0xFF000000;
    pf.dwRBitMask = 0xFF0000;
    pf.dwGBitMask = 0xFF00;
    pf.dwBBitMask = 0xFF;
    callback(&pf, ctx);
    return 0;
}

HRESULT __stdcall SW_D3D_SetLightState(SW_DEVICE *dev, D3DLIGHTSTATETYPE type, DWORD state)
{
    return IDirect3DDevice3_SetLightState(SW_RealD3DDevice, type, state);
}

HRESULT __stdcall SW_D3D_SetTransform(SW_DEVICE *dev, D3DTRANSFORMSTATETYPE type, LPD3DMATRIX matrix)
{
    return IDirect3DDevice3_SetTransform(SW_RealD3DDevice, type, matrix);
}

HRESULT SW_ClearBuffer(SW_DEVICE *dev, DWORD count, D3DRECT *rect, DWORD flags, D3DCOLOR col, D3DVALUE z, DWORD stencil)
{
    if (!dev)
        return 0;
    if (flags & D3DCLEAR_TARGET) {
        dev = dev->buffer;
        flags = SW_SurfDesc.dwWidth * SW_SurfDesc.dwHeight;
        __asm {
            mov ecx, flags
            mov edi, dev
            mov eax, col
            rep stosd
        }
    }
    return 0;
}

void SW_FreeBuffer(void)
{
    GlobalFree(SW_SpanBufferHandle);
    SW_SpanBuffer = 0;
}

HRESULT SW_SetSurface(void *surface, sint32 pitch)
{
    SW_Surface = surface;
    SW_SurfacePitch = pitch;
    return 0;
}

sint32 SW_SetZLimit(float limit)
{
    SW_ZLimit = limit;
    return 0;
}

sint32 SW_BeginFrame(void)
{
    SW_FrameState = 0;
    return 0;
}

sint32 SW_EndFrame(void)
{
    return 1;
}

sint32 SW_SetColorOpScale(sint32 scale)
{
    SW_ColorOpScale = scale;
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
