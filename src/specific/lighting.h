#ifndef SPECIFIC_LIGHTING_H
#define SPECIFIC_LIGHTING_H

#include <windows.h>
#include <d3d.h>

#include "../items.h"
#include "../room.h"
#include "../3dglodef.h"

enum light_types {
    LIGHT_SUN,
    LIGHT_POINT,
    LIGHT_SPOT,
    LIGHT_SHADOW,
    LIGHT_FOG
};

typedef struct {
    LPDIRECT3DLIGHT D3DLight;
    D3DLIGHT2 D3DLightx;
} D3DLIGHT_STRUCT;

extern ITEM_INFO *current_item;
extern ITEM_INFO StaticMeshLightItem;
extern sint32 StaticMeshShade;
extern sint32 SetupLight_thing;
extern sint32 MaxRoomLights;
extern D3DLIGHT_STRUCT *D3DLights;
extern D3DLIGHT_STRUCT *D3DDynamics;

void FreeD3DLights(void);
void SetupLight(D3DLIGHT_STRUCT *d3dlight, PCLIGHT *light, sint32 *ambient);
void InitItemDynamicLighting(ITEM_INFO *item);
void CalcAmbientLight(ITEM_INFO *item);
void CreateLightList(ITEM_INFO *item);
void ApplyMatrix(sint32 *matrix, PHD_VECTOR *start, PHD_VECTOR *dest);
void ApplyTransposeMatrix(sint32 *matrix, PHD_VECTOR *start, PHD_VECTOR *dest);
void S_CalculateStaticMeshLight(sint32 x, sint32 y, sint32 z, sint32 shade, ROOM_INFO *r);
void MallocD3DLights(void);
void ClearObjectLighting(void);
void InitDynamicLighting(void);
void FadeLightList(PCLIGHT *lights, sint32 nLights);
void ClearDynamicLighting(void);
void CreateD3DLights(void);

#endif
