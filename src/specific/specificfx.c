#include <windows.h>
#include <d3d.h>

#include "specificfx.h"
#include "3dmath.h"
#include "../anim.h"
#include "../objnums.h"
#include "../tomb4fx.h"

void InitTarget_2(void)
{
    OBJECT_INFO *obj;
    MESH_DATA *mesh;
    D3DTLVERTEX *v;
    sint32 i;

    obj = &objects[TARGET_GRAPHICS];
    if (!obj->loaded)
        return;
    mesh = (MESH_DATA *)meshes[obj->mesh_index];
    targetMeshP = mesh;
    IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB, DDLOCK_READONLY, (void **)&v, 0);
    for (i = 0; i < mesh->nVerts; i++) {
        v[i].sx = (1.0F / 96.0F) * (v[i].sx * 80.0F);
        v[i].sy = (1.0F / 224.0F) * (v[i].sy * 60.0F);
        v[i].color = 0xFF000000;
        v[i].specular = 0xFF000000;
        v[i].sz = 0;
        v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
    }
    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
}

void InitBinoculars(void)
{
    OBJECT_INFO *obj;
    MESH_DATA *mesh;
    D3DTLVERTEX *v;
    sint32 i;

    obj = &objects[BINOCULAR_GRAPHICS];
    if (!obj->loaded)
        return;
    mesh = (MESH_DATA *)meshes[obj->mesh_index];
    binocsMeshP = mesh;
    IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB, DDLOCK_READONLY, (void **)&v, 0);
    for (i = 0; i < mesh->nVerts; i++) {
        v[i].sx = (1.0F / 96.0F) * (v[i].sx * 32.0F);
        v[i].sy = (1.0F / 224.0F) * (v[i].sy * 30.0F);
        v[i].color = 0xFF000000;
        v[i].specular = 0xFF000000;
        v[i].sz = 0;
        v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
    }
    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
}

void SetFade(sint32 start, sint32 end)
{
    DoFade = 1;
    FadeVal = start;
    FadeStep = (end - start) >> 3;
    FadeCnt = 0;
    FadeEnd = end;
}

void DrawFlash(void)
{
    uint8 r;
    uint8 g;
    uint8 b;

    r = (FlashFadeR * FlashFader) >> 5;
    g = (FlashFadeG * FlashFader) >> 5;
    b = (FlashFadeB * FlashFader) >> 5;
    DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), b | ((g | ((r | (0x62 << 8)) << 8)) << 8), 1, 0);
    DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), b | ((g | ((r | (0xFF << 8)) << 8)) << 8), 2, 0);
}
