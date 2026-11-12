#include "lighting.h"
#include "3dmath.h"
#include "function_stubs.h"
#include "../room.h"
#include "../draw.h"
#include "winmain.h"
#include "dxshell.h"
#include "../control.h"
#include "../text.h"
#include "../delstuff.h"
#include "../lara.h"
#include "../objnums.h"

void MallocD3DLights(void)
{
    if (MaxRoomLights > 21)
        Log(1, "MAX Room Lights of %d Exceeded - %d", 21, MaxRoomLights);
    MaxRoomLights *= 2;
    D3DLights = (D3DLIGHT_STRUCT *)game_malloc(sizeof(D3DLIGHT_STRUCT) * MaxRoomLights, 0);
    D3DDynamics = (D3DLIGHT_STRUCT *)game_malloc(sizeof(D3DLIGHT_STRUCT) * 32, 0);
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
