#include <math.h>

#include "drawroom.h"
#include "polyinsert.h"
#include "3dmath.h"
#include "gamemain.h"
#include "../effect2.h"
#include "winmain.h"
#include "dxshell.h"
#include "output.h"
#include "file.h"
#include "function_table.h"
#include "../camera.h"
#include "../anim.h"
#include "function_stubs.h"
#include "d3dmatrix.h"
#include "../text.h"
#include "lighting.h"

void ProcessMeshData(sint32 num_meshes)
{
    MESH_DATA *mesh;
    D3DVERTEX *vtx;
    D3DVERTEXBUFFERDESC buf;
    sint16 *mesh_ptr;
    sint16 *last_mesh_ptr;
    sint32 lp, i, j;
    sint16 c;

    Log(2, "ProcessMeshData %d", num_meshes);
    num_level_meshes = num_meshes;
    mesh_vtxbuf = (MESH_DATA **)game_malloc(4 * num_meshes, 0);
    mesh_base = (sint16 *)malloc_ptr;
    last_mesh_ptr = 0;
    mesh = (MESH_DATA *)num_meshes;
    for (i = 0; i < num_meshes; i++) {
        mesh_ptr = meshes[i];
        if (last_mesh_ptr == mesh_ptr) {
            meshes[i] = (sint16 *)mesh;
            mesh_vtxbuf[i] = mesh;
        } else {
            last_mesh_ptr = mesh_ptr;
            mesh = (MESH_DATA *)game_malloc(sizeof(MESH_DATA), 0);
            memset(mesh, 0, sizeof(MESH_DATA));
            meshes[i] = (sint16 *)mesh;
            mesh_vtxbuf[i] = mesh;
            mesh->x = *mesh_ptr++;
            mesh->y = *mesh_ptr++;
            mesh->z = *mesh_ptr++;
            mesh->r = *mesh_ptr++;
            mesh->flags = *mesh_ptr++;
            mesh->nVerts = *mesh_ptr & 0xFF;
            if (!mesh->nVerts)
                lp = *mesh_ptr >> 8;
            mesh_ptr++;
            if (mesh->nVerts) {
                memset(&buf, 0, sizeof(buf));
                buf.dwNumVertices = mesh->nVerts;
                buf.dwSize = sizeof(D3DVERTEXBUFFERDESC);
                buf.dwCaps = 0;
                buf.dwFVF = D3DFVF_TEX1 | D3DFVF_NORMAL | D3DFVF_XYZ;
                DXAttempt(IDirect3D3_CreateVertexBuffer(App.dx.lpD3D, &buf, (LPDIRECT3DVERTEXBUFFER *)&mesh->SourceVB, 0, 0));
                IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB, DDLOCK_WRITEONLY, (void **)&vtx, 0);
                for (j = 0; j < mesh->nVerts; j++) {
                    vtx[j].x = *mesh_ptr++;
                    vtx[j].y = *mesh_ptr++;
                    vtx[j].z = *mesh_ptr++;
                }
                mesh->nNorms = *mesh_ptr++;
                if (!mesh->nNorms)
                    mesh->nNorms = mesh->nVerts;
                if (mesh->nNorms > 0) {
                    mesh->Normals = game_malloc(mesh->nNorms * sizeof(D3DVECTOR), 0);
                    for (j = 0; j < mesh->nVerts; j++) {
                        vtx[j].nx = *mesh_ptr++;
                        vtx[j].ny = *mesh_ptr++;
                        vtx[j].nz = *mesh_ptr++;
                        D3DNormalise((D3DVECTOR *)&vtx[j].nx);
                        ((D3DVECTOR *)mesh->Normals)[j].x = vtx[j].nx;
                        ((D3DVECTOR *)mesh->Normals)[j].y = vtx[j].ny;
                        ((D3DVECTOR *)mesh->Normals)[j].z = vtx[j].nz;
                    }
                    mesh->prelight = 0;
                    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
                } else {
                    mesh->Normals = 0;
                    mesh->prelight = (sint32 *)game_malloc(4 * mesh->nVerts, 0);
                    for (j = 0; j < mesh->nVerts; j++) {
                        c = 255 - (*mesh_ptr++ >> 5);
                        mesh->prelight[j] = RGBONLY(c, c, c);
                    }
                    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
                }
            } else
                mesh_ptr += 6 * lp + 1;
            mesh->ngt4 = *mesh_ptr++;
            if (mesh->ngt4) {
                mesh->gt4 = (sint16 *)game_malloc(12 * mesh->ngt4, 0);
                memcpy(mesh->gt4, mesh_ptr, 12 * mesh->ngt4);
                mesh_ptr += 6 * mesh->ngt4;
            }
            mesh->ngt3 = *mesh_ptr++;
            if (mesh->ngt3) {
                mesh->gt3 = (sint16 *)game_malloc(10 * mesh->ngt3, 0);
                memcpy(mesh->gt3, mesh_ptr, 10 * mesh->ngt3);
            }
        }
    }
    Log(2, "End ProcessMeshData");
}
#include "../draw.h"

void ProcessRoomData(ROOM_INFO *r)
{
    D3DVERTEX *vptr;
    D3DVERTEXBUFFERDESC vb;
    sint16 *data_ptr;
    sint16 *faces;
    sint16 *prelight;
    float intensity;
    sint32 nWaterVerts, nShoreVerts, nRestOfVerts, nLights, gt4cnt, gt3cnt, i;
    uint16 cR, cG, cB;

    data_ptr = r->data;
    r->nVerts = *data_ptr++;
    if (!r->nVerts) {
        r->num_lights = 0;
        r->SourceVB = 0;
        return;
    }
    data_ptr += r->nVerts * 6;
    r->FaceData = data_ptr;
    gt4cnt = *data_ptr++;
    data_ptr += gt4cnt * 5;
    gt3cnt = *data_ptr;
    r->gt3cnt = gt3cnt;
    r->gt4cnt = gt4cnt;
    r->verts = (D3DVECTOR *)game_malloc(sizeof(D3DVECTOR) * r->nVerts, 0);
    faces = (sint16 *)malloc(2 * r->nVerts);
    prelight = (sint16 *)malloc(2 * r->nVerts);
    data_ptr = r->data + 1;
    nWaterVerts = 0;
    for (i = 0; i < r->nVerts; i++) {
        if ((sint16)(data_ptr[4] & 0x2000)) {
            r->verts[nWaterVerts].x = (float)data_ptr[0];
            r->verts[nWaterVerts].y = (float)data_ptr[1];
            r->verts[nWaterVerts].z = (float)data_ptr[2];
            prelight[nWaterVerts] = data_ptr[5];
            faces[i] = (sint16)(nWaterVerts | 0x8000);
            nWaterVerts++;
        }
        data_ptr += 6;
    }
    data_ptr = r->data + 1;
    nShoreVerts = 0;
    for (i = 0; i < r->nVerts; i++) {
        if ((sint16)(data_ptr[4] & 0x4000) && !(sint16)(data_ptr[4] & 0x2000)) {
            r->verts[nShoreVerts + nWaterVerts].x = (float)data_ptr[0];
            r->verts[nShoreVerts + nWaterVerts].y = (float)data_ptr[1];
            r->verts[nShoreVerts + nWaterVerts].z = (float)data_ptr[2];
            prelight[nShoreVerts + nWaterVerts] = data_ptr[5];
            faces[i] = (sint16)(nShoreVerts + nWaterVerts);
            nShoreVerts++;
        }
        data_ptr += 6;
    }
    data_ptr = r->data + 1;
    nRestOfVerts = 0;
    for (i = 0; i < r->nVerts; i++) {
        if (!(sint16)(data_ptr[4] & 0x2000) && !(sint16)(data_ptr[4] & 0x4000)) {
            r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].x = (float)data_ptr[0];
            r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].y = (float)data_ptr[1];
            r->verts[nRestOfVerts + nShoreVerts + nWaterVerts].z = (float)data_ptr[2];
            prelight[nRestOfVerts + nShoreVerts + nWaterVerts] = data_ptr[5];
            faces[i] = (sint16)(nRestOfVerts + nShoreVerts + nWaterVerts);
            nRestOfVerts++;
        }
        data_ptr += 6;
    }
    data_ptr = r->FaceData + 1;
    r->nWaterVerts = nWaterVerts;
    r->nShoreVerts = nShoreVerts;
    for (i = 0; i < gt4cnt; i++) {
        if (faces[data_ptr[0]] & 0x8000 || faces[data_ptr[1]] & 0x8000 || faces[data_ptr[2]] & 0x8000 || faces[data_ptr[3]] & 0x8000) {
            data_ptr[4] |= 0x4000;
        }
        data_ptr[0] = faces[data_ptr[0]] & 0x7FFF;
        data_ptr[1] = faces[data_ptr[1]] & 0x7FFF;
        data_ptr[2] = faces[data_ptr[2]] & 0x7FFF;
        data_ptr[3] = faces[data_ptr[3]] & 0x7FFF;
        data_ptr += 5;
    }
    data_ptr++;
    for (i = 0; i < gt3cnt; i++) {
        data_ptr[0] = faces[data_ptr[0]] & 0x7FFF;
        data_ptr[1] = faces[data_ptr[1]] & 0x7FFF;
        data_ptr[2] = faces[data_ptr[2]] & 0x7FFF;
        data_ptr += 4;
    }
    free(faces);
    CreateVertexNormals(r);
    r->prelight = (sint32 *)game_malloc(4 * r->nVerts, 0);
    r->prelightwater = (sint32 *)game_malloc(4 * r->nVerts, 0);
    memset(&vb, 0, sizeof(vb));
    r->watercalc = 0;
    vb.dwNumVertices = r->nVerts;
    vb.dwSize = sizeof(D3DVERTEXBUFFERDESC);
    vb.dwCaps = 0;
    vb.dwFVF = D3DFVF_VERTEX;
    DXAttempt(IDirect3D3_CreateVertexBuffer(App.dx.lpD3D, &vb, (LPDIRECT3DVERTEXBUFFER *)&r->SourceVB, D3DDP_DONOTCLIP, 0));
    IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)r->SourceVB, DDLOCK_WRITEONLY, (void **)&vptr, 0);
    r->posx = (float)r->x;
    r->posy = (float)r->y;
    r->posz = (float)r->z;
    data_ptr = r->data + 1;
    for (i = 0; i < r->nVerts; i++) {
        vptr->x = (float)r->x + r->verts[i].x;
        vptr->y = (float)r->y + r->verts[i].y;
        vptr->z = (float)r->z + r->verts[i].z;
        vptr->nx = r->vnormals[i].x;
        vptr->ny = r->vnormals[i].y;
        vptr->nz = r->vnormals[i].z;
        cR = ((prelight[i] & 0x7C00) >> 10) << 3;
        cG = ((prelight[i] & 0x3E0) >> 5) << 3;
        cB = (prelight[i] & 0x1F) << 3;
        r->prelight[i] = RGBA(cR, cG, cB, 0xFF);
        cR = (uint16)(cR * water_color_R) >> 8;
        cG = (uint16)(cG * water_color_G) >> 8;
        cB = (uint16)(cB * water_color_B) >> 8;
        r->prelightwater[i] = RGBA(cR, cG, cB, 0xFF);
        vptr++;
        data_ptr += 6;
    }
    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)r->SourceVB);
    free(prelight);
    r->pclight = 0;
    if (r->num_lights) {
        r->pclight = (PCLIGHT_INFO *)game_malloc(sizeof(PCLIGHT_INFO) * r->num_lights, 0);
        nLights = 0;
        for (i = 0; i < r->num_lights; i++) {
            if (r->light[i].Type == LIGHT_FOG) {
                if (NumLevelFogBulbs >= 20) {
                    Log(1, "Fog Bulb Discarded - More Than %d", 20);
                    continue;
                }
                FogBulbs[NumLevelFogBulbs].inRange = 0;
                FogBulbs[NumLevelFogBulbs].density = r->light[i].r;
                FogBulbs[NumLevelFogBulbs].WorldPos.x = (float)r->light[i].x;
                FogBulbs[NumLevelFogBulbs].WorldPos.y = (float)r->light[i].y;
                FogBulbs[NumLevelFogBulbs].WorldPos.z = (float)r->light[i].z;
                FogBulbs[NumLevelFogBulbs].rad = r->light[i].Outer;
                FogBulbs[NumLevelFogBulbs].sqrad = SQUARE(r->light[i].Outer);
                FogBulbs[NumLevelFogBulbs].inv_sqrad = 1 / FogBulbs[NumLevelFogBulbs].sqrad;
                NumLevelFogBulbs++;
            } else {
                if (!r->light[i].r && !r->light[i].g && !r->light[i].b && r->light[i].Type == LIGHT_SPOT) {
                    continue;
                }
                r->pclight[nLights].r = r->light[i].r * (1.0F / 255.0F);
                r->pclight[nLights].g = r->light[i].g * (1.0F / 255.0F);
                r->pclight[nLights].b = r->light[i].b * (1.0F / 255.0F);
                intensity = r->light[nLights].Intensity;
                if (intensity < 0) {
                    intensity = -intensity;
                }
                intensity *= 1.0F / 8191.0F;
                r->pclight[nLights].r *= intensity;
                r->pclight[nLights].g *= intensity;
                r->pclight[nLights].b *= intensity;
                if (r->light[nLights].Type == LIGHT_SHADOW) {
                    r->pclight[nLights].shadow = (sint32)(intensity * 255);
                }
                r->pclight[nLights].x = (float)r->light[i].x;
                r->pclight[nLights].y = (float)r->light[i].y;
                r->pclight[nLights].z = (float)r->light[i].z;
                r->pclight[nLights].ix = r->light[i].x;
                r->pclight[nLights].iy = r->light[i].y;
                r->pclight[nLights].iz = r->light[i].z;
                r->pclight[nLights].nx = -r->light[i].nx;
                r->pclight[nLights].ny = -r->light[i].ny;
                r->pclight[nLights].nz = -r->light[i].nz;
                r->pclight[nLights].inx = (sint32)(r->light[i].nx * -16384.0F);
                r->pclight[nLights].iny = (sint32)(r->light[i].ny * -16384.0F);
                r->pclight[nLights].inz = (sint32)(r->light[i].nz * -16384.0F);
                r->pclight[nLights].Inner = r->light[i].Inner;
                r->pclight[nLights].Outer = r->light[i].Outer;
                r->pclight[nLights].InnerAngle = 2 * acos(r->light[i].Inner);
                r->pclight[nLights].OuterAngle = 2 * acos(r->light[i].Outer);
                if (r->light[nLights].Type == LIGHT_SPOT && r->pclight[nLights].OuterAngle > 3.1415927F) {
                    Log(1, "SpotLight Corrected");
                    r->pclight[nLights].OuterAngle = 3.1415927F;
                }
                r->pclight[nLights].Cutoff = r->light[i].Cutoff;
                r->pclight[nLights].Type = r->light[i].Type;
                nLights++;
            }
        }
    }
    if (r->num_lights > MaxRoomLights) {
        MaxRoomLights = r->num_lights;
    }
    IDirect3DVertexBuffer_Optimize((LPDIRECT3DVERTEXBUFFER)r->SourceVB, App.dx._lpD3DDevice, 0);
}

void ProjectVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip)
{
    float zv, x;
    sint32 i;
    sint16 clip_distance;

    for (i = 0; i < nVerts; i++) {
        v->tu = v->sx;
        v->tv = v->sy;
        clip_distance = 0;
        if (v->sz >= f_mznear) {
            zv = f_mpersp / v->sz;
            if (v->sz > FogEnd) {
                clip_distance = 16;
                v->sz = f_zfar;
            }
            v->sx = zv * v->sx + f_centerx;
            v->sy = zv * v->sy + f_centery;
            v->rhw = f_moneopersp * zv;
            if (bWaterEffect) {
                x = v->sx;
                v->sx += vert_wibble_table[((wibble + (sint32)v->sy) >> 3) & 0x1F];
                v->sy += vert_wibble_table[((wibble + (sint32)x) >> 3) & 0x1F];
            }
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
        *clip++ = clip_distance;
        v++;
    }
}

void ProjectWaterVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip)
{
    ROOM_INFO *r;
    float zv, x, y, z;
    sint32 i, rndoff;
    sint32 clip_distance;
    sint16 col, cR, cG, cB;
    uint8 rnd;
    sint16 *c;

    r = &room[current_room];
    c = clip;
    for (i = 0; i < nVerts; i++) {
        clip_distance = 0;
        x = (r->x + r->verts[i].x) * 0.015625F;
        y = (r->y + r->verts[i].y) * 0.015625F;
        z = (r->z + r->verts[i].z) * 0.0078125F;
        rndoff = (sint32)(y + z + x) & 0xFC;
        rnd = WaterTable[(uint8)r->MeshEffect][rndoff & 0x3F].random;
        v->sy += WaterTable[(uint8)r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
        v->tu = v->sx;
        v->tv = v->sy;
        if (v->sz >= f_mznear) {
            zv = f_mpersp / v->sz;
            if (v->sz > FogEnd) {
                clip_distance = 256;
                v->sz = f_zfar;
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
            rnd = WaterTable[(uint8)r->MeshEffect][rndoff & 0x3F].random;
            col = -WaterTable[(uint8)r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].choppy;
            cR = CLRR(v->color) + col;
            cG = CLRG(v->color) + col;
            cB = CLRB(v->color) + col;
            if (cR > 255)
                cR = 255;
            if (cG > 255)
                cG = 255;
            if (cB > 255)
                cB = 255;
            if (cR < 0)
                cR = 0;
            if (cG < 0)
                cG = 0;
            if (cB < 0)
                cB = 0;
            v->color = RGBA(cR, cG, cB, 0xFF);
        } else
            clip_distance = -128;
        *c++ = clip_distance;
        v++;
    }
}

void ProjectShoreVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip)
{
    ROOM_INFO *r;
    float zv, x, y, z;
    sint32 i, rndoff;
    sint16 clip_distance, col, cR, cG, cB;
    sint16 *c;
    uint8 rnd;

    r = &room[current_room];
    c = clip;
    for (i = 0; i < nVerts; i++) {
        x = (r->x + r->verts[i].x) * 0.015625F;
        y = (r->y + r->verts[i].y) * 0.015625F;
        z = (r->z + r->verts[i].z) * 0.0078125F;
        rndoff = (sint32)(y + z + x) & 0xFC;
        v->tu = v->sx;
        v->tv = v->sy;
        clip_distance = 0;
        if (v->sz >= f_mznear) {
            zv = f_mpersp / v->sz;
            if (v->sz > FogEnd) {
                clip_distance = 256;
                v->sz = f_zfar;
            }
            v->sx = zv * v->sx + f_centerx;
            v->sy = zv * v->sy + f_centery;
            v->rhw = f_moneopersp * zv;
            if (bWaterEffect) {
                x = v->sx;
                v->sx += vert_wibble_table[((wibble + (sint32)v->sy) >> 3) & 0x1F];
                v->sy += vert_wibble_table[((wibble + (sint32)x) >> 3) & 0x1F];
            }
            if (v->sx < clip_left)
                clip_distance++;
            else if (v->sx > clip_right)
                clip_distance += 2;
            if (v->sy < clip_top)
                clip_distance += 4;
            else if (v->sy > clip_bottom)
                clip_distance += 8;
            rnd = WaterTable[(uint8)r->MeshEffect][rndoff & 0x3F].random;
            col = (WaterTable[(uint8)r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].abs + WaterTable[(uint8)r->MeshEffect][((wibble >> 2) + rnd) & 0x3F].shimmer) << 3;
            cR = CLRR(v->color) + col;
            cG = CLRG(v->color) + col;
            cB = CLRB(v->color) + col;
            if (cR > 255)
                cR = 255;
            if (cG > 255)
                cG = 255;
            if (cB > 255)
                cB = 255;
            if (cR < 0)
                cR = 0;
            if (cG < 0)
                cG = 0;
            if (cB < 0)
                cB = 0;
            v->color = RGBA(cR, cG, cB, 0xFF);
        } else
            clip_distance = -128;
        *c++ = clip_distance;
        v++;
    }
}

#ifndef FEAT_C_ASM
void PrelightVertsMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r)
{
    sint32 *prelight;
    sint32 p, c, i;

    if (bWaterEffect && !(r->flags & ROOM_UNDERWATER))
        prelight = r->prelightwater;
    else
        prelight = r->prelight;
    for (i = 0; i < r->nWaterVerts; i++) {
        p = r->prelight[i];
        c = v->color;
        __asm {
            movd mm0, p
            movd mm1, c
            paddusb mm1, mm0
            mov edx, v
            add edx, 0x10
            movd [edx], mm1
        }
        v->specular &= 0xFF000000;
        v++;
    }
    for (i = r->nWaterVerts; i < r->nVerts; i++) {
        AddPrelitMMX(prelight[i], &v->color);
        v++;
    }
    __asm {
        emms
    }
}
#else
void PrelightVertsMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r)
{
    sint32 *prelight;
    sint32 p, c, cR, cG, cB, cA, i;

    if (bWaterEffect && !(r->flags & ROOM_UNDERWATER))
        prelight = r->prelightwater;
    else
        prelight = r->prelight;
    for (i = 0; i < r->nWaterVerts; i++) {
        p = r->prelight[i];
        c = v->color;
        cR = CLRR(c) + CLRR(p);
        cG = CLRG(c) + CLRG(p);
        cB = CLRB(c) + CLRB(p);
        cA = ((c >> 24) & 0xFF) + ((p >> 24) & 0xFF);
        if (cR > 255)
            cR = 255;
        if (cG > 255)
            cG = 255;
        if (cB > 255)
            cB = 255;
        if (cA > 255)
            cA = 255;
        v->color = RGBA(cR, cG, cB, cA);
        v->specular &= 0xFF000000;
        v++;
    }
    for (i = r->nWaterVerts; i < r->nVerts; i++) {
        AddPrelitMMX(prelight[i], &v->color);
        v++;
    }
}
#endif

void PrelightVertsNonMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r)
{
    sint32 *prelight;
    sint32 p, c, cR, cG, cB, i;

    if (bWaterEffect && !(r->flags & ROOM_UNDERWATER))
        prelight = r->prelightwater;
    else
        prelight = r->prelight;
    for (i = 0; i < r->nWaterVerts; i++) {
        p = r->prelight[i];
        c = v->color;
        cR = (c & 0xFF0000) + (p & 0xFF0000);
        cG = (c & 0xFF00) + (p & 0xFF00);
        cB = (c & 0xFF) + (p & 0xFF);
        if (cR > 0xFF0000)
            cR = 0xFF0000;
        if (cG > 0xFF00)
            cG = 0xFF00;
        if (cB > 0xFF)
            cB = 0xFF;
        v->color = (v->color & 0xFF000000) | cB | cG | cR;
        v->specular &= 0xFF000000;
        v++;
    }
    for (i = r->nWaterVerts; i < r->nVerts; i++) {
        p = prelight[i];
        c = v->color;
        cR = (c & 0xFF0000) + (p & 0xFF0000);
        cG = (c & 0xFF00) + (p & 0xFF00);
        cB = (c & 0xFF) + (p & 0xFF);
        if (cR > 0xFF0000)
            cR = 0xFF0000;
        if (cG > 0xFF00)
            cG = 0xFF00;
        if (cB > 0xFF)
            cB = 0xFF;
        v->color = (c & 0xFF000000) | cB | cG | cR;
        CalcColorSplit(v->color, &v->color);
        v++;
    }
}

void InsertRoom(ROOM_INFO *r)
{
    TEXTURESTRUCT *pTex;
    D3DTLVERTEX *v;
    sint16 *data;
    sint32 numQuads, numTris, i, doublesided;

    clip_left = r->left;
    clip_top = r->top;
    clip_right = r->right;
    clip_bottom = r->bottom;
    if (r->nVerts) {
        DXAttempt(IDirect3DVertexBuffer_ProcessVertices(DestVB, D3DVOP_LIGHT | D3DVOP_TRANSFORM, 0, r->nVerts, r->SourceVB, 0, App.dx._lpD3DDevice, 0));
        data = r->FaceData;
        numQuads = *data++;
        IDirect3DVertexBuffer_Lock(DestVB, DDLOCK_READONLY, (void **)&v, 0);
        bWaterEffect = camera.underwater != 0;
        ProjectWaterVerts(r->nWaterVerts, v, clipflags);
        ProjectShoreVerts(r->nShoreVerts, &v[r->nWaterVerts], &clipflags[r->nWaterVerts]);
        ProjectVerts(r->nVerts - r->nWaterVerts - r->nShoreVerts, &v[r->nWaterVerts + r->nShoreVerts], &clipflags[r->nWaterVerts + r->nShoreVerts]);
        if (App.mmx)
            PrelightVertsMMX(r->nVerts, v, r);
        else
            PrelightVertsNonMMX(r->nVerts, v, r);
        for (i = 0; i < numQuads; i++, data += 5) {
            pTex = &textinfo[data[4] & 0x3FFF];
            doublesided = (data[4] >> 15) & 1;
            switch (pTex->drawtype) {
            case 1:
            case 2:
                AddQuadSorted(v, data[0], data[1], data[2], data[3], pTex, doublesided);
                break;
            case 0:
                AddQuadZBuffer(v, data[0], data[1], data[2], data[3], pTex, doublesided);
                break;
            }
        }
        numTris = *data++;
        for (i = 0; i < numTris; i++, data += 4) {
            pTex = &textinfo[data[3] & 0x3FFF];
            doublesided = (data[3] >> 15) & 1;
            switch (pTex->drawtype) {
            case 1:
            case 2:
                AddTriSorted(v, data[0], data[1], data[2], pTex, doublesided);
                break;
            case 0:
                AddTriZBuffer(v, data[0], data[1], data[2], pTex, doublesided);
                break;
            }
        }
        IDirect3DVertexBuffer_Unlock(DestVB);
    }
}

void InitBuckets(void)
{
    sint32 n;

    for (n = 0; n < 20; n++) {
        Bucket[n].tpage = -1;
        Bucket[n].nVtx = 0;
    }
}

void FindBucket(sint32 tpage, D3DTLBUMPVERTEX **Vpp, sint32 **nVtxpp)
{
    TEXTUREBUCKET *bucket;
    sint32 i, nVtx, biggest;

    bucket = Bucket;
    for (i = 0; i < 20; i++, bucket++) {
        if (bucket->tpage == tpage && bucket->nVtx < 512) {
            *Vpp = &bucket->vtx[bucket->nVtx];
            *nVtxpp = &bucket->nVtx;
            return;
        }
        if (bucket->nVtx > 512) {
            DrawBucket(&Bucket[i]);
            bucket->tpage = tpage;
            bucket->nVtx = 0;
            *Vpp = bucket->vtx;
            *nVtxpp = &bucket->nVtx;
            return;
        }
    }
    biggest = 0;
    nVtx = 0;
    bucket = Bucket;
    for (i = 0; i < 20; i++, bucket++) {
        if (bucket->tpage == -1) {
            bucket->tpage = tpage;
            *Vpp = bucket->vtx;
            *nVtxpp = &bucket->nVtx;
            return;
        }
        if (bucket->nVtx > nVtx) {
            nVtx = bucket->nVtx;
            biggest = i;
        }
    }
    DrawBucket(&Bucket[biggest]);
    Bucket[biggest].tpage = tpage;
    Bucket[biggest].nVtx = 0;
    *Vpp = Bucket[biggest].vtx;
    *nVtxpp = &Bucket[biggest].nVtx;
}

void DrawBucket(TEXTUREBUCKET *bucket)
{
    if (bucket->tpage == 1)
        bucket->tpage = 1;
    if (!bucket->nVtx)
        return;
    if (Textures[bucket->tpage].bump && App.BumpMapping) {
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_CURRENT);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[Textures[bucket->tpage].bumptpage].tex));
        IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        DrawPrimitiveCnt++;
    }
    DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[bucket->tpage].tex));
    IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
    if (App.BumpMapping)
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
    bucket->nVtx = 0;
    bucket->tpage = -1;
    DrawPrimitiveCnt++;
}

void DrawBuckets(void)
{
    TEXTUREBUCKET *bucket;
    sint32 i;

    if (!App.BumpMapping) {
        for (i = 0; i < 20; i++) {
            bucket = &Bucket[i];
            DrawBucket(bucket);
        }
    } else {
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_CURRENT);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        for (i = 0; i < 20; i++) {
            bucket = &Bucket[i];
            if (Textures[bucket->tpage].bump && bucket->nVtx) {
                DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[Textures[bucket->tpage].bumptpage].tex));
                IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTCLIP);
                DrawPrimitiveCnt++;
            }
        }
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 1);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        IDirect3DDevice3_SetTextureStageState(App.dx.lpD3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        for (i = 0; i < 20; i++) {
            bucket = &Bucket[i];
            if (Textures[bucket->tpage].bump && bucket->nVtx) {
                DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[bucket->tpage].tex));
                IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
                bucket->nVtx = 0;
                bucket->tpage = -1;
                DrawPrimitiveCnt++;
            }
        }
        IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
        for (i = 0; i < 20; i++) {
            bucket = &Bucket[i];
            if (!Textures[bucket->tpage].bump && bucket->nVtx) {
                DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, Textures[bucket->tpage].tex));
                IDirect3DDevice3_DrawPrimitive(App.dx.lpD3DDevice, D3DPT_TRIANGLELIST, FVF, bucket->vtx, bucket->nVtx, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
                bucket->nVtx = 0;
                bucket->tpage = -1;
                DrawPrimitiveCnt++;
            }
        }
    }
}

void CalcTriFaceNormal(D3DVECTOR *p1, D3DVECTOR *p2, D3DVECTOR *p3, D3DVECTOR *N)
{
    float ux, uy, uz, vx, vy, vz;

    ux = p1->x - p2->x;
    uy = p1->y - p2->y;
    uz = p1->z - p2->z;
    vx = p3->x - p2->x;
    vy = p3->y - p2->y;
    vz = p3->z - p2->z;
    N->x = vz * uy - vy * uz;
    N->y = vx * uz - vz * ux;
    N->z = vy * ux - vx * uy;
}

void CreateVertexNormals(ROOM_INFO *r)
{
    D3DVECTOR p1, p2, p3, n1, n2;
    sint16 *data;
    sint32 nQuads, nTris, i, j;

    data = r->FaceData;
    nQuads = *data++;
    r->fnormals = (D3DVECTOR *)game_malloc(sizeof(D3DVECTOR) * (r->gt3cnt + r->gt4cnt), 0);
    for (i = 0; i < nQuads; i++) {
        p1 = r->verts[data[0]];
        p2 = r->verts[data[1]];
        p3 = r->verts[data[2]];
        CalcTriFaceNormal(&p1, &p2, &p3, &n1);
        p1 = r->verts[data[0]];
        p2 = r->verts[data[2]];
        p3 = r->verts[data[3]];
        CalcTriFaceNormal(&p1, &p2, &p3, &n2);
        n1.x += n2.x;
        n1.y += n2.y;
        n1.z += n2.z;
        D3DNormalise(&n1);
        r->fnormals[i] = n1;
        data += 5;
    }
    nTris = *data++;
    for (i = 0; i < nTris; i++) {
        p1 = r->verts[data[0]];
        p2 = r->verts[data[1]];
        p3 = r->verts[data[2]];
        CalcTriFaceNormal(&p1, &p2, &p3, &r->fnormals[nQuads + i]);
        D3DNormalise(&r->fnormals[nQuads + i]);
        data += 4;
    }
    data = r->FaceData;
    nQuads = *data;
    nTris = data[nQuads * 5 + 1];
    r->vnormals = (D3DVECTOR *)game_malloc(sizeof(D3DVECTOR) * r->nVerts, 0);
    for (i = 0; i < r->nVerts; i++) {
        n1.x = 0;
        n1.y = 0;
        n1.z = 0;
        data = r->FaceData + 1;
        for (j = 0; j < nQuads; j++) {
            if (data[0] == i || data[1] == i || data[2] == i || data[3] == i) {
                n1.x += r->fnormals[j].x;
                n1.y += r->fnormals[j].y;
                n1.z += r->fnormals[j].z;
            }
            data += 5;
        }
        data++;
        for (j = 0; j < nTris; j++) {
            if (data[0] == i || data[1] == i || data[2] == i) {
                n1.x += r->fnormals[nQuads + j].x;
                n1.y += r->fnormals[nQuads + j].y;
                n1.z += r->fnormals[nQuads + j].z;
            }
            data += 4;
        }
        D3DNormalise(&n1);
        r->vnormals[i] = n1;
    }
}
