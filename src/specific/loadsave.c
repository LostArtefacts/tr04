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
