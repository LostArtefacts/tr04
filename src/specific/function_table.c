#include "function_table.h"
#include "function_stubs.h"
#include "polyinsert.h"
#include "winmain.h"
#include "dxshell.h"
#include "3dmath.h"

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

void InitialiseFunctionTable(void)
{
    _BeginScene = HWBeginScene;
    _EndScene = HWEndScene;
    IsVisible = _NVisible;
    if (App.dx.lpZBuffer) {
        AddQuadZBuffer = AddQuadClippedZBuffer;
        AddTriZBuffer = AddTriClippedZBuffer;
        AddQuadSorted = AddQuadClippedSorted;
        AddTriSorted = AddTriClippedSorted;
    } else {
        AddQuadZBuffer = AddQuadSubdivide;
        AddTriZBuffer = AddTriSubdivide;
        AddQuadSorted = AddQuadSubdivide;
        AddTriSorted = AddTriSubdivide;
    }
    AddLineSorted = AddLineClippedSorted;
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

void HWInitialise(void)
{
    Log(2, "HWIntialise");
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 3, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 4, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 5, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 6, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 7, D3DTSS_COLOROP, D3DTOP_DISABLE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_TEXCOORDINDEX, 0);
    if (App.Filtering) {
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
    } else {
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_MAGFILTER, D3DTFG_POINT);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_MINFILTER, D3DTFN_POINT);
    }
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_TEXCOORDINDEX, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SPECULARENABLE, 1);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    DXAttempt(IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZENABLE, D3DZB_TRUE));
    DXAttempt(IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 1));
    DXAttempt(IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1));
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DITHERENABLE, 1);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHAREF, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHATESTENABLE, 0);
    DXAttempt(IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_AMBIENT, 0));
    DXAttempt(IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_COLORVERTEX, 0));
    DXAttempt(IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_COLORMODEL, D3DCOLOR_RGB));
    DXAttempt(IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_FOGMODE, D3DFOG_LINEAR));
    DXAttempt(IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_FOGSTART, *(DWORD *)&FogStart));
    DXAttempt(IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_FOGEND, *(DWORD *)&FogEnd));
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGCOLOR, 0xFF000000);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 1);
}

void SetFogColor(sint32 r, sint32 g, sint32 b)
{
    sint32 c;

    c = (r << 16) | (g << 8) | b | 0xFF000000;
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGCOLOR, c);
    CurrentFog = c;
}
