#ifndef DEBRIS_H
#define DEBRIS_H

#include "3dglodef.h"
#include "sphere.h"
#include "room.h"

typedef struct debris_struct {
    void *TextInfo;
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 XYZOffsets1[3];
    sint16 Dir;
    sint16 XYZOffsets2[3];
    sint16 Speed;
    sint16 XYZOffsets3[3];
    sint16 Yvel;
    sint16 Gravity;
    sint16 RoomNumber;
    uint8 On;
    uint8 XRot;
    uint8 YRot;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 Pad[2];
    sint32 color1;
    sint32 color2;
    sint32 color3;
    sint32 ambient;
    sint32 flags;
} DEBRIS_STRUCT;

typedef struct shatter_item {
    SPHERE Sphere;
    ITEM_LIGHT *il;
    sint16 *meshp;
    sint32 Bit;
    sint16 YRot;
    sint16 Flags;
} SHATTER_ITEM;

sint32 GetFreeDebris(void);
void ShatterObject(SHATTER_ITEM *shatter_item, MESH_INFO *StaticMesh, sint16 Num, sint16 RoomNumber, sint32 NoXZVel);
void UpdateDebris(void);
void TriggerDebris(GAME_VECTOR *pos, void *TextInfo, sint16 *Offsets, sint32 *Vels, sint16 rgb);

extern DEBRIS_STRUCT debris[256];
extern sint32 next_debris;
extern sint16 DebrisFlags;
extern MESH_DATA *DebrisMesh;
extern sint32 DebrisMeshC1;
extern sint32 DebrisMeshC2;
extern sint32 DebrisMeshC3;
extern sint32 DebrisMeshAmbient;
extern sint32 DebrisMeshFlags;

#endif
