#include <windows.h>
#include <d3d.h>
#include "debris.h"
#include "control.h"
#include "room.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "anim.h"
#include "draw.h"

void UpdateDebris(void)
{
    DEBRIS_STRUCT *dptr;
    FLOOR_INFO *floor;
    sint32 height, ceiling;
    int i;

    dptr = debris;
    for (i = 256; i > 0; i--, dptr++) {
        if (dptr->On) {
            dptr->Yvel += dptr->Gravity;
            if (dptr->Yvel > 4096)
                dptr->Yvel = 4096;
            dptr->Speed -= dptr->Speed >> 4;
            dptr->x += phd_sin(dptr->Dir) * dptr->Speed >> W2V_SHIFT;
            dptr->y += dptr->Yvel >> 4;
            dptr->z += phd_cos(dptr->Dir) * dptr->Speed >> W2V_SHIFT;
            floor = GetFloor(dptr->x, dptr->y, dptr->z, &dptr->RoomNumber);
            height = GetHeight(floor, dptr->x, dptr->y, dptr->z);
            ceiling = GetCeiling(floor, dptr->x, dptr->y, dptr->z);
            if (dptr->y >= height || dptr->y < ceiling) {
                dptr->On = 0;
            } else {
                dptr->XRot += dptr->Yvel >> 6;
                if (dptr->Yvel)
                    dptr->YRot += dptr->Speed >> 5;
                if (room[dptr->RoomNumber].flags & ROOM_UNDERWATER)
                    dptr->Yvel -= dptr->Yvel >> 2;
            }
        }
    }
}

sint32 GetFreeDebris(void)
{
    DEBRIS_STRUCT *dptr;
    sint32 eldestage, eldestfree, free;
    int i;

    free = next_debris;
    eldestfree = 0;
    eldestage = -0x4000;
    dptr = &debris[next_debris];
    for (i = 0; i < 256; i++) {
        if (!dptr->On) {
            next_debris = (free + 1) & 0xFF;
            return free;
        }
        if (dptr->Yvel > eldestage) {
            eldestfree = free;
            eldestage = dptr->Yvel;
        }
        if (free == 255) {
            dptr = debris;
            free = 0;
        } else {
            free++;
            dptr++;
        }
    }
    next_debris = (eldestfree + 1) & 0xFF;
    return eldestfree;
}

void TriggerDebris(GAME_VECTOR *pos, void *TextInfo, sint16 *Offsets, sint32 *Vels, sint16 rgb)
{
    DEBRIS_STRUCT *dptr;

    if (!(GetRandomControl() & 3) && rgb >= 0)
        TriggerShatterSmoke(pos->x, pos->y, pos->z);
    else if (rgb < 0)
        rgb = -rgb;
    dptr = &debris[GetFreeDebris()];
    dptr->On = 1;
    dptr->x = pos->x;
    dptr->y = pos->y;
    dptr->z = pos->z;
    if (DebrisFlags & 1) {
        dptr->Dir = GetRandomControl() << 1;
        dptr->Speed = (GetRandomControl() & 0xF) + 16;
    } else {
        dptr->Dir = phd_atan(Vels[2], Vels[0]);
        dptr->Speed = (ABS(Vels[0]) + ABS(Vels[2])) >> 2;
    }
    if (!Vels[1]) {
        dptr->Yvel = 0;
        dptr->Gravity = (GetRandomControl() & 0x1F) + 32;
    } else {
        dptr->Yvel = -512 - (GetRandomControl() & 0x1FF);
        dptr->Gravity = (GetRandomControl() & 0x3F) + 64;
        if (Vels[1] == -1)
            dptr->Yvel <<= 1;
        else if (Vels[1] == -2)
            dptr->Yvel >>= 1;
    }
    dptr->RoomNumber = pos->room_number;
    dptr->XYZOffsets1[0] = Offsets[0];
    dptr->XYZOffsets1[1] = Offsets[1];
    dptr->XYZOffsets1[2] = Offsets[2];
    dptr->XYZOffsets2[0] = Offsets[3];
    dptr->XYZOffsets2[1] = Offsets[4];
    dptr->XYZOffsets2[2] = Offsets[5];
    dptr->XYZOffsets3[0] = Offsets[6];
    dptr->XYZOffsets3[1] = Offsets[7];
    dptr->XYZOffsets3[2] = Offsets[8];
    if (DebrisFlags & 1) {
        dptr->YRot = GetRandomControl() << 1;
        dptr->XRot = dptr->YRot;
    } else {
        dptr->YRot = 0;
        dptr->XRot = 0;
    }
    dptr->TextInfo = TextInfo;
    dptr->r = (rgb & 0x1F) << 3;
    dptr->g = ((rgb >> 5) & 0x1F) << 3;
    dptr->b = ((rgb >> 10) & 0x1F) << 3;
    if (DebrisMesh->prelight) {
        dptr->color1 = DebrisMesh->prelight[DebrisMeshC1];
        dptr->color2 = DebrisMesh->prelight[DebrisMeshC2];
        dptr->color3 = DebrisMesh->prelight[DebrisMeshC3];
        dptr->ambient = 0;
    } else {
        dptr->color1 = 0;
        dptr->color2 = 0;
        dptr->color3 = 0;
        dptr->ambient = DebrisMeshAmbient;
    }
    dptr->flags = DebrisMeshFlags;
}

void ShatterObject(SHATTER_ITEM *shatter_item, MESH_INFO *StaticMesh, sint16 Num, sint16 RoomNumber, sint32 NoXZVel)
{
    MESH_DATA *mesh;
    void *tex;
    PHD_VECTOR TPos;
    PHD_VECTOR VPos;
    PHD_VECTOR pos;
    GAME_VECTOR vec;
    float *vtx;
    sint32 *Vels;
    uint16 *face_data;
    sint16 *meshp;
    sint16 *offsets;
    sint16 *RotVerts;
    sint32 lp, nVtx, nTris, nQuads, RotY, rx, ry, rz;
    uint16 v1, v2, v3, c;
    sint16 x, y, z, rnd, rgb;

    rnd = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    if (Num < 0) {
        Num = -Num;
        rnd = 1;
    }
    if (shatter_item) {
        meshp = shatter_item->meshp;
        TPos.x = shatter_item->Sphere.x;
        TPos.y = shatter_item->Sphere.y;
        TPos.z = shatter_item->Sphere.z;
        RotY = shatter_item->YRot;
    } else {
        meshp = meshes[static_objects[StaticMesh->static_number].mesh_number];
        TPos.x = StaticMesh->x;
        TPos.y = StaticMesh->y;
        TPos.z = StaticMesh->z;
        RotY = StaticMesh->y_rot;
        rgb = StaticMesh->shade;
    }
    mesh = (MESH_DATA *)meshp;
    DebrisMesh = mesh;
    IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB, DDLOCK_READONLY, (LPVOID *)&vtx, 0);
    nVtx = mesh->nVerts;
    nTris = mesh->ngt3;
    nQuads = mesh->ngt4;
    if (nVtx > 256)
        nVtx = 256;
    phd_PushUnitMatrix();
    phd_RotY(RotY);
    offsets = (sint16 *)&tsv_buffer[0];
    for (lp = 0; lp < nVtx; lp++) {
        x = (sint16)vtx[0];
        y = (sint16)vtx[1];
        z = (sint16)vtx[2];
        vtx += 8;
        rx = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> W2V_SHIFT;
        ry = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> W2V_SHIFT;
        rz = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> W2V_SHIFT;
        *offsets++ = (sint16)rx;
        *offsets++ = (sint16)ry;
        *offsets++ = (sint16)rz;
        pos.x += rx;
        pos.y += ry;
        pos.z += rz;
    }
    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
    VPos.x = pos.x / lp;
    VPos.y = pos.y / lp;
    VPos.z = pos.z / lp;
    phd_PopMatrix();
    RotVerts = (sint16 *)&tsv_buffer[0];
    Vels = (sint32 *)&tsv_buffer[1536];
    offsets = (sint16 *)&tsv_buffer[1548];
    vec.room_number = RoomNumber;
    DebrisMeshAmbient = room[RoomNumber].ambient;
    face_data = (uint16 *)mesh->gt3;
    if (nTris && Num) {
        do {
            v1 = *face_data++;
            v2 = *face_data++;
            v3 = *face_data++;
            DebrisMeshC1 = v1;
            DebrisMeshC2 = v2;
            DebrisMeshC3 = v3;
            v1 *= 3;
            v2 *= 3;
            v3 *= 3;
            tex = (void *)*face_data++;
            DebrisMeshFlags = *face_data++;
            if (v1 < 0x300 && v2 < 0x300 && v3 < 0x300 && (!rnd || rnd == 1 && GetRandomControl() & 1)) {
                offsets[0] = RotVerts[v1];
                offsets[1] = RotVerts[v1 + 1];
                offsets[2] = RotVerts[v1 + 2];
                offsets[3] = RotVerts[v2];
                offsets[4] = RotVerts[v2 + 1];
                offsets[5] = RotVerts[v2 + 2];
                offsets[6] = RotVerts[v3];
                offsets[7] = RotVerts[v3 + 1];
                offsets[8] = RotVerts[v3 + 2];
                vec.x = (RotVerts[v1] + RotVerts[v2] + RotVerts[v3]) / 3;
                vec.y = (RotVerts[v1 + 1] + RotVerts[v2 + 1] + RotVerts[v3 + 1]) / 3;
                vec.z = (RotVerts[v1 + 2] + RotVerts[v2 + 2] + RotVerts[v3 + 2]) / 3;
                offsets[0] -= (sint16)vec.x;
                offsets[1] -= (sint16)vec.y;
                offsets[2] -= (sint16)vec.z;
                offsets[3] -= (sint16)vec.x;
                offsets[4] -= (sint16)vec.y;
                offsets[5] -= (sint16)vec.z;
                offsets[6] -= (sint16)vec.x;
                offsets[7] -= (sint16)vec.y;
                offsets[8] -= (sint16)vec.z;
                if (NoXZVel > 0) {
                    Vels[0] = 0;
                    Vels[1] = 0;
                    Vels[2] = 0;
                } else {
                    Vels[0] = vec.x - VPos.x;
                    Vels[1] = vec.y - VPos.y;
                    Vels[2] = vec.z - VPos.z;
                }
                if (NoXZVel < 0)
                    Vels[1] = NoXZVel;
                vec.x += TPos.x;
                vec.y += TPos.y;
                vec.z += TPos.z;
                c = rgb;
                if (shatter_item && shatter_item->Flags & 0x400)
                    c = -rgb;
                TriggerDebris(&vec, tex, offsets, Vels, c);
                Num--;
            }
        } while (--nTris && Num);
    }
    face_data = (uint16 *)mesh->gt4;
    if (nQuads && Num) {
        do {
            v1 = *face_data++;
            v2 = *face_data++;
            face_data++;
            v3 = *face_data++;
            DebrisMeshC1 = v1;
            DebrisMeshC2 = v2;
            DebrisMeshC3 = v3;
            v1 *= 3;
            v2 *= 3;
            v3 *= 3;
            tex = (void *)*face_data++;
            DebrisMeshFlags = *face_data++;
            if (v1 < 0x300 && v2 < 0x300 && v3 < 0x300 && (!rnd || rnd == 1 && GetRandomControl() & 1)) {
                offsets[0] = RotVerts[v1];
                offsets[1] = RotVerts[v1 + 1];
                offsets[2] = RotVerts[v1 + 2];
                offsets[3] = RotVerts[v2];
                offsets[4] = RotVerts[v2 + 1];
                offsets[5] = RotVerts[v2 + 2];
                offsets[6] = RotVerts[v3];
                offsets[7] = RotVerts[v3 + 1];
                offsets[8] = RotVerts[v3 + 2];
                vec.x = (RotVerts[v1] + RotVerts[v2] + RotVerts[v3]) / 3;
                vec.y = (RotVerts[v1 + 1] + RotVerts[v2 + 1] + RotVerts[v3 + 1]) / 3;
                vec.z = (RotVerts[v1 + 2] + RotVerts[v2 + 2] + RotVerts[v3 + 2]) / 3;
                offsets[0] -= (sint16)vec.x;
                offsets[1] -= (sint16)vec.y;
                offsets[2] -= (sint16)vec.z;
                offsets[3] -= (sint16)vec.x;
                offsets[4] -= (sint16)vec.y;
                offsets[5] -= (sint16)vec.z;
                offsets[6] -= (sint16)vec.x;
                offsets[7] -= (sint16)vec.y;
                offsets[8] -= (sint16)vec.z;
                if (NoXZVel > 0) {
                    Vels[0] = 0;
                    Vels[1] = 0;
                    Vels[2] = 0;
                } else {
                    Vels[0] = vec.x - VPos.x;
                    Vels[1] = vec.y - VPos.y;
                    Vels[2] = vec.z - VPos.z;
                }
                if (NoXZVel < 0)
                    Vels[1] = NoXZVel;
                vec.x += TPos.x;
                vec.y += TPos.y;
                vec.z += TPos.z;
                c = rgb;
                if (shatter_item && shatter_item->Flags & 0x400)
                    c = -rgb;
                TriggerDebris(&vec, tex, offsets, Vels, c);
                Num--;
            }
        } while (--nQuads && Num);
    }
}
