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
