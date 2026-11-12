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
#include "specificfx.h"
#include "loadsave.h"
#include "../tomb4fx.h"
#include "../camera.h"
#include "../health.h"
#include "../deltapak.h"
#include "../draw.h"
#include "../lara.h"

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

void S_OutputPolyList(void)
{
    D3DRECT r;
    sint32 h;

    RestoreFPCW(FPCW);
    WinFrameRate();
    nPolys = 0;
    nClippedPolys = 0;
    DrawPrimitiveCnt = 0;
    Unk5c0478 = 0;
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
    if (resChangeCounter) {
        WinDisplayString(8, App.dx.dwRenderHeight - 8, "%dx%d", App.dx.dwRenderWidth, App.dx.dwRenderHeight);
        resChangeCounter -= 30.0F / App.fps;
        if (resChangeCounter < 0)
            resChangeCounter = 0;
    }
    if (App.dx.lpZBuffer)
        DrawBuckets();
    if (!gfCurrentLevel) {
        Fade();
        if (App.dx.lpZBuffer)
            DrawSortList();
    }
    SortPolyList(SortCount, SortList);
    DrawSortList();
    if (App.dx.lpZBuffer) {
        r.x1 = App.dx.rViewport.left;
        r.y1 = App.dx.rViewport.top;
        r.x2 = App.dx.rViewport.left + App.dx.rViewport.right;
        r.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
        DXAttempt(IDirect3DViewport3_Clear2(App.dx.lpViewport, 1, &r, D3DCLEAR_ZBUFFER, 0, 1.0F, 0));
    }
    if (BinocularRange && !MonoScreenOn) {
        InitialiseSortList();
        DrawBinoculars();
        DrawSortList();
    }
    if (pickups[CurrentPickup].life != -1 && !MonoScreenOn && !GLOBAL_playing_cutseq) {
        bWaterEffect = 0;
        InitialiseSortList();
        S_DrawPickup(pickups[CurrentPickup].object_number);
        SortPolyList(SortCount, SortList);
        DrawSortList();
    }
    InitialiseSortList();
    if (FadeScreenHeight) {
        h = (sint32)(FadeScreenHeight * ((float)phd_winymax / 256.0F));
        DrawPsxTile(0, phd_winwidth | (h << 16), 0x62FFFFFF, 0, 0);
        DrawPsxTile(phd_winheight - h, phd_winwidth | (h << 16), 0x62FFFFFF, 0, 0);
    }
    if (gfCurrentLevel) {
        Fade();
        if (FlashFader) {
            DrawFlash();
            if (FlashFader)
                FlashFader -= 2;
        }
        DrawSortList();
    }
    if (DoFade == 1) {
        InitialiseSortList();
        DoScreenFade();
        DrawSortList();
    }
    MungeFPCW(&FPCW);
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

void RenderLoadPic(sint32 unused)
{
    sint32 poisoned;

    camera.pos.y = load_cam.y;
    camera.pos.x = load_cam.x;
    camera.pos.z = load_cam.z;
    lara_item->pos.x_pos = camera.pos.x;
    lara_item->pos.y_pos = camera.pos.y;
    lara_item->pos.z_pos = camera.pos.z;
    camera.target.x = load_target.x;
    camera.target.y = load_target.y;
    camera.target.z = load_target.z;
    camera.pos.room_number = load_roomnum;
    if (camera.pos.room_number == 255)
        return;
    KillActiveBaddies((ITEM_INFO *)0xABCDEF);
    SetFade(255, 0);
    poisoned = lara.poisoned;
    FadeScreenHeight = 0;
    lara.poisoned = 0;
    GlobalFogOff = 1;
    BinocularRange = 0;
    if (App.dx.InScene)
        _EndScene();
    do {
        phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
        S_InitialisePolyList();
        RenderIt(camera.pos.room_number);
        S_OutputPolyList();
        S_DumpScreen();
    } while (DoFade != 2);
    phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
    S_InitialisePolyList();
    RenderIt(camera.pos.room_number);
    S_OutputPolyList();
    S_DumpScreen();
    lara.poisoned = poisoned;
    GlobalFogOff = 0;
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
