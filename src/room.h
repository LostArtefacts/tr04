#ifndef ROOM_H
#define ROOM_H

#include "types.h"

#define WALL_SHIFT 10
#define WALL_L 1024
#define STEP_L (WALL_L / 4)

enum room_flags {
    ROOM_UNDERWATER = 1
};

typedef struct floor_info {
    uint16 index;
    uint16 fx:4;
    uint16 box:11;
    uint16 stopper:1;
    uint8 pit_room;
    sint8 floor;
    uint8 sky_room;
    sint8 ceiling;
} FLOOR_INFO;

typedef struct lightinfo {
    sint32 x;
    sint32 y;
    sint32 z;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 Type;
    sint16 Intensity;
    float Inner;
    float Outer;
    float Length;
    float Cutoff;
    float nx;
    float ny;
    float nz;
} LIGHTINFO;

typedef struct mesh_info {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 y_rot;
    sint16 shade;
    sint16 Flags;
    sint16 static_number;
} MESH_INFO;

typedef struct pclight_info {
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    sint32 shadow;
    float Inner;
    float Outer;
    float InnerAngle;
    float OuterAngle;
    float Cutoff;
    float nx;
    float ny;
    float nz;
    sint32 ix;
    sint32 iy;
    sint32 iz;
    sint32 inx;
    sint32 iny;
    sint32 inz;
    uint8 Type;
    uint8 Pad;
} PCLIGHT_INFO;

typedef struct room_info {
    sint16 *data;
    sint16 *door;
    FLOOR_INFO *floor;
    LIGHTINFO *light;
    MESH_INFO *mesh;
    sint32 x;
    sint32 y;
    sint32 z;
    sint32 minfloor;
    sint32 maxceiling;
    sint16 x_size;
    sint16 y_size;
    sint32 ambient;
    sint16 num_lights;
    sint16 num_meshes;
    uint8 ReverbType;
    uint8 FlipNumber;
    sint8 MeshEffect;
    sint8 bound_active;
    sint16 left;
    sint16 right;
    sint16 top;
    sint16 bottom;
    sint16 test_left;
    sint16 test_right;
    sint16 test_top;
    sint16 test_bottom;
    sint16 item_number;
    sint16 fx_number;
    sint16 flipped_room;
    uint16 flags;
    sint32 nVerts;
    sint32 nWaterVerts;
    sint32 nShoreVerts;
    struct IDirect3DVertexBuffer *SourceVB;
    sint16 *FaceData;
    float posx;
    float posy;
    float posz;
    struct _D3DVECTOR *vnormals;
    struct _D3DVECTOR *fnormals;
    sint32 *prelight;
    sint32 *prelightwater;
    sint32 watercalc;
    struct _D3DVECTOR *verts;
    sint32 gt3cnt;
    sint32 gt4cnt;
    PCLIGHT_INFO *pclight;
} ROOM_INFO;

extern ROOM_INFO *room;
extern sint16 number_rooms;

#endif
