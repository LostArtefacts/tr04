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

typedef struct sw_vertex {
    float sx;
    float sy;
    float sz;
    float rhw;
    D3DCOLOR color;
    D3DCOLOR specular;
    float tu;
    float tv;
    float tu2;
    float tv2;
} SW_VERTEX;

typedef struct sw_span {
    sint32 x;
    sint32 n;
    sint32 tex[4];
    uint16 c[4];
} SW_SPAN;

extern float SW_ZLimit;

HRESULT __stdcall SW_D3D_EndScene(SW_DEVICE *dev);
extern LPDIRECT3DDEVICE3 SW_RealD3DDevice;
extern IDirect3DDevice3Vtbl SW_Vtbl;
extern SW_DEVICE SW_Device;
extern HGLOBAL SW_SpanBufferHandle;
extern SW_SPAN *SW_SpanBuffer;
extern void *SW_Surface;
extern sint32 SW_SurfacePitch;
extern sint32 SW_FrameState;
extern sint32 SW_ColorOpScale;
extern DDSURFACEDESC2 SW_SurfDesc;
extern sint32 SW_NumLocked;
extern sint32 SW_UseMMX;
extern sint32 SW_RenderFlags;
extern DWORD SW_TextureBits;
extern DWORD SW_FogColor;
extern sint32 SW_ActiveFlags;
extern sint32 (*SW_TriSetupFunc)(void *v0, void *v1, void *v2);
extern void (*SW_SpanFunc)(void);
extern void (*SW_SpanFogFunc)(void);
extern SW_SPAN *SW_SpanEnd;
extern sint32 SW_SpanY;
extern sint16 SW_SpanGrad[4];
void nullsub_1();
void SW_DrawLineG(SW_VERTEX *v0, SW_VERTEX *v1);
extern UINT DBLog_Message;
extern LPDIRECTDRAWSURFACE4 SW_LockedSurfaces[256];
extern LPDIRECT3DTEXTURE2 SW_LockedTextures[256];
extern void *SW_LockedBits[256];
extern LPDIRECTDRAWSURFACE4 SW_TexSurface;
extern sint32 SW_RecipTable[4096];
extern uint8 SW_MulTable[65536];
extern sint32 SW_TexShift;
extern sint32 SW_SpanDU;
extern sint32 SW_SpanDV;
extern sint32 SW_MipOffsets[5];
extern SW_VERTEX *SW_TriV0;
extern sint32 SW_SpanFogGrad;

#pragma data_seg(".text")
char SW_Kernel32Name[] = "kernel32.dll";
#pragma data_seg()
#pragma data_seg(".text$s")
char SW_SleepName[] = "Sleep";
#pragma data_seg()

void __fastcall SW_WaitUntilInScene(volatile sint32 *flag)
{
    do {
        SW_SleepTick();
    } while (!*flag);
}

void __fastcall SW_WaitWhileInScene(volatile sint32 *flag)
{
    do {
        SW_SleepTick();
    } while (*flag);
}

void SW_SleepTick(void)
{
    ((void (WINAPI *)(DWORD))SW_GetSleepProc(SW_GetKernelModule()))(10);
}

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

HRESULT __stdcall SW_D3D_AddViewport(void *iface, void *arg)
{
    return 0;
}

HRESULT __stdcall SW_D3D_DeleteViewport(void *iface, void *arg)
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

HMODULE SW_GetKernelModule(void)
{
    return GetModuleHandleA(SW_Kernel32Name);
}

HRESULT __stdcall SW_D3D_SetTexture(SW_DEVICE *dev, DWORD stage, LPDIRECT3DTEXTURE2 tex)
{
    DDSURFACEDESC2 desc;
    HRESULT r;
    sint32 i;

    if (dev->in_scene) {
        if (tex) {
            for (i = 0; i < SW_NumLocked; i++) {
                if (tex == SW_LockedTextures[i]) {
                    SW_SetRenderStateFlags(D3DRENDERSTATE_TEXTUREHANDLE, (DWORD)SW_LockedBits[i]);
                    return 0;
                }
            }
            memset(&desc, 0, sizeof(DDSURFACEDESC2));
            desc.dwSize = sizeof(DDSURFACEDESC2);
            IDirect3DTexture2_QueryInterface(tex, &IID_IDirectDrawSurface4, (LPVOID *)&SW_TexSurface);
            r = IDirectDrawSurface4_Lock(SW_TexSurface, 0, &desc, DDLOCK_NOSYSLOCK, 0);
            if (r) {
                other_log("D3DFAKE: Texture lock Failed! (%x) (%x,%i)\n", SW_TexSurface, r, r & 0xFFF);
                SW_SetRenderStateFlags(D3DRENDERSTATE_TEXTUREHANDLE, 0);
                return 0;
            }
            SW_LockedSurfaces[SW_NumLocked] = SW_TexSurface;
            SW_LockedTextures[SW_NumLocked] = tex;
            SW_LockedBits[SW_NumLocked] = desc.lpSurface;
            SW_NumLocked++;
            SW_SetRenderStateFlags(D3DRENDERSTATE_TEXTUREHANDLE, (DWORD)desc.lpSurface);
            return 0;
        }
        SW_SetRenderStateFlags(D3DRENDERSTATE_TEXTUREHANDLE, 0);
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

HRESULT __stdcall SW_D3D_EndScene(SW_DEVICE *dev)
{
    sint32 i;
    sint32 r;
    sint32 y;

    SW_EndFrame();
    for (i = 0; i < SW_NumLocked; i++) {
        IDirectDrawSurface4_Unlock(SW_LockedSurfaces[i], 0);
        IDirectDrawSurface4_Release(SW_LockedSurfaces[i]);
    }
    memset(&SW_SurfDesc, 0, sizeof(DDSURFACEDESC2));
    SW_NumLocked = 0;
    SW_SurfDesc.dwSize = sizeof(DDSURFACEDESC2);
    r = DXAttempt(IDirectDrawSurface4_Lock((LPDIRECTDRAWSURFACE4)dev->target, 0, &SW_SurfDesc, DDLOCK_WAIT, 0));
    if (r) {
        DXAttempt(IDirectDrawSurface4_Restore(App.dx.lpPrimaryBuffer));
        DXAttempt(IDirectDrawSurface4_Restore(App.dx.lpBackBuffer));
        other_log("Can't lock rendertarget!!", r);
    } else {
        if (SW_SurfDesc.ddpfPixelFormat.dwRGBBitCount == 16) {
            uint8 *dst;
            uint32 *src;

            dst = SW_SurfDesc.lpSurface;
            src = dev->buffer;
            if (SW_SurfDesc.ddpfPixelFormat.dwGBitMask == 0x7E0) {
                for (y = 0; y < (sint32)SW_SurfDesc.dwHeight; y++) {
                    SW_Convert32To565(dst, src, SW_SurfDesc.dwWidth);
                    src += dev->width;
                    dst += SW_SurfDesc.lPitch;
                }
            } else {
                for (y = 0; y < (sint32)SW_SurfDesc.dwHeight; y++) {
                    SW_Convert32To555(dst, src, SW_SurfDesc.dwWidth);
                    src += dev->width;
                    dst += SW_SurfDesc.lPitch;
                }
            }
        } else {
            uint8 *dst;
            uint32 *src;

            dst = SW_SurfDesc.lpSurface;
            src = dev->buffer;
            if (SW_SurfDesc.ddpfPixelFormat.dwRGBBitCount == 24) {
                for (y = 0; y < (sint32)SW_SurfDesc.dwHeight; y++) {
                    SW_Convert32To24(dst, src, SW_SurfDesc.dwWidth);
                    src += dev->width;
                    dst += SW_SurfDesc.lPitch;
                }
            } else {
                for (y = 0; y < (sint32)SW_SurfDesc.dwHeight; y++) {
                    SW_CopyBuffer(dst, src, SW_SurfDesc.dwWidth);
                    src += dev->width;
                    dst += SW_SurfDesc.lPitch;
                }
            }
        }
        IDirectDrawSurface4_Unlock((LPDIRECTDRAWSURFACE4)dev->target, 0);
    }
    SW_SetSurface(0, 0);
    dev->in_scene = 0;
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

HRESULT SW_D3D_CreateDevice(void)
{
    SW_RealD3DDevice = App.dx.lpD3DDevice;
    SW_Vtbl.GetDirect3D = SW_Vtbl.NextViewport = SW_Vtbl.GetStats = SW_Vtbl.QueryInterface = (void *)SW_D3D_QueryInterface;
    SW_Vtbl.Release = (void *)SW_D3D_Release;
    SW_Vtbl.AddRef = (void *)SW_D3D_AddRef;
    SW_Vtbl.GetCaps = (void *)SW_D3D_GetCaps;
    SW_Vtbl.AddViewport = (void *)SW_D3D_AddViewport;
    SW_Vtbl.DeleteViewport = (void *)SW_D3D_DeleteViewport;
    SW_Vtbl.EnumTextureFormats = (void *)SW_D3D_EnumTextureFormats;
    SW_Vtbl.BeginScene = (void *)SW_D3D_BeginScene;
    SW_Vtbl.EndScene = (void *)SW_D3D_EndScene;
    SW_Vtbl.SetCurrentViewport = (void *)SW_D3D_SetCurrentViewport;
    SW_Vtbl.GetCurrentViewport = (void *)SW_D3D_GetCurrentViewport;
    SW_Vtbl.SetRenderTarget = (void *)SW_D3D_SetRenderTarget;
    SW_Vtbl.GetRenderTarget = (void *)SW_D3D_GetRenderTarget;
    SW_Vtbl.SetRenderState = (void *)SW_D3D_SetRenderState;
    SW_Vtbl.SetTexture = (void *)SW_D3D_SetTexture;
    SW_Vtbl.SetTextureStageState = (void *)SW_D3D_SetTextureStageState;
    SW_Vtbl.DrawPrimitive = (void *)SW_D3D_DrawPrimitive;
    SW_Vtbl.SetLightState = (void *)SW_D3D_SetLightState;
    SW_Vtbl.SetTransform = (void *)SW_D3D_SetTransform;
    SW_Device.refcount = 1;
    SW_Device.lpVtbl = &SW_Vtbl;
    if (App.dx.Flags & 1)
        SW_Device.target = App.dx.lpPrimaryBuffer;
    else
        SW_Device.target = App.dx.lpBackBuffer;
    SW_Device.in_scene = 0;
    SW_NumLocked = 0;
    memset(&SW_SurfDesc, 0, sizeof(DDSURFACEDESC2));
    App.dx.lpD3DDevice = (LPDIRECT3DDEVICE3)&SW_Device;
    SW_SurfDesc.dwSize = sizeof(DDSURFACEDESC2);
    IDirectDrawSurface4_GetSurfaceDesc(App.dx.lpBackBuffer, &SW_SurfDesc);
    SW_Device.width = SW_SurfDesc.dwWidth;
    SW_Device.height = SW_SurfDesc.dwHeight;
    SW_Device.hmem = GlobalAlloc(0, SW_Device.width * SW_Device.height * 4 + 32);
    SW_Device.buffer = (void *)(((sint32)SW_Device.hmem + 32) & ~0x1F);
    if (!App.mmx)
        SW_D3D_SetupSurface(SW_Device.width, SW_Device.height, 0);
    else
        SW_D3D_SetupSurface(SW_Device.width, SW_Device.height, 1);
    SW_SetZLimit(0.6F);
    return 0;
}

HRESULT SW_ClearBuffer(SW_DEVICE *dev, DWORD count, D3DRECT *rect, DWORD flags, D3DCOLOR col, D3DVALUE z, DWORD stencil)
{
    D3DCOLOR *p;

    if (!dev)
        return 0;
    if (flags & D3DCLEAR_TARGET) {
        p = dev->buffer;
        for (flags = SW_SurfDesc.dwWidth * SW_SurfDesc.dwHeight; flags; flags--)
            *p++ = col;
    }
    return 0;
}

void SW_D3D_SetupSurface(sint32 w, sint32 h, sint32 mmx)
{
    sint32 *p;
    sint32 i, j, k, v;
    uint8 *row;
    float f;

    SW_RecipTable[0] = 0xFF800000;
    SW_UseMMX = mmx;
    p = &SW_RecipTable[1];
    i = 1;
    do {
        f = 1.0F / (1.0F + (float)i * (1.0F / 4096.0F));
        i++;
        *p++ = (*(sint32 *)&f & 0x7FFFFF) - 0x1000000;
    } while (i < 4096);
    for (k = 0, row = SW_MulTable; (sint32)row < (sint32)&SW_MulTable[65536]; row += 256, k++) {
        for (j = 0, v = 0; j < 256; j++) {
            row[j] = (uint8)(v >> 8);
            v += k;
        }
    }
    if (!SW_SpanBuffer) {
        SW_SpanBufferHandle = GlobalAlloc(0, (sint32)((float)(((w >> 1) + 32) * h) * 0.75F));
        SW_SpanBuffer = (void *)(((sint32)SW_SpanBufferHandle + 32) & ~0x1F);
    }
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

sint32 SW_SelectRasterizer(void *v0, void *v1, void *v2)
{
    sint32 f;

    f = SW_RenderFlags;
    if (SW_ActiveFlags != f) {
        SW_ActiveFlags = f;
        if (!SW_TextureBits && (f & 0x200)) {
            f &= ~0x200;
            SW_ActiveFlags = f;
        }
        if (SW_UseMMX) {
            if (f & 0x200) {
                if (f & 0x100) {
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTLit : SW_TriSetupGT;
                    if ((f & 0x80) && (f & 0x2)) {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTPAddBilinear : SW_SpanGTPAdd;
                        SW_SpanFogFunc = SW_SpanGFogAdd;
                    } else if ((f & 0x8) && (f & 0x2)) {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTPAlphaBilinear : SW_SpanGTPAlpha;
                        SW_SpanFogFunc = SW_SpanGFogAdd;
                    } else if ((f & 0x4) && (f & 0x2)) {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTPAlphaBilinear : SW_SpanGTPKey;
                        SW_SpanFogFunc = SW_SpanGFogAdd;
                    } else {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTPBilinear : SW_SpanGTP;
                        SW_SpanFogFunc = SW_SpanGFogAdd;
                    }
                } else {
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGALit : SW_TriSetupG;
                    if ((f & 0x80) && (f & 0x2)) {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTAddBilinear : SW_SpanGTAdd;
                        SW_SpanFogFunc = SW_SpanGFog;
                    } else if ((f & 0x8) && (f & 0x2)) {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTAlphaBilinear : SW_SpanGTAlpha;
                        SW_SpanFogFunc = SW_SpanGFogAlpha;
                    } else if ((f & 0x4) && (f & 0x2)) {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTAlphaBilinear : SW_SpanGTKey;
                        SW_SpanFogFunc = SW_SpanGFogAlpha;
                    } else {
                        SW_SpanFunc = (f & 0x1) ? SW_SpanGTBilinear : SW_SpanGT;
                        SW_SpanFogFunc = SW_SpanGFog;
                    }
                }
            } else {
                SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTA : SW_TriSetupGA;
                if (f & 0x2) {
                    if (f & 0x80) {
                        SW_SpanFunc = SW_SpanGAdd;
                        SW_SpanFogFunc = SW_SpanGFog;
                    } else {
                        SW_SpanFunc = SW_SpanGAlpha;
                        SW_SpanFogFunc = SW_SpanGFog;
                    }
                } else {
                    SW_SpanFunc = SW_SpanG;
                    SW_SpanFogFunc = SW_SpanGFog;
                }
            }
        } else {
            if (f & 0x200) {
                if (f & 0x100) {
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTLit : SW_TriSetupGT;
                    if ((f & 0x80) && (f & 0x2)) {
                        SW_SpanFunc = SW_SpanGTPAddInt;
                        SW_SpanFogFunc = nullsub_1;
                    } else if ((f & 0x8) && (f & 0x2)) {
                        SW_SpanFunc = SW_SpanGTLitPersp;
                        SW_SpanFogFunc = nullsub_1;
                    } else if ((f & 0x4) && (f & 0x2)) {
                        SW_SpanFunc = SW_SpanGTPKeyInt;
                        SW_SpanFogFunc = nullsub_1;
                    } else {
                        SW_SpanFunc = SW_SpanGTPInt;
                        SW_SpanFogFunc = nullsub_1;
                    }
                } else {
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGALit : SW_TriSetupG;
                    if ((f & 0x80) && (f & 0x2)) {
                        SW_SpanFunc = SW_SpanGTAddInt;
                        SW_SpanFogFunc = nullsub_1;
                    } else if ((f & 0x8) && (f & 0x2)) {
                        SW_SpanFunc = SW_SpanGTLit;
                        SW_SpanFogFunc = nullsub_1;
                    } else if ((f & 0x4) && (f & 0x2)) {
                        SW_SpanFunc = SW_SpanGTKeyInt;
                        SW_SpanFogFunc = nullsub_1;
                    } else {
                        SW_SpanFunc = SW_SpanGTInt;
                        SW_SpanFogFunc = nullsub_1;
                    }
                }
            } else {
                SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTA : SW_TriSetupGA;
                if (f & 0x2) {
                    if (f & 0x80) {
                        SW_SpanFunc = SW_SpanGAddInt;
                        SW_SpanFogFunc = nullsub_1;
                    } else {
                        SW_SpanFunc = SW_SpanGLit;
                        SW_SpanFogFunc = nullsub_1;
                    }
                } else {
                    SW_SpanFunc = SW_SpanGInt;
                    SW_SpanFogFunc = nullsub_1;
                }
            }
        }
    }
    if (SW_TriSetupFunc)
        SW_TriSetupFunc(v0, v1, v2);
    return 0;
}

sint32 SW_SetColorOpScale(sint32 scale)
{
    SW_ColorOpScale = scale;
    return 0;
}

FARPROC SW_GetSleepProc(HMODULE module)
{
    return GetProcAddress(module, SW_SleepName);
}

HRESULT SW_SetRenderStateFlags(D3DRENDERSTATETYPE type, DWORD state)
{
    switch (type) {
    case D3DRENDERSTATE_FOGENABLE:
        if (state) {
            SW_RenderFlags |= 0x400;
            return 0;
        }
        SW_RenderFlags &= ~0x400;
        return 0;
    case D3DRENDERSTATE_FOGCOLOR:
        SW_FogColor = state;
        return 0;
    case D3DRENDERSTATE_TEXTUREMIN:
        if (state == D3DFILTER_LINEAR || state == D3DFILTER_NEAREST) {
            SW_RenderFlags &= ~0x40;
            return 0;
        }
        SW_RenderFlags |= 0x40;
        return 0;
    case D3DRENDERSTATE_TEXTUREMAG:
        if (state == D3DFILTER_LINEAR || state == D3DFILTER_MIPLINEAR) {
            SW_RenderFlags |= 0x1;
            return 0;
        }
        SW_RenderFlags &= ~0x1;
        return 0;
    case D3DRENDERSTATE_DESTBLEND:
        if (state == D3DBLEND_ONE) {
            SW_RenderFlags = (SW_RenderFlags & ~0x8) | 0x80;
            return 0;
        }
        if (state == D3DBLEND_SRCALPHA || state == D3DBLEND_INVSRCALPHA) {
            SW_RenderFlags = (SW_RenderFlags & ~0x80) | 0x8;
            return 0;
        }
        SW_RenderFlags &= ~0x88;
        return 0;
    case D3DRENDERSTATE_ALPHABLENDENABLE:
        if (state == 1) {
            SW_RenderFlags |= 0x2;
            return 0;
        }
        SW_RenderFlags &= ~0x2;
        return 0;
    case D3DRENDERSTATE_TEXTUREPERSPECTIVE:
        if (state) {
            SW_RenderFlags |= 0x100;
            return 0;
        }
        SW_RenderFlags &= ~0x100;
        return 0;
    case D3DRENDERSTATE_TEXTUREHANDLE:
        SW_TextureBits = state;
        if (state) {
            SW_RenderFlags |= 0x200;
            return 0;
        }
        SW_RenderFlags &= ~0x200;
        return 0;
    }
    return 0;
}

HRESULT SW_DrawPrimitives(D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags)
{
    SW_VERTEX *v;
    sint32 n, c, saved;

    if (SW_Surface) {
        switch (type) {
        case D3DPT_TRIANGLELIST:
            v = (SW_VERTEX *)verts;
            n = count;
            do {
                saved = SW_RenderFlags;
                if ((SW_RenderFlags & 0x8) && (v[0].color & v[2].color & v[1].color & 0xFF000000) == 0xFF000000)
                    SW_RenderFlags = (SW_RenderFlags & ~0x8) | 0x4;
                if ((SW_RenderFlags & 0x400) && (v[2].specular & v[0].specular & v[1].specular & 0xFE000000) == 0xFE000000)
                    SW_RenderFlags &= ~0x400;
                if (SW_RenderFlags & 0x100) {
                    c = 0;
                    if (*(sint32 *)&v[0].rhw < *(sint32 *)&SW_ZLimit) {
                        v[0].rhw = SW_ZLimit;
                        c = 1;
                    }
                    if (*(sint32 *)&v[1].rhw < *(sint32 *)&SW_ZLimit) {
                        v[1].rhw = SW_ZLimit;
                        c++;
                    }
                    if (*(sint32 *)&v[2].rhw < *(sint32 *)&SW_ZLimit) {
                        v[2].rhw = SW_ZLimit;
                        c++;
                    }
                    if (c == 3)
                        SW_RenderFlags &= ~0x100;
                }
                SW_SelectRasterizer(&v[0], &v[1], &v[2]);
                n -= 3;
                SW_RenderFlags = saved;
                v += 3;
            } while (n > 0);
            break;
        case D3DPT_LINELIST:
            v = (SW_VERTEX *)verts;
            for (count >>= 1; count; count--) {
                SW_DrawLineG(v, v + 1);
                v += 2;
            }
            break;
        case D3DPT_LINESTRIP:
            return 0;
        case D3DPT_TRIANGLESTRIP:
            return 0;
        case D3DPT_TRIANGLEFAN:
            return 0;
        }
    }
    return 0;
}

void SW_DrawLineG(SW_VERTEX *v0, SW_VERTEX *v1)
{
    uint32 *p;
    sint32 x0, y0, dx, dy, nx, ny, xs, ys, pitch, step, frac;
    sint32 r0, g0, c0, r1, g1, c1;
    sint32 dr, dg, db, dr2, dg2, db2;
    sint32 r, g, b;
    uint32 pix, col0, col1;

    dr2 = dg2 = db2 = 0;
    dr = dg = db = 0;
    x0 = (sint32)v0->sx;
    y0 = (sint32)v0->sy;
    col0 = v0->color;
    r0 = ((col0 >> 16) & 0xFF) << 16;
    g0 = ((col0 >> 8) & 0xFF) << 16;
    c0 = (col0 & 0xFF) << 16;
    col1 = v1->color;
    r1 = ((col1 >> 16) & 0xFF) << 16;
    g1 = ((col1 >> 8) & 0xFF) << 16;
    c1 = (col1 & 0xFF) << 16;
    pitch = SW_SurfacePitch >> 2;
    p = (uint32 *)SW_Surface + pitch * y0 + x0;
    dx = (sint32)v1->sx - x0;
    dy = (sint32)v1->sy - y0;
    if (!dx && !dy) {
        pix = *p;
        r = (pix & 0xFF0000) + (r0 & 0xFFFF0000);
        g = ((g0 >> 8) & 0xFFFFFF00) + (pix & 0xFF00);
        b = (pix & 0xFF) + (c0 >> 16);
        if (r > 0xFF0000)
            r = 0xFF0000;
        if (g > 0xFF00)
            g = 0xFF00;
        if (b > 0xFF)
            b = 0xFF;
        *p = b | g | r;
        return;
    }
    if (dx >= 0) {
        xs = 1;
    } else {
        xs = -1;
        dx = -dx;
    }
    if (dy >= 0) {
        ys = pitch;
    } else {
        ys = -SW_SurfacePitch >> 2;
        dy = -dy;
    }
    nx = dx;
    ny = dy;
    nx++;
    ny++;
    frac = 0;
    if (dx < dy) {
        if (ny) {
            dr = (r1 - r0) / ny;
            dg = (g1 - g0) / ny;
            db = (c1 - c0) / ny;
        }
        step = (nx << 16) / ny;
        for (; ny > 0; ny--) {
            pix = *p;
            r = (pix & 0xFF0000) + (r0 & 0xFFFF0000);
            g = ((g0 >> 8) & 0xFFFFFF00) + (pix & 0xFF00);
            b = (pix & 0xFF) + (c0 >> 16);
            if (r > 0xFF0000)
                r = 0xFF0000;
            if (g > 0xFF00)
                g = 0xFF00;
            if (b > 0xFF)
                b = 0xFF;
            *p = b | g | r;
            p += ys;
            frac += step;
            r0 += dr;
            g0 += dg;
            c0 += db;
            if (frac >= 0x10000) {
                frac -= 0x10000;
                p += xs;
            }
        }
    } else {
        step = (ny << 16) / nx;
        if (nx) {
            dr2 = (r1 - r0) / nx;
            dg2 = (g1 - g0) / nx;
            db2 = (c1 - c0) / nx;
        }
        for (; nx > 0; nx--) {
            pix = *p;
            r = (pix & 0xFF0000) + (r0 & 0xFFFF0000);
            g = ((g0 >> 8) & 0xFFFFFF00) + (pix & 0xFF00);
            b = (pix & 0xFF) + (c0 >> 16);
            if (r > 0xFF0000)
                r = 0xFF0000;
            if (g > 0xFF00)
                g = 0xFF00;
            if (b > 0xFF)
                b = 0xFF;
            *p = b | g | r;
            p += xs;
            frac += step;
            r0 += dr2;
            g0 += dg2;
            c0 += db2;
            if (frac >= 0x10000) {
                frac -= 0x10000;
                p += ys;
            }
        }
    }
}

HRESULT j_SW_DrawPrimitives(D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags)
{
    return SW_DrawPrimitives(type, fvf, verts, count, flags);
}

void SW_SpanG(void)
{
    SW_SPAN *s;
    uint8 *row, *p;
    uint16 c0, c1, c2, c3;
    sint32 n;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                do {
                    p[0] = c0 >> 8;
                    p[1] = c1 >> 8;
                    p[2] = c2 >> 8;
                    p[3] = c3 >> 8;
                    c0 += SW_SpanGrad[0];
                    c1 += SW_SpanGrad[1];
                    c2 += SW_SpanGrad[2];
                    c3 += SW_SpanGrad[3];
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGFogAdd(void)
{
    SW_SPAN *s;
    uint8 *row, *p;
    uint16 v0, v1, v2, d0, d1, d2;
    sint32 n, f, f0, f1, f2, x;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd && SW_FogColor) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        f0 = (SW_FogColor & 0xFF) << 7;
        f1 = ((SW_FogColor >> 8) & 0xFF) << 7;
        f2 = ((SW_FogColor >> 16) & 0xFF) << 7;
        d0 = ((sint16)SW_SpanFogGrad * f0 >> 16) << 1;
        d1 = ((sint16)SW_SpanFogGrad * f1 >> 16) << 1;
        d2 = ((sint16)SW_SpanFogGrad * f2 >> 16) << 1;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                f = (~s->tex[1] & 0xFFFF) >> 1;
                v0 = (f * f0 >> 16) << 2;
                v1 = (f * f1 >> 16) << 2;
                v2 = (f * f2 >> 16) << 2;
                do {
                    x = p[0] + (v0 >> 8);
                    p[0] = x > 0xFF ? 0xFF : x;
                    x = p[1] + (v1 >> 8);
                    p[1] = x > 0xFF ? 0xFF : x;
                    x = p[2] + (v2 >> 8);
                    p[2] = x > 0xFF ? 0xFF : x;
                    v0 -= d0;
                    v1 -= d1;
                    v2 -= d2;
                    p += 4;
                } while (--n);
                s = (SW_SPAN *)((sint32 *)(s + 1) + ((uint32)(s->n + 15) >> 4) * 2);
            } else {
                s = (SW_SPAN *)((sint32 *)(s + 1) + 2);
            }
            row += SW_SurfacePitch;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGFog(void)
{
    SW_SPAN *s;
    uint8 *row, *p;
    uint16 v0, v1, v2, d0, d1, d2;
    sint32 n, f, f0, f1, f2, x;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd && SW_FogColor) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        f0 = (SW_FogColor & 0xFF) << 7;
        f1 = ((SW_FogColor >> 8) & 0xFF) << 7;
        f2 = ((SW_FogColor >> 16) & 0xFF) << 7;
        d0 = ((sint16)SW_SpanFogGrad * f0 >> 16) << 1;
        d1 = ((sint16)SW_SpanFogGrad * f1 >> 16) << 1;
        d2 = ((sint16)SW_SpanFogGrad * f2 >> 16) << 1;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                f = (~s->tex[1] & 0xFFFF) >> 1;
                v0 = (f * f0 >> 16) << 2;
                v1 = (f * f1 >> 16) << 2;
                v2 = (f * f2 >> 16) << 2;
                do {
                    x = p[0] + (v0 >> 8);
                    p[0] = x > 0xFF ? 0xFF : x;
                    x = p[1] + (v1 >> 8);
                    p[1] = x > 0xFF ? 0xFF : x;
                    x = p[2] + (v2 >> 8);
                    p[2] = x > 0xFF ? 0xFF : x;
                    v0 -= d0;
                    v1 -= d1;
                    v2 -= d2;
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGFogAlpha(void)
{
    SW_SPAN *s;
    uint8 *row, *p;
    uint32 *tex;
    uint32 u, v, mask, fog;
    uint16 f, df;
    sint32 n, sh, x;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                df = (uint16)SW_SpanFogGrad;
                f = ~s->tex[1];
                do {
                    if (tex[((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16))]) {
                        fog = SW_FogColor;
                        x = p[0] + (((fog & 0xFF) * (f >> 8)) >> 8);
                        p[0] = x > 0xFF ? 0xFF : x;
                        x = p[1] + ((((fog >> 8) & 0xFF) * (f >> 8)) >> 8);
                        p[1] = x > 0xFF ? 0xFF : x;
                        x = p[2] + ((((fog >> 16) & 0xFF) * (f >> 8)) >> 8);
                        p[2] = x > 0xFF ? 0xFF : x;
                        x = p[3] + (((fog >> 24) * (f >> 8)) >> 8);
                        p[3] = x > 0xFF ? 0xFF : x;
                    }
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    f -= df;
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGAdd(void)
{
    SW_SPAN *s;
    uint8 *row, *p;
    uint16 c0, c1, c2, c3;
    sint32 n, v;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                do {
                    v = p[0] + (c0 >> 8);
                    p[0] = v > 0xFF ? 0xFF : v;
                    v = p[1] + (c1 >> 8);
                    p[1] = v > 0xFF ? 0xFF : v;
                    v = p[2] + (c2 >> 8);
                    p[2] = v > 0xFF ? 0xFF : v;
                    v = p[3] + (c3 >> 8);
                    p[3] = v > 0xFF ? 0xFF : v;
                    c0 += SW_SpanGrad[0];
                    c1 += SW_SpanGrad[1];
                    c2 += SW_SpanGrad[2];
                    c3 += SW_SpanGrad[3];
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGAlpha(void)
{
    SW_SPAN *s;
    uint8 *row, *p;
    uint16 c0, c1, c2, c3;
    sint32 n, a;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                do {
                    a = c3 >> 8;
                    p[0] = ((c0 >> 8) * a + p[0] * (255 - a)) >> 8;
                    p[1] = ((c1 >> 8) * a + p[1] * (255 - a)) >> 8;
                    p[2] = ((c2 >> 8) * a + p[2] * (255 - a)) >> 8;
                    p[3] = (p[3] * 255) >> 8;
                    c0 += SW_SpanGrad[0];
                    c1 += SW_SpanGrad[1];
                    c2 += SW_SpanGrad[2];
                    c3 += SW_SpanGrad[3];
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTAlpha(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    uint32 u, v, mask;
    uint16 c0, c1, c2, c3;
    sint32 n, sh, al, a0, a1;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        al = SW_TriV0->color >> 24;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                do {
                    t = (uint8 *)&tex[((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16))];
                    a0 = (t[0] | t[1]) ? al : 0;
                    a1 = (t[2] | t[3]) ? al : 0;
                    p[0] = (((t[0] * (c0 >> 8)) >> 8) * a0 + p[0] * (255 - a0)) >> 8;
                    p[1] = (((t[1] * (c1 >> 8)) >> 8) * a0 + p[1] * (255 - a0)) >> 8;
                    p[2] = (((t[2] * (c2 >> 8)) >> 8) * a1 + p[2] * (255 - a1)) >> 8;
                    p[3] = (((t[3] * (c3 >> 8)) >> 8) * a1 + p[3] * (255 - a1)) >> 8;
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    c0 += SW_SpanGrad[0];
                    c1 += SW_SpanGrad[1];
                    c2 += SW_SpanGrad[2];
                    c3 += SW_SpanGrad[3];
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPAlpha(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    sint32 *d;
    uint32 u, v, mask, idx;
    uint16 c0, c1, c2, c3;
    sint32 n, m, sh, al, a0, a1;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        al = SW_TriV0->color >> 24;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                u += d[0];
                v += d[1];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        t = (uint8 *)&tex[idx];
                        a0 = (t[0] | t[1]) ? al : 0;
                        a1 = (t[2] | t[3]) ? al : 0;
                        p[0] = (((t[0] * (c0 >> 8)) >> 8) * a0 + p[0] * (255 - a0)) >> 8;
                        p[1] = (((t[1] * (c1 >> 8)) >> 8) * a0 + p[1] * (255 - a0)) >> 8;
                        p[2] = (((t[2] * (c2 >> 8)) >> 8) * a1 + p[2] * (255 - a1)) >> 8;
                        p[3] = (((t[3] * (c3 >> 8)) >> 8) * a1 + p[3] * (255 - a1)) >> 8;
                        idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                        u += d[0];
                        v += d[1];
                        c0 += SW_SpanGrad[0];
                        c1 += SW_SpanGrad[1];
                        c2 += SW_SpanGrad[2];
                        c3 += SW_SpanGrad[3];
                        p += 4;
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGT(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    uint32 u, v, mask;
    uint16 c0, c1, c2, c3;
    sint32 n, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                do {
                    t = (uint8 *)&tex[((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16))];
                    p[0] = (t[0] * (c0 >> 8)) >> 8;
                    p[1] = (t[1] * (c1 >> 8)) >> 8;
                    p[2] = (t[2] * (c2 >> 8)) >> 8;
                    p[3] = (t[3] * (c3 >> 8)) >> 8;
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    c0 += SW_SpanGrad[0];
                    c1 += SW_SpanGrad[1];
                    c2 += SW_SpanGrad[2];
                    c3 += SW_SpanGrad[3];
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTAdd(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    uint32 u, v, mask;
    uint16 c0, c1, c2, c3;
    sint32 n, sh, v2;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                do {
                    t = (uint8 *)&tex[((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16))];
                    v2 = p[0] + ((t[0] * (c0 >> 8)) >> 8);
                    p[0] = v2 > 0xFF ? 0xFF : v2;
                    v2 = p[1] + ((t[1] * (c1 >> 8)) >> 8);
                    p[1] = v2 > 0xFF ? 0xFF : v2;
                    v2 = p[2] + ((t[2] * (c2 >> 8)) >> 8);
                    p[2] = v2 > 0xFF ? 0xFF : v2;
                    v2 = p[3] + ((t[3] * (c3 >> 8)) >> 8);
                    p[3] = v2 > 0xFF ? 0xFF : v2;
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    c0 += SW_SpanGrad[0];
                    c1 += SW_SpanGrad[1];
                    c2 += SW_SpanGrad[2];
                    c3 += SW_SpanGrad[3];
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTKey(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    uint32 u, v, mask;
    uint16 c0, c1, c2, c3;
    sint32 n, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                do {
                    t = (uint8 *)&tex[((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16))];
                    if (*(uint32 *)t) {
                        p[0] = (t[0] * (c0 >> 8)) >> 8;
                        p[1] = (t[1] * (c1 >> 8)) >> 8;
                        p[2] = (t[2] * (c2 >> 8)) >> 8;
                        p[3] = (t[3] * (c3 >> 8)) >> 8;
                    }
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    c0 += SW_SpanGrad[0];
                    c1 += SW_SpanGrad[1];
                    c2 += SW_SpanGrad[2];
                    c3 += SW_SpanGrad[3];
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTP(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    sint32 *d;
    uint32 u, v, mask, idx;
    uint16 c0, c1, c2, c3;
    sint32 n, m, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                u += d[0];
                v += d[1];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        t = (uint8 *)&tex[idx];
                        p[0] = (t[0] * (c0 >> 8)) >> 8;
                        p[1] = (t[1] * (c1 >> 8)) >> 8;
                        p[2] = (t[2] * (c2 >> 8)) >> 8;
                        p[3] = (t[3] * (c3 >> 8)) >> 8;
                        idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                        u += d[0];
                        v += d[1];
                        c0 += SW_SpanGrad[0];
                        c1 += SW_SpanGrad[1];
                        c2 += SW_SpanGrad[2];
                        c3 += SW_SpanGrad[3];
                        p += 4;
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPKey(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    sint32 *d;
    uint32 u, v, mask, idx;
    uint16 c0, c1, c2, c3;
    sint32 n, m, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                u += d[0];
                v += d[1];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        t = (uint8 *)&tex[idx];
                        if (*(uint32 *)t) {
                            p[0] = (t[0] * (c0 >> 8)) >> 8;
                            p[1] = (t[1] * (c1 >> 8)) >> 8;
                            p[2] = (t[2] * (c2 >> 8)) >> 8;
                            p[3] = (t[3] * (c3 >> 8)) >> 8;
                        }
                        idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                        u += d[0];
                        v += d[1];
                        c0 += SW_SpanGrad[0];
                        c1 += SW_SpanGrad[1];
                        c2 += SW_SpanGrad[2];
                        c3 += SW_SpanGrad[3];
                        p += 4;
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPAdd(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t;
    uint32 *tex;
    sint32 *d;
    uint32 u, v, mask, idx;
    uint16 c0, c1, c2, c3;
    uint16 k0, k1, k2, k3;
    sint32 n, m, sh, v2;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = row + (s->x << 2);
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                c3 = s->c[3];
                k0 = c0;
                k1 = c1;
                k2 = c2;
                k3 = c3;
                idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                u += d[0];
                v += d[1];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        t = (uint8 *)&tex[idx];
                        v2 = p[0] + ((t[0] * (k0 >> 8)) >> 8);
                        p[0] = v2 > 0xFF ? 0xFF : v2;
                        v2 = p[1] + ((t[1] * (k1 >> 8)) >> 8);
                        p[1] = v2 > 0xFF ? 0xFF : v2;
                        v2 = p[2] + ((t[2] * (k2 >> 8)) >> 8);
                        p[2] = v2 > 0xFF ? 0xFF : v2;
                        v2 = p[3] + ((t[3] * (k3 >> 8)) >> 8);
                        p[3] = v2 > 0xFF ? 0xFF : v2;
                        idx = ((v & mask) >> (sh * 2 + 8)) | ((u & mask) >> (sh + 16));
                        u += d[0];
                        v += d[1];
                        k0 = c0;
                        k1 = c1;
                        k2 = c2;
                        k3 = c3;
                        c0 += SW_SpanGrad[0];
                        c1 += SW_SpanGrad[1];
                        c2 += SW_SpanGrad[2];
                        c3 += SW_SpanGrad[3];
                        p += 4;
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTBilinear(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t0, *t1, *t2, *t3;
    uint32 *tex;
    uint32 u, v, idx;
    uint16 c[4];
    sint32 n, i, fu, fv, top, bot;

    s = SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        tex = (uint32 *)SW_TextureBits;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0] - 0x8000;
                v = s->tex[2] - 0x8000;
                for (i = 0; i < 4; i++)
                    c[i] = s->c[i];
                do {
                    idx = ((v >> 8) & 0xFF00) | ((u >> 16) & 0xFF);
                    t0 = (uint8 *)&tex[idx];
                    t1 = (uint8 *)&tex[idx + 1];
                    t2 = (uint8 *)&tex[idx + 256];
                    t3 = (uint8 *)&tex[idx + 257];
                    fu = (u & 0xFFFF) >> 9;
                    fv = (v & 0xFFFF) >> 9;
                    for (i = 0; i < 4; i++) {
                        top = t0[i] + (((t1[i] - t0[i]) * fu) >> 7);
                        bot = t2[i] + (((t3[i] - t2[i]) * fu) >> 7);
                        p[i] = ((top + (((bot - top) * fv) >> 7)) * (c[i] >> 8)) >> 8;
                        c[i] += SW_SpanGrad[i];
                    }
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTAddBilinear(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t0, *t1, *t2, *t3;
    uint32 *tex;
    uint32 u, v, idx;
    uint16 c[4];
    sint32 n, i, fu, fv, top, bot, sum;

    s = SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        tex = (uint32 *)SW_TextureBits;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0] - 0x8000;
                v = s->tex[2] - 0x8000;
                for (i = 0; i < 4; i++)
                    c[i] = s->c[i];
                do {
                    idx = ((v >> 8) & 0xFF00) | ((u >> 16) & 0xFF);
                    t0 = (uint8 *)&tex[idx];
                    t1 = (uint8 *)&tex[idx + 1];
                    t2 = (uint8 *)&tex[idx + 256];
                    t3 = (uint8 *)&tex[idx + 257];
                    fu = (u & 0xFFFF) >> 9;
                    fv = (v & 0xFFFF) >> 9;
                    for (i = 0; i < 4; i++) {
                        top = t0[i] + (((t1[i] - t0[i]) * fu) >> 7);
                        bot = t2[i] + (((t3[i] - t2[i]) * fu) >> 7);
                        sum = p[i] + (((top + (((bot - top) * fv) >> 7)) * (c[i] >> 8)) >> 8);
                        p[i] = sum > 0xFF ? 0xFF : sum;
                        c[i] += SW_SpanGrad[i];
                    }
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPBilinear(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t0, *t1, *t2, *t3;
    uint32 *tex;
    sint32 *d;
    uint32 u, v, idx;
    uint16 c[4];
    sint32 n, m, i, fu, fv, top, bot;

    s = SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        tex = (uint32 *)SW_TextureBits;
        do {
            p = row + (s->x << 2);
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0] - 0x8000;
                v = s->tex[2] - 0x8000;
                for (i = 0; i < 4; i++)
                    c[i] = s->c[i];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        idx = ((v >> 8) & 0xFF00) | ((u >> 16) & 0xFF);
                        t0 = (uint8 *)&tex[idx];
                        t1 = (uint8 *)&tex[idx + 1];
                        t2 = (uint8 *)&tex[idx + 256];
                        t3 = (uint8 *)&tex[idx + 257];
                        fu = (u & 0xFFFF) >> 9;
                        fv = (v & 0xFFFF) >> 9;
                        for (i = 0; i < 4; i++) {
                            top = t0[i] + (((t1[i] - t0[i]) * fu) >> 7);
                            bot = t2[i] + (((t3[i] - t2[i]) * fu) >> 7);
                            p[i] = ((top + (((bot - top) * fv) >> 7)) * (c[i] >> 8)) >> 8;
                            c[i] += SW_SpanGrad[i];
                        }
                        u += d[0];
                        v += d[1];
                        p += 4;
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPAddBilinear(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t0, *t1, *t2, *t3;
    uint32 *tex;
    sint32 *d;
    uint32 u, v, idx;
    uint16 c[4];
    sint32 n, m, i, fu, fv, top, bot, sum;

    s = SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        tex = (uint32 *)SW_TextureBits;
        do {
            p = row + (s->x << 2);
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0] - 0x8000;
                v = s->tex[2] - 0x8000;
                for (i = 0; i < 4; i++)
                    c[i] = s->c[i];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        idx = ((v >> 8) & 0xFF00) | ((u >> 16) & 0xFF);
                        t0 = (uint8 *)&tex[idx];
                        t1 = (uint8 *)&tex[idx + 1];
                        t2 = (uint8 *)&tex[idx + 256];
                        t3 = (uint8 *)&tex[idx + 257];
                        fu = (u & 0xFFFF) >> 9;
                        fv = (v & 0xFFFF) >> 9;
                        for (i = 0; i < 4; i++) {
                            top = t0[i] + (((t1[i] - t0[i]) * fu) >> 7);
                            bot = t2[i] + (((t3[i] - t2[i]) * fu) >> 7);
                            sum = p[i] + (((top + (((bot - top) * fv) >> 7)) * (c[i] >> 8)) >> 8);
                            p[i] = sum > 0xFF ? 0xFF : sum;
                            c[i] += SW_SpanGrad[i];
                        }
                        u += d[0];
                        v += d[1];
                        p += 4;
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTAlphaBilinear(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t0, *t1, *t2, *t3;
    uint32 *tex;
    uint32 u, v, idx;
    uint16 c[3];
    sint32 n, i, fu, fv, top, bot, al, a, col[4];

    s = SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        al = SW_TriV0->color >> 24;
        tex = (uint32 *)SW_TextureBits;
        do {
            p = row + (s->x << 2);
            n = s->n;
            if (n > 0) {
                u = s->tex[0] - 0x8000;
                v = s->tex[2] - 0x8000;
                for (i = 0; i < 3; i++)
                    c[i] = s->c[i];
                do {
                    idx = ((v >> 8) & 0xFF00) | ((u >> 16) & 0xFF);
                    t0 = (uint8 *)&tex[idx];
                    t1 = (uint8 *)&tex[idx + 1];
                    t2 = (uint8 *)&tex[idx + 256];
                    t3 = (uint8 *)&tex[idx + 257];
                    fu = (u & 0xFFFF) >> 9;
                    fv = (v & 0xFFFF) >> 9;
                    for (i = 0; i < 4; i++) {
                        top = t0[i] + (((t1[i] - t0[i]) * fu) >> 7);
                        bot = t2[i] + (((t3[i] - t2[i]) * fu) >> 7);
                        col[i] = top + (((bot - top) * fv) >> 7);
                    }
                    a = (col[3] * al) >> 8;
                    for (i = 0; i < 3; i++) {
                        p[i] = ((((col[i] * (c[i] >> 8)) >> 8) * a) + p[i] * (255 - a)) >> 8;
                        c[i] += SW_SpanGrad[i];
                    }
                    p[3] = (p[3] * 255) >> 8;
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                    p += 4;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPAlphaBilinear(void)
{
    SW_SPAN *s;
    uint8 *row, *p, *t0, *t1, *t2, *t3;
    uint32 *tex;
    sint32 *d;
    uint32 u, v, idx;
    uint16 c[3];
    sint32 n, m, i, fu, fv, top, bot, al, a, col[4];

    s = SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        al = SW_TriV0->color >> 24;
        tex = (uint32 *)SW_TextureBits;
        do {
            p = row + (s->x << 2);
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0] - 0x8000;
                v = s->tex[2] - 0x8000;
                for (i = 0; i < 3; i++)
                    c[i] = s->c[i];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        idx = ((v >> 8) & 0xFF00) | ((u >> 16) & 0xFF);
                        t0 = (uint8 *)&tex[idx];
                        t1 = (uint8 *)&tex[idx + 1];
                        t2 = (uint8 *)&tex[idx + 256];
                        t3 = (uint8 *)&tex[idx + 257];
                        fu = (u & 0xFFFF) >> 9;
                        fv = (v & 0xFFFF) >> 9;
                        for (i = 0; i < 4; i++) {
                            top = t0[i] + (((t1[i] - t0[i]) * fu) >> 7);
                            bot = t2[i] + (((t3[i] - t2[i]) * fu) >> 7);
                            col[i] = top + (((bot - top) * fv) >> 7);
                        }
                        a = (col[3] * al) >> 8;
                        for (i = 0; i < 3; i++) {
                            p[i] = ((((col[i] * (c[i] >> 8)) >> 8) * a) + p[i] * (255 - a)) >> 8;
                            c[i] += SW_SpanGrad[i];
                        }
                        p[3] = (p[3] * 255) >> 8;
                        u += d[0];
                        v += d[1];
                        p += 4;
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGInt(void)
{
    SW_SPAN *s;
    uint8 *row;
    uint32 *p;
    uint32 rb, g, drb;
    sint32 n;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        drb = ((uint16)SW_SpanGrad[2] << 16) | (uint16)SW_SpanGrad[0];
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            if (n > 0) {
                rb = (s->c[2] << 16) | s->c[0];
                g = *(uint32 *)&s->c[1];
                do {
                    *p++ = ((rb >> 8) & 0xFF00FF) | (g & 0xFF00);
                    g += *(uint32 *)&SW_SpanGrad[1];
                    rb += drb;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGAddInt(void)
{
    SW_SPAN *s;
    uint8 *row;
    uint32 *p;
    uint32 rb, g, drb, col, carry;
    sint32 n;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        drb = ((uint16)SW_SpanGrad[2] << 16) | (uint16)SW_SpanGrad[0];
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            if (n > 0) {
                rb = (s->c[2] << 16) | s->c[0];
                g = *(uint32 *)&s->c[1];
                do {
                    col = (((rb >> 8) & 0xFE00FF) | (g & 0xFE00)) + (*p & 0xFEFEFF);
                    carry = col & 0x1010100;
                    *p++ = col | (carry - (carry >> 8));
                    g += *(uint32 *)&SW_SpanGrad[1];
                    rb += drb;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGLit(void)
{
    SW_SPAN *s;
    uint32 *row, *p;
    uint32 d;
    sint32 db, dg, dr, da;
    sint32 b, g, r, a, al, n;
    sint32 cr, cb, cg;

    row = (uint32 *)SW_Surface + (SW_SpanY * SW_SurfacePitch >> 2);
    da = SW_SpanGrad[3];
    db = SW_SpanGrad[0];
    dr = SW_SpanGrad[2];
    dg = SW_SpanGrad[1];
    for (s = SW_SpanBuffer; s != SW_SpanEnd; s++) {
        p = row + s->x;
        n = s->n;
        g = s->c[1];
        r = s->c[2];
        a = s->c[3];
        b = s->c[0];
        for (; n > 0; n--) {
            d = *p;
            al = a >> 8;
            cr = SW_MulTable[255 + (((d >> 16) & 0xFF) << 8) - al] + SW_MulTable[al + (r & 0xFF00)];
            cb = SW_MulTable[255 + ((d & 0xFF) << 8) - al] + SW_MulTable[al + (b & 0xFF00)];
            cg = SW_MulTable[255 + (d & 0xFF00) - al] + SW_MulTable[(g & 0xFF00) + al];
            *p++ = (cr << 16) | cb | (cg << 8);
            g += dg;
            a += da;
            r += dr;
            b += db;
        }
        row += SW_SurfacePitch >> 2;
    }
}

void SW_SpanGTLit(void)
{
    SW_SPAN *s;
    uint32 *row, *p;
    uint32 u, v, texel, d;
    uint32 c0, c1, c2, dc0, dc1, dc2, du, dv;
    sint32 n, a, ia;

    row = (uint32 *)SW_Surface + (SW_SurfacePitch * SW_SpanY >> 2);
    dc2 = (uint16)SW_SpanGrad[2];
    a = SW_TriV0->color >> 24;
    dc1 = (uint16)SW_SpanGrad[1];
    dc0 = (uint16)SW_SpanGrad[0];
    du = SW_SpanDU;
    dv = SW_SpanDV;
    ia = 255 - a;
    a <<= 8;
    if (SW_SpanBuffer != SW_SpanEnd) {
        s = SW_SpanBuffer;
        do {
            p = row + s->x;
            n = s->n;
            c2 = s->c[2];
            c1 = s->c[1];
            c0 = s->c[0];
            u = s->tex[0];
            v = s->tex[2];
            if (n > 0) {
                do {
                    texel = ((uint32 *)SW_TextureBits)[(uint8)(u >> 16) | ((v >> 8) & 0xFF00)];
                    if (texel) {
                        d = *p;
                        *p = ((SW_MulTable[SW_MulTable[(c2 & 0xFF00) | ((texel >> 16) & 0xFF)] + a] + SW_MulTable[((d >> 8) & 0xFF00) + ia]) << 16)
                            | ((SW_MulTable[SW_MulTable[(c1 & 0xFF00) | ((texel >> 8) & 0xFF)] + a] + SW_MulTable[(d & 0xFF00) + ia]) << 8)
                            | (SW_MulTable[SW_MulTable[(c0 & 0xFF00) | (texel & 0xFF)] + a] + SW_MulTable[((d & 0xFF) << 8) + ia]);
                    }
                    p++;
                    c2 += dc2;
                    c1 += dc1;
                    c0 += dc0;
                    u += du;
                    v += dv;
                } while (--n);
            }
            row += SW_SurfacePitch >> 2;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTInt(void)
{
    SW_SPAN *s;
    uint8 *row, *t;
    uint32 *p, *tex;
    uint32 u, v, mask;
    uint16 c0, c1, c2;
    sint32 n, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            if (n > 0) {
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                u = s->tex[0];
                v = s->tex[2];
                do {
                    t = (uint8 *)&tex[((u & mask) >> (sh + 16)) | ((v & mask) >> (sh * 2 + 8))];
                    *p++ = (SW_MulTable[(c2 & 0xFF00) + t[2]] << 16) | (SW_MulTable[(c1 & 0xFF00) + t[1]] << 8) | SW_MulTable[(c0 & 0xFF00) + t[0]];
                    c0 += SW_SpanGrad[0];
                    c2 += SW_SpanGrad[2];
                    c1 += SW_SpanGrad[1];
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTAddInt(void)
{
    SW_SPAN *s;
    uint8 *row, *t;
    uint32 *p, *tex;
    uint32 u, v, mask, col, carry;
    uint16 c0, c1, c2;
    sint32 n, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            if (n > 0) {
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                u = s->tex[0];
                v = s->tex[2];
                do {
                    t = (uint8 *)&tex[((u & mask) >> (sh + 16)) | ((v & mask) >> (sh * 2 + 8))];
                    col = (SW_MulTable[(c2 & 0xFF00) + t[2]] << 16) | (SW_MulTable[(c1 & 0xFF00) + t[1]] << 8) | SW_MulTable[(c0 & 0xFF00) + t[0]];
                    col = (col & 0xFEFEFF) + (*p & 0xFEFEFF);
                    carry = col & 0x1010100;
                    *p++ = col | (carry - (carry >> 8));
                    c0 += SW_SpanGrad[0];
                    c2 += SW_SpanGrad[2];
                    c1 += SW_SpanGrad[1];
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTKeyInt(void)
{
    SW_SPAN *s;
    uint8 *row, *t;
    uint32 *p, *tex;
    uint32 u, v, mask, col;
    uint16 c0, c1, c2;
    sint32 n, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            if (n > 0) {
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                u = s->tex[0];
                v = s->tex[2];
                do {
                    t = (uint8 *)&tex[((u & mask) >> (sh + 16)) | ((v & mask) >> (sh * 2 + 8))];
                    col = (SW_MulTable[(c2 & 0xFF00) + t[2]] << 16) | (SW_MulTable[(c1 & 0xFF00) + t[1]] << 8) | SW_MulTable[(c0 & 0xFF00) + t[0]];
                    if (col)
                        *p = col;
                    p++;
                    c0 += SW_SpanGrad[0];
                    c2 += SW_SpanGrad[2];
                    c1 += SW_SpanGrad[1];
                    u += SW_SpanDU;
                    v += SW_SpanDV;
                } while (--n);
            }
            row += SW_SurfacePitch;
            s++;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPInt(void)
{
    SW_SPAN *s;
    uint8 *row, *t;
    uint32 *p, *tex;
    sint32 *d;
    uint32 u, v, mask, col;
    uint16 c0, c1, c2;
    sint32 n, m, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        t = (uint8 *)&tex[((u & mask) >> (sh + 16)) | ((v & mask) >> (sh * 2 + 8))];
                        col = (SW_MulTable[(c2 & 0xFF00) + t[2]] << 16) | (SW_MulTable[(c1 & 0xFF00) + t[1]] << 8) | SW_MulTable[(c0 & 0xFF00) + t[0]];
                        *p++ = col;
                        u += d[0];
                        v += d[1];
                        c0 += SW_SpanGrad[0];
                        c1 += SW_SpanGrad[1];
                        c2 += SW_SpanGrad[2];
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPAddInt(void)
{
    SW_SPAN *s;
    uint8 *row, *t;
    uint32 *p, *tex;
    sint32 *d;
    uint32 u, v, mask, col, carry;
    uint16 c0, c1, c2;
    sint32 n, m, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        t = (uint8 *)&tex[((u & mask) >> (sh + 16)) | ((v & mask) >> (sh * 2 + 8))];
                        col = (SW_MulTable[(c2 & 0xFF00) + t[2]] << 16) | (SW_MulTable[(c1 & 0xFF00) + t[1]] << 8) | SW_MulTable[(c0 & 0xFF00) + t[0]];
                        col = (col & 0xFEFEFF) + (*p & 0xFEFEFF);
                        carry = col & 0x1010100;
                        *p++ = col | (carry - (carry >> 8));
                        u += d[0];
                        v += d[1];
                        c0 += SW_SpanGrad[0];
                        c1 += SW_SpanGrad[1];
                        c2 += SW_SpanGrad[2];
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_SpanGTPKeyInt(void)
{
    SW_SPAN *s;
    uint8 *row, *t;
    uint32 *p, *tex;
    sint32 *d;
    uint32 u, v, mask, col;
    uint16 c0, c1, c2;
    sint32 n, m, sh;

    s = (SW_SPAN *)SW_SpanBuffer;
    if (s != SW_SpanEnd) {
        row = (uint8 *)SW_Surface + SW_SpanY * SW_SurfacePitch;
        sh = SW_TexShift;
        mask = (0xFF0000 << sh) & 0xFF0000;
        tex = (uint32 *)(SW_TextureBits + SW_MipOffsets[sh]);
        do {
            p = (uint32 *)row + s->x;
            n = s->n;
            d = (sint32 *)(s + 1);
            if (n > 0) {
                u = s->tex[0];
                v = s->tex[2];
                c0 = s->c[0];
                c1 = s->c[1];
                c2 = s->c[2];
                do {
                    m = n > 16 ? 16 : n;
                    do {
                        t = (uint8 *)&tex[((u & mask) >> (sh + 16)) | ((v & mask) >> (sh * 2 + 8))];
                        col = (SW_MulTable[(c2 & 0xFF00) + t[2]] << 16) | (SW_MulTable[(c1 & 0xFF00) + t[1]] << 8) | SW_MulTable[(c0 & 0xFF00) + t[0]];
                        if (col)
                            *p = col;
                        p++;
                        u += d[0];
                        v += d[1];
                        c0 += SW_SpanGrad[0];
                        c1 += SW_SpanGrad[1];
                        c2 += SW_SpanGrad[2];
                    } while (--m);
                    d += 2;
                    n -= 16;
                } while (n > 0);
            } else {
                d += 2;
            }
            row += SW_SurfacePitch;
            s = (SW_SPAN *)d;
        } while (s != SW_SpanEnd);
    }
}

void SW_CopyBuffer(void *dst, void *src, sint32 width)
{
    if (!SW_UseMMX)
        memcpy(dst, src, width * 4);
    else
        memcpy(dst, src, ((uint32)width >> 1) * 8);
}

void SW_Convert32To555(void *dst, void *src, sint32 width)
{
    uint32 *s;
    uint16 *d;
    uint32 c;
    sint32 n;

    s = src;
    d = dst;
    if (!SW_UseMMX)
        n = width;
    else
        n = ((width - 1) & ~7) + 8;
    do {
        c = *s++;
        *d++ = (uint16)(((c & 0xF8) >> 3) | ((c & 0xF800) >> 6) | ((c & 0xF80000) >> 9));
    } while (--n);
}

void SW_Convert32To565(void *dst, void *src, sint32 width)
{
    uint32 *s;
    uint16 *d;
    uint32 c;
    sint32 n;

    s = src;
    d = dst;
    if (!SW_UseMMX)
        n = width;
    else
        n = ((uint32)width >> 2) << 2;
    do {
        c = *s++;
        *d++ = (uint16)(((c & 0xF8) >> 3) | ((c & 0xFC00) >> 5) | ((c & 0xF80000) >> 8));
    } while (--n);
}

void SW_Convert32To24(void *dst, void *src, sint32 width)
{
    uint32 *s;
    uint8 *d;
    uint32 c;
    sint32 n, i;

    s = src;
    d = dst;
    n = width;
    if (!SW_UseMMX) {
        do {
            *(uint16 *)d = (uint16)s[0];
            *(uint16 *)(d + 3) = (uint16)s[1];
            d[6] = (uint8)(s[0] >> 16);
            d[9] = (uint8)(s[1] >> 16);
            d += 10;
        } while (--n);
    } else {
        do {
            for (i = 0; i < 4; i++) {
                c = *s++;
                d[0] = (uint8)c;
                d[1] = (uint8)(c >> 8);
                d[2] = (uint8)(c >> 16);
                d += 3;
            }
            n -= 4;
        } while (n);
    }
}

LPVOID DBLog_MapView(HANDLE hMap)
{
    return MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0x100000);
}

HANDLE DBLog_OpenMapping(LPCSTR name)
{
    return OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, name);
}

void DBLog_Post(HWND hwnd, LPARAM lparam)
{
    while (!PostMessageA(hwnd, DBLog_Message, 0, lparam)) {
        if (!FindWindowA("DBLogWindowClass", "DBLog Server"))
            return;
    }
}
