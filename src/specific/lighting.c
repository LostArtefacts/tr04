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
