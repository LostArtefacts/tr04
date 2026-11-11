#ifndef SPECIFIC_WINMAIN_H
#define SPECIFIC_WINMAIN_H

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>

#include "../types.h"

typedef struct DXPTR {
    LPDIRECTDRAW4 lpDD;
    LPDIRECT3D3 lpD3D;
    LPDIRECT3DDEVICE3 lpD3DDevice;
    LPDIRECT3DDEVICE3 _lpD3DDevice;
    LPDIRECTDRAWSURFACE4 lpPrimaryBuffer;
    LPDIRECTDRAWSURFACE4 lpBackBuffer;
    LPDIRECTDRAWSURFACE4 lpZBuffer;
    LPDIRECT3DVIEWPORT3 lpViewport;
    LPDIRECTSOUND lpDS;
    uint32 dwRenderWidth;
    uint32 dwRenderHeight;
    RECT rViewport;
    RECT rScreen;
    sint32 Flags;
    sint32 WindowStyle;
    sint32 CoopLevel;
    LPDIRECTINPUT2 lpDirectInput;
    LPDIRECTINPUTDEVICE2 Keyboard;
    HWND hWnd;
    volatile sint32 InScene;
    volatile sint32 WaitAtBeginScene;
    volatile sint32 DoneBlit;
} DXPTR;

typedef struct DXDISPLAYMODE {
    sint32 w;
    sint32 h;
    sint32 bpp;
    sint32 RefreshRate;
    sint32 bPalette;
    DDSURFACEDESC2 ddsd;
    uint8 rbpp;
    uint8 gbpp;
    uint8 bbpp;
    uint8 rshift;
    uint8 gshift;
    uint8 bshift;
} DXDISPLAYMODE;

typedef struct DXTEXTUREINFO {
    DDPIXELFORMAT ddpf;
    uint32 bpp;
    sint32 bPalette;
    sint32 bAlpha;
    uint8 rbpp;
    uint8 gbpp;
    uint8 bbpp;
    uint8 abpp;
    uint8 rshift;
    uint8 gshift;
    uint8 bshift;
    uint8 ashift;
} DXTEXTUREINFO;

typedef struct DXZBUFFERINFO {
    DDPIXELFORMAT ddpf;
    uint32 bpp;
} DXZBUFFERINFO;

typedef struct DXD3DDEVICE {
    char Name[30];
    char About[80];
    LPGUID lpGuid;
    GUID Guid;
    D3DDEVICEDESC DeviceDesc;
    sint32 bHardware;
    sint32 nDisplayModes;
    DXDISPLAYMODE *DisplayModes;
    sint32 nTextureInfos;
    DXTEXTUREINFO *TextureInfos;
    sint32 nZBufferInfos;
    DXZBUFFERINFO *ZBufferInfos;
} DXD3DDEVICE;

typedef struct DXDIRECTDRAWINFO {
    char Name[30];
    char About[80];
    LPGUID lpGuid;
    GUID Guid;
    DDCAPS DDCaps;
    DDDEVICEIDENTIFIER DDIdentifier;
    sint32 nDisplayModes;
    DXDISPLAYMODE *DisplayModes;
    sint32 nD3DDevices;
    DXD3DDEVICE *D3DDevices;
} DXDIRECTDRAWINFO;

typedef struct DXDIRECTSOUNDINFO {
    char Name[30];
    char About[80];
    LPGUID lpGuid;
    GUID Guid;
} DXDIRECTSOUNDINFO;

typedef struct DXINFO {
    sint32 nDDInfo;
    sint32 nDSInfo;
    DXDIRECTDRAWINFO *DDInfo;
    DXDIRECTSOUNDINFO *DSInfo;
    sint32 nDD;
    sint32 nD3D;
    sint32 nDisplayMode;
    sint32 nTexture;
    sint32 nZBuffer;
    sint32 nDS;
    uint8 bHardware;
} DXINFO;

typedef struct WINAPP {
    HINSTANCE hInstance;
    HWND hWnd;
    WNDCLASS WindowClass;
    DXINFO DXInfo;
    DXPTR dx;
    HANDLE mutex;
    float fps;
    LPDIRECT3DMATERIAL3 GlobalMaterial;
    D3DMATERIALHANDLE GlobalMaterialHandle;
    HACCEL hAccel;
    uint8 SetupComplete;
    uint8 BumpMapping;
    sint32 TextureSize;
    sint32 BumpMapSize;
    uint8 mmx;
    uint8 Filtering;
    uint8 Volumetric;
    uint8 SoundDisabled;
    sint32 StartFlags;
    volatile uint8 fmv;
    sint32 Desktopbpp;
    sint32 AutoTarget;
} WINAPP;

extern WINAPP App;

#endif
