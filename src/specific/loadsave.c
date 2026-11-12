#include <windows.h>
#include <ddraw.h>

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

void CreateMonoScreen(void)
{
    MonoScreenOn = 1;
    ConvertSurfaceToTextures(App.dx.lpPrimaryBuffer);
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
