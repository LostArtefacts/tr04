#include "winmain.h"
#include "output.h"
#include "dxshell.h"
#include "function_table.h"
#include "time.h"
#include "../text.h"
#include "d3dmatrix.h"
#include "3dmath.h"
#include "drawroom.h"
#include "polyinsert.h"
#include "../effects.h"
#include "../gameflow.h"
#include "file.h"

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

void S_AnimateTextures(sint32 n)
{
    TEXTURESTRUCT *tex;
    TEXTURESTRUCT tex2;
    sint16 *range;
    float voff;
    static sint32 comp;
    sint32 i;
    sint32 nRanges;
    sint32 nRangeFrames;

    for (comp += n; comp > 5; comp -= 5) {
        nRanges = *aranges;
        range = aranges + 1;
        for (i = 0; i < nRanges; i++, range++) {
            nRangeFrames = *range++;
            if (i < nAnimUVRanges && gfUVRotate) {
                while (nRangeFrames > 0) {
                    range++;
                    nRangeFrames--;
                }
            } else {
                tex2 = textinfo[*range];
                while (nRangeFrames > 0) {
                    textinfo[range[0]] = textinfo[range[1]];
                    range++;
                    nRangeFrames--;
                }
                textinfo[*range] = tex2;
            }
        }
    }
    if (gfUVRotate) {
        range = aranges + 1;
        AnimatingTexturesVOffset = (AnimatingTexturesVOffset - gfUVRotate * (n >> 1)) & 0x1F;
        for (i = 0; i < nAnimUVRanges; i++) {
            nRangeFrames = *range++;
            while (nRangeFrames >= 0) {
                tex = &textinfo[range[0]];
                voff = AnimatingTexturesVOffset * (1.0F / 256.0F);
                tex->v1 = voff + AnimatingTexturesV[i][nRangeFrames][0];
                tex->v2 = voff + AnimatingTexturesV[i][nRangeFrames][0];
                tex->v3 = voff + AnimatingTexturesV[i][nRangeFrames][0] + 0.125F;
                tex->v4 = voff + AnimatingTexturesV[i][nRangeFrames][0] + 0.125F;
                nRangeFrames--;
                range++;
            }
        }
    }
}

void ProjectTrainVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip, sint32 x)
{
    float zv;
    sint32 i;
    sint16 clip_distance;

    for (i = 0; i < nVerts; i++) {
        clip_distance = 0;
        v->tu = v->sx;
        v->tv = v->sy;
        if (v->sz >= f_mznear) {
            zv = f_mpersp / v->sz;
            if (v->sz > FogEnd) {
                v->sz = f_zfar;
                clip_distance = 256;
            }
            v->sx = zv * v->sx + f_centerx;
            v->sy = zv * v->sy + f_centery;
            v->rhw = f_moneopersp * zv;
            if (v->sx < clip_left)
                clip_distance++;
            else if (v->sx > clip_right)
                clip_distance += 2;
            if (v->sy < clip_top)
                clip_distance += 4;
            else if (v->sy > clip_bottom)
                clip_distance += 8;
        } else
            clip_distance = -128;
        clip[0] = clip_distance;
        clip++;
        v++;
    }
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

void do_boot_screen(sint32 language)
{
    Log(2, "do_boot_screen");
    switch (language) {
    case ENGLISH:
    case DUTCH:
        _LoadBitmap(App.dx.lpBackBuffer, "uk.bmp");
        S_DumpScreen();
        _LoadBitmap(App.dx.lpBackBuffer, "uk.bmp");
        break;
    case FRENCH:
        _LoadBitmap(App.dx.lpBackBuffer, "france.bmp");
        S_DumpScreen();
        _LoadBitmap(App.dx.lpBackBuffer, "france.bmp");
        break;
    case GERMAN:
        _LoadBitmap(App.dx.lpBackBuffer, "germany.bmp");
        S_DumpScreen();
        _LoadBitmap(App.dx.lpBackBuffer, "germany.bmp");
        break;
    case ITALIAN:
        _LoadBitmap(App.dx.lpBackBuffer, "italy.bmp");
        S_DumpScreen();
        _LoadBitmap(App.dx.lpBackBuffer, "italy.bmp");
        break;
    case SPANISH:
        _LoadBitmap(App.dx.lpBackBuffer, "spain.bmp");
        S_DumpScreen();
        _LoadBitmap(App.dx.lpBackBuffer, "spain.bmp");
        break;
    case US:
        _LoadBitmap(App.dx.lpBackBuffer, "usa.bmp");
        S_DumpScreen();
        _LoadBitmap(App.dx.lpBackBuffer, "usa.bmp");
        break;
    case JAPAN:
        _LoadBitmap(App.dx.lpBackBuffer, "japan.bmp");
        S_DumpScreen();
        _LoadBitmap(App.dx.lpBackBuffer, "japan.bmp");
        break;
    }
}
