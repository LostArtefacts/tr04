#include <windows.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
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
extern HANDLE DBLog_Mapping;
extern UINT DBLog_ResetMessage;
extern sint32 DBLog_Offset;
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
extern sint32 SW_TriPB0;
extern sint32 SW_TriPB1;
extern sint32 SW_TriPB2;
extern float SW_TriPBDX;
extern float SW_TriPBDY;
extern double SW_TriPBOff;
extern float SW_TriPD01;
extern float SW_TriPD02;
extern float SW_TriPDX01;
extern float SW_TriPDX02;
extern float SW_TriPDY01;
extern float SW_TriPDY02;
extern SW_VERTEX *SW_TriPEdgeA;
extern SW_VERTEX *SW_TriPEdgeB;
extern sint32 SW_TriPEdges;
extern sint32 SW_TriPG0;
extern sint32 SW_TriPG1;
extern sint32 SW_TriPG2;
extern float SW_TriPGDX;
extern float SW_TriPGDY;
extern double SW_TriPGOff;
extern sint32 SW_TriPR0;
extern sint32 SW_TriPR1;
extern sint32 SW_TriPR2;
extern float SW_TriPRDX;
extern float SW_TriPRDY;
extern double SW_TriPROff;
extern float SW_TriPRecip;
extern float SW_TriPRecipTab[16];
extern sint32 SW_TriPStepA[2];
extern sint32 SW_TriPStepB[2];
extern float SW_TriPStepTab[16];
extern float SW_TriPTemp;
extern sint32 SW_TriPTmp[2];
extern float SW_TriPU;
extern float SW_TriPU0;
extern float SW_TriPU1;
extern float SW_TriPU2;
extern float SW_TriPUDX;
extern float SW_TriPUDX16;
extern float SW_TriPUDY;
extern float SW_TriPUOff;
extern float SW_TriPV;
extern float SW_TriPV0;
extern float SW_TriPV1;
extern float SW_TriPV2;
extern float SW_TriPVDX;
extern float SW_TriPVDX16;
extern float SW_TriPVDY;
extern float SW_TriPVOff;
extern float SW_TriPW;
extern float SW_TriPWDX;
extern float SW_TriPWDX16;
extern float SW_TriPWDY;
extern float SW_TriPWOff;
extern sint32 SW_TriPY[2];
extern sint32 SW_TriPFB0;
extern sint32 SW_TriPFB1;
extern sint32 SW_TriPFB2;
extern float SW_TriPFBDX;
extern float SW_TriPFBDY;
extern double SW_TriPFBOff;
extern float SW_TriPFD01;
extern float SW_TriPFD02;
extern float SW_TriPFDX01;
extern float SW_TriPFDX02;
extern float SW_TriPFDY01;
extern float SW_TriPFDY02;
extern SW_VERTEX *SW_TriPFEdgeA;
extern SW_VERTEX *SW_TriPFEdgeB;
extern sint32 SW_TriPFEdges;
extern sint32 SW_TriPFF0;
extern sint32 SW_TriPFF1;
extern sint32 SW_TriPFF2;
extern float SW_TriPFFDX;
extern float SW_TriPFFDY;
extern double SW_TriPFFOff;
extern float SW_TriPFFog;
extern sint32 SW_TriPFG0;
extern sint32 SW_TriPFG1;
extern sint32 SW_TriPFG2;
extern float SW_TriPFGDX;
extern float SW_TriPFGDY;
extern double SW_TriPFGOff;
extern sint32 SW_TriPFR0;
extern sint32 SW_TriPFR1;
extern sint32 SW_TriPFR2;
extern float SW_TriPFRDX;
extern float SW_TriPFRDY;
extern double SW_TriPFROff;
extern float SW_TriPFRecip;
extern sint32 SW_TriPFStepA[2];
extern sint32 SW_TriPFStepB[2];
extern float SW_TriPFTemp;
extern sint32 SW_TriPFTmp[2];
extern float SW_TriPFU;
extern float SW_TriPFU0;
extern float SW_TriPFU1;
extern float SW_TriPFU2;
extern float SW_TriPFUDX;
extern float SW_TriPFUDX16;
extern float SW_TriPFUDY;
extern float SW_TriPFUOff;
extern float SW_TriPFV;
extern float SW_TriPFV0;
extern float SW_TriPFV1;
extern float SW_TriPFV2;
extern float SW_TriPFVDX;
extern float SW_TriPFVDX16;
extern float SW_TriPFVDY;
extern float SW_TriPFVOff;
extern float SW_TriPFW;
extern float SW_TriPFWDX;
extern float SW_TriPFWDX16;
extern float SW_TriPFWDY;
extern float SW_TriPFWOff;
extern sint32 SW_TriPFY[2];
extern float SW_TriDX01;
extern float SW_TriDY02;
extern float SW_TriDX02;
extern float SW_TriDY01;
extern float SW_TriTemp;
extern float SW_TriD01;
extern float SW_TriD02;
extern float SW_TriRDX;
extern float SW_TriRDY;
extern float SW_TriGDX;
extern float SW_TriGDY;
extern float SW_TriBDX;
extern float SW_TriBDY;
extern float SW_TriADX;
extern float SW_TriADY;
extern float SW_TriSlopeA;
extern float SW_TriXA;
extern float SW_TriSlopeB;
extern float SW_TriXB;
extern double SW_TriROff;
extern double SW_TriGOff;
extern double SW_TriBOff;
extern double SW_TriAOff;
extern sint32 SW_TriY[2];
extern sint32 SW_TriEndY[2];
extern SW_VERTEX *SW_TriEdgeA;
extern SW_VERTEX *SW_TriEdgeB;
extern D3DCOLOR SW_TriColor0;
extern sint32 SW_TriR0;
extern sint32 SW_TriG0;
extern sint32 SW_TriB0;
extern sint32 SW_TriA0;
extern sint32 SW_TriR1;
extern sint32 SW_TriG1;
extern sint32 SW_TriB1;
extern sint32 SW_TriA1;
extern sint32 SW_TriR2;
extern sint32 SW_TriG2;
extern sint32 SW_TriB2;
extern sint32 SW_TriA2;
extern sint32 SW_TriEdges;
extern sint32 SW_TriStepA;
extern sint32 SW_TriStepB;
extern float SW_TriFogDX01;
extern float SW_TriFogDY02;
extern float SW_TriFogDX02;
extern float SW_TriFogDY01;
extern float SW_TriFogTemp;
extern float SW_TriFogD01;
extern float SW_TriFogD02;
extern float SW_TriFogRDX;
extern float SW_TriFogRDY;
extern float SW_TriFogGDX;
extern float SW_TriFogGDY;
extern float SW_TriFogBDX;
extern float SW_TriFogBDY;
extern float SW_TriFogADX;
extern float SW_TriFogADY;
extern float SW_TriFogSlopeA;
extern float SW_TriFogXA;
extern float SW_TriFogSlopeB;
extern float SW_TriFogXB;
extern double SW_TriFogROff;
extern double SW_TriFogGOff;
extern double SW_TriFogBOff;
extern double SW_TriFogAOff;
extern sint32 SW_TriFogY[2];
extern sint32 SW_TriFogEndY[2];
extern SW_VERTEX *SW_TriFogEdgeA;
extern SW_VERTEX *SW_TriFogEdgeB;
extern sint32 SW_TriFogR0;
extern sint32 SW_TriFogG0;
extern sint32 SW_TriFogB0;
extern sint32 SW_TriFogA0;
extern sint32 SW_TriFogR1;
extern sint32 SW_TriFogG1;
extern sint32 SW_TriFogB1;
extern sint32 SW_TriFogA1;
extern sint32 SW_TriFogR2;
extern sint32 SW_TriFogG2;
extern sint32 SW_TriFogB2;
extern sint32 SW_TriFogA2;
extern sint32 SW_TriFogEdges;
extern sint32 SW_TriFogStepA;
extern sint32 SW_TriFogStepB;
extern float SW_TriTDX01;
extern float SW_TriTDY02;
extern float SW_TriTDX02;
extern float SW_TriTDY01;
extern float SW_TriTTemp;
extern SW_VERTEX *SW_TriTEdgeA;
extern SW_VERTEX *SW_TriTEdgeB;
extern sint32 SW_TriTG0;
extern sint32 SW_TriTB0;
extern sint32 SW_TriTR0;
extern sint32 SW_TriTG1;
extern sint32 SW_TriTB1;
extern sint32 SW_TriTR1;
extern sint32 SW_TriTG2;
extern sint32 SW_TriTB2;
extern sint32 SW_TriTR2;
extern float SW_TriTD01;
extern float SW_TriTD02;
extern float SW_TriTRDX;
extern float SW_TriTRDY;
extern double SW_TriTROff;
extern float SW_TriTGDX;
extern float SW_TriTGDY;
extern double SW_TriTGOff;
extern float SW_TriTBDX;
extern float SW_TriTBDY;
extern double SW_TriTBOff;
extern float SW_TriTUDX;
extern float SW_TriTUDY;
extern double SW_TriTUOff;
extern float SW_TriTVDX;
extern float SW_TriTVDY;
extern double SW_TriTVOff;
extern sint32 SW_TriTEdges;
extern sint32 SW_TriTY[2];
extern sint32 SW_TriTEndY[2];
extern float SW_TriTSlopeA;
extern float SW_TriTXA;
extern sint32 SW_TriTStepA;
extern float SW_TriTSlopeB;
extern float SW_TriTXB;
extern sint32 SW_TriTStepB;
extern float SW_TriTFogDX01;
extern float SW_TriTFogDY02;
extern float SW_TriTFogDX02;
extern float SW_TriTFogDY01;
extern float SW_TriTFogTemp;
extern SW_VERTEX *SW_TriTFogEdgeA;
extern SW_VERTEX *SW_TriTFogEdgeB;
extern sint32 SW_TriTFogG0;
extern sint32 SW_TriTFogB0;
extern sint32 SW_TriTFogR0;
extern sint32 SW_TriTFogG1;
extern sint32 SW_TriTFogB1;
extern sint32 SW_TriTFogR1;
extern sint32 SW_TriTFogG2;
extern sint32 SW_TriTFogB2;
extern sint32 SW_TriTFogR2;
extern float SW_TriTFogD01;
extern float SW_TriTFogD02;
extern float SW_TriTFogRDX;
extern float SW_TriTFogRDY;
extern double SW_TriTFogROff;
extern float SW_TriTFogGDX;
extern float SW_TriTFogGDY;
extern double SW_TriTFogGOff;
extern float SW_TriTFogBDX;
extern float SW_TriTFogBDY;
extern double SW_TriTFogBOff;
extern float SW_TriTFogUDX;
extern float SW_TriTFogUDY;
extern double SW_TriTFogUOff;
extern float SW_TriTFogVDX;
extern float SW_TriTFogVDY;
extern double SW_TriTFogVOff;
extern sint32 SW_TriTFogEdges;
extern sint32 SW_TriTFogY[2];
extern sint32 SW_TriTFogEndY[2];
extern float SW_TriTFogSlopeA;
extern float SW_TriTFogXA;
extern sint32 SW_TriTFogStepA;
extern float SW_TriTFogSlopeB;
extern float SW_TriTFogXB;
extern sint32 SW_TriTFogStepB;
extern sint32 SW_TriTFogF0;
extern sint32 SW_TriTFogF1;
extern sint32 SW_TriTFogF2;
extern float SW_TriTFogFDX;
extern float SW_TriTFogFDY;
extern double SW_TriTFogFOff;

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

#ifndef FEAT_C_ASM
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
#else
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
#endif

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
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTPFog : SW_TriSetupGTP;
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
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTFog : SW_TriSetupGT;
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
                SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGFog : SW_TriSetupG;
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
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTPFog : SW_TriSetupGTP;
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
                    SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGTFog : SW_TriSetupGT;
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
                SW_TriSetupFunc = (f & 0x400) ? SW_TriSetupGFog : SW_TriSetupG;
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

sint32 SW_RoundL(double d)
{
    double t;

    if (d >= -2147483648.5 && d < 2147483647.5) {
        t = d + 6755399441055744.0;
        return *(sint32 *)&t;
    }
    return 0x80000000;
}

sint16 SW_RoundW(double d)
{
    double t;

    if (d >= -32768.5 && d < 32767.5) {
        t = d + 6755399441055744.0;
        return *(sint16 *)&t;
    }
    return -32768;
}

sint32 SW_TriSetupGTP(void *p0, void *p1, void *p2)
{
    SW_VERTEX *v0, *v1, *v2, *t, *n;
    SW_SPAN *span;
    sint32 *pair;
    double a, b, inv, d01, d02, g, h, s, u, v, w;
    float f;
    uint32 sy, c, xa, xb, x;
    sint32 na, nb, cnt;

    v0 = (SW_VERTEX *)p0;
    v1 = (SW_VERTEX *)p1;
    v2 = (SW_VERTEX *)p2;
    SW_TriV0 = v0;
    SW_TriPU0 = v0->tu * v0->rhw;
    SW_TriPV0 = v0->tv * v0->rhw;
    SW_TriPU1 = v1->tu * v1->rhw;
    SW_TriPV1 = v1->tv * v1->rhw;
    SW_TriPU2 = v2->tu * v2->rhw;
    SW_TriPV2 = v2->tv * v2->rhw;
    a = (double)v0->sx - v1->sx;
    b = (double)v0->sy - v2->sy;
    SW_TriPDX01 = (float)a;
    SW_TriPDY02 = (float)b;
    g = a * b;
    a = (double)v0->sx - v2->sx;
    b = (double)v0->sy - v1->sy;
    SW_TriPDX02 = (float)a;
    SW_TriPDY01 = (float)b;
    g -= a * b;
    SW_TriPTemp = (float)g;
    inv = 1.0;
    if (*(uint32 *)&SW_TriPTemp & 0x80000000) {
        t = v1;
        v1 = v2;
        v2 = t;
        inv = -1.0;
        f = SW_TriPDX01;
        SW_TriPDX01 = SW_TriPDX02;
        SW_TriPDX02 = f;
        f = SW_TriPDY01;
        SW_TriPDY01 = SW_TriPDY02;
        SW_TriPDY02 = f;
    }
    inv /= g;
    *(SW_VERTEX **)&v0->sz = v1;
    *(SW_VERTEX **)&v1->sz = v2;
    *(SW_VERTEX **)&v2->sz = v0;
    *(SW_VERTEX **)&v0->specular = v2;
    *(SW_VERTEX **)&v1->specular = v0;
    *(SW_VERTEX **)&v2->specular = v1;
    t = v0;
    sy = *(uint32 *)&v0->sy;
    if (sy >= *(uint32 *)&v1->sy) {
        sy = *(uint32 *)&v1->sy;
        t = v1;
    }
    if (sy >= *(uint32 *)&v2->sy)
        t = v2;
    SW_TriPEdgeA = t;
    SW_TriPEdgeB = t;
    SW_TriColor0 = v0->color;
    c = v0->color;
    SW_TriPG0 = (c & 0xFF00) + 0x80;
    SW_TriPB0 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriPR0 = ((c >> 8) & 0xFF00) + 0x80;
    c = v1->color;
    SW_TriPG1 = (c & 0xFF00) + 0x80;
    SW_TriPB1 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriPR1 = ((c >> 8) & 0xFF00) + 0x80;
    c = v2->color;
    SW_TriPG2 = (c & 0xFF00) + 0x80;
    SW_TriPB2 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriPR2 = ((c >> 8) & 0xFF00) + 0x80;
    d01 = (double)SW_TriPR0 - SW_TriPR1;
    d02 = (double)SW_TriPR0 - SW_TriPR2;
    SW_TriPD01 = (float)d01;
    SW_TriPD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPDY02 - d02 * *(volatile float *)&SW_TriPDY01) * inv;
    SW_SpanGrad[2] = SW_RoundW(g);
    SW_TriPRDX = (float)g;
    h = (*(volatile float *)&SW_TriPD02 * *(volatile float *)&SW_TriPDX01 - *(volatile float *)&SW_TriPD01 * *(volatile float *)&SW_TriPDX02) * inv;
    SW_TriPRDY = (float)h;
    SW_TriPROff = ((double)SW_TriPR0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriPRDX);
    d01 = (double)SW_TriPG0 - SW_TriPG1;
    d02 = (double)SW_TriPG0 - SW_TriPG2;
    SW_TriPD01 = (float)d01;
    SW_TriPD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPDY02 - d02 * *(volatile float *)&SW_TriPDY01) * inv;
    SW_SpanGrad[1] = SW_RoundW(g);
    SW_TriPGDX = (float)g;
    h = (*(volatile float *)&SW_TriPD02 * *(volatile float *)&SW_TriPDX01 - *(volatile float *)&SW_TriPD01 * *(volatile float *)&SW_TriPDX02) * inv;
    SW_TriPGDY = (float)h;
    SW_TriPGOff = ((double)SW_TriPG0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriPGDX);
    d01 = (double)SW_TriPB0 - SW_TriPB1;
    d02 = (double)SW_TriPB0 - SW_TriPB2;
    SW_TriPD01 = (float)d01;
    SW_TriPD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPDY02 - d02 * *(volatile float *)&SW_TriPDY01) * inv;
    SW_SpanGrad[0] = SW_RoundW(g);
    SW_TriPBDX = (float)g;
    h = (*(volatile float *)&SW_TriPD02 * *(volatile float *)&SW_TriPDX01 - *(volatile float *)&SW_TriPD01 * *(volatile float *)&SW_TriPDX02) * inv;
    SW_TriPBDY = (float)h;
    SW_TriPBOff = ((double)SW_TriPB0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriPBDX);
    d01 = (double)*(volatile float *)&SW_TriPU0 - *(volatile float *)&SW_TriPU1;
    d02 = (double)*(volatile float *)&SW_TriPU0 - *(volatile float *)&SW_TriPU2;
    SW_TriPD01 = (float)d01;
    SW_TriPD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPDY02 - d02 * *(volatile float *)&SW_TriPDY01) * inv;
    SW_TriPUDX = (float)g;
    h = (*(volatile float *)&SW_TriPD02 * *(volatile float *)&SW_TriPDX01 - *(volatile float *)&SW_TriPD01 * *(volatile float *)&SW_TriPDX02) * inv;
    SW_TriPUDX16 = (float)(g * 16.0F);
    SW_TriPUDY = (float)h;
    SW_TriPUOff = (float)(*(volatile float *)&SW_TriPU0 - (v0->sx * *(volatile float *)&SW_TriPUDX + v0->sy * *(volatile float *)&SW_TriPUDY));
    d01 = (double)*(volatile float *)&SW_TriPV0 - *(volatile float *)&SW_TriPV1;
    d02 = (double)*(volatile float *)&SW_TriPV0 - *(volatile float *)&SW_TriPV2;
    SW_TriPD01 = (float)d01;
    SW_TriPD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPDY02 - d02 * *(volatile float *)&SW_TriPDY01) * inv;
    SW_TriPVDX = (float)g;
    h = (*(volatile float *)&SW_TriPD02 * *(volatile float *)&SW_TriPDX01 - *(volatile float *)&SW_TriPD01 * *(volatile float *)&SW_TriPDX02) * inv;
    SW_TriPVDX16 = (float)(g * 16.0F);
    SW_TriPVDY = (float)h;
    SW_TriPVOff = (float)(*(volatile float *)&SW_TriPV0 - (v0->sx * *(volatile float *)&SW_TriPVDX + v0->sy * *(volatile float *)&SW_TriPVDY));
    d01 = (double)v0->rhw - v1->rhw;
    d02 = (double)v0->rhw - v2->rhw;
    SW_TriPD01 = (float)d01;
    SW_TriPD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPDY02 - d02 * *(volatile float *)&SW_TriPDY01) * inv;
    SW_TriPWDX = (float)g;
    h = (*(volatile float *)&SW_TriPD02 * *(volatile float *)&SW_TriPDX01 - *(volatile float *)&SW_TriPD01 * *(volatile float *)&SW_TriPDX02) * inv;
    SW_TriPWDX16 = (float)(g * 16.0F);
    SW_TriPWDY = (float)h;
    SW_TriPWOff = (float)(v0->rhw - (v0->sx * *(volatile float *)&SW_TriPWDX + v0->sy * *(volatile float *)&SW_TriPWDY));
    SW_TriPEdges = 3;
    span = SW_SpanBuffer;
    *(double *)SW_TriPY = ((double)t->sy - 0.5001F) + 6755399441055744.0;
    SW_TriPY[0]++;
    SW_SpanY = SW_TriPY[0];
    na = 0;
    nb = 0;
    xa = 0;
    xb = 0;
    for (;;) {
        if (!na) {
            do {
                t = SW_TriPEdgeA;
                if (SW_TriPEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->sz;
                SW_TriPEdges--;
                SW_TriPEdgeA = n;
                s = ((double)t->sx - n->sx) / ((double)t->sy - n->sy);
                *(double *)SW_TriPStepA = s + 103079215104.0;
                *(double *)SW_TriPTmp = ((double)SW_TriPY[0] - t->sy) * s + t->sx + 103079215104.0;
                xa = SW_TriPTmp[0];
                *(double *)SW_TriPTmp = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                na = SW_TriPTmp[0] - SW_TriPY[0] + 1;
            } while (!na);
            xa += 0xFFFF;
        }
        if (!nb) {
            do {
                t = SW_TriPEdgeB;
                if (SW_TriPEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->specular;
                SW_TriPEdges--;
                SW_TriPEdgeB = n;
                s = ((double)t->sx - n->sx) / ((double)t->sy - n->sy);
                *(double *)SW_TriPStepB = s + 103079215104.0;
                *(double *)SW_TriPTmp = ((double)SW_TriPY[0] - t->sy) * s + t->sx + 103079215104.0;
                xb = SW_TriPTmp[0];
                *(double *)SW_TriPTmp = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                nb = SW_TriPTmp[0] - SW_TriPY[0] + 1;
            } while (!nb);
            xb += 0xFFFF;
        }
        do {
            x = xb >> 16;
            *(sint32 *)&SW_TriPTemp = x;
            a = (double)SW_TriPY[0];
            b = (double)(sint32)x;
            cnt = (xa >> 16) - x;
            span->n = cnt;
            span->x = x;
            f = (float)((SW_TriPBDX * b + SW_TriPBDY * a) + SW_TriPBOff);
            span->c[0] = *(uint16 *)&f;
            SW_TriPTemp = (float)((SW_TriPGDX * b + SW_TriPGDY * a) + SW_TriPGOff);
            f = (float)((SW_TriPRDX * b + SW_TriPRDY * a) + SW_TriPROff);
            span->c[1] = *(uint16 *)&SW_TriPTemp;
            span->c[2] = ((uint16 *)&f)[0];
            span->c[3] = ((uint16 *)&f)[1];
            SW_TriPW = (float)((SW_TriPWDX * b + SW_TriPWDY * a) + SW_TriPWOff);
            u = (SW_TriPUDX * b + SW_TriPUDY * a) + SW_TriPUOff;
            v = (SW_TriPVDX * b + SW_TriPVDY * a) + SW_TriPVOff;
            c = *(volatile uint32 *)&SW_TriPW;
            *(uint32 *)&SW_TriPRecip = ((c ^ 0x7F800000) & 0xFF800000) + SW_RecipTable[(c >> 11) & 0xFFF];
            g = u * *(volatile float *)&SW_TriPRecip;
            h = v * *(volatile float *)&SW_TriPRecip;
            SW_TriPU = (float)g;
            SW_TriPV = (float)h;
            *(double *)&span->tex[0] = g + 402653184.0;
            *(double *)&span->tex[2] = h + 402653184.0;
            w = *(volatile float *)&SW_TriPW;
            pair = (sint32 *)(span + 1);
            for (cnt -= 16; cnt > 0; cnt -= 16) {
                w += SW_TriPWDX16;
                u += SW_TriPUDX16;
                v += SW_TriPVDX16;
                SW_TriPTemp = (float)w;
                c = *(volatile uint32 *)&SW_TriPTemp;
                *(uint32 *)&SW_TriPRecip = ((c ^ 0x7F800000) & 0xFF800000) + SW_RecipTable[(c >> 11) & 0xFFF];
                g = u * *(volatile float *)&SW_TriPRecip;
                h = v * *(volatile float *)&SW_TriPRecip;
                d01 = g - *(volatile float *)&SW_TriPU;
                SW_TriPU = (float)g;
                d02 = h - *(volatile float *)&SW_TriPV;
                SW_TriPV = (float)h;
                *(double *)pair = d01 * 0.0625F + 402653184.0;
                *(double *)SW_TriPTmp = d02 * 0.0625F + 402653184.0;
                pair[1] = SW_TriPTmp[0];
                pair += 2;
            }
            cnt &= 0xF;
            w += SW_TriPWDX * SW_TriPStepTab[cnt];
            SW_TriPW = (float)w;
            u += SW_TriPUDX * SW_TriPStepTab[cnt];
            v += SW_TriPVDX * SW_TriPStepTab[cnt];
            c = *(volatile uint32 *)&SW_TriPW;
            *(uint32 *)&SW_TriPRecip = ((c ^ 0x7F800000) & 0xFF800000) + SW_RecipTable[(c >> 11) & 0xFFF];
            d01 = u * *(volatile float *)&SW_TriPRecip - *(volatile float *)&SW_TriPU;
            d02 = v * *(volatile float *)&SW_TriPRecip - *(volatile float *)&SW_TriPV;
            *(double *)pair = d01 * SW_TriPRecipTab[cnt] + 402653184.0;
            *(double *)SW_TriPTmp = d02 * SW_TriPRecipTab[cnt] + 402653184.0;
            pair[1] = SW_TriPTmp[0];
            span = (SW_SPAN *)(pair + 2);
            xb += SW_TriPStepB[0];
            xa += SW_TriPStepA[0];
            SW_TriPY[0]++;
            nb--;
            na--;
        } while (na && nb);
    }
done:
    SW_SpanEnd = span;
    SW_SpanFunc();
    return 0;
}

sint32 SW_TriSetupGT(void *p0, void *p1, void *p2)
{
    SW_VERTEX *v0, *v1, *v2, *t, *n;
    SW_SPAN *span;
    double a, b, inv, d01, d02, g, h;
    float f;
    uint32 sy, c, xa, xb, x;
    sint32 na, nb;

    v0 = (SW_VERTEX *)p0;
    v1 = (SW_VERTEX *)p1;
    v2 = (SW_VERTEX *)p2;
    SW_TriV0 = v0;
    a = (double)v0->sx - v1->sx;
    b = (double)v0->sy - v2->sy;
    SW_TriTDX01 = (float)a;
    SW_TriTDY02 = (float)b;
    g = a * b;
    a = (double)v0->sx - v2->sx;
    b = (double)v0->sy - v1->sy;
    SW_TriTDX02 = (float)a;
    SW_TriTDY01 = (float)b;
    g -= a * b;
    SW_TriTTemp = (float)g;
    inv = 1.0;
    if (*(uint32 *)&SW_TriTTemp & 0x80000000) {
        t = v1;
        v1 = v2;
        v2 = t;
        inv = -1.0;
        f = SW_TriTDX01;
        SW_TriTDX01 = SW_TriTDX02;
        SW_TriTDX02 = f;
        f = SW_TriTDY01;
        SW_TriTDY01 = SW_TriTDY02;
        SW_TriTDY02 = f;
    }
    inv /= g;
    *(SW_VERTEX **)&v0->sz = v1;
    *(SW_VERTEX **)&v1->sz = v2;
    *(SW_VERTEX **)&v2->sz = v0;
    *(SW_VERTEX **)&v0->specular = v2;
    *(SW_VERTEX **)&v1->specular = v0;
    *(SW_VERTEX **)&v2->specular = v1;
    t = v0;
    sy = *(uint32 *)&v0->sy;
    if (sy >= *(uint32 *)&v1->sy) {
        sy = *(uint32 *)&v1->sy;
        t = v1;
    }
    if (sy >= *(uint32 *)&v2->sy)
        t = v2;
    SW_TriTEdgeA = t;
    SW_TriTEdgeB = t;
    SW_TriColor0 = v0->color;
    c = v0->color;
    SW_TriTG0 = (c & 0xFF00) + 0x80;
    SW_TriTB0 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriTR0 = ((c >> 8) & 0xFF00) + 0x80;
    c = v1->color;
    SW_TriTG1 = (c & 0xFF00) + 0x80;
    SW_TriTB1 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriTR1 = ((c >> 8) & 0xFF00) + 0x80;
    c = v2->color;
    SW_TriTG2 = (c & 0xFF00) + 0x80;
    SW_TriTB2 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriTR2 = ((c >> 8) & 0xFF00) + 0x80;
    d01 = (double)SW_TriTR0 - SW_TriTR1;
    d02 = (double)SW_TriTR0 - SW_TriTR2;
    SW_TriTD01 = (float)d01;
    SW_TriTD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTDY02 - d02 * *(volatile float *)&SW_TriTDY01) * inv;
    SW_SpanGrad[2] = SW_RoundW(g);
    SW_TriTRDX = (float)g;
    h = (*(volatile float *)&SW_TriTD02 * *(volatile float *)&SW_TriTDX01 - *(volatile float *)&SW_TriTD01 * *(volatile float *)&SW_TriTDX02) * inv;
    SW_TriTRDY = (float)h;
    SW_TriTROff = ((double)SW_TriTR0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriTRDX);
    d01 = (double)SW_TriTG0 - SW_TriTG1;
    d02 = (double)SW_TriTG0 - SW_TriTG2;
    SW_TriTD01 = (float)d01;
    SW_TriTD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTDY02 - d02 * *(volatile float *)&SW_TriTDY01) * inv;
    SW_SpanGrad[1] = SW_RoundW(g);
    SW_TriTGDX = (float)g;
    h = (*(volatile float *)&SW_TriTD02 * *(volatile float *)&SW_TriTDX01 - *(volatile float *)&SW_TriTD01 * *(volatile float *)&SW_TriTDX02) * inv;
    SW_TriTGDY = (float)h;
    SW_TriTGOff = ((double)SW_TriTG0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriTGDX);
    d01 = (double)SW_TriTB0 - SW_TriTB1;
    d02 = (double)SW_TriTB0 - SW_TriTB2;
    SW_TriTD01 = (float)d01;
    SW_TriTD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTDY02 - d02 * *(volatile float *)&SW_TriTDY01) * inv;
    SW_SpanGrad[0] = SW_RoundW(g);
    SW_TriTBDX = (float)g;
    h = (*(volatile float *)&SW_TriTD02 * *(volatile float *)&SW_TriTDX01 - *(volatile float *)&SW_TriTD01 * *(volatile float *)&SW_TriTDX02) * inv;
    SW_TriTBDY = (float)h;
    SW_TriTBOff = ((double)SW_TriTB0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriTBDX);
    d01 = (double)v0->tu - v1->tu;
    d02 = (double)v0->tu - v2->tu;
    SW_TriTD01 = (float)d01;
    SW_TriTD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTDY02 - d02 * *(volatile float *)&SW_TriTDY01) * inv;
    SW_TriTUDX = (float)g;
    h = (*(volatile float *)&SW_TriTD02 * *(volatile float *)&SW_TriTDX01 - *(volatile float *)&SW_TriTD01 * *(volatile float *)&SW_TriTDX02) * inv;
    SW_TriTUDY = (float)h;
    *(double *)&SW_SpanDU = g + 402653184.0;
    SW_TriTUOff = ((double)v0->tu + 402653184.0) - (v0->sx * *(volatile float *)&SW_TriTUDX + v0->sy * *(volatile float *)&SW_TriTUDY);
    d01 = (double)v0->tv - v1->tv;
    d02 = (double)v0->tv - v2->tv;
    SW_TriTD01 = (float)d01;
    SW_TriTD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTDY02 - d02 * *(volatile float *)&SW_TriTDY01) * inv;
    SW_TriTVDX = (float)g;
    h = (*(volatile float *)&SW_TriTD02 * *(volatile float *)&SW_TriTDX01 - *(volatile float *)&SW_TriTD01 * *(volatile float *)&SW_TriTDX02) * inv;
    SW_TriTVDY = (float)h;
    *(double *)&SW_SpanDV = g + 402653184.0;
    SW_TriTVOff = ((double)v0->tv + 402653184.0) - (v0->sx * *(volatile float *)&SW_TriTVDX + v0->sy * *(volatile float *)&SW_TriTVDY);
    SW_TriTEdges = 3;
    span = SW_SpanBuffer;
    *(double *)SW_TriTY = ((double)t->sy - 0.5001F) + 6755399441055744.0;
    SW_TriTY[0]++;
    SW_SpanY = SW_TriTY[0];
    na = 0;
    nb = 0;
    xa = 0;
    xb = 0;
    for (;;) {
        if (!na) {
            do {
                t = SW_TriTEdgeA;
                if (SW_TriTEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->sz;
                SW_TriTEdges--;
                SW_TriTEdgeA = n;
                SW_TriTSlopeA = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriTXA = (float)(((double)SW_TriTY[0] - t->sy) * *(volatile float *)&SW_TriTSlopeA + t->sx);
                *(double *)SW_TriTEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                na = SW_TriTEndY[0] - SW_TriTY[0] + 1;
            } while (!na);
            SW_TriTStepA = SW_RoundL(*(volatile float *)&SW_TriTSlopeA * 65536.0F);
            *(sint32 *)&SW_TriTTemp = SW_RoundL(*(volatile float *)&SW_TriTXA * 65536.0F);
            xa = *(sint32 *)&SW_TriTTemp + 0xFFFF;
        }
        if (!nb) {
            do {
                t = SW_TriTEdgeB;
                if (SW_TriTEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->specular;
                SW_TriTEdges--;
                SW_TriTEdgeB = n;
                SW_TriTSlopeB = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriTXB = (float)(((double)SW_TriTY[0] - t->sy) * *(volatile float *)&SW_TriTSlopeB + t->sx);
                *(double *)SW_TriTEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                nb = SW_TriTEndY[0] - SW_TriTY[0] + 1;
            } while (!nb);
            SW_TriTStepB = SW_RoundL(*(volatile float *)&SW_TriTSlopeB * 65536.0F);
            *(sint32 *)&SW_TriTTemp = SW_RoundL(*(volatile float *)&SW_TriTXB * 65536.0F);
            xb = *(sint32 *)&SW_TriTTemp + 0xFFFF;
        }
        do {
            x = xb >> 16;
            *(sint32 *)&SW_TriTTemp = x;
            a = (double)SW_TriTY[0];
            b = (double)(sint32)x;
            span->n = (xa >> 16) - x;
            span->x = x;
            *(double *)&span->tex[0] = (SW_TriTUDX * b + SW_TriTUDY * a) + SW_TriTUOff;
            *(double *)&span->tex[2] = (SW_TriTVDX * b + SW_TriTVDY * a) + SW_TriTVOff;
            SW_TriTY[0]++;
            f = (float)((SW_TriTBDX * b + SW_TriTBDY * a) + SW_TriTBOff);
            span->c[0] = *(uint16 *)&f;
            SW_TriTTemp = (float)((SW_TriTGDX * b + SW_TriTGDY * a) + SW_TriTGOff);
            f = (float)((SW_TriTRDX * b + SW_TriTRDY * a) + SW_TriTROff);
            span->c[1] = *(uint16 *)&SW_TriTTemp;
            span->c[2] = ((uint16 *)&f)[0];
            span->c[3] = ((uint16 *)&f)[1];
            span++;
            xb += SW_TriTStepB;
            xa += SW_TriTStepA;
            nb--;
            na--;
        } while (na && nb);
    }
done:
    SW_SpanEnd = span;
    SW_SpanFunc();
    return 0;
}

sint32 SW_TriSetupG(void *p0, void *p1, void *p2)
{
    SW_VERTEX *v0, *v1, *v2, *t, *n;
    SW_SPAN *span;
    double a, b, inv, d01, d02, g, h;
    float f;
    uint32 sy, c, xa, xb, x;
    sint32 na, nb;

    v0 = (SW_VERTEX *)p0;
    v1 = (SW_VERTEX *)p1;
    v2 = (SW_VERTEX *)p2;
    SW_TriV0 = v0;
    a = (double)v0->sx - v1->sx;
    b = (double)v0->sy - v2->sy;
    SW_TriDX01 = (float)a;
    SW_TriDY02 = (float)b;
    g = a * b;
    a = (double)v0->sx - v2->sx;
    b = (double)v0->sy - v1->sy;
    SW_TriDX02 = (float)a;
    SW_TriDY01 = (float)b;
    g -= a * b;
    SW_TriTemp = (float)g;
    inv = 1.0;
    if (*(uint32 *)&SW_TriTemp & 0x80000000) {
        t = v1;
        v1 = v2;
        v2 = t;
        inv = -1.0;
        f = SW_TriDX01;
        SW_TriDX01 = SW_TriDX02;
        SW_TriDX02 = f;
        f = SW_TriDY01;
        SW_TriDY01 = SW_TriDY02;
        SW_TriDY02 = f;
    }
    inv /= g;
    *(SW_VERTEX **)&v0->sz = v1;
    *(SW_VERTEX **)&v1->sz = v2;
    *(SW_VERTEX **)&v2->sz = v0;
    *(SW_VERTEX **)&v0->specular = v2;
    *(SW_VERTEX **)&v1->specular = v0;
    *(SW_VERTEX **)&v2->specular = v1;
    t = v0;
    sy = *(uint32 *)&v0->sy;
    if (sy >= *(uint32 *)&v1->sy) {
        sy = *(uint32 *)&v1->sy;
        t = v1;
    }
    if (sy >= *(uint32 *)&v2->sy)
        t = v2;
    SW_TriEdgeA = t;
    SW_TriEdgeB = t;
    SW_TriColor0 = v0->color;
    c = v0->color;
    SW_TriG0 = (c & 0xFF00) + 0x80;
    SW_TriB0 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriR0 = ((c >> 8) & 0xFF00) + 0x80;
    SW_TriA0 = ((c >> 16) & 0xFF00) + 0x80;
    c = v1->color;
    SW_TriG1 = (c & 0xFF00) + 0x80;
    SW_TriB1 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriR1 = ((c >> 8) & 0xFF00) + 0x80;
    SW_TriA1 = (c >> 16) + 0x80;
    c = v2->color;
    SW_TriG2 = (c & 0xFF00) + 0x80;
    SW_TriB2 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriR2 = ((c >> 8) & 0xFF00) + 0x80;
    SW_TriA2 = (c >> 16) + 0x80;
    d01 = (double)SW_TriR0 - SW_TriR1;
    d02 = (double)SW_TriR0 - SW_TriR2;
    SW_TriD01 = (float)d01;
    SW_TriD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriDY02 - d02 * *(volatile float *)&SW_TriDY01) * inv;
    SW_SpanGrad[2] = SW_RoundW(g);
    SW_TriRDX = (float)g;
    h = (SW_TriD02 * *(volatile float *)&SW_TriDX01 - SW_TriD01 * *(volatile float *)&SW_TriDX02) * inv;
    SW_TriRDY = (float)h;
    SW_TriROff = ((double)SW_TriR0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriRDX);
    d01 = (double)SW_TriG0 - SW_TriG1;
    d02 = (double)SW_TriG0 - SW_TriG2;
    SW_TriD01 = (float)d01;
    SW_TriD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriDY02 - d02 * *(volatile float *)&SW_TriDY01) * inv;
    SW_SpanGrad[1] = SW_RoundW(g);
    SW_TriGDX = (float)g;
    h = (SW_TriD02 * *(volatile float *)&SW_TriDX01 - SW_TriD01 * *(volatile float *)&SW_TriDX02) * inv;
    SW_TriGDY = (float)h;
    SW_TriGOff = ((double)SW_TriG0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriGDX);
    d01 = (double)SW_TriB0 - SW_TriB1;
    d02 = (double)SW_TriB0 - SW_TriB2;
    SW_TriD01 = (float)d01;
    SW_TriD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriDY02 - d02 * *(volatile float *)&SW_TriDY01) * inv;
    SW_SpanGrad[0] = SW_RoundW(g);
    SW_TriBDX = (float)g;
    h = (SW_TriD02 * *(volatile float *)&SW_TriDX01 - SW_TriD01 * *(volatile float *)&SW_TriDX02) * inv;
    SW_TriBDY = (float)h;
    SW_TriBOff = ((double)SW_TriB0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriBDX);
    d01 = (double)SW_TriA0 - SW_TriA1;
    d02 = (double)SW_TriA0 - SW_TriA2;
    SW_TriD01 = (float)d01;
    SW_TriD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriDY02 - d02 * *(volatile float *)&SW_TriDY01) * inv;
    SW_SpanGrad[3] = SW_RoundW(g);
    SW_TriADX = (float)g;
    h = (SW_TriD02 * *(volatile float *)&SW_TriDX01 - SW_TriD01 * *(volatile float *)&SW_TriDX02) * inv;
    SW_TriADY = (float)h;
    SW_TriAOff = ((double)SW_TriA0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriADX);
    SW_TriEdges = 3;
    span = SW_SpanBuffer;
    *(double *)SW_TriY = ((double)t->sy - 0.5001F) + 6755399441055744.0;
    SW_TriY[0]++;
    SW_SpanY = SW_TriY[0];
    na = 0;
    nb = 0;
    xa = 0;
    xb = 0;
    for (;;) {
        if (!na) {
            do {
                t = SW_TriEdgeA;
                if (SW_TriEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->sz;
                SW_TriEdges--;
                SW_TriEdgeA = n;
                SW_TriSlopeA = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriXA = (float)(((double)SW_TriY[0] - t->sy) * *(volatile float *)&SW_TriSlopeA + t->sx);
                *(double *)SW_TriEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                na = SW_TriEndY[0] - SW_TriY[0] + 1;
            } while (!na);
            SW_TriStepA = SW_RoundL(*(volatile float *)&SW_TriSlopeA * 65536.0F);
            *(sint32 *)&SW_TriTemp = SW_RoundL(*(volatile float *)&SW_TriXA * 65536.0F);
            xa = *(sint32 *)&SW_TriTemp + 0xFFFF;
        }
        if (!nb) {
            do {
                t = SW_TriEdgeB;
                if (SW_TriEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->specular;
                SW_TriEdges--;
                SW_TriEdgeB = n;
                SW_TriSlopeB = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriXB = (float)(((double)SW_TriY[0] - t->sy) * *(volatile float *)&SW_TriSlopeB + t->sx);
                *(double *)SW_TriEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                nb = SW_TriEndY[0] - SW_TriY[0] + 1;
            } while (!nb);
            SW_TriStepB = SW_RoundL(*(volatile float *)&SW_TriSlopeB * 65536.0F);
            *(sint32 *)&SW_TriTemp = SW_RoundL(*(volatile float *)&SW_TriXB * 65536.0F);
            xb = *(sint32 *)&SW_TriTemp + 0xFFFF;
        }
        do {
            x = xb >> 16;
            *(sint32 *)&SW_TriTemp = x;
            a = (double)SW_TriY[0];
            b = (double)(sint32)x;
            SW_TriTemp = (float)((SW_TriADX * b + SW_TriADY * a) + SW_TriAOff);
            span->n = (xa >> 16) - x;
            span->x = x;
            f = (float)((SW_TriBDX * b + SW_TriBDY * a) + SW_TriBOff);
            span->c[0] = *(uint16 *)&f;
            f = (float)((SW_TriGDX * b + SW_TriGDY * a) + SW_TriGOff);
            span->c[1] = *(uint16 *)&f;
            f = (float)((SW_TriRDX * b + SW_TriRDY * a) + SW_TriROff);
            span->c[2] = *(uint16 *)&f;
            span->c[3] = *(uint16 *)&SW_TriTemp;
            span++;
            SW_TriY[0]++;
            xb += SW_TriStepB;
            xa += SW_TriStepA;
            nb--;
            na--;
        } while (na && nb);
    }
done:
    SW_SpanEnd = span;
    SW_SpanFunc();
    return 0;
}

sint32 SW_TriSetupGTPFog(void *p0, void *p1, void *p2)
{
    SW_VERTEX *v0, *v1, *v2, *t, *n;
    SW_SPAN *span;
    sint32 *pair;
    double a, b, inv, d01, d02, g, h, s, u, v, w;
    float f;
    uint32 sy, c, sp, xa, xb, x;
    sint32 na, nb, cnt;

    v0 = (SW_VERTEX *)p0;
    v1 = (SW_VERTEX *)p1;
    v2 = (SW_VERTEX *)p2;
    SW_TriV0 = v0;
    SW_TriPFU0 = v0->tu * v0->rhw;
    SW_TriPFV0 = v0->tv * v0->rhw;
    SW_TriPFU1 = v1->tu * v1->rhw;
    SW_TriPFV1 = v1->tv * v1->rhw;
    SW_TriPFU2 = v2->tu * v2->rhw;
    SW_TriPFV2 = v2->tv * v2->rhw;
    a = (double)v0->sx - v1->sx;
    b = (double)v0->sy - v2->sy;
    SW_TriPFDX01 = (float)a;
    SW_TriPFDY02 = (float)b;
    g = a * b;
    a = (double)v0->sx - v2->sx;
    b = (double)v0->sy - v1->sy;
    SW_TriPFDX02 = (float)a;
    SW_TriPFDY01 = (float)b;
    g -= a * b;
    SW_TriPFTemp = (float)g;
    inv = 1.0;
    if (*(uint32 *)&SW_TriPFTemp & 0x80000000) {
        t = v1;
        v1 = v2;
        v2 = t;
        inv = -1.0;
        f = SW_TriPFDX01;
        SW_TriPFDX01 = SW_TriPFDX02;
        SW_TriPFDX02 = f;
        f = SW_TriPFDY01;
        SW_TriPFDY01 = SW_TriPFDY02;
        SW_TriPFDY02 = f;
    }
    inv /= g;
    t = v0;
    sy = *(uint32 *)&v0->sy;
    if (sy >= *(uint32 *)&v1->sy) {
        sy = *(uint32 *)&v1->sy;
        t = v1;
    }
    if (sy >= *(uint32 *)&v2->sy)
        t = v2;
    SW_TriPFEdgeA = t;
    SW_TriPFEdgeB = t;
    SW_TriColor0 = v0->color;
    c = v0->color;
    sp = v0->specular;
    SW_TriPFF0 = (sp >> 16) & 0xFF00;
    SW_TriPFR0 = (SW_MulTable[((c >> 16) & 0xFF) + SW_TriPFF0] << 8) | 0x80;
    SW_TriPFG0 = (SW_MulTable[((c >> 8) & 0xFF) + SW_TriPFF0] << 8) | 0x80;
    SW_TriPFB0 = (SW_MulTable[(c & 0xFF) + SW_TriPFF0] << 8) | 0x80;
    c = v1->color;
    sp = v1->specular;
    SW_TriPFF1 = (sp >> 16) & 0xFF00;
    SW_TriPFR1 = (SW_MulTable[((c >> 16) & 0xFF) + SW_TriPFF1] << 8) | 0x80;
    SW_TriPFG1 = (SW_MulTable[((c >> 8) & 0xFF) + SW_TriPFF1] << 8) | 0x80;
    SW_TriPFB1 = (SW_MulTable[(c & 0xFF) + SW_TriPFF1] << 8) | 0x80;
    c = v2->color;
    sp = v2->specular;
    SW_TriPFF2 = (sp >> 16) & 0xFF00;
    SW_TriPFR2 = (SW_MulTable[((c >> 16) & 0xFF) + SW_TriPFF2] << 8) | 0x80;
    SW_TriPFG2 = (SW_MulTable[((c >> 8) & 0xFF) + SW_TriPFF2] << 8) | 0x80;
    SW_TriPFB2 = (SW_MulTable[(c & 0xFF) + SW_TriPFF2] << 8) | 0x80;
    *(SW_VERTEX **)&v0->sz = v1;
    *(SW_VERTEX **)&v1->sz = v2;
    *(SW_VERTEX **)&v2->sz = v0;
    *(SW_VERTEX **)&v0->specular = v2;
    *(SW_VERTEX **)&v1->specular = v0;
    *(SW_VERTEX **)&v2->specular = v1;
    d01 = (double)SW_TriPFF0 - SW_TriPFF1;
    d02 = (double)SW_TriPFF0 - SW_TriPFF2;
    SW_TriPFD01 = (float)d01;
    SW_TriPFD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPFDY02 - d02 * *(volatile float *)&SW_TriPFDY01) * inv;
    SW_SpanFogGrad = SW_RoundL(g);
    SW_TriPFFDX = (float)g;
    h = (*(volatile float *)&SW_TriPFD02 * *(volatile float *)&SW_TriPFDX01 - *(volatile float *)&SW_TriPFD01 * *(volatile float *)&SW_TriPFDX02) * inv;
    SW_TriPFFDY = (float)h;
    SW_TriPFFOff = ((double)SW_TriPFF0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriPFFDX);
    d01 = (double)SW_TriPFR0 - SW_TriPFR1;
    d02 = (double)SW_TriPFR0 - SW_TriPFR2;
    SW_TriPFD01 = (float)d01;
    SW_TriPFD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPFDY02 - d02 * *(volatile float *)&SW_TriPFDY01) * inv;
    SW_SpanGrad[2] = SW_RoundW(g);
    SW_TriPFRDX = (float)g;
    h = (*(volatile float *)&SW_TriPFD02 * *(volatile float *)&SW_TriPFDX01 - *(volatile float *)&SW_TriPFD01 * *(volatile float *)&SW_TriPFDX02) * inv;
    SW_TriPFRDY = (float)h;
    SW_TriPFROff = ((double)SW_TriPFR0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriPFRDX);
    d01 = (double)SW_TriPFG0 - SW_TriPFG1;
    d02 = (double)SW_TriPFG0 - SW_TriPFG2;
    SW_TriPFD01 = (float)d01;
    SW_TriPFD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPFDY02 - d02 * *(volatile float *)&SW_TriPFDY01) * inv;
    SW_SpanGrad[1] = SW_RoundW(g);
    SW_TriPFGDX = (float)g;
    h = (*(volatile float *)&SW_TriPFD02 * *(volatile float *)&SW_TriPFDX01 - *(volatile float *)&SW_TriPFD01 * *(volatile float *)&SW_TriPFDX02) * inv;
    SW_TriPFGDY = (float)h;
    SW_TriPFGOff = ((double)SW_TriPFG0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriPFGDX);
    d01 = (double)SW_TriPFB0 - SW_TriPFB1;
    d02 = (double)SW_TriPFB0 - SW_TriPFB2;
    SW_TriPFD01 = (float)d01;
    SW_TriPFD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPFDY02 - d02 * *(volatile float *)&SW_TriPFDY01) * inv;
    SW_SpanGrad[0] = SW_RoundW(g);
    SW_TriPFBDX = (float)g;
    h = (*(volatile float *)&SW_TriPFD02 * *(volatile float *)&SW_TriPFDX01 - *(volatile float *)&SW_TriPFD01 * *(volatile float *)&SW_TriPFDX02) * inv;
    SW_TriPFBDY = (float)h;
    SW_TriPFBOff = ((double)SW_TriPFB0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriPFBDX);
    d01 = (double)*(volatile float *)&SW_TriPFU0 - *(volatile float *)&SW_TriPFU1;
    d02 = (double)*(volatile float *)&SW_TriPFU0 - *(volatile float *)&SW_TriPFU2;
    SW_TriPFD01 = (float)d01;
    SW_TriPFD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPFDY02 - d02 * *(volatile float *)&SW_TriPFDY01) * inv;
    SW_TriPFUDX = (float)g;
    h = (*(volatile float *)&SW_TriPFD02 * *(volatile float *)&SW_TriPFDX01 - *(volatile float *)&SW_TriPFD01 * *(volatile float *)&SW_TriPFDX02) * inv;
    SW_TriPFUDX16 = (float)(g * 16.0F);
    SW_TriPFUDY = (float)h;
    SW_TriPFUOff = (float)(*(volatile float *)&SW_TriPFU0 - (v0->sx * *(volatile float *)&SW_TriPFUDX + v0->sy * *(volatile float *)&SW_TriPFUDY));
    d01 = (double)*(volatile float *)&SW_TriPFV0 - *(volatile float *)&SW_TriPFV1;
    d02 = (double)*(volatile float *)&SW_TriPFV0 - *(volatile float *)&SW_TriPFV2;
    SW_TriPFD01 = (float)d01;
    SW_TriPFD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPFDY02 - d02 * *(volatile float *)&SW_TriPFDY01) * inv;
    SW_TriPFVDX = (float)g;
    h = (*(volatile float *)&SW_TriPFD02 * *(volatile float *)&SW_TriPFDX01 - *(volatile float *)&SW_TriPFD01 * *(volatile float *)&SW_TriPFDX02) * inv;
    SW_TriPFVDX16 = (float)(g * 16.0F);
    SW_TriPFVDY = (float)h;
    SW_TriPFVOff = (float)(*(volatile float *)&SW_TriPFV0 - (v0->sx * *(volatile float *)&SW_TriPFVDX + v0->sy * *(volatile float *)&SW_TriPFVDY));
    d01 = (double)v0->rhw - v1->rhw;
    d02 = (double)v0->rhw - v2->rhw;
    SW_TriPFD01 = (float)d01;
    SW_TriPFD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriPFDY02 - d02 * *(volatile float *)&SW_TriPFDY01) * inv;
    SW_TriPFWDX = (float)g;
    h = (*(volatile float *)&SW_TriPFD02 * *(volatile float *)&SW_TriPFDX01 - *(volatile float *)&SW_TriPFD01 * *(volatile float *)&SW_TriPFDX02) * inv;
    SW_TriPFWDX16 = (float)(g * 16.0F);
    SW_TriPFWDY = (float)h;
    SW_TriPFWOff = (float)(v0->rhw - (v0->sx * *(volatile float *)&SW_TriPFWDX + v0->sy * *(volatile float *)&SW_TriPFWDY));
    SW_TriPFEdges = 3;
    span = SW_SpanBuffer;
    *(double *)SW_TriPFY = ((double)t->sy - 0.5001F) + 6755399441055744.0;
    SW_TriPFY[0]++;
    SW_SpanY = SW_TriPFY[0];
    na = 0;
    nb = 0;
    xa = 0;
    xb = 0;
    for (;;) {
        if (!na) {
            do {
                t = SW_TriPFEdgeA;
                if (SW_TriPFEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->sz;
                SW_TriPFEdges--;
                SW_TriPFEdgeA = n;
                s = ((double)t->sx - n->sx) / ((double)t->sy - n->sy);
                *(double *)SW_TriPFStepA = s + 103079215104.0;
                *(double *)SW_TriPFTmp = ((double)SW_TriPFY[0] - t->sy) * s + t->sx + 103079215104.0;
                xa = SW_TriPFTmp[0];
                *(double *)SW_TriPFTmp = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                na = SW_TriPFTmp[0] - SW_TriPFY[0] + 1;
            } while (!na);
            xa += 0xFFFF;
        }
        if (!nb) {
            do {
                t = SW_TriPFEdgeB;
                if (SW_TriPFEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->specular;
                SW_TriPFEdges--;
                SW_TriPFEdgeB = n;
                s = ((double)t->sx - n->sx) / ((double)t->sy - n->sy);
                *(double *)SW_TriPFStepB = s + 103079215104.0;
                *(double *)SW_TriPFTmp = ((double)SW_TriPFY[0] - t->sy) * s + t->sx + 103079215104.0;
                xb = SW_TriPFTmp[0];
                *(double *)SW_TriPFTmp = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                nb = SW_TriPFTmp[0] - SW_TriPFY[0] + 1;
            } while (!nb);
            xb += 0xFFFF;
        }
        do {
            x = xb >> 16;
            *(sint32 *)&SW_TriPFTemp = x;
            a = (double)SW_TriPFY[0];
            b = (double)(sint32)x;
            cnt = (xa >> 16) - x;
            span->n = cnt;
            span->x = x;
            SW_TriPFFog = (float)((SW_TriPFFDX * b + SW_TriPFFDY * a) + SW_TriPFFOff);
            f = (float)((SW_TriPFBDX * b + SW_TriPFBDY * a) + SW_TriPFBOff);
            span->c[0] = *(uint16 *)&f;
            SW_TriPFTemp = (float)((SW_TriPFGDX * b + SW_TriPFGDY * a) + SW_TriPFGOff);
            f = (float)((SW_TriPFRDX * b + SW_TriPFRDY * a) + SW_TriPFROff);
            span->c[1] = *(uint16 *)&SW_TriPFTemp;
            span->c[2] = ((uint16 *)&f)[0];
            span->c[3] = ((uint16 *)&f)[1];
            SW_TriPFW = (float)((SW_TriPFWDX * b + SW_TriPFWDY * a) + SW_TriPFWOff);
            u = (SW_TriPFUDX * b + SW_TriPFUDY * a) + SW_TriPFUOff;
            v = (SW_TriPFVDX * b + SW_TriPFVDY * a) + SW_TriPFVOff;
            c = *(volatile uint32 *)&SW_TriPFW;
            *(uint32 *)&SW_TriPFRecip = ((c ^ 0x7F800000) & 0xFF800000) + SW_RecipTable[(c >> 11) & 0xFFF];
            g = u * *(volatile float *)&SW_TriPFRecip;
            h = v * *(volatile float *)&SW_TriPFRecip;
            SW_TriPFU = (float)g;
            SW_TriPFV = (float)h;
            *(double *)&span->tex[0] = g + 402653184.0;
            *(double *)&span->tex[2] = h + 402653184.0;
            span->tex[1] = *(sint32 *)&SW_TriPFFog;
            w = *(volatile float *)&SW_TriPFW;
            pair = (sint32 *)(span + 1);
            for (cnt -= 16; cnt > 0; cnt -= 16) {
                w += SW_TriPFWDX16;
                u += SW_TriPFUDX16;
                v += SW_TriPFVDX16;
                SW_TriPFTemp = (float)w;
                c = *(volatile uint32 *)&SW_TriPFTemp;
                *(uint32 *)&SW_TriPFRecip = ((c ^ 0x7F800000) & 0xFF800000) + SW_RecipTable[(c >> 11) & 0xFFF];
                g = u * *(volatile float *)&SW_TriPFRecip;
                h = v * *(volatile float *)&SW_TriPFRecip;
                d01 = g - *(volatile float *)&SW_TriPFU;
                SW_TriPFU = (float)g;
                d02 = h - *(volatile float *)&SW_TriPFV;
                SW_TriPFV = (float)h;
                *(double *)pair = d01 * 0.0625F + 402653184.0;
                *(double *)SW_TriPFTmp = d02 * 0.0625F + 402653184.0;
                pair[1] = SW_TriPFTmp[0];
                pair += 2;
            }
            cnt &= 0xF;
            w += SW_TriPFWDX * SW_TriPStepTab[cnt];
            SW_TriPFW = (float)w;
            u += SW_TriPFUDX * SW_TriPStepTab[cnt];
            v += SW_TriPFVDX * SW_TriPStepTab[cnt];
            c = *(volatile uint32 *)&SW_TriPFW;
            *(uint32 *)&SW_TriPFRecip = ((c ^ 0x7F800000) & 0xFF800000) + SW_RecipTable[(c >> 11) & 0xFFF];
            d01 = u * *(volatile float *)&SW_TriPFRecip - *(volatile float *)&SW_TriPFU;
            d02 = v * *(volatile float *)&SW_TriPFRecip - *(volatile float *)&SW_TriPFV;
            *(double *)pair = d01 * SW_TriPRecipTab[cnt] + 402653184.0;
            *(double *)SW_TriPFTmp = d02 * SW_TriPRecipTab[cnt] + 402653184.0;
            pair[1] = SW_TriPFTmp[0];
            span = (SW_SPAN *)(pair + 2);
            xb += SW_TriPFStepB[0];
            xa += SW_TriPFStepA[0];
            SW_TriPFY[0]++;
            nb--;
            na--;
        } while (na && nb);
    }
done:
    SW_SpanEnd = span;
    SW_SpanFunc();
    SW_SpanFogFunc();
    return 0;
}

sint32 SW_TriSetupGTFog(void *p0, void *p1, void *p2)
{
    SW_VERTEX *v0, *v1, *v2, *t, *n;
    SW_SPAN *span;
    double a, b, inv, d01, d02, g, h;
    float f;
    uint32 sy, c, s, xa, xb, x;
    sint32 na, nb;

    v0 = (SW_VERTEX *)p0;
    v1 = (SW_VERTEX *)p1;
    v2 = (SW_VERTEX *)p2;
    SW_TriV0 = v0;
    a = (double)v0->sx - v1->sx;
    b = (double)v0->sy - v2->sy;
    SW_TriTFogDX01 = (float)a;
    SW_TriTFogDY02 = (float)b;
    g = a * b;
    a = (double)v0->sx - v2->sx;
    b = (double)v0->sy - v1->sy;
    SW_TriTFogDX02 = (float)a;
    SW_TriTFogDY01 = (float)b;
    g -= a * b;
    SW_TriTFogTemp = (float)g;
    inv = 1.0;
    if (*(uint32 *)&SW_TriTFogTemp & 0x80000000) {
        t = v1;
        v1 = v2;
        v2 = t;
        inv = -1.0;
        f = SW_TriTFogDX01;
        SW_TriTFogDX01 = SW_TriTFogDX02;
        SW_TriTFogDX02 = f;
        f = SW_TriTFogDY01;
        SW_TriTFogDY01 = SW_TriTFogDY02;
        SW_TriTFogDY02 = f;
    }
    inv /= g;
    t = v0;
    sy = *(uint32 *)&v0->sy;
    if (sy >= *(uint32 *)&v1->sy) {
        sy = *(uint32 *)&v1->sy;
        t = v1;
    }
    if (sy >= *(uint32 *)&v2->sy)
        t = v2;
    SW_TriTFogEdgeA = t;
    SW_TriTFogEdgeB = t;
    SW_TriColor0 = v0->color;
    c = v0->color;
    s = v0->specular;
    SW_TriTFogF0 = (s >> 16) & 0xFF00;
    SW_TriTFogR0 = (SW_MulTable[((c >> 16) & 0xFF) + SW_TriTFogF0] << 8) | 0x80;
    SW_TriTFogG0 = (SW_MulTable[((c >> 8) & 0xFF) + SW_TriTFogF0] << 8) | 0x80;
    SW_TriTFogB0 = (SW_MulTable[(c & 0xFF) + SW_TriTFogF0] << 8) | 0x80;
    c = v1->color;
    s = v1->specular;
    SW_TriTFogF1 = (s >> 16) & 0xFF00;
    SW_TriTFogR1 = (SW_MulTable[((c >> 16) & 0xFF) + SW_TriTFogF1] << 8) | 0x80;
    SW_TriTFogG1 = (SW_MulTable[((c >> 8) & 0xFF) + SW_TriTFogF1] << 8) | 0x80;
    SW_TriTFogB1 = (SW_MulTable[(c & 0xFF) + SW_TriTFogF1] << 8) | 0x80;
    c = v2->color;
    s = v2->specular;
    SW_TriTFogF2 = (s >> 16) & 0xFF00;
    SW_TriTFogR2 = (SW_MulTable[((c >> 16) & 0xFF) + SW_TriTFogF2] << 8) | 0x80;
    SW_TriTFogG2 = (SW_MulTable[((c >> 8) & 0xFF) + SW_TriTFogF2] << 8) | 0x80;
    SW_TriTFogB2 = (SW_MulTable[(c & 0xFF) + SW_TriTFogF2] << 8) | 0x80;
    *(SW_VERTEX **)&v0->sz = v1;
    *(SW_VERTEX **)&v1->sz = v2;
    *(SW_VERTEX **)&v2->sz = v0;
    *(SW_VERTEX **)&v0->specular = v2;
    *(SW_VERTEX **)&v1->specular = v0;
    *(SW_VERTEX **)&v2->specular = v1;
    d01 = (double)SW_TriTFogF0 - SW_TriTFogF1;
    d02 = (double)SW_TriTFogF0 - SW_TriTFogF2;
    SW_TriTFogD01 = (float)d01;
    SW_TriTFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTFogDY02 - d02 * *(volatile float *)&SW_TriTFogDY01) * inv;
    SW_SpanFogGrad = SW_RoundL(g);
    SW_TriTFogFDX = (float)g;
    h = (*(volatile float *)&SW_TriTFogD02 * *(volatile float *)&SW_TriTFogDX01 - *(volatile float *)&SW_TriTFogD01 * *(volatile float *)&SW_TriTFogDX02) * inv;
    SW_TriTFogFDY = (float)h;
    SW_TriTFogFOff = ((double)SW_TriTFogF0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriTFogFDX);
    d01 = (double)SW_TriTFogR0 - SW_TriTFogR1;
    d02 = (double)SW_TriTFogR0 - SW_TriTFogR2;
    SW_TriTFogD01 = (float)d01;
    SW_TriTFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTFogDY02 - d02 * *(volatile float *)&SW_TriTFogDY01) * inv;
    SW_SpanGrad[2] = SW_RoundW(g);
    SW_TriTFogRDX = (float)g;
    h = (*(volatile float *)&SW_TriTFogD02 * *(volatile float *)&SW_TriTFogDX01 - *(volatile float *)&SW_TriTFogD01 * *(volatile float *)&SW_TriTFogDX02) * inv;
    SW_TriTFogRDY = (float)h;
    SW_TriTFogROff = ((double)SW_TriTFogR0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriTFogRDX);
    d01 = (double)SW_TriTFogG0 - SW_TriTFogG1;
    d02 = (double)SW_TriTFogG0 - SW_TriTFogG2;
    SW_TriTFogD01 = (float)d01;
    SW_TriTFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTFogDY02 - d02 * *(volatile float *)&SW_TriTFogDY01) * inv;
    SW_SpanGrad[1] = SW_RoundW(g);
    SW_TriTFogGDX = (float)g;
    h = (*(volatile float *)&SW_TriTFogD02 * *(volatile float *)&SW_TriTFogDX01 - *(volatile float *)&SW_TriTFogD01 * *(volatile float *)&SW_TriTFogDX02) * inv;
    SW_TriTFogGDY = (float)h;
    SW_TriTFogGOff = ((double)SW_TriTFogG0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriTFogGDX);
    d01 = (double)SW_TriTFogB0 - SW_TriTFogB1;
    d02 = (double)SW_TriTFogB0 - SW_TriTFogB2;
    SW_TriTFogD01 = (float)d01;
    SW_TriTFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTFogDY02 - d02 * *(volatile float *)&SW_TriTFogDY01) * inv;
    SW_SpanGrad[0] = SW_RoundW(g);
    SW_TriTFogBDX = (float)g;
    h = (*(volatile float *)&SW_TriTFogD02 * *(volatile float *)&SW_TriTFogDX01 - *(volatile float *)&SW_TriTFogD01 * *(volatile float *)&SW_TriTFogDX02) * inv;
    SW_TriTFogBDY = (float)h;
    SW_TriTFogBOff = ((double)SW_TriTFogB0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriTFogBDX);
    d01 = (double)v0->tu - v1->tu;
    d02 = (double)v0->tu - v2->tu;
    SW_TriTFogD01 = (float)d01;
    SW_TriTFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTFogDY02 - d02 * *(volatile float *)&SW_TriTFogDY01) * inv;
    SW_TriTFogUDX = (float)g;
    h = (*(volatile float *)&SW_TriTFogD02 * *(volatile float *)&SW_TriTFogDX01 - *(volatile float *)&SW_TriTFogD01 * *(volatile float *)&SW_TriTFogDX02) * inv;
    SW_TriTFogUDY = (float)h;
    *(double *)&SW_SpanDU = g + 402653184.0;
    SW_TriTFogUOff = ((double)v0->tu + 402653184.0) - (v0->sx * *(volatile float *)&SW_TriTFogUDX + v0->sy * *(volatile float *)&SW_TriTFogUDY);
    d01 = (double)v0->tv - v1->tv;
    d02 = (double)v0->tv - v2->tv;
    SW_TriTFogD01 = (float)d01;
    SW_TriTFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriTFogDY02 - d02 * *(volatile float *)&SW_TriTFogDY01) * inv;
    SW_TriTFogVDX = (float)g;
    h = (*(volatile float *)&SW_TriTFogD02 * *(volatile float *)&SW_TriTFogDX01 - *(volatile float *)&SW_TriTFogD01 * *(volatile float *)&SW_TriTFogDX02) * inv;
    SW_TriTFogVDY = (float)h;
    *(double *)&SW_SpanDV = g + 402653184.0;
    SW_TriTFogVOff = ((double)v0->tv + 402653184.0) - (v0->sx * *(volatile float *)&SW_TriTFogVDX + v0->sy * *(volatile float *)&SW_TriTFogVDY);
    SW_TriTFogEdges = 3;
    span = SW_SpanBuffer;
    *(double *)SW_TriTFogY = ((double)t->sy - 0.5001F) + 6755399441055744.0;
    SW_TriTFogY[0]++;
    SW_SpanY = SW_TriTFogY[0];
    na = 0;
    nb = 0;
    xa = 0;
    xb = 0;
    for (;;) {
        if (!na) {
            do {
                t = SW_TriTFogEdgeA;
                if (SW_TriTFogEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->sz;
                SW_TriTFogEdges--;
                SW_TriTFogEdgeA = n;
                SW_TriTFogSlopeA = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriTFogXA = (float)(((double)SW_TriTFogY[0] - t->sy) * *(volatile float *)&SW_TriTFogSlopeA + t->sx);
                *(double *)SW_TriTFogEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                na = SW_TriTFogEndY[0] - SW_TriTFogY[0] + 1;
            } while (!na);
            SW_TriTFogStepA = SW_RoundL(*(volatile float *)&SW_TriTFogSlopeA * 65536.0F);
            *(sint32 *)&SW_TriTFogTemp = SW_RoundL(*(volatile float *)&SW_TriTFogXA * 65536.0F);
            xa = *(sint32 *)&SW_TriTFogTemp + 0xFFFF;
        }
        if (!nb) {
            do {
                t = SW_TriTFogEdgeB;
                if (SW_TriTFogEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->specular;
                SW_TriTFogEdges--;
                SW_TriTFogEdgeB = n;
                SW_TriTFogSlopeB = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriTFogXB = (float)(((double)SW_TriTFogY[0] - t->sy) * *(volatile float *)&SW_TriTFogSlopeB + t->sx);
                *(double *)SW_TriTFogEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                nb = SW_TriTFogEndY[0] - SW_TriTFogY[0] + 1;
            } while (!nb);
            SW_TriTFogStepB = SW_RoundL(*(volatile float *)&SW_TriTFogSlopeB * 65536.0F);
            *(sint32 *)&SW_TriTFogTemp = SW_RoundL(*(volatile float *)&SW_TriTFogXB * 65536.0F);
            xb = *(sint32 *)&SW_TriTFogTemp + 0xFFFF;
        }
        do {
            x = xb >> 16;
            *(sint32 *)&SW_TriTFogTemp = x;
            a = (double)SW_TriTFogY[0];
            b = (double)(sint32)x;
            span->n = (xa >> 16) - x;
            span->x = x;
            *(double *)&span->tex[0] = (SW_TriTFogUDX * b + SW_TriTFogUDY * a) + SW_TriTFogUOff;
            *(double *)&span->tex[2] = (SW_TriTFogVDX * b + SW_TriTFogVDY * a) + SW_TriTFogVOff;
            *(float *)&span->tex[1] = (float)((SW_TriTFogFDX * b + SW_TriTFogFDY * a) + SW_TriTFogFOff);
            SW_TriTFogY[0]++;
            f = (float)((SW_TriTFogBDX * b + SW_TriTFogBDY * a) + SW_TriTFogBOff);
            span->c[0] = *(uint16 *)&f;
            SW_TriTFogTemp = (float)((SW_TriTFogGDX * b + SW_TriTFogGDY * a) + SW_TriTFogGOff);
            f = (float)((SW_TriTFogRDX * b + SW_TriTFogRDY * a) + SW_TriTFogROff);
            span->c[1] = *(uint16 *)&SW_TriTFogTemp;
            span->c[2] = ((uint16 *)&f)[0];
            span->c[3] = ((uint16 *)&f)[1];
            span++;
            xb += SW_TriTFogStepB;
            xa += SW_TriTFogStepA;
            nb--;
            na--;
        } while (na && nb);
    }
done:
    SW_SpanEnd = span;
    SW_SpanFunc();
    SW_SpanFogFunc();
    return 0;
}

sint32 SW_TriSetupGFog(void *p0, void *p1, void *p2)
{
    SW_VERTEX *v0, *v1, *v2, *t, *n;
    SW_SPAN *span;
    double a, b, inv, d01, d02, g, h;
    float f;
    uint32 sy, c, xa, xb, x;
    sint32 na, nb;

    v0 = (SW_VERTEX *)p0;
    v1 = (SW_VERTEX *)p1;
    v2 = (SW_VERTEX *)p2;
    SW_TriV0 = v0;
    a = (double)v0->sx - v1->sx;
    b = (double)v0->sy - v2->sy;
    SW_TriFogDX01 = (float)a;
    SW_TriFogDY02 = (float)b;
    g = a * b;
    a = (double)v0->sx - v2->sx;
    b = (double)v0->sy - v1->sy;
    SW_TriFogDX02 = (float)a;
    SW_TriFogDY01 = (float)b;
    g -= a * b;
    SW_TriFogTemp = (float)g;
    inv = 1.0;
    if (*(uint32 *)&SW_TriFogTemp & 0x80000000) {
        t = v1;
        v1 = v2;
        v2 = t;
        inv = -1.0;
        f = SW_TriFogDX01;
        SW_TriFogDX01 = SW_TriFogDX02;
        SW_TriFogDX02 = f;
        f = SW_TriFogDY01;
        SW_TriFogDY01 = SW_TriFogDY02;
        SW_TriFogDY02 = f;
    }
    inv /= g;
    *(SW_VERTEX **)&v0->sz = v1;
    *(SW_VERTEX **)&v1->sz = v2;
    *(SW_VERTEX **)&v2->sz = v0;
    *(SW_VERTEX **)&v0->specular = v2;
    *(SW_VERTEX **)&v1->specular = v0;
    *(SW_VERTEX **)&v2->specular = v1;
    t = v0;
    sy = *(uint32 *)&v0->sy;
    if (sy >= *(uint32 *)&v1->sy) {
        sy = *(uint32 *)&v1->sy;
        t = v1;
    }
    if (sy >= *(uint32 *)&v2->sy)
        t = v2;
    SW_TriFogEdgeA = t;
    SW_TriFogEdgeB = t;
    SW_TriColor0 = v0->color;
    c = v0->color;
    SW_TriFogG0 = (c & 0xFF00) + 0x80;
    SW_TriFogB0 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriFogR0 = ((c >> 8) & 0xFF00) + 0x80;
    SW_TriFogA0 = ((c >> 16) & 0xFF00) + 0x80;
    c = v1->color;
    SW_TriFogG1 = (c & 0xFF00) + 0x80;
    SW_TriFogB1 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriFogR1 = ((c >> 8) & 0xFF00) + 0x80;
    SW_TriFogA1 = (c >> 16) + 0x80;
    c = v2->color;
    SW_TriFogG2 = (c & 0xFF00) + 0x80;
    SW_TriFogB2 = ((c << 8) & 0xFF00) + 0x80;
    SW_TriFogR2 = ((c >> 8) & 0xFF00) + 0x80;
    SW_TriFogA2 = (c >> 16) + 0x80;
    d01 = (double)SW_TriFogR0 - SW_TriFogR1;
    d02 = (double)SW_TriFogR0 - SW_TriFogR2;
    SW_TriFogD01 = (float)d01;
    SW_TriFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriFogDY02 - d02 * *(volatile float *)&SW_TriFogDY01) * inv;
    SW_SpanGrad[2] = SW_RoundW(g);
    SW_TriFogRDX = (float)g;
    h = (SW_TriFogD02 * *(volatile float *)&SW_TriFogDX01 - SW_TriFogD01 * *(volatile float *)&SW_TriFogDX02) * inv;
    SW_TriFogRDY = (float)h;
    SW_TriFogROff = ((double)SW_TriFogR0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriFogRDX);
    d01 = (double)SW_TriFogG0 - SW_TriFogG1;
    d02 = (double)SW_TriFogG0 - SW_TriFogG2;
    SW_TriFogD01 = (float)d01;
    SW_TriFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriFogDY02 - d02 * *(volatile float *)&SW_TriFogDY01) * inv;
    SW_SpanGrad[1] = SW_RoundW(g);
    SW_TriFogGDX = (float)g;
    h = (SW_TriFogD02 * *(volatile float *)&SW_TriFogDX01 - SW_TriFogD01 * *(volatile float *)&SW_TriFogDX02) * inv;
    SW_TriFogGDY = (float)h;
    SW_TriFogGOff = ((double)SW_TriFogG0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriFogGDX);
    d01 = (double)SW_TriFogB0 - SW_TriFogB1;
    d02 = (double)SW_TriFogB0 - SW_TriFogB2;
    SW_TriFogD01 = (float)d01;
    SW_TriFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriFogDY02 - d02 * *(volatile float *)&SW_TriFogDY01) * inv;
    SW_SpanGrad[0] = SW_RoundW(g);
    SW_TriFogBDX = (float)g;
    h = (SW_TriFogD02 * *(volatile float *)&SW_TriFogDX01 - SW_TriFogD01 * *(volatile float *)&SW_TriFogDX02) * inv;
    SW_TriFogBDY = (float)h;
    SW_TriFogBOff = ((double)SW_TriFogB0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriFogBDX);
    d01 = (double)SW_TriFogA0 - SW_TriFogA1;
    d02 = (double)SW_TriFogA0 - SW_TriFogA2;
    SW_TriFogD01 = (float)d01;
    SW_TriFogD02 = (float)d02;
    g = (d01 * *(volatile float *)&SW_TriFogDY02 - d02 * *(volatile float *)&SW_TriFogDY01) * inv;
    SW_SpanGrad[3] = SW_RoundW(g);
    SW_TriFogADX = (float)g;
    h = (SW_TriFogD02 * *(volatile float *)&SW_TriFogDX01 - SW_TriFogD01 * *(volatile float *)&SW_TriFogDX02) * inv;
    SW_TriFogADY = (float)h;
    SW_TriFogAOff = ((double)SW_TriFogA0 + 12582912.0) - (v0->sy * h + v0->sx * *(volatile float *)&SW_TriFogADX);
    SW_TriFogEdges = 3;
    span = SW_SpanBuffer;
    *(double *)SW_TriFogY = ((double)t->sy - 0.5001F) + 6755399441055744.0;
    SW_TriFogY[0]++;
    SW_SpanY = SW_TriFogY[0];
    na = 0;
    nb = 0;
    xa = 0;
    xb = 0;
    for (;;) {
        if (!na) {
            do {
                t = SW_TriFogEdgeA;
                if (SW_TriFogEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->sz;
                SW_TriFogEdges--;
                SW_TriFogEdgeA = n;
                SW_TriFogSlopeA = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriFogXA = (float)(((double)SW_TriFogY[0] - t->sy) * *(volatile float *)&SW_TriFogSlopeA + t->sx);
                *(double *)SW_TriFogEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                na = SW_TriFogEndY[0] - SW_TriFogY[0] + 1;
            } while (!na);
            SW_TriFogStepA = SW_RoundL(*(volatile float *)&SW_TriFogSlopeA * 65536.0F);
            *(sint32 *)&SW_TriFogTemp = SW_RoundL(*(volatile float *)&SW_TriFogXA * 65536.0F);
            xa = *(sint32 *)&SW_TriFogTemp + 0xFFFF;
        }
        if (!nb) {
            do {
                t = SW_TriFogEdgeB;
                if (SW_TriFogEdges - 1 < 0)
                    goto done;
                n = *(SW_VERTEX **)&t->specular;
                SW_TriFogEdges--;
                SW_TriFogEdgeB = n;
                SW_TriFogSlopeB = (float)(((double)t->sx - n->sx) / ((double)t->sy - n->sy));
                SW_TriFogXB = (float)(((double)SW_TriFogY[0] - t->sy) * *(volatile float *)&SW_TriFogSlopeB + t->sx);
                *(double *)SW_TriFogEndY = ((double)n->sy - 0.5001F) + 6755399441055744.0;
                nb = SW_TriFogEndY[0] - SW_TriFogY[0] + 1;
            } while (!nb);
            SW_TriFogStepB = SW_RoundL(*(volatile float *)&SW_TriFogSlopeB * 65536.0F);
            *(sint32 *)&SW_TriFogTemp = SW_RoundL(*(volatile float *)&SW_TriFogXB * 65536.0F);
            xb = *(sint32 *)&SW_TriFogTemp + 0xFFFF;
        }
        do {
            x = xb >> 16;
            *(sint32 *)&SW_TriFogTemp = x;
            a = (double)SW_TriFogY[0];
            b = (double)(sint32)x;
            SW_TriFogTemp = (float)((SW_TriFogADX * b + SW_TriFogADY * a) + SW_TriFogAOff);
            span->n = (xa >> 16) - x;
            span->x = x;
            f = (float)((SW_TriFogBDX * b + SW_TriFogBDY * a) + SW_TriFogBOff);
            span->c[0] = *(uint16 *)&f;
            f = (float)((SW_TriFogGDX * b + SW_TriFogGDY * a) + SW_TriFogGOff);
            span->c[1] = *(uint16 *)&f;
            f = (float)((SW_TriFogRDX * b + SW_TriFogRDY * a) + SW_TriFogROff);
            span->c[2] = *(uint16 *)&f;
            span->c[3] = *(uint16 *)&SW_TriFogTemp;
            span++;
            SW_TriFogY[0]++;
            xb += SW_TriFogStepB;
            xa += SW_TriFogStepA;
            nb--;
            na--;
        } while (na && nb);
    }
done:
    SW_SpanEnd = span;
    SW_SpanFunc();
    return 0;
}

#ifndef FEAT_C_ASM
void SW_SpanG(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        shl ebx, 2
        push edi
        add edi, ebx
        or eax, eax
        jle L4
        movq mm0, qword ptr [esi + 0x18]
        movq mm1, qword ptr [SW_SpanGrad]
        test edi, 7
        je L1
        movq mm7, mm0
        add edi, 4
        psrlw mm7, 8
        paddw mm0, mm1
        packuswb mm7, mm7
        dec eax
        movd dword ptr [edi - 4], mm7
        je L4
    L1:
        cmp eax, 2
        jb L3
        push eax
        movq mm2, mm0
        paddw mm2, mm1
        shr eax, 1
        psllw mm1, 1
    L2:
        add edi, 8
        movq mm6, mm0
        psrlw mm6, 8
        movq mm7, mm2
        psrlw mm7, 8
        packuswb mm6, mm7
        paddw mm2, mm1
        paddw mm0, mm1
        movq qword ptr [edi - 8], mm6
        dec eax
        jne L2
        pop eax
        and eax, 1
        je L4
    L3:
        movq mm2, qword ptr [SW_SpanGrad]
        movq mm1, mm0
        psrlw mm1, 8
        packuswb mm1, mm1
        movd dword ptr [edi], mm1
    L4:
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        add esi, 0x20
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

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

#ifndef FEAT_C_ASM
extern uint32 SW_SpanGFogConst[2];
extern uint32 SW_SpanGFogTmp[2];
void SW_SpanGFog(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        cmp dword ptr [SW_FogColor], 0
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        movq mm4, qword ptr [SW_FogColor]
        pxor mm5, mm5
        punpcklbw mm5, mm4
        psrlw mm5, 1
        movq mm1, qword ptr [SW_SpanFogGrad]
        pand mm1, qword ptr [SW_SpanGFogConst]
        movq mm0, mm1
        movq mm2, mm1
        psllq mm1, 0x10
        psllq mm2, 0x20
        por mm0, mm1
        por mm0, mm2
        pmulhw mm0, mm5
        psllw mm0, 1
        movq qword ptr [SW_SpanGFogTmp], mm0
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        shl ebx, 2
        push edi
        add edi, ebx
        or eax, eax
        jle L4
        movd mm0, dword ptr [esi + 0xc]
        pand mm0, qword ptr [SW_SpanGFogConst]
        pxor mm0, qword ptr [SW_SpanGFogConst]
        movq mm2, mm0
        psllq mm2, 0x10
        por mm0, mm2
        psllq mm2, 0x10
        por mm0, mm2
        movq mm4, qword ptr [SW_FogColor]
        pxor mm5, mm5
        punpcklbw mm5, mm4
        psrlw mm5, 1
        psrlw mm0, 1
        pmulhw mm0, mm5
        psllw mm0, 2
        movq mm1, qword ptr [SW_SpanGFogTmp]
        test edi, 7
        je L1
        movq mm7, mm0
        add edi, 4
        psrlw mm7, 8
        psubw mm0, mm1
        movd mm6, dword ptr [edi - 4]
        packuswb mm7, mm7
        paddusb mm7, mm6
        dec eax
        movd dword ptr [edi - 4], mm7
        je L4
    L1:
        cmp eax, 2
        jb L3
        push eax
        movq mm2, mm0
        psubw mm2, mm1
        shr eax, 1
        psllw mm1, 1
    L2:
        add edi, 8
        movq mm6, mm0
        psrlw mm6, 8
        movq mm7, mm2
        movq mm5, qword ptr [edi - 8]
        psrlw mm7, 8
        packuswb mm6, mm7
        psubw mm2, mm1
        paddusb mm6, mm5
        psubw mm0, mm1
        movq qword ptr [edi - 8], mm6
        dec eax
        jne L2
        pop eax
        and eax, 1
        je L4
    L3:
        movq mm4, mm0
        psrlw mm4, 8
        movd mm6, dword ptr [edi]
        packuswb mm4, mm4
        paddusb mm4, mm6
        movd dword ptr [edi], mm4
    L4:
        add esi, 0x20
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
        jmp Done
    Done:
    }
}
#else
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
#endif

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

#ifndef FEAT_C_ASM
void SW_SpanGAdd(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        shl ebx, 2
        push edi
        add edi, ebx
        or eax, eax
        jle L4
        movq mm0, qword ptr [esi + 0x18]
        movq mm1, qword ptr [SW_SpanGrad]
        test edi, 7
        je L1
        movq mm7, mm0
        add edi, 4
        psrlw mm7, 8
        paddw mm0, mm1
        movd mm6, dword ptr [edi - 4]
        packuswb mm7, mm7
        paddusb mm7, mm6
        dec eax
        movd dword ptr [edi - 4], mm7
        je L4
    L1:
        cmp eax, 2
        jb L3
        push eax
        movq mm2, mm0
        paddw mm2, mm1
        shr eax, 1
        psllw mm1, 1
    L2:
        add edi, 8
        movq mm6, mm0
        psrlw mm6, 8
        movq mm7, mm2
        movq mm5, qword ptr [edi - 8]
        psrlw mm7, 8
        packuswb mm6, mm7
        paddw mm2, mm1
        paddusb mm6, mm5
        paddw mm0, mm1
        movq qword ptr [edi - 8], mm6
        dec eax
        jne L2
        pop eax
        and eax, 1
        je L4
    L3:
        movq mm2, qword ptr [SW_SpanGrad]
        movq mm1, mm0
        psrlw mm1, 8
        movd mm6, dword ptr [edi]
        packuswb mm1, mm1
        paddusb mm1, mm6
        movd dword ptr [edi], mm1
    L4:
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        add esi, 0x20
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

#ifndef FEAT_C_ASM
void SW_SpanGAlpha(void)
{
    uint32 amask[2];
    uint32 cmask[2];

    amask[0] = 0;
    amask[1] = 0xFF000000;
    cmask[0] = 0xFF00FF;
    cmask[1] = 0xFF00FF;
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        pxor mm4, mm4
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        shl ebx, 2
        push edi
        add edi, ebx
        or eax, eax
        jle L2
        movq mm0, qword ptr [esi + 0x18]
        movq mm1, qword ptr [SW_SpanGrad]
    L1:
        movd mm6, dword ptr [edi]
        movq mm5, mm0
        movq mm3, mm0
        pand mm5, qword ptr [amask]
        movq mm7, mm5
        psrlq mm5, 0x28
        psrlq mm7, 0x38
        punpcklbw mm6, mm4
        por mm7, mm5
        psllq mm5, 0x10
        psrlw mm3, 8
        por mm7, mm5
        pmullw mm3, mm7
        pxor mm7, qword ptr [cmask]
        pmullw mm6, mm7
        paddw mm0, mm1
        add edi, 4
        paddw mm6, mm3
        psrlw mm6, 8
        packuswb mm6, mm6
        dec eax
        movd dword ptr [edi - 4], mm6
        jne L1
    L2:
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        add esi, 0x20
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

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

#ifndef FEAT_C_ASM
extern uint32 SW_SpanGTMask;
void SW_SpanGT(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        mov ecx, dword ptr [SW_TexShift]
        mov eax, 0xff0000
        shl eax, cl
        and eax, 0xff0000
        mov dword ptr [SW_SpanGTMask], eax
        mov dword ptr [P3 + 2], eax
        mov dword ptr [P6 + 2], eax
        mov al, cl
        add al, 8
        add al, cl
        mov byte ptr [P0 + 2], al
        mov byte ptr [P5 + 2], al
        mov al, cl
        add al, 0x10
        mov byte ptr [P1 + 2], al
        mov byte ptr [P7 + 2], al
        mov ebx, dword ptr [SW_SpanDU]
        mov eax, dword ptr [SW_SpanDV]
        mov edx, dword ptr [SW_TextureBits]
        add edx, dword ptr [ecx*4 + SW_MipOffsets]
        mov dword ptr [P8 + 2], ebx
        mov dword ptr [P4 + 2], eax
        mov dword ptr [P2 + 4], edx
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x8d
        _emit 0x64
        _emit 0x24
        _emit 0x00
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        push edi
        lea edi, [edi + ebx*4]
        or eax, eax
        jle L1
        mov ebx, dword ptr [esi + 8]
        mov ecx, dword ptr [esi + 0x10]
        mov edx, ecx
        mov ebp, ebx
        and edx, dword ptr [SW_SpanGTMask]
        and ebp, dword ptr [SW_SpanGTMask]
    P0:
        shr edx, 8
        pxor mm5, mm5
    P1:
        shr ebp, 0x10
        movq mm1, qword ptr [SW_SpanGrad]
        or edx, ebp
        movq mm0, qword ptr [esi + 0x18]
        movq mm6, mm0
        add ecx, dword ptr [SW_SpanDV]
        add ebx, dword ptr [SW_SpanDU]
        _emit 0x8d
        _emit 0x64
        _emit 0x24
        _emit 0x00
    P2:
        movd mm7, dword ptr [edx*4 + 0x12345678]
        psrlw mm6, 8
        mov edx, ecx
        punpcklbw mm7, mm5
        pmullw mm7, mm6
        mov ebp, ebx
    P3:
        and edx, 0xff0000
    P4:
        add ecx, 0x12345678
    P5:
        shr edx, 8
    P6:
        and ebp, 0xff0000
    P7:
        shr ebp, 0x10
        psrlw mm7, 8
        packuswb mm7, mm7
        add edi, 4
    P8:
        add ebx, 0x12345678
        or edx, ebp
        movd dword ptr [edi - 4], mm7
        paddw mm0, mm1
        movq mm6, mm0
        dec eax
        jne P2
    L1:
        pop edi
        add esi, 0x20
        add edi, dword ptr [SW_SurfacePitch]
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

#ifndef FEAT_C_ASM
extern uint32 SW_SpanGTAddMask;
void SW_SpanGTAdd(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        mov ecx, dword ptr [SW_TexShift]
        mov eax, 0xff0000
        shl eax, cl
        and eax, 0xff0000
        mov dword ptr [SW_SpanGTAddMask], eax
        mov dword ptr [P4 + 2], eax
        mov dword ptr [P5 + 2], eax
        mov al, cl
        add al, 8
        add al, cl
        mov byte ptr [P7 + 2], al
        mov byte ptr [P0 + 2], al
        mov al, cl
        add al, 0x10
        mov byte ptr [P8 + 2], al
        mov byte ptr [P1 + 2], al
        mov ebx, dword ptr [SW_SpanDU]
        mov eax, dword ptr [SW_SpanDV]
        mov edx, dword ptr [SW_TextureBits]
        add edx, dword ptr [ecx*4 + SW_MipOffsets]
        mov dword ptr [P2 + 2], ebx
        mov dword ptr [P6 + 2], eax
        mov dword ptr [P3 + 4], edx
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x8d
        _emit 0x64
        _emit 0x24
        _emit 0x00
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        shl ebx, 2
        push edi
        add edi, ebx
        or eax, eax
        jle L2
        movq mm0, qword ptr [esi + 0x18]
        movq mm1, qword ptr [SW_SpanGrad]
        push ebx
        push ecx
        push esi
        mov ebx, dword ptr [esi + 8]
        mov ecx, dword ptr [esi + 0x10]
        mov edx, ecx
        mov ebp, ebx
        and edx, dword ptr [SW_SpanGTAddMask]
        and ebp, dword ptr [SW_SpanGTAddMask]
        pxor mm5, mm5
    P0:
        shr edx, 8
    P1:
        shr ebp, 0x10
        add ecx, dword ptr [SW_SpanDV]
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x8d
        _emit 0x49
        _emit 0x00
    L1:
        movq mm6, mm0
        or edx, ebp
        psrlw mm6, 8
    P2:
        add ebx, 0x12345678
    P3:
        movd mm7, dword ptr [edx*4 + 0x12345678]
        punpcklbw mm7, mm5
        mov ebp, ebx
        pmullw mm7, mm6
        mov edx, ecx
        movd mm4, dword ptr [edi]
    P4:
        and edx, 0xff0000
    P5:
        and ebp, 0xff0000
        psrlw mm7, 8
    P6:
        add ecx, 0x12345678
    P7:
        shr edx, 8
        packuswb mm7, mm7
    P8:
        shr ebp, 0x10
        paddusb mm7, mm4
        paddw mm0, mm1
        dec eax
        movd dword ptr [edi], mm7
        lea edi, [edi + 4]
        jne L1
        pop esi
        pop ebx
        pop ecx
    L2:
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        add esi, 0x20
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

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

#ifndef FEAT_C_ASM
extern uint32 SW_SpanGTPMask;
void SW_SpanGTP(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        mov ecx, dword ptr [SW_TexShift]
        mov eax, 0xff0000
        shl eax, cl
        and eax, 0xff0000
        mov dword ptr [SW_SpanGTPMask], eax
        mov dword ptr [P3 + 2], eax
        mov dword ptr [P5 + 2], eax
        mov al, cl
        add al, 8
        add al, cl
        mov byte ptr [P0 + 2], al
        mov byte ptr [P4 + 2], al
        mov al, cl
        add al, 0x10
        mov byte ptr [P1 + 2], al
        mov byte ptr [P6 + 2], al
        mov edx, dword ptr [SW_TextureBits]
        add edx, dword ptr [ecx*4 + SW_MipOffsets]
        mov dword ptr [P2 + 4], edx
        nop
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        push edi
        lea edi, [edi + ebx*4]
        add esi, 0x20
        or eax, eax
        jle L2
        mov ebx, dword ptr [esi - 0x18]
        mov ecx, dword ptr [esi - 0x10]
        mov edx, ecx
        mov ebp, ebx
        and edx, dword ptr [SW_SpanGTPMask]
        and ebp, dword ptr [SW_SpanGTPMask]
    P0:
        shr edx, 8
        pxor mm5, mm5
    P1:
        shr ebp, 0x10
        movq mm1, qword ptr [SW_SpanGrad]
        or edx, ebp
        movq mm0, qword ptr [esi - 8]
        movq mm6, mm0
        add ebx, dword ptr [esi]
        add ecx, dword ptr [esi + 4]
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
        nop
    L1:
        push eax
        cmp eax, 0x10
        jle P2
        mov eax, 0x10
        nop
        _emit 0x8d
        _emit 0x64
        _emit 0x24
        _emit 0x00
    P2:
        movd mm7, dword ptr [edx*4 + 0x12345678]
        psrlw mm6, 8
        mov edx, ecx
        punpcklbw mm7, mm5
        pmullw mm7, mm6
        mov ebp, ebx
    P3:
        and edx, 0xff0000
        add ecx, dword ptr [esi + 4]
    P4:
        shr edx, 8
    P5:
        and ebp, 0xff0000
    P6:
        shr ebp, 0x10
        psrlw mm7, 8
        packuswb mm7, mm7
        add edi, 4
        or edx, ebp
        add ebx, dword ptr [esi]
        movd dword ptr [edi - 4], mm7
        paddw mm0, mm1
        movq mm6, mm0
        dec eax
        jne P2
        pop eax
        add esi, 8
        sub eax, 0x10
        jg L1
        sub esi, 8
    L2:
        add esi, 8
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

#ifndef FEAT_C_ASM
extern uint32 SW_SpanGTPKeyConst[2];
extern uint32 SW_SpanGTPKeyMask;
void SW_SpanGTPKey(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        mov ecx, dword ptr [SW_TexShift]
        mov eax, 0xff0000
        shl eax, cl
        and eax, 0xff0000
        mov dword ptr [SW_SpanGTPKeyMask], eax
        mov dword ptr [P3 + 2], eax
        mov dword ptr [P5 + 2], eax
        mov al, cl
        add al, 8
        add al, cl
        mov byte ptr [P0 + 2], al
        mov byte ptr [P4 + 2], al
        mov al, cl
        add al, 0x10
        mov byte ptr [P1 + 2], al
        mov byte ptr [P6 + 2], al
        mov edx, dword ptr [SW_TextureBits]
        add edx, dword ptr [ecx*4 + SW_MipOffsets]
        mov dword ptr [P2 + 4], edx
        nop
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        push edi
        lea edi, [edi + ebx*4]
        add esi, 0x20
        or eax, eax
        jle L2
        mov ebx, dword ptr [esi - 0x18]
        mov ecx, dword ptr [esi - 0x10]
        mov edx, ecx
        mov ebp, ebx
        and edx, dword ptr [SW_SpanGTPKeyMask]
        and ebp, dword ptr [SW_SpanGTPKeyMask]
    P0:
        shr edx, 8
        pxor mm5, mm5
    P1:
        shr ebp, 0x10
        movq mm1, qword ptr [SW_SpanGrad]
        or edx, ebp
        movq mm0, qword ptr [esi - 8]
        movq mm6, mm0
        add ebx, dword ptr [esi]
        add ecx, dword ptr [esi + 4]
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
        nop
    L1:
        push eax
        cmp eax, 0x10
        jle P2
        mov eax, 0x10
        nop
        _emit 0x8d
        _emit 0x64
        _emit 0x24
        _emit 0x00
    P2:
        movd mm7, dword ptr [edx*4 + 0x12345678]
        psrlw mm6, 8
        mov edx, ecx
        mov ebp, ebx
        movq mm4, mm7
        punpcklbw mm7, mm5
        pmullw mm7, mm6
    P3:
        and edx, 0xff0000
        movq mm6, qword ptr [edi]
        pcmpeqd mm4, mm5
        pand mm6, mm4
        pxor mm4, qword ptr [SW_SpanGTPKeyConst]
    P4:
        shr edx, 8
    P5:
        and ebp, 0xff0000
    P6:
        shr ebp, 0x10
        add ecx, dword ptr [esi + 4]
        psrlw mm7, 8
        add edi, 4
        packuswb mm7, mm7
        or edx, ebp
        pand mm7, mm4
        paddw mm0, mm1
        por mm7, mm6
        add ebx, dword ptr [esi]
        movd dword ptr [edi - 4], mm7
        movq mm6, mm0
        dec eax
        jne P2
        pop eax
        add esi, 8
        sub eax, 0x10
        jg L1
        sub esi, 8
    L2:
        add esi, 8
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

#ifndef FEAT_C_ASM
extern uint32 SW_SpanGTPAddMask;
void SW_SpanGTPAdd(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        mov ecx, dword ptr [SW_TexShift]
        mov eax, 0xff0000
        shl eax, cl
        and eax, 0xff0000
        mov dword ptr [SW_SpanGTPAddMask], eax
        mov dword ptr [P3 + 2], eax
        mov dword ptr [P5 + 2], eax
        mov al, cl
        add al, 8
        add al, cl
        mov byte ptr [P0 + 2], al
        mov byte ptr [P4 + 2], al
        mov al, cl
        add al, 0x10
        mov byte ptr [P1 + 2], al
        mov byte ptr [P6 + 2], al
        mov edx, dword ptr [SW_TextureBits]
        add edx, dword ptr [ecx*4 + SW_MipOffsets]
        mov dword ptr [P2 + 4], edx
        pxor mm5, mm5
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        push edi
        lea edi, [edi + ebx*4]
        add esi, 0x20
        or eax, eax
        jle L2
        mov ebx, dword ptr [esi - 0x18]
        mov ecx, dword ptr [esi - 0x10]
        mov edx, ecx
        mov ebp, ebx
        and edx, dword ptr [SW_SpanGTPAddMask]
        and ebp, dword ptr [SW_SpanGTPAddMask]
    P0:
        shr edx, 8
        pxor mm5, mm5
    P1:
        shr ebp, 0x10
        movq mm1, qword ptr [SW_SpanGrad]
        or edx, ebp
        movq mm0, qword ptr [esi - 8]
        movq mm6, mm0
        add ebx, dword ptr [esi]
        add ecx, dword ptr [esi + 4]
        _emit 0x8d
        _emit 0xa4
        _emit 0x24
        _emit 0x00
        _emit 0x00
        _emit 0x00
        _emit 0x00
        nop
    L1:
        push eax
        cmp eax, 0x10
        jle P2
        mov eax, 0x10
        nop
        _emit 0x8d
        _emit 0x64
        _emit 0x24
        _emit 0x00
    P2:
        movd mm7, dword ptr [edx*4 + 0x12345678]
        psrlw mm6, 8
        movd mm4, dword ptr [edi]
        punpcklbw mm7, mm5
        pmullw mm7, mm6
        mov edx, ecx
        mov ebp, ebx
    P3:
        and edx, 0xff0000
    P4:
        shr edx, 8
        add ecx, dword ptr [esi + 4]
    P5:
        and ebp, 0xff0000
        psrlw mm7, 8
        add edi, 4
    P6:
        shr ebp, 0x10
        packuswb mm7, mm7
        paddusb mm7, mm4
        movq mm6, mm0
        or edx, ebp
        add ebx, dword ptr [esi]
        movd dword ptr [edi - 4], mm7
        paddw mm0, mm1
        dec eax
        jne P2
        pop eax
        add esi, 8
        sub eax, 0x10
        jg L1
        sub esi, 8
    L2:
        add esi, 8
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        emms
        pop ebp
    Done:
    }
}
#else
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
#endif

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

#ifndef FEAT_C_ASM
void SW_SpanGInt(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        mov ax, word ptr [SW_SpanGrad + 4]
        shl eax, 0x10
        mov ax, word ptr [SW_SpanGrad]
        mov dword ptr [P0 + 2], eax
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        shl ebx, 2
        push edi
        add edi, ebx
        or eax, eax
        jle L2
        mov bx, word ptr [esi + 0x1c]
        shl ebx, 0x10
        mov bx, word ptr [esi + 0x18]
        mov ecx, dword ptr [esi + 0x1a]
        mov edx, dword ptr [SW_SpanGrad + 2]
        push esi
    L1:
        mov ebp, ebx
        mov esi, ecx
        shr ebp, 8
        add ecx, edx
    P0:
        add ebx, 0x12345678
        and esi, 0xff00
        and ebp, 0xff00ff
        add edi, 4
        or ebp, esi
        dec eax
        mov dword ptr [edi - 4], ebp
        jne L1
        pop esi
    L2:
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        add esi, 0x20
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        pop ebp
    Done:
    }
}
#else
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
#endif

#ifndef FEAT_C_ASM
void SW_SpanGAddInt(void)
{
    __asm {
        mov esi, dword ptr [SW_SpanBuffer]
        cmp esi, dword ptr [SW_SpanEnd]
        je Done
        push ebp
        mov edi, dword ptr [SW_Surface]
        mov eax, dword ptr [SW_SpanY]
        imul eax, dword ptr [SW_SurfacePitch]
        add edi, eax
        mov ax, word ptr [SW_SpanGrad + 4]
        shl eax, 0x10
        mov ax, word ptr [SW_SpanGrad]
        mov dword ptr [P0 + 2], eax
    L0:
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        shl ebx, 2
        push edi
        add edi, ebx
        or eax, eax
        jle L2
        mov bx, word ptr [esi + 0x1c]
        shl ebx, 0x10
        mov bx, word ptr [esi + 0x18]
        mov ecx, dword ptr [esi + 0x1a]
        mov edx, dword ptr [SW_SpanGrad + 2]
        push esi
    L1:
        mov ebp, ebx
        mov esi, ecx
        shr ebp, 8
        add ecx, edx
    P0:
        add ebx, 0x12345678
        and esi, 0xfe00
        and ebp, 0xfe00ff
        add edi, 4
        push edx
        or ebp, esi
        push ecx
        mov edx, dword ptr [edi - 4]
        and edx, 0xfefeff
        add ebp, edx
        mov edx, ebp
        and edx, 0x1010100
        mov ecx, edx
        shr edx, 8
        sub ecx, edx
        or ebp, ecx
        pop ecx
        pop edx
        dec eax
        mov dword ptr [edi - 4], ebp
        jne L1
        pop esi
    L2:
        pop edi
        add edi, dword ptr [SW_SurfacePitch]
        add esi, 0x20
        cmp dword ptr [SW_SpanEnd], esi
        jne L0
        pop ebp
    Done:
    }
}
#else
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
#endif

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

void SW_SpanGTLitPersp(void)
{
    SW_SPAN *s;
    uint32 *row, *p;
    uint32 u, v, texel, d, du, dv;
    uint32 c0, c1, c2, dc0, dc1, dc2;
    sint32 n, m, a, ia;

    row = (uint32 *)SW_Surface + (SW_SpanY * SW_SurfacePitch >> 2);
    a = SW_TriV0->color >> 24;
    s = SW_SpanBuffer;
    dc2 = (uint16)SW_SpanGrad[2];
    dc1 = (uint16)SW_SpanGrad[1];
    dc0 = (uint16)SW_SpanGrad[0];
    ia = 255 - a;
    a <<= 8;
    if (s != SW_SpanEnd) {
        do {
            c2 = s->c[2];
            c1 = s->c[1];
            n = s->n;
            p = row + s->x;
            c0 = s->c[0];
            u = s->tex[0];
            v = s->tex[2];
            if (n > 0) {
                do {
                    du = ((sint32 *)(s + 1))[0];
                    dv = ((sint32 *)(s + 1))[1];
                    for (m = n > 16 ? 16 : n; m > 0; m--) {
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
                    }
                    n -= 16;
                    s = (SW_SPAN *)((sint32 *)s + 2);
                } while (n > 0);
            } else {
                s = (SW_SPAN *)((sint32 *)s + 2);
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

#ifndef FEAT_C_ASM
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
#else
void SW_CopyBuffer(void *dst, void *src, sint32 width)
{
    if (!SW_UseMMX)
        memcpy(dst, src, width * 4);
    else
        memcpy(dst, src, ((uint32)width >> 1) * 8);
}
#endif

#ifndef FEAT_C_ASM
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
#else
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
#endif

#ifndef FEAT_C_ASM
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
#else
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
#endif

#ifndef FEAT_C_ASM
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
#else
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
#endif

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

void Log(unsigned int type, const char *fmt, ...)
{
    HWND hwnd;
    char *dst;
    char *src;
    sint32 i;
    sint32 count;
    va_list args;
    char buf[1024];

    hwnd = FindWindowA("DBLogWindowClass", "DBLog Server");
    if (!hwnd)
        return;
    DBLog_Mapping = DBLog_OpenMapping("DBLOGMAPMEM");
    dst = DBLog_MapView(DBLog_Mapping);
    if (!dst)
        return;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    dst += DBLog_Offset;
    *dst++ = (char)type;
    src = buf;
    count = 1;
    for (i = strlen(buf) + 1; i > 0; i--) {
        if (*src == '\n' || !*src) {
            if (count > 1) {
                *dst++ = 0;
                DBLog_Post(hwnd, DBLog_Offset);
                DBLog_Offset += count + 1;
                *dst++ = (char)type;
            }
            count = 0;
        } else
            *dst++ = *src;
        count++;
        src++;
    }
    if (DBLog_Offset > 0xFFC00) {
        DBLog_Offset = 0;
        SendMessageA(hwnd, DBLog_ResetMessage, 0, 0);
    }
    CloseHandle(DBLog_Mapping);
}
