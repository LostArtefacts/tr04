#include "function_table.h"
#include "function_stubs.h"
#include "polyinsert.h"
#include "winmain.h"

uint8 _NVisible(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2)
{
    if ((v0->sy - v1->sy) * (v2->sx - v1->sx) - (v2->sy - v1->sy) * (v0->sx - v1->sx) < 0)
        return 1;
    return 0;
}

uint8 _Visible(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2)
{
    if ((v0->sy - v1->sy) * (v2->sx - v1->sx) - (v2->sy - v1->sy) * (v0->sx - v1->sx) > 0)
        return 1;
    return 0;
}

void SetCullCW(void)
{
    IsVisible = _Visible;
}

void SetCullCCW(void)
{
    IsVisible = _NVisible;
}

HRESULT HWBeginScene(void)
{
    if (App.dx.InScene)
        Log(1, "Already In Scene");
    App.dx.InScene = 1;
    App.dx.DoneBlit = 0;
    while (App.dx.WaitAtBeginScene) {
    }
    return IDirect3DDevice3_BeginScene(App.dx.lpD3DDevice);
}

HRESULT HWEndScene(void)
{
    App.dx.InScene = 0;
    return IDirect3DDevice3_EndScene(App.dx.lpD3DDevice);
}

void SetFogColor(sint32 r, sint32 g, sint32 b)
{
    sint32 c;

    c = (r << 16) | (g << 8) | b | 0xFF000000;
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGCOLOR, c);
    CurrentFog = c;
}
