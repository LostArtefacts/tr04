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
#include "winmain.h"
#include "file.h"
#include "drawroom.h"
#include "../control.h"
#include "../text.h"
#include "function_stubs.h"

void setXY4(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 x3, sint32 y3, sint32 x4, sint32 y4, sint32 z, sint16 *clip)
{
    float zv;
    sint32 clip_distance;

    clip_distance = 0;
    zv = f_mpersp / (float)z;
    v->sx = (float)x1;
    v->sy = (float)y1;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x1 < phd_winxmin)
        clip_distance = 1;
    else if (x1 > phd_winxmax)
        clip_distance = 2;
    if (y1 < phd_winymin)
        clip_distance += 4;
    else if (y1 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    clip_distance = 0;
    v->sx = (float)x2;
    v->sy = (float)y2;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x2 < phd_winxmin)
        clip_distance = 1;
    else if (x2 > phd_winxmax)
        clip_distance = 2;
    if (y2 < phd_winymin)
        clip_distance += 4;
    else if (y2 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    clip_distance = 0;
    v->sx = (float)x3;
    v->sy = (float)y3;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x3 < phd_winxmin)
        clip_distance = 1;
    else if (x3 > phd_winxmax)
        clip_distance = 2;
    if (y3 < phd_winymin)
        clip_distance += 4;
    else if (y3 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    v->sx = (float)x4;
    v->sy = (float)y4;
    v->sz = (float)z;
    v->rhw = zv * f_moneopersp;
    clip_distance = 0;
    if (x4 < phd_winxmin)
        clip_distance = 1;
    else if (x4 > phd_winxmax)
        clip_distance = 2;
    if (y4 < phd_winymin)
        clip_distance += 4;
    else if (y4 > phd_winymax)
        clip_distance += 8;
    *clip = clip_distance;
}

void setXY3(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 x3, sint32 y3, sint32 z, sint16 *clip)
{
    float zv;
    sint32 clip_distance;

    clip_distance = 0;
    zv = f_mpersp / (float)z;
    v->sx = (float)x1;
    v->sy = (float)y1;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x1 < phd_winxmin)
        clip_distance = 1;
    else if (x1 > phd_winxmax)
        clip_distance = 2;
    if (y1 < phd_winymin)
        clip_distance += 4;
    else if (y1 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    clip_distance = 0;
    v->sx = (float)x2;
    v->sy = (float)y2;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x2 < phd_winxmin)
        clip_distance = 1;
    else if (x2 > phd_winxmax)
        clip_distance = 2;
    if (y2 < phd_winymin)
        clip_distance += 4;
    else if (y2 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    v->sx = (float)x3;
    v->sy = (float)y3;
    v->sz = (float)z;
    v->rhw = zv * f_moneopersp;
    clip_distance = 0;
    if (x3 < phd_winxmin)
        clip_distance = 1;
    else if (x3 > phd_winxmax)
        clip_distance = 2;
    if (y3 < phd_winymin)
        clip_distance += 4;
    else if (y3 > phd_winymax)
        clip_distance += 8;
    *clip = clip_distance;
}

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

uint8 ClipLine(sint32 *x1, sint32 *y1, sint32 z1, sint32 *x2, sint32 *y2, sint32 z2, sint32 xMin, sint32 yMin, sint32 w, sint32 h)
{
    float clip;

    if (z1 < 20 || z2 < 20)
        return 0;
    if (*x1 < xMin && *x2 < xMin || *y1 < yMin && *y2 < yMin)
        return 0;
    if (*x1 > w && *x2 > w || *y1 > h && *y2 > h)
        return 0;
    if (*x1 > w) {
        clip = ((float)w - *x2) / (float)(*x1 - *x2);
        *x1 = w;
        *y1 = (sint32)((*y1 - *y2) * clip + *y2);
    }
    if (*x2 > w) {
        clip = ((float)w - *x1) / (float)(*x2 - *x1);
        *x2 = w;
        *y2 = (sint32)((*y2 - *y1) * clip + *y1);
    }
    if (*x1 < xMin) {
        clip = ((float)xMin - *x1) / (float)(*x2 - *x1);
        *x1 = xMin;
        *y1 = (sint32)((*y2 - *y1) * clip + *y1);
    }
    if (*x2 < xMin) {
        clip = ((float)xMin - *x2) / (float)(*x1 - *x2);
        *x2 = xMin;
        *y2 = (sint32)((*y1 - *y2) * clip + *y2);
    }
    if (*y1 > h) {
        clip = ((float)h - *y2) / (float)(*y1 - *y2);
        *y1 = h;
        *x1 = (sint32)((*x1 - *x2) * clip + *x2);
    }
    if (*y2 > h) {
        clip = ((float)h - *y1) / (float)(*y2 - *y1);
        *y2 = h;
        *x2 = (sint32)((*x2 - *x1) * clip + *x1);
    }
    if (*y1 < yMin) {
        clip = ((float)yMin - *y1) / (float)(*y2 - *y1);
        *y1 = yMin;
        *x1 = (sint32)((*x2 - *x1) * clip + *x1);
    }
    if (*y2 < yMin) {
        clip = ((float)yMin - *y2) / (float)(*y1 - *y2);
        *y2 = yMin;
        *x2 = (sint32)((*x1 - *x2) * clip + *x2);
    }
    return 1;
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

void OutputSky(void)
{
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZENABLE, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 0);
    DrawBuckets();
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZENABLE, 1);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 1);
    SortPolyList(SortCount, SortList);
    RestoreFPCW(FPCW);
    SW_SetZLimit(0);
    DrawSortList();
    SW_SetZLimit(0.6F);
    MungeFPCW(&FPCW);
    InitBuckets();
    InitialiseSortList();
}

void DrawTrainFloorStrip(sint32 x, sint32 z, TEXTURESTRUCT *tex, sint32 y_and_flags)
{
    SVECTOR *offsets;
    D3DTLVERTEX v[4];
    PHD_VECTOR p1, p2, p3;
    sint32 *mptr;
    sint32 *Z;
    sint16 *XY;
    sint32 i, j, z1, z2, z3, z4, spec;
    sint16 num, x1, y1, x2, y2, x3, y3, x4, y4;

    mptr = phd_mxptr;
    num = 0;
    offsets = (SVECTOR *)&scratchpad[984];
    offsets[0].z = (sint16)z;
    offsets[1].z = (sint16)(z + 512);
    offsets[2].z = (sint16)(z + 1024);
    if (y_and_flags & 0x1000000) {
        offsets[1].z += 1024;
        offsets[2].z += 2048;
    }
    offsets[0].y = ((y_and_flags >> 16) & 0xFF) << 4;
    offsets[1].y = ((y_and_flags >> 8) & 0xFF) << 4;
    offsets[2].y = (y_and_flags & 0xFF) << 4;
    offsets[0].x = (sint16)x;
    offsets[1].x = (sint16)x;
    offsets[2].x = (sint16)x;
    for (i = 2; i > 0; i--) {
        XY = (sint16 *)&scratchpad[0];
        Z = (sint32 *)&scratchpad[492];
        XY -= 6;
        Z -= 3;
        for (j = 41; j > 0; j--) {
            p1.x = (offsets[0].x * mptr[M00] + offsets[0].y * mptr[M01] + offsets[0].z * mptr[M02] + mptr[M03]) >> W2V_SHIFT;
            p1.y = (offsets[0].x * mptr[M10] + offsets[0].y * mptr[M11] + offsets[0].z * mptr[M12] + mptr[M13]) >> W2V_SHIFT;
            p1.z = (offsets[0].x * mptr[M20] + offsets[0].y * mptr[M21] + offsets[0].z * mptr[M22] + mptr[M23]) >> W2V_SHIFT;
            p2.x = (offsets[1].x * mptr[M00] + offsets[1].y * mptr[M01] + offsets[1].z * mptr[M02] + mptr[M03]) >> W2V_SHIFT;
            p2.y = (offsets[1].x * mptr[M10] + offsets[1].y * mptr[M11] + offsets[1].z * mptr[M12] + mptr[M13]) >> W2V_SHIFT;
            p2.z = (offsets[1].x * mptr[M20] + offsets[1].y * mptr[M21] + offsets[1].z * mptr[M22] + mptr[M23]) >> W2V_SHIFT;
            p3.x = (offsets[2].x * mptr[M00] + offsets[2].y * mptr[M01] + offsets[2].z * mptr[M02] + mptr[M03]) >> W2V_SHIFT;
            p3.y = (offsets[2].x * mptr[M10] + offsets[2].y * mptr[M11] + offsets[2].z * mptr[M12] + mptr[M13]) >> W2V_SHIFT;
            p3.z = (offsets[2].x * mptr[M20] + offsets[2].y * mptr[M21] + offsets[2].z * mptr[M22] + mptr[M23]) >> W2V_SHIFT;
            offsets[0].x += 512;
            offsets[1].x += 512;
            offsets[2].x += 512;
            XY += 6;
            Z += 3;
            XY[0] = (sint16)p1.x;
            XY[1] = (sint16)p1.y;
            Z[0] = p1.z;
            XY[2] = (sint16)p2.x;
            XY[3] = (sint16)p2.y;
            Z[1] = p2.z;
            XY[4] = (sint16)p3.x;
            XY[5] = (sint16)p3.y;
            Z[2] = p3.z;
        }
        offsets[0].x -= 512;
        offsets[1].x -= 512;
        offsets[2].x -= 512;
        XY = (sint16 *)&scratchpad[0];
        Z = (sint32 *)&scratchpad[492];
        for (j = num; j < num + 20; j++, XY += 12, Z += 6) {
            z1 = Z[0];
            z2 = Z[2];
            z3 = Z[6];
            z4 = Z[8];
            if (!(z1 | z2 | z3 | z4))
                continue;
            x1 = XY[0];
            y1 = XY[1];
            x2 = XY[4];
            y2 = XY[5];
            x3 = XY[12];
            y3 = XY[13];
            x4 = XY[16];
            y4 = XY[17];
            if (j < 7)
                spec = (j + 1) * 0x101010;
            else if (j < 33)
                spec = 0x808080;
            else
                spec = (40 - j) * 0x101010;
            setXYZ4(v, x1, y1, z1, x2, y2, z2, x4, y4, z4, x3, y3, z3, clipflags);
            spec = ((spec & 0xFF) - 1) << 25;
            v[0].color = 0xFFFFFFFF;
            v[1].color = 0xFFFFFFFF;
            v[2].color = 0xFFFFFFFF;
            v[3].color = 0xFFFFFFFF;
            v[0].specular = spec;
            v[1].specular = spec;
            v[2].specular = spec;
            v[3].specular = spec;
            AddQuadSorted(v, 0, 1, 2, 3, tex, 0);
        }
        num += 20;
    }
}

void DrawTrainStrips(void)
{
    sint16 *ar;

    ar = &aranges[2];
    DrawTrainFloorStrip(-20480, -5120, &textinfo[ar[5]], 0x1101010);
    DrawTrainFloorStrip(-20480, 3072, &textinfo[ar[5]], 0x1101010);
    DrawTrainFloorStrip(-20480, -2048, &textinfo[ar[3]], 0x100800);
    DrawTrainFloorStrip(-20480, 2048, &textinfo[ar[4]], 0x810);
    DrawTrainFloorStrip(-20480, -1024, &textinfo[ar[1]], 0);
    DrawTrainFloorStrip(-20480, 1024, &textinfo[ar[2]], 0);
    DrawTrainFloorStrip(-20480, 0, &textinfo[ar[0]], 0);
}

void DrawSprite(sint32 x, sint32 y, sint32 slot, sint32 col, sint32 size, sint32 z)
{
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    sint32 s;

    s = (sint32)((float)(phd_winwidth / 640.0F) * (size << 1));
    if (z)
        setXY4(v, x - s, y - s, s + x, y - s, x - s, s + y, s + x, s + y, (sint32)(z + f_mznear), clipflags);
    else
        setXY4(v, x - s, y - s, s + x, y - s, x - s, s + y, s + x, s + y, (sint32)f_mzfar, clipflags);
    sprite = &spriteinfo[slot + objects[DEFAULT_SPRITES].mesh_index];
    v[0].specular = 0xFF000000;
    v[3].specular = 0xFF000000;
    v[2].specular = 0xFF000000;
    v[1].specular = 0xFF000000;
    CalcColorSplit(col, &v[0].color);
    v[3].color = v[0].color | 0xFF000000;
    v[2].color = v[0].color | 0xFF000000;
    v[1].color = v[0].color | 0xFF000000;
    tex.drawtype = 2;
    tex.flag = 0;
    tex.tpage = sprite->tpage;
    tex.u1 = sprite->x1;
    tex.v1 = sprite->y1;
    tex.u2 = sprite->x2;
    tex.v2 = sprite->y1;
    tex.u3 = sprite->x2;
    tex.v3 = sprite->y2;
    tex.u4 = sprite->x1;
    tex.v4 = sprite->y2;
    AddQuadSorted(v, 0, 1, 3, 2, &tex, 0);
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

void DrawPsxTile(sint32 x_y, sint32 height_width, sint32 color, sint32 u0, sint32 u1)
{
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    float x, y, z, rhw, w, h;
    sint32 col;
    sint32 drawtype;
    sint32 alpha;

    x = (float)((uint32)x_y >> 16);
    y = (float)(x_y & 0xFFFF);
    h = (float)((uint32)height_width >> 16) + 1;
    w = (float)(height_width & 0xFFFF) + 1;
    nPolyType = 6;
    alpha = color & 0xFF000000;
    if (alpha == 0x62000000) {
        drawtype = 3;
        col = color << 24;
    } else {
        drawtype = 2;
        col = color | 0xFF000000;
    }
    z = f_znear;
    rhw = f_moneoznear;
    if (!gfCurrentLevel) {
        z = f_znear + 10;
        rhw = f_moneoznear + 50;
    }
    v[0].sx = x;
    v[0].sy = y;
    v[0].sz = z;
    v[0].rhw = rhw;
    v[0].color = col;
    v[0].specular = 0xFF000000;
    v[1].sx = x + w;
    v[1].sy = y;
    v[1].sz = z;
    v[1].rhw = rhw;
    v[1].color = col;
    v[1].specular = 0xFF000000;
    v[2].sx = x + w;
    v[2].sy = y + h;
    v[2].sz = z;
    v[2].rhw = rhw;
    v[2].color = col;
    v[2].specular = 0xFF000000;
    v[3].sx = x;
    v[3].sy = y + h;
    v[3].sz = z;
    v[3].rhw = rhw;
    v[3].color = col;
    v[3].specular = 0xFF000000;
    tex.drawtype = (uint16)drawtype;
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

void DrawBikeSpeedo(sint32 ux, sint32 uy, sint32 vel, sint32 maxVel, sint32 turboVel, sint32 size, sint32 unk)
{
    D3DTLVERTEX v[2];
    sint32 x0, y0, x1, y1;
    sint32 rSize, i;

    ux = (sint32)(448.0F * ((float)phd_winxmax * (1.0F / 512.0F)));
    uy = (sint32)(224.0F * ((float)phd_winymax * (1.0F / 240.0F)));
    rSize = (7 * size) >> 3;
    vel = abs(vel >> 1);
    if (vel) {
        vel += (((rcossin_tbl[(((GlobalCounter & 7) << 13) >> 3) & 0x1FFE] << 2) * ((vel - 4096) >> 5)) >> W2V_SHIFT);
        if (vel < 0)
            vel = 0;
    }
    maxVel >>= 1;
    turboVel >>= 1;
    for (i = -0x4000; i + 0x4000 <= turboVel; i += 2048) {
        x0 = (rSize * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x1 = (size * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y0 = (-(rSize * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y1 = (-(size * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        v[0].sx = (float)(ux + x0);
        v[0].sy = (float)(uy + y0);
        v[0].sz = f_mznear;
        v[0].rhw = f_moneoznear;
        v[1].sx = (float)(ux + x1);
        v[1].sy = (float)(uy + y1);
        v[1].sz = f_mznear;
        v[1].rhw = f_moneoznear;
        v[0].color = v[1].color = i + 0x4000 > maxVel ? 0xFFFF0000 : 0xFFFFFFFF;
        v[1].specular = v[0].specular;
        AddLineSorted(v, &v[1], 6);
    }
    size -= size >> 4;
    x1 = (size * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (-4 * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y0 = (-(-4 * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y1 = (-(size * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    v[0].sx = (float)(ux + x0);
    v[0].sy = (float)(uy + y0);
    v[0].sz = f_mznear;
    v[0].rhw = f_moneoznear;
    v[1].sx = (float)(ux + x1);
    v[1].sy = (float)(uy + y1);
    v[1].sz = f_mznear;
    v[1].rhw = f_moneoznear;
    v[1].color = v[0].color;
    v[1].specular = v[0].specular;
    AddLineSorted(v, &v[1], 6);
}

void S_DrawDarts(ITEM_INFO *item)
{
    D3DTLVERTEX v[2];
    PHD_VECTOR pos;
    sint32 x1, y1, z1, x2, y2, z2, num;
    sint16 XY[2];
    sint16 sx, sy, mxx, mxy, mxz;
    float zv;

    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    pos.z = phd_mxptr[M23];
    pos.x = phd_mxptr[M03];
    pos.y = phd_mxptr[M13];
    zv = f_persp / (float)pos.z;
    XY[0] = (sint16)(pos.x * zv + f_centerx);
    XY[1] = (sint16)(pos.y * zv + f_centery);
    z1 = pos.z >> W2V_SHIFT;
    num = (-96 * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
    mxx = ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * num) >> W2V_SHIFT;
    mxy = (96 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
    mxz = ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * num) >> W2V_SHIFT;
    pos.x = phd_mxptr[M00] * mxx + phd_mxptr[M01] * mxy + phd_mxptr[M02] * mxz + phd_mxptr[M03];
    pos.y = phd_mxptr[M10] * mxx + phd_mxptr[M11] * mxy + phd_mxptr[M12] * mxz + phd_mxptr[M13];
    pos.z = phd_mxptr[M20] * mxx + phd_mxptr[M21] * mxy + phd_mxptr[M22] * mxz + phd_mxptr[M23];
    zv = f_persp / (float)pos.z;
    sx = (sint16)(pos.x * zv + f_centerx);
    sy = (sint16)(pos.y * zv + f_centery);
    z2 = pos.z >> W2V_SHIFT;
    x1 = XY[0];
    y1 = XY[1];
    x2 = sx;
    y2 = sy;
    if (ClipLine(&x1, &y1, z1, &x2, &y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax)) {
        v[0].color = 0xFF000000;
        v[0].specular = 0xFF000000;
        v[1].color = 0xFF783C14;
        v[1].specular = 0xFF000000;
        v[0].sx = (float)x1;
        v[0].sy = (float)y1;
        v[1].sx = (float)x2;
        v[1].sy = (float)y2;
        zv = f_mpersp / (float)z1 * f_moneopersp;
        v[0].rhw = zv;
        v[1].rhw = zv;
        zv = f_a - zv * f_boo;
        v[0].sz = zv;
        v[1].sz = zv;
        AddLineSorted(v, &v[1], 6);
    }
    phd_PopMatrix();
}

void Draw2DSprite(sint32 x, sint32 y, sint32 slot, sint32 unused, sint32 unused2)
{
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    sint32 w;
    sint32 h;
    float u1;
    float v1;
    float u2;
    float v2;

    sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + slot];
    w = (sint32)((float)(sprite->width >> 8) * phd_centerx * (1.0F / 320.0F));
    h = (sint32)((float)(sprite->height >> 8) * phd_centery * (1.0F / 240.0F));
    setXY4(v, (sint32)(float)x, (sint32)(float)y, (sint32)(float)(x + w + 1), (sint32)(float)y, (sint32)(float)(x + w + 1), (sint32)(float)(h + y + 1), (sint32)(float)x, (sint32)(float)(h + y + 1), (sint32)f_mznear, clipflags);
    v[0].specular = 0xFF000000;
    v[1].specular = 0xFF000000;
    v[2].specular = 0xFF000000;
    v[3].specular = 0xFF000000;
    v[0].color = 0xFFFFFFFF;
    v[1].color = 0xFFFFFFFF;
    v[2].color = 0xFFFFFFFF;
    v[3].color = 0xFFFFFFFF;
    u1 = sprite->x1;
    v1 = sprite->y1;
    u2 = sprite->x2;
    v2 = sprite->y2;
    tex.drawtype = 1;
    tex.flag = 0;
    tex.tpage = sprite->tpage;
    tex.u1 = u1;
    tex.v1 = v1;
    tex.u2 = u2;
    tex.v2 = v1;
    tex.u3 = u2;
    tex.v3 = v2;
    tex.u4 = u1;
    tex.v4 = v2;
    AddQuadClippedSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawJeepSpeedo(sint32 ux, sint32 uy, sint32 vel, sint32 maxVel, sint32 turboVel, sint32 size, sint32 spriteSlot)
{
    D3DTLVERTEX v[2];
    sint32 x0, y0, x1, y1;
    sint32 rSize, i;

    ux = (sint32)(448.0F * ((float)phd_winxmax * (1.0F / 512.0F)));
    uy = (sint32)(224.0F * ((float)phd_winymax * (1.0F / 240.0F)));
    Draw2DSprite(ux + 24, uy - 16, spriteSlot + 17, RGBONLY(GetRandomDraw() & 0xFF, GetRandomDraw() & 0xFF, GetRandomDraw() & 0xFF), 0);
    rSize = (7 * size) >> 3;
    vel = abs(vel >> 1);
    if (vel) {
        vel += (((rcossin_tbl[(((GlobalCounter & 7) << 13) >> 3) & 0x1FFE] << 2) * ((vel - 4096) >> 5)) >> W2V_SHIFT);
        if (vel < 0)
            vel = 0;
    }
    maxVel >>= 1;
    turboVel >>= 1;
    for (i = -0x4000; i + 0x4000 <= turboVel; i += 2048) {
        x0 = (rSize * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x1 = (size * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y0 = (-(rSize * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y1 = (-(size * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        v[0].sx = (float)(ux + x0);
        v[0].sy = (float)(uy + y0);
        v[0].sz = f_mznear;
        v[0].rhw = f_moneoznear;
        v[1].sx = (float)(ux + x1);
        v[1].sy = (float)(uy + y1);
        v[1].sz = f_mznear;
        v[1].rhw = f_moneoznear;
        v[0].color = v[1].color = i + 0x4000 > maxVel ? 0xFFFF0000 : 0xFFFFFFFF;
        v[1].specular = v[0].specular;
        AddLineSorted(v, &v[1], 6);
    }
    size -= size >> 4;
    x1 = (size * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (-4 * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y0 = (-(-4 * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y1 = (-(size * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    v[0].sx = (float)(ux + x0);
    v[0].sy = (float)(uy + y0);
    v[0].sz = f_mznear;
    v[0].rhw = f_moneoznear;
    v[1].sx = (float)(ux + x1);
    v[1].sy = (float)(uy + y1);
    v[1].sz = f_mznear;
    v[1].rhw = f_moneoznear;
    v[1].color = v[0].color;
    v[1].specular = v[0].specular;
    AddLineSorted(v, &v[1], 6);
}
