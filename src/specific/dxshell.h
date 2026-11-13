#ifndef SPECIFIC_DXSHELL_H
#define SPECIFIC_DXSHELL_H

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "../types.h"

void DI_ReadKeyboard(char *KeyMap);
char *DXGetErrorString(HRESULT hr);
sint32 DXAttempt(HRESULT r);
void DXBitMask2ShiftCnt(uint32 mask, uint8 *shift, uint8 *count);
HRESULT DXShowFrame(void);
sint32 BPPToDDBD(sint32 BPP);
void *AddStruct(void *p, sint32 num, sint32 size);
void DXMove(sint32 x, sint32 y);
sint32 DXCreateSurface(LPDIRECTDRAW4 dd, LPDDSURFACEDESC2 desc, LPDIRECTDRAWSURFACE4 *surf);
sint32 DXSetVideoMode(LPDIRECTDRAW4 dd, sint32 dwWidth, sint32 dwHeight, sint32 dwBPP);
sint32 DXD3DCreate(LPDIRECTDRAW4 pDD4, void **pD3D);
sint32 DXCreateD3DDevice(LPDIRECT3D3 d3d, GUID guid, LPDIRECTDRAWSURFACE4 surf, LPDIRECT3DDEVICE3 *device);
sint32 DXDDCreate(LPGUID pGuid, void **pDD4);
sint32 DXCreateViewPort(LPDIRECT3D3 d3d, LPDIRECT3DDEVICE3 device, sint32 w, sint32 h, LPDIRECT3DVIEWPORT3 *viewport);
void DXInitKeyboard(HWND hwnd, HINSTANCE hinstance);
sint32 DXSetCooperativeLevel(LPDIRECTDRAW4 dd, HWND hwnd, sint32 flags);
BOOL CALLBACK DXEnumDirectDraw(GUID *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);
void DXClose(void);
HRESULT SW_D3D_CreateDevice(void);

extern uint8 keymap[256];
extern uint8 keymap2[256];
extern sint32 DXUnk754124[16];
extern sint32 DXUnk7541ac;
extern uint8 TgaHeader[18];
extern LPDIRECTDRAW4 G_ddraw;
extern LPDIRECT3D3 G_d3d;

#endif
