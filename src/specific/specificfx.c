#include <windows.h>
#include <d3d.h>

#include "specificfx.h"
#include "3dmath.h"
#include "../anim.h"
#include "../objnums.h"
#include "../tomb4fx.h"
#include "function_table.h"
#include "polyinsert.h"
#include "../gameflow.h"

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

void ClipCheckPoint(D3DTLVERTEX *v, float x, float y, float z, sint16 *clip)
{
    float perspz;
    sint16 clipdistance;

    clipdistance = 0;
    v->tu = x;
    v->tv = y;
    v->sz = z;
    if (!(v->sz < f_mznear)) {
        perspz = f_mpersp / v->sz;
        if (v->sz > FogEnd) {
            v->sz = f_zfar;
            clipdistance = 256;
        }
        v->sx = perspz * v->tu + f_centerx;
        v->sy = perspz * v->tv + f_centery;
        v->rhw = perspz * f_moneopersp;
        if (v->sx < phd_winxmin)
            clipdistance++;
        else if (phd_winxmax < v->sx)
            clipdistance += 2;
        if (v->sy < phd_winymin)
            clipdistance += 4;
        else if (v->sy > phd_winymax)
            clipdistance += 8;
    } else {
        clipdistance = -128;
    }
    clip[0] = clipdistance;
}

void SetFade(sint32 start, sint32 end)
{
    DoFade = 1;
    FadeVal = start;
    FadeStep = (end - start) >> 3;
    FadeCnt = 0;
    FadeEnd = end;
}

void DoScreenFade(void)
{
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    sint32 a;

    a = FadeVal << 24;
    FadeVal += FadeStep;
    FadeCnt++;
    if (FadeCnt > 8) {
        DoFade = 2;
        a = FadeEnd << 24;
    }
    v[0].sx = 0;
    v[0].sy = 0;
    v[0].sz = 0;
    v[0].rhw = f_moneoznear;
    v[0].color = a;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(phd_winxmax + 1);
    v[1].sy = (float)phd_winymin;
    v[1].sz = 0;
    v[1].rhw = f_moneoznear;
    v[1].color = a;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(phd_winxmax + 1);
    v[2].sy = (float)(phd_winymax + 1);
    v[2].sz = 0;
    v[2].rhw = f_moneoznear;
    v[2].color = a;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)phd_winxmin;
    v[3].sy = (float)(phd_winymax + 1);
    v[3].sz = 0;
    v[3].rhw = f_moneoznear;
    v[3].color = a;
    v[3].specular = 0xFF000000;
    tex.drawtype = 3;
    tex.flag = 0;
    tex.tpage = 0;
    clipflags[0] = 0;
    clipflags[1] = 0;
    clipflags[2] = 0;
    clipflags[3] = 0;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
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
