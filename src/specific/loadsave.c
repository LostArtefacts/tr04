#include <windows.h>
#include <ddraw.h>
#include <stdio.h>
#include <string.h>

#include "loadsave.h"
#include "winmain.h"
#include "input.h"
#include "../text.h"
#include "../lara.h"
#include "../gameflow.h"
#include "function_table.h"
#include "drawroom.h"
#include "polyinsert.h"
#include "output.h"
#include "3dmath.h"
#include "specificfx.h"
#include "time.h"
#include "../newinv.h"
#include "../control.h"
#include "../savegame.h"
#include "gamemain.h"
#include "dxshell.h"
#include "../sound.h"

void MemBltSurf(void *dest, sint32 x, sint32 y, sint32 w, sint32 h, sint32 dadd, void *source, sint32 x2, sint32 y2, DDSURFACEDESC2 surface, float xsize, float ysize)
{
    uint32 *pDest;
    sint16 *psSrc;
    uint8 *pSrc;
    sint32 xadd, yadd, rx2, ry2, xoff, yoff, curY, i, j;
    sint16 andVal;
    uint8 r, g, b, rshift, gshift, bshift, rcount, gcount, bcount;

    xadd = (sint32)(((float)App.dx.dwRenderWidth / 640.0F) * xsize * 65536.0F);
    yadd = (sint32)(((float)App.dx.dwRenderHeight / 480.0F) * ysize * 65536.0F);
    rx2 = (sint32)((float)x2 * (float)App.dx.dwRenderWidth / 639.0F);
    ry2 = (sint32)((float)y2 * (float)App.dx.dwRenderHeight / 479.0F);
    if (App.dx.Flags & 2) {
        rx2 += App.dx.rScreen.left;
        ry2 += App.dx.rScreen.top;
    }
    DXBitMask2ShiftCnt(surface.ddpfPixelFormat.dwRBitMask, &rshift, &rcount);
    DXBitMask2ShiftCnt(surface.ddpfPixelFormat.dwGBitMask, &gshift, &gcount);
    DXBitMask2ShiftCnt(surface.ddpfPixelFormat.dwBBitMask, &bshift, &bcount);
    pDest = (uint32 *)dest + 4 * h * y + x;
    pSrc = (uint8 *)source + rx2 * (surface.ddpfPixelFormat.dwRGBBitCount >> 3) + ry2 * surface.lPitch;
    psSrc = (sint16 *)pSrc;
    curY = 0;
    yoff = 0;
    switch (surface.ddpfPixelFormat.dwRGBBitCount) {
    case 16:
        for (i = 0; i < h; i++) {
            xoff = 0;
            for (j = 0; j < w; j++) {
                andVal = psSrc[curY + (xoff >> 16)];
                r = (uint8)(((surface.ddpfPixelFormat.dwRBitMask & andVal) >> rshift) << (8 - rcount));
                g = (uint8)(((surface.ddpfPixelFormat.dwGBitMask & andVal) >> gshift) << (8 - gcount));
                b = (uint8)(((surface.ddpfPixelFormat.dwBBitMask & andVal) >> bshift) << (8 - bcount));
                *pDest = RGBA(r, g, b, 0xFF);
                pDest++;
                xoff += xadd;
            }
            yoff += yadd;
            curY = (surface.lPitch >> 1) * (yoff >> 16);
            pDest += dadd - w;
        }
        break;
    case 24:
        for (i = 0; i < h; i++) {
            xoff = 0;
            for (j = 0; j < w; j++) {
                r = pSrc[curY + (xoff >> 16)];
                g = pSrc[curY + 1 + (xoff >> 16)];
                b = pSrc[curY + 2 + (xoff >> 16)];
                *pDest = RGBA(r, g, b, 0xFF);
                pDest++;
                xoff += 3 * xadd;
            }
            yoff += yadd;
            curY = surface.lPitch * (yoff >> 16);
            pDest += dadd - w;
        }
        break;
    case 32:
        for (i = 0; i < h; i++) {
            xoff = 0;
            for (j = 0; j < w; j++) {
                r = pSrc[curY + (xoff >> 16)];
                g = pSrc[curY + 1 + (xoff >> 16)];
                b = pSrc[curY + 2 + (xoff >> 16)];
                *pDest = RGBA(r, g, b, 0xFF);
                pDest++;
                xoff += xadd << 2;
            }
            yoff += yadd;
            curY = surface.lPitch * (yoff >> 16);
            pDest += dadd - w;
        }
        break;
    }
}

void RGBM_Mono(uint8 *r, uint8 *g, uint8 *b)
{
    sint16 c;

    c = (sint16)(*r + *b) >> 1;
    *r = (uint8)c;
    *g = (uint8)c;
    *b = (uint8)c;
}

void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface)
{
    DDSURFACEDESC2 tSurf;
    uint16 *pTexture;
    uint16 *pSrc;

    memset(&tSurf, 0, sizeof(tSurf));
    tSurf.dwSize = sizeof(DDSURFACEDESC2);
    IDirectDrawSurface4_Lock(surface, 0, &tSurf, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, 0);
    pSrc = (uint16 *)tSurf.lpSurface;
    pTexture = (uint16 *)malloc(0x40000);
    MemBltSurf(pTexture, 0, 0, 256, 256, 256, pSrc, 0, 0, tSurf, 1.0F, 1.0F);
    MonoScreen[0].surface = CreateTexturePage(256, 256, 0, (sint32 *)pTexture, RGBM_Mono, 0);
    DXAttempt(IDirectDrawSurface4_QueryInterface(MonoScreen[0].surface, &IID_IDirect3DTexture2, (void **)&MonoScreen[0].tex));
    MemBltSurf(pTexture, 0, 0, 256, 256, 256, pSrc, 256, 0, tSurf, 1.0F, 1.0F);
    MonoScreen[1].surface = CreateTexturePage(256, 256, 0, (sint32 *)pTexture, RGBM_Mono, 0);
    DXAttempt(IDirectDrawSurface4_QueryInterface(MonoScreen[1].surface, &IID_IDirect3DTexture2, (void **)&MonoScreen[1].tex));
    MemBltSurf(pTexture, 0, 0, 128, 256, 256, pSrc, 512, 0, tSurf, 1.0F, 1.0F);
    MemBltSurf(pTexture, 128, 0, 128, 224, 256, pSrc, 512, 256, tSurf, 1.0F, 1.0F);
    MonoScreen[2].surface = CreateTexturePage(256, 256, 0, (sint32 *)pTexture, RGBM_Mono, 0);
    DXAttempt(IDirectDrawSurface4_QueryInterface(MonoScreen[2].surface, &IID_IDirect3DTexture2, (void **)&MonoScreen[2].tex));
    MemBltSurf(pTexture, 0, 0, 256, 224, 256, pSrc, 0, 256, tSurf, 1.0F, 1.0F);
    MonoScreen[3].surface = CreateTexturePage(256, 256, 0, (sint32 *)pTexture, RGBM_Mono, 0);
    DXAttempt(IDirectDrawSurface4_QueryInterface(MonoScreen[3].surface, &IID_IDirect3DTexture2, (void **)&MonoScreen[3].tex));
    MemBltSurf(pTexture, 0, 0, 256, 224, 256, pSrc, 256, 256, tSurf, 1.0F, 1.0F);
    MonoScreen[4].surface = CreateTexturePage(256, 256, 0, (sint32 *)pTexture, RGBM_Mono, 0);
    DXAttempt(IDirectDrawSurface4_QueryInterface(MonoScreen[4].surface, &IID_IDirect3DTexture2, (void **)&MonoScreen[4].tex));
    IDirectDrawSurface4_Unlock(surface, 0);
    free(pTexture);
}

void CreateMonoScreen(void)
{
    MonoScreenOn = 1;
    ConvertSurfaceToTextures(App.dx.lpPrimaryBuffer);
}

void FreeMonoScreen(void)
{
    if (MonoScreen[0].surface) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[0].surface, IDirectDrawSurface4_Release(MonoScreen[0].surface));
        MonoScreen[0].surface = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");
    if (MonoScreen[1].surface) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[1].surface, IDirectDrawSurface4_Release(MonoScreen[1].surface));
        MonoScreen[1].surface = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");
    if (MonoScreen[2].surface) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[2].surface, IDirectDrawSurface4_Release(MonoScreen[2].surface));
        MonoScreen[2].surface = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");
    if (MonoScreen[3].surface) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[3].surface, IDirectDrawSurface4_Release(MonoScreen[3].surface));
        MonoScreen[3].surface = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");
    if (MonoScreen[4].surface) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Surface", MonoScreen[4].surface, IDirectDrawSurface4_Release(MonoScreen[4].surface));
        MonoScreen[4].surface = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Surface");
    if (MonoScreen[0].tex) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[0].tex, IDirect3DTexture2_Release(MonoScreen[0].tex));
        MonoScreen[0].tex = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");
    if (MonoScreen[1].tex) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[1].tex, IDirect3DTexture2_Release(MonoScreen[1].tex));
        MonoScreen[1].tex = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");
    if (MonoScreen[2].tex) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[2].tex, IDirect3DTexture2_Release(MonoScreen[2].tex));
        MonoScreen[2].tex = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");
    if (MonoScreen[3].tex) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[3].tex, IDirect3DTexture2_Release(MonoScreen[3].tex));
        MonoScreen[3].tex = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");
    if (MonoScreen[4].tex) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Mono Screen Texture", MonoScreen[4].tex, IDirect3DTexture2_Release(MonoScreen[4].tex));
        MonoScreen[4].tex = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Mono Screen Texture");
    MonoScreenOn = 0;
}

void S_DisplayMonoScreen(void)
{
    sint32 x[4];
    sint32 y[4];
    uint32 col;
    sint32 i;

    for (i = 0; i < 3; i++) {
        x[i] = phd_winxmin + phd_winwidth * MonoScreenX[i] / 640;
        y[i] = phd_winymin + phd_winheight * MonoScreenY[i] / 480;
    }
    x[3] = phd_winxmin + phd_winwidth * MonoScreenX[3] / 640;
    RestoreFPCW(FPCW);
    col = 0xFFFFFFFF;
    S_DrawTile(x[0], y[0], x[1] - x[0], y[1] - y[0], MonoScreen[0].tex, 0, 0, 256, 256, col, col, col, col);
    S_DrawTile(x[1], y[0], x[2] - x[1], y[1] - y[0], MonoScreen[1].tex, 0, 0, 256, 256, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    S_DrawTile(x[2], y[0], x[3] - x[2], y[1] - y[0], MonoScreen[2].tex, 0, 0, 128, 256, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    S_DrawTile(x[0], y[1], x[1] - x[0], y[2] - y[1], MonoScreen[3].tex, 0, 0, 256, 224, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    S_DrawTile(x[1], y[1], x[2] - x[1], y[2] - y[1], MonoScreen[4].tex, 0, 0, 256, 224, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    S_DrawTile(x[2], y[1], x[3] - x[2], y[2] - y[1], MonoScreen[2].tex, 128, 0, 128, 224, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    MungeFPCW(&FPCW);
}

sint32 GetSaveLoadFiles(void)
{
    FILE *file;
    SAVEFILE_INFO *pSave;
    SAVEGAME_INFO save_info;
    char name[80];
    sint32 i;

    SaveCounter = 0;
    for (i = 0; i < 15; i++) {
        pSave = &SaveGames[i];
        wsprintf(name, "savegame.%d", i);
        file = fopen(name, "rb");
        if (file) {
            fread(&pSave->name, sizeof(char), 75, file);
            fread(&pSave->num, sizeof(sint32), 1, file);
            fread(&pSave->days, sizeof(sint16), 1, file);
            fread(&pSave->hours, sizeof(sint16), 1, file);
            fread(&pSave->minutes, sizeof(sint16), 1, file);
            fread(&pSave->seconds, sizeof(sint16), 1, file);
            fread(&save_info, 1, sizeof(SAVEGAME_INFO), file);
            if (CheckSumValid((sint8 *)&save_info)) {
                if (pSave->num > SaveCounter)
                    SaveCounter = pSave->num;
                pSave->valid = 1;
                fclose(file);
                nSaves++;
            } else {
                strcpy(pSave->name, SCRIPT_TEXT(TXT_Empty_Slot));
            }
        } else {
            pSave->valid = 0;
            strcpy(pSave->name, SCRIPT_TEXT(TXT_Empty_Slot));
        }
    }
    SaveCounter++;
    return nSaves;
}

sint32 DoLoadSave(sint32 LoadSave)
{
    SAVEFILE_INFO *pSave;
    static sint32 selection;
    sint32 color, l, i, y;
    char string[80];
    char name[41];

    y = font_height;
    if (LoadSave & IN_SAVE)
        PrintString((uint16)phd_centerx, (uint16)y, 6, SCRIPT_TEXT(TXT_Save_Game), FF_CENTER);
    else
        PrintString((uint16)phd_centerx, (uint16)y, 6, SCRIPT_TEXT(TXT_Load_Game), FF_CENTER);
    for (i = 0; i < 15; i++) {
        pSave = &SaveGames[i];
        color = 2;
        if (i == selection)
            color = 1;
        memset(name, ' ', 40);
        l = strlen(pSave->name);
        if (l > 40)
            l = 40;
        strncpy(name, pSave->name, l);
        name[40] = 0;
        small_font = 1;
        if (pSave->valid) {
            wsprintf(string, "%03d", pSave->num);
            PrintString((uint16)(phd_centerx - (sint32)(310.0F * ((float)phd_winwidth * (1.0F / 640.0F)))), (uint16)(y + font_height * (i + 2)), (uint8)color, string, 0);
            PrintString((uint16)(phd_centerx - (sint32)(270.0F * ((float)phd_winwidth * (1.0F / 640.0F)))), (uint16)(y + font_height * (i + 2)), (uint8)color, name, 0);
            wsprintf(string, "%d %s %02d:%02d:%02d", pSave->days, SCRIPT_TEXT(TXT_days), pSave->hours, pSave->minutes, pSave->seconds);
            PrintString((uint16)(phd_centerx - (sint32)(-135.0F * ((float)phd_winwidth * (1.0F / 640.0F)))), (uint16)(y + font_height * (i + 2)), (uint8)color, string, 0);
        } else {
            wsprintf(string, "%s", pSave->name);
            PrintString((uint16)phd_centerx, (uint16)(y + font_height * (i + 2)), (uint8)color, string, FF_CENTER);
        }
        small_font = 0;
    }
    if (dbinput & IN_FORWARD) {
        selection--;
        SoundEffect(SFX_MENU_CHOOSE, 0, SFX_DEFAULT);
    }
    if (dbinput & IN_BACK) {
        selection++;
        SoundEffect(SFX_MENU_CHOOSE, 0, SFX_DEFAULT);
    }
    if (selection < 0)
        selection = 0;
    else if (selection > 14)
        selection = 14;
    if (dbinput & IN_SELECT) {
        if (SaveGames[selection].valid || LoadSave == IN_SAVE)
            return selection;
        SoundEffect(SFX_LARA_NO, 0, SFX_DEFAULT);
    }
    return -1;
}

void DoSlider(sint32 x, sint32 y, sint32 width, sint32 height, sint32 pos, sint32 clr1, sint32 clr2, sint32 clr3)
{
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    float scale;
    sint32 px, w, h, sx;
    static float V;

    nPolyType = 4;
    V += 0.01F;
    if (V > 0.99F)
        V = 0;
    clipflags[0] = 0;
    clipflags[1] = 0;
    clipflags[2] = 0;
    clipflags[3] = 0;
    scale = (float)phd_winxmax * (1.0F / 640.0F);
    px = (sint32)(x * scale);
    w = (sint32)(width * scale);
    h = (sint32)(((float)phd_winymax / 480.0F) * (height >> 1));
    v[0].sx = (float)px;
    v[0].sy = (float)y;
    v[0].sz = f_mznear;
    v[0].rhw = f_moneoznear - 2.0F;
    v[0].color = clr1;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(px + w);
    v[1].sy = (float)y;
    v[1].sz = f_mznear;
    v[1].rhw = f_moneoznear - 2.0F;
    v[1].color = clr1;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(px + w);
    v[2].sy = (float)(y + h);
    v[2].sz = f_mznear;
    v[2].rhw = f_moneoznear - 2.0F;
    v[2].color = clr2;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)px;
    v[3].sy = (float)(y + h);
    v[3].sz = f_mznear;
    v[3].rhw = f_moneoznear - 2.0F;
    v[3].color = clr2;
    v[3].specular = 0xFF000000;
    tex.drawtype = 0;
    tex.tpage = (uint16)(nTextures - 1);
    tex.flag = 0;
    tex.u1 = 0;
    tex.v1 = V;
    tex.u2 = 1;
    tex.v2 = V;
    tex.u3 = 1;
    tex.v3 = V + 0.01F;
    tex.u4 = 0;
    tex.v4 = V + 0.01F;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
    v[0].sx = (float)px;
    v[0].sy = (float)(y + h);
    v[0].sz = f_mznear;
    v[0].rhw = f_moneoznear - 2.0F;
    v[0].color = clr2;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(px + w);
    v[1].sy = (float)(y + h);
    v[1].sz = f_mznear;
    v[1].rhw = f_moneoznear - 2.0F;
    v[1].color = clr2;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(px + w);
    v[2].sy = (float)(y + 2 * h);
    v[2].sz = f_mznear;
    v[2].rhw = f_moneoznear - 2.0F;
    v[2].color = clr1;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)px;
    v[3].sy = (float)(y + 2 * h);
    v[3].sz = f_mznear;
    v[3].rhw = f_moneoznear - 2.0F;
    v[3].color = clr1;
    v[3].specular = 0xFF000000;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
    v[0].sx = (float)(px - 1);
    v[0].sy = (float)(y - 1);
    v[0].sz = f_mznear + 2.0F;
    v[0].rhw = f_moneoznear - 3.0F;
    v[0].color = 0xFFFFFFFF;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(px + w + 1);
    v[1].sy = (float)(y - 1);
    v[1].sz = f_mznear + 2.0F;
    v[1].rhw = f_moneoznear - 3.0F;
    v[1].color = 0xFFFFFFFF;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(px + w + 1);
    v[2].sy = (float)(y + 2 * h + 1);
    v[2].sz = f_mznear + 2.0F;
    v[2].rhw = f_moneoznear - 3.0F;
    v[2].color = 0xFFFFFFFF;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)(px - 1);
    v[3].sy = (float)(y + 2 * h + 1);
    v[3].sz = f_mznear + 2.0F;
    v[3].rhw = f_moneoznear - 3.0F;
    v[3].color = 0xFFFFFFFF;
    v[3].specular = 0xFF000000;
    tex.tpage = 0;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
    sx = px + w * pos / 100;
    v[0].sx = (float)px;
    v[0].sy = (float)y;
    v[0].sz = f_mznear - 1.0F;
    v[0].rhw = f_moneoznear - 1.0F;
    v[0].color = clr3;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(sx + 1);
    v[1].sy = (float)y;
    v[1].sz = f_mznear - 1.0F;
    v[1].rhw = f_moneoznear - 1.0F;
    v[1].color = clr3;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)sx;
    v[2].sy = (float)(y + 2 * h);
    v[2].sz = f_mznear - 1.0F;
    v[2].rhw = f_moneoznear - 1.0F;
    v[2].color = clr3;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)(px - 1);
    v[3].sy = (float)(y + 2 * h);
    v[3].sz = f_mznear - 1.0F;
    v[3].rhw = f_moneoznear - 1.0F;
    v[3].color = clr3;
    v[3].specular = 0xFF000000;
    tex.tpage = 0;
    tex.drawtype = 2;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

void CheckKeyConflicts(void)
{
    sint16 key;
    sint32 i;
    sint32 j;

    for (i = 0; i < 18; i++) {
        key = layout[0][i];
        conflict[i] = 0;
        for (j = 0; j < 18; j++) {
            if (key == layout[1][j]) {
                conflict[i] = 1;
                break;
            }
        }
    }
}

void DoStatScreen(void)
{
    sint32 y, t;
    sint16 days, hours, min, sec;
    char buf[40];

    y = phd_centery - (font_height << 2);
    PrintString((uint16)phd_centerx, (uint16)y, 6, SCRIPT_TEXT(TXT_Statistics), FF_CENTER);
    PrintString((uint16)phd_centerx, (uint16)(y + 2 * font_height), 2, SCRIPT_TEXT(gfLevelNames[gfCurrentLevel]), FF_CENTER);
    PrintString((uint16)(phd_centerx >> 2), (uint16)(y + 3 * font_height), 2, SCRIPT_TEXT(TXT_Time_Taken), 0);
    PrintString((uint16)(phd_centerx >> 2), (uint16)(y + 4 * font_height), 2, SCRIPT_TEXT(TXT_Distance_Travelled), 0);
    PrintString((uint16)(phd_centerx >> 2), (uint16)(y + 5 * font_height), 2, SCRIPT_TEXT(TXT_Ammo_Used), 0);
    PrintString((uint16)(phd_centerx >> 2), (uint16)(y + 6 * font_height), 2, SCRIPT_TEXT(TXT_Health_Packs_Used), 0);
    PrintString((uint16)(phd_centerx >> 2), (uint16)(y + 7 * font_height), 2, SCRIPT_TEXT(TXT_Secrets_Found), 0);
    t = GameTimer / 30;
    days = (sint16)(t / 86400);
    hours = (sint16)((t % 86400) / 3600);
    min = (sint16)((t / 60) % 60);
    sec = (sint16)(t % 60);
    sprintf(buf, "%02d:%02d:%02d", days * 24 + hours, min, sec);
    PrintString((uint16)(phd_centerx + (phd_centerx >> 2)), (uint16)(y + 3 * font_height), 6, buf, 0);
    sprintf(buf, "%dm", (sint32)savegame.Game.Distance / 419);
    PrintString((uint16)(phd_centerx + (phd_centerx >> 2)), (uint16)(y + 4 * font_height), 6, buf, 0);
    sprintf(buf, "%d", savegame.Game.AmmoUsed);
    PrintString((uint16)(phd_centerx + (phd_centerx >> 2)), (uint16)(y + 5 * font_height), 6, buf, 0);
    sprintf(buf, "%d", savegame.Game.HealthUsed);
    PrintString((uint16)(phd_centerx + (phd_centerx >> 2)), (uint16)(y + 6 * font_height), 6, buf, 0);
    sprintf(buf, "%d / 70", savegame.Game.Secrets);
    PrintString((uint16)(phd_centerx + (phd_centerx >> 2)), (uint16)(y + 7 * font_height), 6, buf, 0);
}

sint32 S_DisplayPauseMenu(sint32 reset)
{
    sint32 y;

    if (!pause_menu) {
        if (reset) {
            pause_selection = reset;
            pause_menu = 0;
        } else {
            y = phd_centery - font_height;
            PrintString((uint16)phd_centerx, (uint16)(y - ((3 * font_height) >> 1)), 6, SCRIPT_TEXT(TXT_Paused), FF_CENTER);
            PrintString((uint16)phd_centerx, (uint16)y, pause_selection & 1 ? 1 : 2, SCRIPT_TEXT(TXT_Statistics), FF_CENTER);
            PrintString((uint16)phd_centerx, (uint16)(y + font_height), pause_selection & 2 ? 1 : 2, SCRIPT_TEXT(TXT_Options), FF_CENTER);
            PrintString((uint16)phd_centerx, (uint16)(y + 2 * font_height), pause_selection & 4 ? 1 : 2, SCRIPT_TEXT(TXT_Exit_to_Title), FF_CENTER);
            if (dbinput & IN_FORWARD) {
                if (pause_selection > 1)
                    pause_selection >>= 1;
                SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
            }
            if (dbinput & IN_BACK) {
                if (pause_selection < 4)
                    pause_selection <<= 1;
                SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
            }
            if (dbinput & IN_DESELECT) {
                SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
                return 1;
            }
            if (dbinput & IN_SELECT && !keymap[DIK_LALT]) {
                SoundEffect(SFX_MENU_SELECT, 0, SFX_DEFAULT);
                if (pause_selection & 1)
                    pause_menu = 2;
                else if (pause_selection & 2)
                    pause_menu = 1;
                else if (pause_selection & 4)
                    return 8;
            }
        }
    } else if (pause_menu == 1) {
        DoOptions();
        if (dbinput & IN_DESELECT) {
            pause_menu = 0;
            SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
        }
    } else if (pause_menu == 2) {
        DoStatScreen();
        if (dbinput & IN_DESELECT) {
            pause_menu = 0;
            SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
        }
    }
    return 0;
}

sint32 S_PauseMenu(void)
{
    sint32 fade;
    sint32 ret;

    fade = 0;
    CreateMonoScreen();
    S_DisplayPauseMenu(1);
    InventoryActive = 1;
    while (1) {
        S_InitialisePolyList();
        if (!fade)
            S_UpdateInput();
        else
            dbinput = 0;
        SetDebounce = 1;
        S_DisplayMonoScreen();
        ret = S_DisplayPauseMenu(0);
        UpdatePulseColour();
        S_OutputPolyList();
        S_DumpScreen();
        if (ret == 1)
            break;
        if (ret == 8) {
            fade = 8;
            ret = 0;
            SetFade(0, 255);
        }
        if (fade && DoFade == 2) {
            ret = fade;
            break;
        }
        if (thread_ended)
            break;
    }
    TIME_Init();
    FreeMonoScreen();
    InventoryActive = 0;
    return ret;
}

sint32 S_LoadSave(sint32 load_or_save, sint32 mono)
{
    sint32 fade;
    sint32 ret;

    fade = 0;
    if (!mono)
        CreateMonoScreen();
    GetSaveLoadFiles();
    InventoryActive = 1;
    while (1) {
        S_InitialisePolyList();
        if (!fade)
            S_UpdateInput();
        else
            dbinput = 0;
        SetDebounce = 1;
        S_DisplayMonoScreen();
        ret = DoLoadSave(load_or_save);
        UpdatePulseColour();
        S_OutputPolyList();
        S_DumpScreen();
        if (ret >= 0) {
            if (!(load_or_save & IN_SAVE)) {
                fade = ret + 1;
                S_LoadGame(ret);
                SetFade(0, 255);
                ret = -1;
            } else {
                sgSaveGame();
                S_SaveGame(ret);
                GetSaveLoadFiles();
                break;
            }
        }
        if (fade && DoFade == 2) {
            ret = fade - 1;
            break;
        }
        if (input & IN_OPTION) {
            ret = -1;
            break;
        }
        if (thread_ended)
            break;
    }
    TIME_Init();
    if (!mono)
        FreeMonoScreen();
    InventoryActive = 0;
    return ret;
}

void DoBar(sint32 x, sint32 y, sint32 width, sint32 height, sint32 pos, sint32 clr1, sint32 clr2)
{
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    static float wat = 0;
    sint32 x2, sx, sy;

    nPolyType = 4;
    wat += 0.01F;
    if (wat > 0.99F)
        wat = 0;
    clipflags[0] = 0;
    clipflags[1] = 0;
    clipflags[2] = 0;
    clipflags[3] = 0;
    x2 = (sint32)(x * ((float)phd_winxmax / 640.0F));
    sy = (sint32)((height >> 1) * ((float)phd_winymax / 480.0F));
    sx = (sint32)(width * ((float)phd_winxmax / 640.0F)) * pos / 100;
    v[0].tu = v[0].tv = 0;
    v[1].tu = v[1].tv = 0;
    v[2].tu = v[2].tv = 0;
    v[3].tu = v[3].tv = 0;
    v[0].sx = (float)x2;
    v[0].sy = (float)y;
    v[0].color = clr1;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(x2 + sx);
    v[1].sy = (float)y;
    v[1].color = clr1;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(x2 + sx);
    v[2].sy = (float)(y + sy);
    v[2].color = clr2;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)x2;
    v[3].sy = (float)(y + sy);
    v[3].color = clr2;
    v[3].specular = 0xFF000000;
    v[0].sz = f_mznear - 6;
    v[0].rhw = f_moneoznear - 2;
    v[1].sz = f_mznear - 6;
    v[1].rhw = f_moneoznear - 2;
    v[2].sz = f_mznear - 6;
    v[2].rhw = f_moneoznear - 2;
    v[3].sz = f_mznear - 6;
    v[3].rhw = f_moneoznear - 2;
    tex.drawtype = 0;
    tex.flag = 0;
    tex.tpage = 0;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
    v[0].sx = (float)x2;
    v[0].sy = (float)(y + sy);
    v[0].color = clr2;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(x2 + sx);
    v[1].sy = (float)(y + sy);
    v[1].color = clr2;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(x2 + sx);
    v[2].sy = (float)(y + 2 * sy);
    v[2].color = clr1;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)x2;
    v[3].sy = (float)(y + 2 * sy);
    v[3].color = clr1;
    v[3].specular = 0xFF000000;
    v[0].sz = f_mznear - 6;
    v[0].rhw = f_moneoznear - 2;
    v[1].sz = f_mznear - 6;
    v[1].rhw = f_moneoznear - 2;
    v[2].sz = f_mznear - 6;
    v[2].rhw = f_moneoznear - 2;
    v[3].sz = f_mznear - 6;
    v[3].rhw = f_moneoznear - 2;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
    x2 = (sint32)(x * ((float)phd_winxmax / 640.0F));
    sx = (sint32)(width * ((float)phd_winxmax / 640.0F));
    sy = (sint32)((height >> 1) * ((float)phd_winymax / 480.0F));
    v[0].sx = (float)(x2 - 1);
    v[0].sy = (float)(y - 1);
    v[0].color = 0xFFFFFFFF;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(x2 + sx + 1);
    v[1].sy = (float)(y - 1);
    v[1].color = 0xFFFFFFFF;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(x2 + sx + 1);
    v[2].sy = (float)(y + 2 * sy + 1);
    v[2].color = 0xFFFFFFFF;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)(x2 - 1);
    v[3].sy = (float)(y + 2 * sy + 1);
    v[3].color = 0xFFFFFFFF;
    v[3].specular = 0xFF000000;
    v[0].sz = f_mznear - 3;
    v[0].rhw = f_moneoznear - 4;
    v[1].sz = f_mznear - 3;
    v[1].rhw = f_moneoznear - 4;
    v[2].sz = f_mznear - 3;
    v[2].rhw = f_moneoznear - 4;
    v[3].sz = f_mznear - 3;
    v[3].rhw = f_moneoznear - 4;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
    v[0].sx = (float)x2;
    v[0].sy = (float)y;
    v[0].color = 0xFF000000;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(x2 + sx);
    v[1].sy = (float)y;
    v[1].color = 0xFF000000;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(x2 + sx);
    v[2].sy = (float)(y + 2 * sy);
    v[2].color = 0xFF000000;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)x2;
    v[3].sy = (float)(y + 2 * sy);
    v[3].color = 0xFF000000;
    v[3].specular = 0xFF000000;
    v[0].sz = f_mznear - 4;
    v[0].rhw = f_moneoznear - 3;
    v[1].sz = f_mznear - 4;
    v[1].rhw = f_moneoznear - 3;
    v[2].sz = f_mznear - 4;
    v[2].rhw = f_moneoznear - 3;
    v[3].sz = f_mznear - 4;
    v[3].rhw = f_moneoznear - 3;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

void S_DrawDashBar(sint32 pos)
{
    if (gfCurrentLevel)
        DoBar(490 - (font_height >> 2), font_height >> 2, 150, 12, pos, 0xFF000000, 0xFF00FF00);
}

void S_DrawAirBar(sint32 pos)
{
    if (gfCurrentLevel)
        DoBar(490 - (font_height >> 2), (font_height >> 2) + (font_height >> 1), 150, 12, pos, 0xFF000000, 0xFF0000FF);
}

void S_DrawHealthBar(sint32 pos)
{
    if (gfCurrentLevel)
        DoBar(font_height >> 2, font_height >> 2, 150, 12, pos, 0xFF000000, lara.poisoned ? 0xFFFFFF00 : 0xFFFF0000);
}

void S_InitLoadBar(sint32 maxpos)
{
    loadbar_pos = 0;
    loadbar_maxpos = maxpos;
}

void S_LoadBar(void)
{
    if (gfCurrentLevel || App.dx.Flags & 0x80) {
        _BeginScene();
        InitBuckets();
        InitialiseSortList();
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
        loadbar_pos += 100.0F / loadbar_maxpos;
        DoBar(20, phd_winymax - font_height, 600, 15, (sint32)loadbar_pos, 0xFF000000, 0xFF9F1F80);
        SortPolyList(SortCount, SortList);
        RestoreFPCW(FPCW);
        DrawSortList();
        MungeFPCW(&FPCW);
        S_DumpScreen();
    }
}
