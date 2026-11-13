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

HRESULT __stdcall SW_D3D_EndScene(SW_DEVICE *dev);
extern LPDIRECT3DDEVICE3 SW_RealD3DDevice;
extern IDirect3DDevice3Vtbl SW_Vtbl;
extern SW_DEVICE SW_Device;
extern HGLOBAL SW_SpanBufferHandle;
extern void *SW_SpanBuffer;
extern void *SW_Surface;
extern sint32 SW_SurfacePitch;
extern sint32 SW_FrameState;
extern sint32 SW_ColorOpScale;
extern DDSURFACEDESC2 SW_SurfDesc;
extern sint32 SW_NumLocked;
extern sint32 SW_UseMMX;
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
