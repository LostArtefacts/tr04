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
#include "lighting.h"
#include "loadsave.h"
#include "../tomb4fx.h"
#include "../camera.h"
#include "../health.h"
#include "../deltapak.h"
#include "../draw.h"
#include "../lara.h"

#define RGB_M(clr, m) (col = (clr), cr = (CLRR(col) * (m)) >> 8, cg = (CLRG(col) * (m)) >> 8, cb = (CLRB(col) * (m)) >> 8, clr = RGBONLY(cr, cg, cb) | (col & 0xFF000000))

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

void j_nullsub_9(void)
{
    nullsub_9();
}

void nullsub_9(void)
{
}

void phd_PutPolygons(sint16 *objptr, sint32 clip)
{
    MESH_DATA *mesh;
    SPRITESTRUCT *envmap_sprite;
    D3DTLVERTEX *v;
    TEXTURESTRUCT *pTex;
    D3DVECTOR n0, n1, n2, n3;
    TEXTURESTRUCT envmap_texture;
    sint16 *quad;
    sint16 *tri;
    float u1, v1, u2, v2, u3, v3, u4, v4;
    sint32 c0, c1, c2, c3, s0, s1, s2, s3;
    sint32 num, col, m, cr, cg, cb;
    uint16 drawbak;
    uint8 envmap;
    sint32 i;

    SetD3DViewMatrix();
    mesh = (MESH_DATA *)objptr;
    if (mesh) {
        if ((sint16 *)mesh != meshes[objects[LARA_DOUBLE].mesh_index])
            envmap_sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];
        else
            envmap_sprite = &spriteinfo[objects[SKY_GRAPHICS].mesh_index];
        if (GlobalAmbient) {
            ClearObjectLighting();
            ClearDynamicLighting();
            IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_AMBIENT, GlobalAmbient);
            GlobalAmbient = 0;
        } else if (!mesh->prelight)
            InitObjectLighting(current_item);
        else {
            ClearObjectLighting();
            InitItemDynamicLighting(current_item);
        }
        if (mesh->nVerts)
            DXAttempt(IDirect3DVertexBuffer_ProcessVertices(DestVB, D3DVOP_LIGHT | D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));
        IDirect3DVertexBuffer_Lock(DestVB, DDLOCK_READONLY, (void **)&v, 0);
        clip_left = f_left;
        clip_top = f_top;
        clip_right = f_right;
        clip_bottom = f_bottom;
        if (mesh->nVerts) {
            bWaterEffect = camera.underwater != 0;
            ProjectVerts(mesh->nVerts, v, clipflags);
            if (mesh->prelight)
                PrelightVerts(mesh->nVerts, v, mesh);
            else
                CalcVertsColorSplitMMX(mesh->nVerts, v);
        }
        quad = mesh->gt4;
        for (i = 0; i < mesh->ngt4; i++, quad += 6) {
            pTex = &textinfo[quad[4] & 0x7FFF];
            envmap = 0;
            drawbak = pTex->drawtype;
            if (quad[5] & 1)
                pTex->drawtype = 2;
            if (quad[5] & 2) {
                envmap = 1;
                num = (quad[5] >> 2) & 0x1F;
                n0 = ((D3DVECTOR *)mesh->Normals)[quad[0]];
                n1 = ((D3DVECTOR *)mesh->Normals)[quad[1]];
                n2 = ((D3DVECTOR *)mesh->Normals)[quad[2]];
                n3 = ((D3DVECTOR *)mesh->Normals)[quad[3]];
                D3DTransform(&n0, &D3DMView);
                D3DTransform(&n1, &D3DMView);
                D3DTransform(&n2, &D3DMView);
                D3DTransform(&n3, &D3DMView);
                n0.x *= 0.125F;
                n0.y *= 0.125F;
                n0.z *= 0.125F;
                n1.x *= 0.125F;
                n1.y *= 0.125F;
                n1.z *= 0.125F;
                n2.x *= 0.125F;
                n2.y *= 0.125F;
                n2.z *= 0.125F;
                n3.x *= 0.125F;
                n3.y *= 0.125F;
                n3.z *= 0.125F;
                u1 = n0.x + envmap_sprite->x1 + 0.125F;
                v1 = n0.y + envmap_sprite->y1 + 0.125F;
                u2 = n1.x + envmap_sprite->x1 + 0.125F;
                v2 = n1.y + envmap_sprite->y1 + 0.125F;
                u3 = n2.x + envmap_sprite->x1 + 0.125F;
                v3 = n2.y + envmap_sprite->y1 + 0.125F;
                u4 = n3.x + envmap_sprite->x1 + 0.125F;
                v4 = n3.y + envmap_sprite->y1 + 0.125F;
                envmap_texture.drawtype = 2;
                envmap_texture.flag = pTex->flag;
                envmap_texture.tpage = envmap_sprite->tpage;
                envmap_texture.u1 = u1;
                envmap_texture.v1 = v1;
                envmap_texture.u2 = u2;
                envmap_texture.v2 = v2;
                envmap_texture.u3 = u3;
                envmap_texture.v3 = v3;
                envmap_texture.u4 = u4;
                envmap_texture.v4 = v4;
            }
            if (GlobalAlpha == (sint32)0xFF000000) {
                switch (pTex->drawtype) {
                case 1:
                case 2:
                    AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
                    break;
                case 0:
                    AddQuadZBuffer(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
                    break;
                }
                if (envmap) {
                    c0 = v[quad[0]].color;
                    c1 = v[quad[1]].color;
                    c2 = v[quad[2]].color;
                    c3 = v[quad[3]].color;
                    s0 = v[quad[0]].specular;
                    s1 = v[quad[1]].specular;
                    s2 = v[quad[2]].specular;
                    s3 = v[quad[3]].specular;
                    m = num * 8;
                    RGB_M(v[quad[0]].color, m);
                    RGB_M(v[quad[1]].color, m);
                    RGB_M(v[quad[2]].color, m);
                    RGB_M(v[quad[3]].color, m);
                    RGB_M(v[quad[0]].specular, m);
                    RGB_M(v[quad[1]].specular, m);
                    RGB_M(v[quad[2]].specular, m);
                    RGB_M(v[quad[3]].specular, m);
                    AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], &envmap_texture, 0);
                    v[quad[0]].color = c0;
                    v[quad[1]].color = c1;
                    v[quad[2]].color = c2;
                    v[quad[3]].color = c3;
                    v[quad[0]].specular = s0;
                    v[quad[1]].specular = s1;
                    v[quad[2]].specular = s2;
                    v[quad[3]].specular = s3;
                }
            } else {
                pTex->drawtype = 7;
                AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
            }
            pTex->drawtype = drawbak;
        }
        tri = mesh->gt3;
        for (i = 0; i < mesh->ngt3; i++, tri += 5) {
            pTex = &textinfo[tri[3] & 0x7FFF];
            envmap = 0;
            drawbak = pTex->drawtype;
            if (tri[4] & 1)
                pTex->drawtype = 2;
            if (tri[4] & 2) {
                envmap = 1;
                num = (tri[4] >> 2) & 0x1F;
                n0 = ((D3DVECTOR *)mesh->Normals)[tri[0]];
                n1 = ((D3DVECTOR *)mesh->Normals)[tri[1]];
                n2 = ((D3DVECTOR *)mesh->Normals)[tri[2]];
                D3DTransform(&n0, &D3DMView);
                D3DTransform(&n1, &D3DMView);
                D3DTransform(&n2, &D3DMView);
                n0.x *= 0.125F;
                n0.y *= 0.125F;
                n0.z *= 0.125F;
                n1.x *= 0.125F;
                n1.y *= 0.125F;
                n1.z *= 0.125F;
                n2.x *= 0.125F;
                n2.y *= 0.125F;
                n2.z *= 0.125F;
                u1 = n0.x + envmap_sprite->x1 + 0.125F;
                v1 = n0.y + envmap_sprite->y1 + 0.125F;
                u2 = n1.x + envmap_sprite->x1 + 0.125F;
                v2 = n1.y + envmap_sprite->y1 + 0.125F;
                u3 = n2.x + envmap_sprite->x1 + 0.125F;
                v3 = n2.y + envmap_sprite->y1 + 0.125F;
                envmap_texture.drawtype = 2;
                envmap_texture.flag = pTex->flag;
                envmap_texture.tpage = envmap_sprite->tpage;
                envmap_texture.u1 = u1;
                envmap_texture.v1 = v1;
                envmap_texture.u2 = u2;
                envmap_texture.v2 = v2;
                envmap_texture.u3 = u3;
                envmap_texture.v3 = v3;
            }
            if (GlobalAlpha == (sint32)0xFF000000) {
                switch (pTex->drawtype) {
                case 1:
                case 2:
                    AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
                    break;
                case 0:
                    AddTriZBuffer(v, tri[0], tri[1], tri[2], pTex, 0);
                    break;
                }
                if (envmap) {
                    c0 = v[tri[0]].color;
                    c1 = v[tri[1]].color;
                    c2 = v[tri[2]].color;
                    s0 = v[tri[0]].specular;
                    s1 = v[tri[1]].specular;
                    s2 = v[tri[2]].specular;
                    m = num * 8;
                    RGB_M(v[tri[0]].color, m);
                    RGB_M(v[tri[1]].color, m);
                    RGB_M(v[tri[2]].color, m);
                    RGB_M(v[tri[0]].specular, m);
                    RGB_M(v[tri[1]].specular, m);
                    RGB_M(v[tri[2]].specular, m);
                    AddTriSorted(v, tri[0], tri[1], tri[2], &envmap_texture, 0);
                    v[tri[0]].color = c0;
                    v[tri[1]].color = c1;
                    v[tri[2]].color = c2;
                    v[tri[0]].specular = s0;
                    v[tri[1]].specular = s1;
                    v[tri[2]].specular = s2;
                }
            } else {
                pTex->drawtype = 7;
                AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
            }
            pTex->drawtype = drawbak;
        }
        IDirect3DVertexBuffer_Unlock(DestVB);
    }
}

void phd_PutPolygonsPickup(sint16 *objptr, float x, float y, sint32 color)
{
    MESH_DATA *mesh;
    SPRITESTRUCT *envmap_sprite;
    D3DTLVERTEX *v;
    TEXTURESTRUCT *pTex;
    D3DVECTOR n0, n1, n2, n3;
    TEXTURESTRUCT envmap_texture;
    sint16 *quad;
    sint16 *tri;
    float fcx, fcy, u1, v1, u2, v2, u3, v3, u4, v4;
    sint32 c0, c1, c2, c3, s0, s1, s2, s3;
    sint32 num, col, m, cr, cg, cb;
    sint32 drawbak;
    uint8 envmap;
    sint32 i;

    bWaterEffect = 0;
    SetD3DViewMatrix();
    mesh = (MESH_DATA *)objptr;
    envmap_sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 11];
    ClearDynamicLighting();
    ClearObjectLighting();
    IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_AMBIENT, color);
    if (mesh->nVerts)
        DXAttempt(IDirect3DVertexBuffer_ProcessVertices(DestVB, D3DVOP_LIGHT | D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));
    IDirect3DVertexBuffer_Lock(DestVB, DDLOCK_READONLY, (void **)&v, 0);
    clip_left = f_left;
    clip_right = f_right;
    clip_bottom = f_bottom;
    clip_top = f_top;
    fcx = f_centerx;
    fcy = f_centery;
    f_centerx = x;
    f_centery = y;
    if (mesh->nVerts) {
        ProjectVerts(mesh->nVerts, v, clipflags);
        if (mesh->prelight)
            PrelightVerts(mesh->nVerts, v, mesh);
        else
            CalcVertsColorSplitMMX(mesh->nVerts, v);
    }
    f_centerx = fcx;
    f_centery = fcy;
    quad = mesh->gt4;
    for (i = 0; i < mesh->ngt4; i++, quad += 6) {
        pTex = &textinfo[quad[4] & 0x7FFF];
        envmap = 0;
        drawbak = pTex->drawtype;
        if (quad[5] & 1)
            pTex->drawtype = 2;
        if (quad[5] & 2) {
            envmap = 1;
            num = (quad[5] >> 2) & 0x1F;
            n0 = ((D3DVECTOR *)mesh->Normals)[quad[0]];
            n1 = ((D3DVECTOR *)mesh->Normals)[quad[1]];
            n2 = ((D3DVECTOR *)mesh->Normals)[quad[2]];
            n3 = ((D3DVECTOR *)mesh->Normals)[quad[3]];
            D3DTransform(&n0, &D3DMView);
            D3DTransform(&n1, &D3DMView);
            D3DTransform(&n2, &D3DMView);
            D3DTransform(&n3, &D3DMView);
            n0.x *= 0.125F;
            n0.y *= 0.125F;
            n0.z *= 0.125F;
            n1.x *= 0.125F;
            n1.y *= 0.125F;
            n1.z *= 0.125F;
            n2.x *= 0.125F;
            n2.y *= 0.125F;
            n2.z *= 0.125F;
            n3.x *= 0.125F;
            n3.y *= 0.125F;
            n3.z *= 0.125F;
            u1 = n0.x + envmap_sprite->x1 + 0.125F;
            v1 = n0.y + envmap_sprite->y1 + 0.125F;
            u2 = n1.x + envmap_sprite->x1 + 0.125F;
            v2 = n1.y + envmap_sprite->y1 + 0.125F;
            u3 = n2.x + envmap_sprite->x1 + 0.125F;
            v3 = n2.y + envmap_sprite->y1 + 0.125F;
            u4 = n3.x + envmap_sprite->x1 + 0.125F;
            v4 = n3.y + envmap_sprite->y1 + 0.125F;
            envmap_texture.drawtype = 2;
            envmap_texture.flag = pTex->flag;
            envmap_texture.tpage = envmap_sprite->tpage;
            envmap_texture.u1 = u1;
            envmap_texture.v1 = v1;
            envmap_texture.u2 = u2;
            envmap_texture.v2 = v2;
            envmap_texture.u3 = u3;
            envmap_texture.v3 = v3;
            envmap_texture.u4 = u4;
            envmap_texture.v4 = v4;
        }
        if (GlobalAlpha != (sint32)0xFF000000)
            pTex->drawtype = 3;
        AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
        if (envmap) {
            c0 = v[quad[0]].color;
            c1 = v[quad[1]].color;
            c2 = v[quad[2]].color;
            c3 = v[quad[3]].color;
            s0 = v[quad[0]].specular;
            s1 = v[quad[1]].specular;
            s2 = v[quad[2]].specular;
            s3 = v[quad[3]].specular;
            m = num * 8;
            RGB_M(v[quad[0]].color, m);
            RGB_M(v[quad[1]].color, m);
            RGB_M(v[quad[2]].color, m);
            RGB_M(v[quad[3]].color, m);
            RGB_M(v[quad[0]].specular, m);
            RGB_M(v[quad[1]].specular, m);
            RGB_M(v[quad[2]].specular, m);
            RGB_M(v[quad[3]].specular, m);
            AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], &envmap_texture, 0);
            v[quad[0]].color = c0;
            v[quad[1]].color = c1;
            v[quad[2]].color = c2;
            v[quad[3]].color = c3;
            v[quad[0]].specular = s0;
            v[quad[1]].specular = s1;
            v[quad[2]].specular = s2;
            v[quad[3]].specular = s3;
        }
        pTex->drawtype = drawbak;
    }
    tri = mesh->gt3;
    for (i = 0; i < mesh->ngt3; i++, tri += 5) {
        pTex = &textinfo[tri[3] & 0x7FFF];
        envmap = 0;
        drawbak = pTex->drawtype;
        if (tri[4] & 1)
            pTex->drawtype = 2;
        if (tri[4] & 2) {
            envmap = 1;
            num = (tri[4] >> 2) & 0x1F;
            n0 = ((D3DVECTOR *)mesh->Normals)[tri[0]];
            n1 = ((D3DVECTOR *)mesh->Normals)[tri[1]];
            n2 = ((D3DVECTOR *)mesh->Normals)[tri[2]];
            D3DTransform(&n0, &D3DMView);
            D3DTransform(&n1, &D3DMView);
            D3DTransform(&n2, &D3DMView);
            n0.x *= 0.125F;
            n0.y *= 0.125F;
            n0.z *= 0.125F;
            n1.x *= 0.125F;
            n1.y *= 0.125F;
            n1.z *= 0.125F;
            n2.x *= 0.125F;
            n2.y *= 0.125F;
            n2.z *= 0.125F;
            u1 = n0.x + envmap_sprite->x1 + 0.125F;
            v1 = n0.y + envmap_sprite->y1 + 0.125F;
            u2 = n1.x + envmap_sprite->x1 + 0.125F;
            v2 = n1.y + envmap_sprite->y1 + 0.125F;
            u3 = n2.x + envmap_sprite->x1 + 0.125F;
            v3 = n2.y + envmap_sprite->y1 + 0.125F;
            envmap_texture.drawtype = 2;
            envmap_texture.flag = pTex->flag;
            envmap_texture.tpage = envmap_sprite->tpage;
            envmap_texture.u1 = u1;
            envmap_texture.v1 = v1;
            envmap_texture.u2 = u2;
            envmap_texture.v2 = v2;
            envmap_texture.u3 = u3;
            envmap_texture.v3 = v3;
        }
        if (GlobalAlpha != (sint32)0xFF000000)
            pTex->drawtype = 3;
        AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
        if (envmap) {
            c0 = v[tri[0]].color;
            c1 = v[tri[1]].color;
            c2 = v[tri[2]].color;
            s0 = v[tri[0]].specular;
            s1 = v[tri[1]].specular;
            s2 = v[tri[2]].specular;
            m = num * 8;
            RGB_M(v[tri[0]].color, m);
            RGB_M(v[tri[1]].color, m);
            RGB_M(v[tri[2]].color, m);
            RGB_M(v[tri[0]].specular, m);
            RGB_M(v[tri[1]].specular, m);
            RGB_M(v[tri[2]].specular, m);
            AddTriSorted(v, tri[0], tri[1], tri[2], &envmap_texture, 0);
            v[tri[0]].color = c0;
            v[tri[1]].color = c1;
            v[tri[2]].color = c2;
            v[tri[0]].specular = s0;
            v[tri[1]].specular = s1;
            v[tri[2]].specular = s2;
        }
        pTex->drawtype = drawbak;
    }
    IDirect3DVertexBuffer_Unlock(DestVB);
}

void phd_PutPolygonSkyMesh(sint16 *objptr, sint32 clipstatus)
{
    D3DTLVERTEX *v;
    TEXTURESTRUCT *pTex;
    MESH_DATA *mesh;
    sint16 *quad;
    sint16 *tri;
    sint32 i;
    uint16 drawbak;

    SetD3DViewMatrix();
    mesh = (MESH_DATA *)objptr;
    DXAttempt(IDirect3DVertexBuffer_ProcessVertices(DestVB, D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));
    IDirect3DVertexBuffer_Lock(DestVB, DDLOCK_READONLY, (void **)&v, 0);
    clip_left = f_left;
    clip_right = f_right;
    clip_bottom = f_bottom;
    clip_top = f_top;
    ProjectVerts(mesh->nVerts, v, clipflags);
    quad = mesh->gt4;
    for (i = 0; i < mesh->ngt4; i++, quad += 6) {
        pTex = &textinfo[quad[4] & 0x7FFF];
        drawbak = pTex->drawtype;
        if (quad[5] & 1) {
            if (gfLevelFlags & GF_HORIZONCOLADD)
                pTex->drawtype = 2;
            else if (App.dx.lpZBuffer) {
                v[quad[0]].color = 0;
                v[quad[1]].color = 0;
                v[quad[2]].color = 0xFF000000;
                v[quad[3]].color = 0xFF000000;
                pTex->drawtype = 3;
            } else {
                v[quad[0]].color = 0;
                v[quad[1]].color = 0;
                v[quad[2]].color = 0;
                v[quad[3]].color = 0;
                pTex->drawtype = 0;
            }
        } else
            pTex->drawtype = 4;
        if (gfLevelFlags & GF_TRAIN || gfCurrentLevel == 5 || gfCurrentLevel == 6) {
            if (i < 16) {
                v[quad[0]].color = 0xFFFFFFFF;
                v[quad[1]].color = 0xFFFFFFFF;
                v[quad[2]].color = 0xFFFFFFFF;
                v[quad[3]].color = 0xFFFFFFFF;
                v[quad[0]].specular = 0x7F000000;
                v[quad[1]].specular = 0x7F000000;
                v[quad[2]].specular = 0;
                v[quad[3]].specular = 0;
            } else {
                v[quad[0]].color = 0xFFFFFFFF;
                v[quad[1]].color = 0xFFFFFFFF;
                v[quad[2]].color = 0xFFFFFFFF;
                v[quad[3]].color = 0xFFFFFFFF;
            }
        }
        AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
        pTex->drawtype = drawbak;
    }
    tri = mesh->gt3;
    for (i = 0; i < mesh->ngt3; i++, tri += 5) {
        pTex = &textinfo[tri[3] & 0x7FFF];
        drawbak = pTex->drawtype;
        pTex->drawtype = 4;
        AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
        pTex->drawtype = drawbak;
    }
    IDirect3DVertexBuffer_Unlock(DestVB);
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

void phd_PutPolygons_train(sint16 *objptr, sint32 x)
{
    MESH_DATA *mesh;
    D3DTLVERTEX *v;
    TEXTURESTRUCT *pTex;
    sint16 *quad;
    sint16 *tri;
    uint16 drawbak;

    phd_PushMatrix();
    phd_TranslateRel(x, 0, 0);
    SetD3DViewMatrix();
    mesh = (MESH_DATA *)objptr;
    phd_PopMatrix();
    if (objptr) {
        sint32 i;

        ClearObjectLighting();
        ClearDynamicLighting();
        IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_AMBIENT, 0xFFFFFF);
        if (mesh->nVerts)
            DXAttempt(IDirect3DVertexBuffer_ProcessVertices(DestVB, D3DVOP_LIGHT | D3DVOP_TRANSFORM, 0, mesh->nVerts, mesh->SourceVB, 0, App.dx._lpD3DDevice, 0));
        IDirect3DVertexBuffer_Lock(DestVB, DDLOCK_READONLY, (void **)&v, 0);
        clip_left = f_left;
        clip_right = f_right;
        clip_bottom = f_bottom;
        clip_top = f_top;
        if (mesh->nVerts)
            ProjectTrainVerts(mesh->nVerts, v, clipflags, x);
        quad = mesh->gt4;
        for (i = 0; i < mesh->ngt4; i++, quad += 6) {
            pTex = &textinfo[quad[4] & 0x7FFF];
            drawbak = pTex->drawtype;
            if (quad[5] & 1)
                pTex->drawtype = 2;
            switch (pTex->drawtype) {
            case 1:
            case 2:
                AddQuadSorted(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
                break;
            case 0:
                AddQuadZBuffer(v, quad[0], quad[1], quad[2], quad[3], pTex, 0);
                break;
            }
            pTex->drawtype = drawbak;
        }
        tri = mesh->gt3;
        for (i = 0; i < mesh->ngt3; i++, tri += 5) {
            pTex = &textinfo[tri[3] & 0x7FFF];
            drawbak = pTex->drawtype;
            if (tri[4] & 1)
                pTex->drawtype = 2;
            switch (pTex->drawtype) {
            case 1:
            case 2:
                AddTriSorted(v, tri[0], tri[1], tri[2], pTex, 0);
                break;
            case 0:
                AddTriZBuffer(v, tri[0], tri[1], tri[2], pTex, 0);
                break;
            }
            pTex->drawtype = drawbak;
        }
        IDirect3DVertexBuffer_Unlock(DestVB);
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
