#include <string.h>
#include "lighting.h"
#include "3dmath.h"
#include "function_stubs.h"
#include "../room.h"
#include "../draw.h"
#include "winmain.h"
#include "dxshell.h"
#include "function_table.h"
#include "../control.h"
#include "../text.h"
#include "../delstuff.h"
#include "../lara.h"
#include "../objnums.h"
#include "../effect2.h"
#include "../camera.h"

void MallocD3DLights(void)
{
    if (MaxRoomLights > 21)
        Log(1, "MAX Room Lights of %d Exceeded - %d", 21, MaxRoomLights);
    MaxRoomLights *= 2;
    D3DLights = (D3DLIGHT_STRUCT *)game_malloc(sizeof(D3DLIGHT_STRUCT) * MaxRoomLights, 0);
    D3DDynamics = (D3DLIGHT_STRUCT *)game_malloc(sizeof(D3DLIGHT_STRUCT) * 32, 0);
}

void CreateD3DLights(void)
{
    D3DMATERIAL material;
    sint32 i;

    Log(2, "CreateD3DLights");
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_FOGCOLOR, CurrentFog);
    for (i = 0; i < MaxRoomLights; i++) {
        DXAttempt(IDirect3D3_CreateLight(App.dx.lpD3D, &D3DLights[i].D3DLight, 0));
        memset(&D3DLights[i].D3DLightx, 0, sizeof(D3DLights[i].D3DLightx));
        D3DLights[i].D3DLightx.dwSize = sizeof(D3DLights[i].D3DLightx);
        D3DLights[i].D3DLightx.dltType = D3DLIGHT_POINT;
        D3DLights[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
        DXAttempt(IDirect3DLight_SetLight(D3DLights[i].D3DLight, (LPD3DLIGHT)&D3DLights[i].D3DLightx));
        DXAttempt(IDirect3DViewport3_AddLight(App.dx.lpViewport, D3DLights[i].D3DLight));
    }
    if (D3DDynamics) {
        for (i = 0; i < 32; i++) {
            DXAttempt(IDirect3D3_CreateLight(App.dx.lpD3D, &D3DDynamics[i].D3DLight, 0));
            memset(&D3DDynamics[i].D3DLightx, 0, sizeof(D3DDynamics[i].D3DLightx));
            D3DDynamics[i].D3DLightx.dwSize = sizeof(D3DDynamics[i].D3DLightx);
            D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
            D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
            DXAttempt(IDirect3DLight_SetLight(D3DDynamics[i].D3DLight, (LPD3DLIGHT)&D3DDynamics[i].D3DLightx));
            DXAttempt(IDirect3DViewport3_AddLight(App.dx.lpViewport, D3DDynamics[i].D3DLight));
        }
        DXAttempt(IDirect3D3_CreateMaterial(App.dx.lpD3D, &App.GlobalMaterial, 0));
        memset(&material, 0, sizeof(material));
        material.dwSize = sizeof(material);
        material.ambient.a = 0;
        material.diffuse.a = 0;
        material.ambient.r = 1;
        material.ambient.g = 1;
        material.ambient.b = 1;
        material.diffuse.r = 1;
        material.diffuse.g = 1;
        material.diffuse.b = 1;
        DXAttempt(IDirect3DMaterial3_SetMaterial(App.GlobalMaterial, &material));
        DXAttempt(IDirect3DMaterial3_GetHandle(App.GlobalMaterial, App.dx._lpD3DDevice, &App.GlobalMaterialHandle));
        DXAttempt(IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_MATERIAL, App.GlobalMaterialHandle));
    }
}

void FreeD3DLights(void)
{
    sint32 i;

    if (MaxRoomLights) {
        for (i = 0; i < MaxRoomLights; i++) {
            IDirect3DViewport3_DeleteLight(App.dx.lpViewport, D3DLights[i].D3DLight);
            if (D3DLights[i].D3DLight) {
                Log(4, "Released %s @ %x - RefCnt = %d", "Room Light", D3DLights[i].D3DLight, IDirect3DLight_Release(D3DLights[i].D3DLight));
                D3DLights[i].D3DLight = 0;
            } else
                Log(1, "%s Attempt To Release NULL Ptr", "Room Light");
        }
    }
    if (D3DDynamics) {
        for (i = 0; i < 32; i++) {
            DXAttempt(IDirect3DViewport3_DeleteLight(App.dx.lpViewport, D3DDynamics[i].D3DLight));
            if (D3DDynamics[i].D3DLight) {
                Log(4, "Released %s @ %x - RefCnt = %d", "Dynamic Light", D3DDynamics[i].D3DLight, IDirect3DLight_Release(D3DDynamics[i].D3DLight));
                D3DDynamics[i].D3DLight = 0;
            } else
                Log(1, "%s Attempt To Release NULL Ptr", "Dynamic Light");
        }
    }
    if (App.GlobalMaterial) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Global Material", App.GlobalMaterial, IDirect3DMaterial3_Release(App.GlobalMaterial));
        App.GlobalMaterial = 0;
    } else
        Log(1, "%s Attempt To Release NULL Ptr", "Global Material");
}

void ApplyMatrix(sint32 *matrix, PHD_VECTOR *start, PHD_VECTOR *dest)
{
    dest->x = (start->x * matrix[M00] + start->y * matrix[M01] + start->z * matrix[M02]) >> W2V_SHIFT;
    dest->y = (start->x * matrix[M10] + start->y * matrix[M11] + start->z * matrix[M12]) >> W2V_SHIFT;
    dest->z = (start->x * matrix[M20] + start->y * matrix[M21] + start->z * matrix[M22]) >> W2V_SHIFT;
}

void ApplyTransposeMatrix(sint32 *matrix, PHD_VECTOR *start, PHD_VECTOR *dest)
{
    dest->x = (start->x * matrix[M00] + start->y * matrix[M10] + start->z * matrix[M20]) >> W2V_SHIFT;
    dest->y = (start->x * matrix[M01] + start->y * matrix[M11] + start->z * matrix[M21]) >> W2V_SHIFT;
    dest->z = (start->x * matrix[M02] + start->y * matrix[M12] + start->z * matrix[M22]) >> W2V_SHIFT;
}

void S_CalculateStaticMeshLight(sint32 x, sint32 y, sint32 z, sint32 shade, ROOM_INFO *r)
{
    StaticMeshLightItem.il.ambient = r->ambient;
    StaticMeshLightItem.il.item_pos.x = x;
    StaticMeshLightItem.il.item_pos.y = y;
    StaticMeshLightItem.il.item_pos.z = z;
    StaticMeshLightItem.room_number = current_room;
    StaticMeshShade = shade;
    current_item = &StaticMeshLightItem;
}

void FadeLightList(PCLIGHT *lights, sint32 nLights)
{
    sint32 i;

    for (i = nLights; i > 0; i--, lights++) {
        if (lights->Active && lights->fcnt) {
            if (lights->Type != LIGHT_SHADOW) {
                lights->r = lights->rs + lights->r;
                lights->g = lights->gs + lights->g;
                lights->b = lights->bs + lights->b;
            } else
                lights->shadow += lights->iny;
            lights->fcnt--;
            if (lights->Type != LIGHT_SHADOW) {
                if (lights->r <= 0 && lights->g <= 0 && lights->b <= 0)
                    lights->Active = 0;
            } else if (lights->shadow <= 0)
                lights->Active = 0;
        }
    }
}

void CreateLightList(ITEM_INFO *item)
{
    ROOM_INFO *r;
    PCLIGHT *current_lights;
    PCLIGHT *prev_lights;
    PCLIGHT_INFO *room_light;
    D3DVECTOR vec;
    void *bakPtr;
    sint32 bakNum, dx, dy, dz, range;
    uint8 in_range;
    sint32 i;

    r = &room[item->room_number];
    if (item->il.room_number != item->room_number) {
        bakPtr = item->il.pCurrentLights;
        bakNum = item->il.nCurrentLights;
        item->il.pCurrentLights = item->il.pPrevLights;
        item->il.room_number = item->room_number;
        item->il.nCurrentLights = item->il.nPrevLights;
        item->il.pPrevLights = bakPtr;
        item->il.nPrevLights = bakNum;
        prev_lights = (PCLIGHT *)item->il.pPrevLights;
        for (i = 0; i < item->il.nPrevLights; i++) {
            if (prev_lights[i].Active) {
                if (prev_lights[i].Type != LIGHT_SHADOW) {
                    prev_lights[i].rs = prev_lights[i].r * -0.125F;
                    prev_lights[i].gs = prev_lights[i].g * -0.125F;
                    prev_lights[i].bs = prev_lights[i].b * -0.125F;
                    prev_lights[i].tr = 0;
                    prev_lights[i].tg = 0;
                    prev_lights[i].tb = 0;
                } else
                    prev_lights[i].iny = -prev_lights[i].shadow >> 3;
                prev_lights[i].fcnt = 8;
            }
        }
        item->il.nCurrentLights = r->num_lights;
        current_lights = (PCLIGHT *)item->il.pCurrentLights;
        room_light = r->pclight;
        for (i = 0; i < r->num_lights; i++) {
            current_lights->r = room_light->r;
            current_lights->g = room_light->g;
            current_lights->b = room_light->b;
            current_lights->tr = room_light->r;
            current_lights->tg = room_light->g;
            current_lights->tb = room_light->b;
            current_lights->x = room_light->x;
            current_lights->y = room_light->y;
            current_lights->z = room_light->z;
            current_lights->shadow = room_light->shadow << 3;
            current_lights->Inner = room_light->Inner;
            current_lights->Outer = room_light->Outer;
            current_lights->InnerAngle = room_light->InnerAngle;
            current_lights->OuterAngle = room_light->OuterAngle;
            current_lights->Cutoff = room_light->Cutoff;
            current_lights->inx = room_light->inx;
            current_lights->iny = room_light->iny;
            current_lights->inz = room_light->inz;
            current_lights->ix = room_light->ix;
            current_lights->iy = room_light->iy;
            current_lights->iz = room_light->iz;
            current_lights->nx = room_light->nx;
            current_lights->ny = room_light->ny;
            current_lights->nz = room_light->nz;
            current_lights->Type = room_light->Type;
            current_lights->Active = 0;
            if (current_lights->Type == LIGHT_SHADOW)
                current_lights->inx = room_light->shadow;
            current_lights++;
            room_light++;
        }
    }
    current_lights = (PCLIGHT *)item->il.pCurrentLights;
    prev_lights = (PCLIGHT *)item->il.pPrevLights;
    for (i = 0; i < item->il.nCurrentLights; i++) {
        in_range = 1;
        dx = current_lights[i].ix - item->il.item_pos.x;
        dy = current_lights[i].iy - item->il.item_pos.y;
        dz = current_lights[i].iz - item->il.item_pos.z;
        switch (current_lights[i].Type) {
        case LIGHT_SPOT:
            range = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);
            if (range > SQUARE(current_lights[i].Cutoff))
                in_range = 0;
            else {
                vec.x = (float)-dx;
                vec.y = (float)-dy;
                vec.z = (float)-dz;
                D3DNormalise(&vec);
                if (current_lights[i].nz * vec.z + current_lights[i].ny * vec.y + current_lights[i].nx * vec.x <= current_lights[i].Outer)
                    in_range = 0;
            }
            break;
        case LIGHT_POINT:
        case LIGHT_SHADOW:
            range = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);
            if (range > SQUARE(current_lights[i].Outer))
                in_range = 0;
            break;
        }
        current_lights[i].rlp.x = dx;
        current_lights[i].rlp.y = dy;
        current_lights[i].rlp.z = dz;
        current_lights[i].Range = range;
        if (in_range) {
            if (!current_lights[i].Active) {
                if (current_lights[i].Type != LIGHT_SHADOW) {
                    current_lights[i].r = 0;
                    current_lights[i].g = 0;
                    current_lights[i].b = 0;
                    current_lights[i].rs = current_lights[i].tr * 0.125F;
                    current_lights[i].gs = current_lights[i].tg * 0.125F;
                    current_lights[i].bs = current_lights[i].tb * 0.125F;
                    current_lights[i].fcnt = 8;
                    current_lights[i].Active = 1;
                } else {
                    current_lights[i].Active = 1;
                    current_lights[i].iny = current_lights[i].inx;
                    current_lights[i].shadow = 0;
                    current_lights[i].fcnt = 8;
                }
            }
        } else if (current_lights[i].Active && !current_lights[i].fcnt) {
            if (current_lights[i].Type != LIGHT_SHADOW) {
                current_lights[i].rs = current_lights[i].r * -0.125F;
                current_lights[i].gs = current_lights[i].g * -0.125F;
                current_lights[i].bs = current_lights[i].b * -0.125F;
            } else
                current_lights[i].iny = -current_lights[i].shadow >> 3;
            current_lights[i].fcnt = 8;
        }
    }
    FadeLightList(current_lights, item->il.nCurrentLights);
    FadeLightList(prev_lights, item->il.nPrevLights);
}

void ClearObjectLighting(void)
{
    sint32 i;

    IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_AMBIENT, 0);
    for (i = 0; i < MaxRoomLights; i++) {
        if (D3DLights[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE) {
            D3DLights[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
            D3DLights[i].D3DLightx.dltType = D3DLIGHT_POINT;
            IDirect3DLight_SetLight(D3DLights[i].D3DLight, (LPD3DLIGHT)&D3DLights[i].D3DLightx);
        }
    }
}

void InitDynamicLighting(void)
{
    DYNAMIC *dptr;
    float f;
    sint32 last_off, i;

    last_off = -1;
    ClearObjectLighting();
    IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_AMBIENT, 0);
    for (i = 0; i < MAX_DYNAMICS; i++) {
        dptr = &dynamics[i];
        if (dptr->on) {
            D3DDynamics[i].D3DLightx.dcvColor.r = dptr->r / 255.0F;
            D3DDynamics[i].D3DLightx.dcvColor.g = dptr->g / 255.0F;
            D3DDynamics[i].D3DLightx.dcvColor.b = dptr->b / 255.0F;
            D3DDynamics[i].D3DLightx.dvPosition.x = (float)dptr->x;
            D3DDynamics[i].D3DLightx.dvPosition.y = (float)dptr->y;
            D3DDynamics[i].D3DLightx.dvPosition.z = (float)dptr->z;
            D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_ACTIVE | D3DLIGHT_NO_SPECULAR;
            D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
            D3DDynamics[i].D3DLightx.dvAttenuation1 = 1;
            D3DDynamics[i].D3DLightx.dvRange = (float)((dptr->falloff >> 1) + (dptr->falloff >> 3));
            IDirect3DLight_SetLight(D3DDynamics[i].D3DLight, (LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
        } else {
            if (D3DDynamics[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE) {
                D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
                D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
                IDirect3DLight_SetLight(D3DDynamics[i].D3DLight, (LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
            }
            last_off = i;
        }
    }
    if (last_off >= 0 && bLaraTorch) {
        f = LaraTorchIntensity / 255.0F;
        D3DDynamics[last_off].D3DLightx.dcvColor.r = f;
        D3DDynamics[last_off].D3DLightx.dcvColor.g = f;
        D3DDynamics[last_off].D3DLightx.dcvColor.b = 0;
        D3DDynamics[last_off].D3DLightx.dvPosition.x = (float)LaraTorchStart.x;
        D3DDynamics[last_off].D3DLightx.dvPosition.y = (float)LaraTorchStart.y;
        D3DDynamics[last_off].D3DLightx.dvPosition.z = (float)LaraTorchStart.z;
        D3DDynamics[last_off].D3DLightx.dvDirection.x = (float)(LaraTorchEnd.x - LaraTorchStart.x);
        D3DDynamics[last_off].D3DLightx.dvDirection.y = (float)(LaraTorchEnd.y - LaraTorchStart.y);
        D3DDynamics[last_off].D3DLightx.dvDirection.z = (float)(LaraTorchEnd.z - LaraTorchStart.z);
        D3DDynamics[last_off].D3DLightx.dwFlags = D3DLIGHT_ACTIVE | D3DLIGHT_NO_SPECULAR;
        D3DDynamics[last_off].D3DLightx.dltType = D3DLIGHT_SPOT;
        D3DDynamics[last_off].D3DLightx.dvAttenuation1 = 1;
        D3DDynamics[last_off].D3DLightx.dvFalloff = 1;
        D3DDynamics[last_off].D3DLightx.dvRange = 20480;
        D3DDynamics[last_off].D3DLightx.dvTheta = 0.25F;
        D3DDynamics[last_off].D3DLightx.dvPhi = 1.25F;
        IDirect3DLight_SetLight(D3DDynamics[last_off].D3DLight, (LPD3DLIGHT)&D3DDynamics[last_off].D3DLightx);
    }
}

void InitItemDynamicLighting(ITEM_INFO *item)
{
    PHD_VECTOR d;
    PHD_VECTOR vec;
    PHD_VECTOR d2;
    PHD_VECTOR vec2;
    D3DVECTOR l;
    float f;
    sint32 last_off, i;

    for (i = 0; i < MAX_DYNAMICS; i++) {
        if (dynamics[i].on) {
            d.x = dynamics[i].x - item->il.item_pos.x;
            d.y = dynamics[i].y - item->il.item_pos.y;
            d.z = dynamics[i].z - item->il.item_pos.z;
            ApplyMatrix(w2v_matrix, &d, &vec);
            ApplyTransposeMatrix(phd_mxptr, &vec, &d);
            D3DDynamics[i].D3DLightx.dvPosition.x = (float)d.x;
            D3DDynamics[i].D3DLightx.dvPosition.y = (float)d.y;
            D3DDynamics[i].D3DLightx.dvPosition.z = (float)d.z;
            D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_ACTIVE | D3DLIGHT_NO_SPECULAR;
            D3DDynamics[i].D3DLightx.dcvColor.r = dynamics[i].r / 255.0F;
            D3DDynamics[i].D3DLightx.dcvColor.g = dynamics[i].g / 255.0F;
            D3DDynamics[i].D3DLightx.dcvColor.b = dynamics[i].b / 255.0F;
            D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
            D3DDynamics[i].D3DLightx.dvAttenuation1 = 1;
            D3DDynamics[i].D3DLightx.dvRange = (float)((dynamics[i].falloff >> 1) + (dynamics[i].falloff >> 3));
            IDirect3DLight_SetLight(D3DDynamics[i].D3DLight, (LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
        } else {
            if (D3DDynamics[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE) {
                D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
                D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
                IDirect3DLight_SetLight(D3DDynamics[i].D3DLight, (LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
            }
            last_off = i;
        }
    }
    if (last_off >= 0 && bLaraTorch) {
        d2.x = LaraTorchEnd.x - LaraTorchStart.x;
        d2.y = LaraTorchEnd.y - LaraTorchStart.y;
        d2.z = LaraTorchEnd.z - LaraTorchStart.z;
        ApplyMatrix(w2v_matrix, &d2, &vec2);
        ApplyTransposeMatrix(phd_mxptr, &vec2, &d2);
        l.x = (float)d2.x;
        l.y = (float)d2.y;
        l.z = (float)d2.z;
        d2.x = LaraTorchStart.x - item->il.item_pos.x;
        d2.y = LaraTorchStart.y - item->il.item_pos.y;
        d2.z = LaraTorchStart.z - item->il.item_pos.z;
        ApplyMatrix(w2v_matrix, &d2, &vec2);
        ApplyTransposeMatrix(phd_mxptr, &vec2, &d2);
        f = LaraTorchIntensity / 255.0F;
        D3DDynamics[last_off].D3DLightx.dcvColor.r = f;
        D3DDynamics[last_off].D3DLightx.dcvColor.g = f;
        D3DDynamics[last_off].D3DLightx.dcvColor.b = 0;
        D3DDynamics[last_off].D3DLightx.dvPosition.x = (float)d2.x;
        D3DDynamics[last_off].D3DLightx.dvPosition.y = (float)d2.y;
        D3DDynamics[last_off].D3DLightx.dvPosition.z = (float)d2.z;
        D3DDynamics[last_off].D3DLightx.dvDirection.x = l.x;
        D3DDynamics[last_off].D3DLightx.dvDirection.y = l.y;
        D3DDynamics[last_off].D3DLightx.dvDirection.z = l.z;
        D3DDynamics[last_off].D3DLightx.dwFlags = D3DLIGHT_ACTIVE | D3DLIGHT_NO_SPECULAR;
        D3DDynamics[last_off].D3DLightx.dltType = D3DLIGHT_SPOT;
        D3DDynamics[last_off].D3DLightx.dvAttenuation1 = 1;
        D3DDynamics[last_off].D3DLightx.dvFalloff = 1;
        D3DDynamics[last_off].D3DLightx.dvRange = 20480;
        D3DDynamics[last_off].D3DLightx.dvTheta = 0.25F;
        D3DDynamics[last_off].D3DLightx.dvPhi = 1.25F;
        IDirect3DLight_SetLight(D3DDynamics[last_off].D3DLight, (LPD3DLIGHT)&D3DDynamics[last_off].D3DLightx);
    }
}
void ClearDynamicLighting(void)
{
    sint32 i;

    IDirect3DDevice3_SetLightState(App.dx.lpD3DDevice, D3DLIGHTSTATE_AMBIENT, 0);
    for (i = 0; i < 32; i++) {
        if (D3DDynamics[i].D3DLightx.dwFlags & D3DLIGHT_ACTIVE) {
            D3DDynamics[i].D3DLightx.dwFlags = D3DLIGHT_NO_SPECULAR;
            D3DDynamics[i].D3DLightx.dltType = D3DLIGHT_POINT;
            IDirect3DLight_SetLight(D3DDynamics[i].D3DLight, (LPD3DLIGHT)&D3DDynamics[i].D3DLightx);
        }
    }
}
