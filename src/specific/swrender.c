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

extern char SW_Kernel32Name[];
extern char SW_SleepName[];

__declspec(naked) void __fastcall SW_WaitUntilInScene(volatile sint32 *flag)
{
    __asm {
    spin:
        push ecx
        call SW_SleepTick
        pop ecx
        mov eax, [ecx]
        test eax, eax
        je spin
        ret
    }
}

__declspec(naked) void __fastcall SW_WaitWhileInScene(volatile sint32 *flag)
{
    __asm {
    spin:
        push ecx
        call SW_SleepTick
        pop ecx
        mov eax, [ecx]
        test eax, eax
        jne spin
        ret
    }
}

__declspec(naked) void SW_SleepTick(void)
{
    __asm {
        call SW_GetKernelModule
        call SW_GetSleepProc
        push 10
        call eax
        ret
    }
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

__declspec(naked) HMODULE SW_GetKernelModule(void)
{
    __asm {
        push offset SW_Kernel32Name
        call dword ptr GetModuleHandleA
        ret
    }
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

__declspec(naked) FARPROC SW_GetSleepProc(void)
{
    __asm {
        push offset SW_SleepName
        push eax
        call dword ptr GetProcAddress
        ret
    }
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

void SW_CopyBuffer(void *dst, void *src, sint32 width)
{
    if (!SW_UseMMX) {
        __asm {
            mov esi, src
            mov edi, dst
            mov ecx, width
            rep movsd
        }
    } else {
        __asm {
            mov esi, src
            mov edi, dst
            mov ecx, width
            shr ecx, 1
        copy:
            movq mm0, [esi]
            add esi, 8
            movq [edi], mm0
            add edi, 8
            dec ecx
            jne copy
            emms
        }
    }
}

void SW_Convert32To555(void *dst, void *src, sint32 width)
{
    uint32 scale[2];
    uint32 gmask[2];
    uint32 rbmask[2];

    scale[0] = 0x20000008;
    scale[1] = 0x20000008;
    rbmask[0] = 0xF800F8;
    rbmask[1] = 0xF800F8;
    gmask[0] = 0xF800;
    gmask[1] = 0xF800;
    if (!SW_UseMMX) {
        __asm {
            mov edx, width
            mov esi, src
            mov edi, dst
        plain:
            mov eax, [esi]
            mov ebx, eax
            mov ecx, eax
            and eax, 0xF8
            and ebx, 0xF800
            shr eax, 3
            and ecx, 0xF80000
            shr ebx, 6
            shr ecx, 9
            or eax, ebx
            or eax, ecx
            add esi, 4
            mov [edi], ax
            add edi, 2
            dec edx
            jne plain
        }
    } else {
        __asm {
            mov eax, width
            mov ebx, src
            sub eax, 1
            mov edx, dst
            and eax, 0xFFFFFFF8
            movq mm7, scale
            movq mm6, gmask
            movq mm2, [ebx + eax * 4 + 8]
            movq mm0, [ebx + eax * 4]
            movq mm3, mm2
            pand mm3, rbmask
            movq mm1, mm0
            pand mm1, rbmask
            pmaddwd mm3, mm7
            pmaddwd mm1, mm7
            pand mm2, mm6
        mmx:
            movq mm4, [ebx + eax * 4 + 0x18]
            pand mm0, mm6
            movq mm5, [ebx + eax * 4 + 0x10]
            por mm3, mm2
            psrld mm3, 6
            por mm1, mm0
            movq mm0, mm4
            psrld mm1, 6
            pand mm0, rbmask
            packssdw mm1, mm3
            movq mm3, mm5
            pmaddwd mm0, mm7
            pand mm3, rbmask
            pand mm4, mm6
            movq [edx + eax * 2], mm1
            pmaddwd mm3, mm7
            sub eax, 8
            por mm4, mm0
            pand mm5, mm6
            psrld mm4, 6
            movq mm2, [ebx + eax * 4 + 8]
            por mm5, mm3
            movq mm0, [ebx + eax * 4]
            psrld mm5, 6
            movq mm3, mm2
            movq mm1, mm0
            pand mm3, rbmask
            packssdw mm5, mm4
            pand mm1, rbmask
            pand mm2, mm6
            movq [edx + eax * 2 + 0x18], mm5
            pmaddwd mm3, mm7
            pmaddwd mm1, mm7
            jg mmx
            movq mm4, [ebx + eax * 4 + 0x18]
            pand mm0, mm6
            movq mm5, [ebx + eax * 4 + 0x10]
            por mm3, mm2
            psrld mm3, 6
            por mm1, mm0
            movq mm0, mm4
            psrld mm1, 6
            pand mm0, rbmask
            packssdw mm1, mm3
            movq mm3, mm5
            pmaddwd mm0, mm7
            pand mm3, rbmask
            pand mm4, mm6
            movq [edx + eax * 2], mm1
            pmaddwd mm3, mm7
            sub eax, 8
            por mm4, mm0
            pand mm5, mm6
            psrld mm4, 6
            por mm5, mm3
            psrld mm5, 6
            packssdw mm5, mm4
            movq [edx + eax * 2 + 0x18], mm5
            emms
        }
    }
}

void SW_Convert32To565(void *dst, void *src, sint32 width)
{
    uint32 rmask[2];
    uint32 gmask[2];
    uint32 bmask[2];

    rmask[0] = 0xF80000;
    rmask[1] = 0xF80000;
    gmask[0] = 0xFC00;
    gmask[1] = 0xFC00;
    bmask[0] = 0xF8;
    bmask[1] = 0xF8;
    if (!SW_UseMMX) {
        __asm {
            mov edx, width
            mov esi, src
            mov edi, dst
        plain:
            mov eax, [esi]
            mov ebx, eax
            mov ecx, eax
            and eax, 0xF8
            and ebx, 0xFC00
            shr eax, 3
            and ecx, 0xF80000
            shr ebx, 5
            shr ecx, 8
            or eax, ebx
            or eax, ecx
            add esi, 4
            mov [edi], ax
            add edi, 2
            dec edx
            jne plain
        }
    } else {
        __asm {
            mov eax, width
            shr eax, 2
            mov esi, src
            mov edi, dst
            movq mm3, rmask
            movq mm7, gmask
            align 16
        mmx:
            movq mm0, [esi]
            movq mm4, [esi + 8]
            movq mm1, mm0
            movq mm2, mm0
            movq mm5, mm4
            movq mm6, mm4
            pand mm0, mm3
            pand mm1, mm7
            pand mm2, bmask
            pand mm4, mm3
            pand mm5, mm7
            pand mm6, bmask
            pslld mm0, 8
            pslld mm4, 8
            psrad mm0, 16
            psrld mm1, 5
            psrld mm2, 3
            psrad mm4, 16
            psrld mm5, 5
            psrld mm6, 3
            por mm0, mm1
            por mm4, mm5
            por mm0, mm2
            por mm4, mm6
            packssdw mm0, mm4
            movq [edi], mm0
            add edi, 8
            add esi, 16
            dec eax
            jne mmx
            emms
        }
    }
}

void SW_Convert32To24(void *dst, void *src, sint32 width)
{
    uint32 lomask[2];
    uint32 midmask[2];
    uint32 himask[2];
    uint32 topmask[2];

    lomask[0] = 0xFFFFFF;
    lomask[1] = 0;
    himask[0] = 0;
    midmask[0] = 0xFFFF;
    midmask[1] = 0;
    himask[1] = 0xFFFFFF;
    topmask[0] = 0xFF0000;
    topmask[1] = 0;
    if (!SW_UseMMX) {
        __asm {
            mov edx, width
            mov esi, src
            mov edi, dst
        plain:
            mov eax, [esi]
            mov ebx, [esi + 4]
            mov [edi], ax
            mov [edi + 3], bx
            shr eax, 16
            add edi, 4
            shr ebx, 16
            mov [edi + 2], al
            mov [edi + 5], bl
            add edi, 6
            dec edx
            jne plain
        }
    } else {
        __asm {
            mov esi, src
            mov edi, dst
            mov eax, width
        mmx:
            movq mm0, [esi]
            movq mm1, [esi + 8]
            movq mm2, mm0
            movq mm3, mm1
            sub eax, 4
            movq mm4, mm1
            pand mm0, lomask
            pand mm1, midmask
            pand mm2, himask
            psrlq mm2, 8
            psllq mm1, 48
            por mm1, mm2
            por mm0, mm1
            pand mm3, topmask
            pand mm4, himask
            psrlq mm3, 16
            psrlq mm4, 24
            por mm3, mm4
            lea esi, [esi + 16]
            movq [edi], mm0
            movd [edi + 8], mm3
            lea edi, [edi + 12]
            jne mmx
            emms
        }
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
