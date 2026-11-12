#include "winmain.h"
#include "output.h"
#include "dxshell.h"
#include "function_table.h"
#include "time.h"
#include "../text.h"
#include "d3dmatrix.h"
#include "drawroom.h"
#include "polyinsert.h"
#include "../effects.h"
#include "../gameflow.h"

void S_InitialisePolyList(void)
{
    D3DRECT rect;
    sint32 col;

    rect.x1 = App.dx.rViewport.left;
    rect.y1 = App.dx.rViewport.top;
    rect.x2 = App.dx.rViewport.left + App.dx.rViewport.right;
    rect.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
    if (gfLevelFlags & GF_TRAIN)
        col = 0xCEAE60;
    else if (gfCurrentLevel == 5 || gfCurrentLevel == 6) {
        col = FogTableColor[19];
        SetFogColor(CLRR(FogTableColor[19]), CLRG(FogTableColor[19]), CLRB(FogTableColor[19]));
    } else
        col = 0;
    if (App.dx.Flags & 0x80)
        DXAttempt(IDirect3DViewport3_Clear2(App.dx.lpViewport, 1, &rect, D3DCLEAR_TARGET, col, 1.0F, 0));
    else
        SW_ClearBuffer(App.dx.lpD3DDevice, 1, &rect, D3DCLEAR_TARGET, col, 1.0F, 0);
    _BeginScene();
    InitBuckets();
    InitialiseSortList();
}

void _InsertRoom(ROOM_INFO *r)
{
    SetD3DViewMatrix();
    InsertRoom(r);
}

sint32 S_DumpScreen(void)
{
    sint32 n;

    n = Sync();
    while (n < 2) {
        while (!Sync())
            ;
        n++;
    }
    GnFrameCounter++;
    _EndScene();
    DXShowFrame();
    App.dx.DoneBlit = 1;
    return n;
}

HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE4 surf, HBITMAP hbm, sint32 x, sint32 y, sint32 dx, sint32 dy)
{
    HDC hdc;
    HDC hdc2;
    BITMAP bitmap;
    DDSURFACEDESC2 desc;
    HRESULT result;
    sint32 l, t;

    if (!hbm || !surf)
        return E_FAIL;
    IDirectDrawSurface4_Restore(surf);
    hdc = CreateCompatibleDC(0);
    if (!hdc)
        OutputDebugString("createcompatible dc failed\n");
    SelectObject(hdc, hbm);
    GetObject(hbm, sizeof(BITMAP), &bitmap);
    if (!dx)
        dx = bitmap.bmWidth;
    if (!dy)
        dy = bitmap.bmHeight;
    desc.dwSize = sizeof(DDSURFACEDESC2);
    desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT;
    IDirectDrawSurface4_GetSurfaceDesc(surf, &desc);
    l = 0;
    t = 0;
    if (!(App.dx.Flags & 0x80)) {
        surf = App.dx.lpPrimaryBuffer;
        if (App.dx.Flags & 2) {
            l = App.dx.rScreen.left;
            t = App.dx.rScreen.top;
        }
    }
    result = IDirectDrawSurface4_GetDC(surf, &hdc2);
    if (!result) {
        StretchBlt(hdc2, l, t, desc.dwWidth, desc.dwHeight, hdc, x, y, dx, dy, SRCCOPY);
        IDirectDrawSurface4_ReleaseDC(surf, hdc2);
    }
    DeleteDC(hdc);
    return result;
}

HRESULT _LoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name)
{
    HBITMAP hBitmap;
    HRESULT result;

    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(0), name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (!hBitmap)
        hBitmap = (HBITMAP)LoadImage(0, name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    if (!hBitmap) {
        OutputDebugString("handle is null\n");
        return E_FAIL;
    }
    result = DDCopyBitmap(surf, hBitmap, 0, 0, 0, 0);
    if (result != DD_OK)
        OutputDebugString("ddcopybitmap failed\n");
    DeleteObject(hBitmap);
    return result;
}
