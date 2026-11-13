#ifndef SPECIFIC_DRAWROOM_H
#define SPECIFIC_DRAWROOM_H

#include <windows.h>
#include <d3d.h>
#include "../types.h"
#include "../room.h"

typedef struct d3dtlbumpvertex {
    float sx;
    float sy;
    float sz;
    float rhw;
    uint32 color;
    uint32 specular;
    float tu;
    float tv;
    float tx;
    float ty;
} D3DTLBUMPVERTEX;

typedef struct texturebucket {
    sint32 tpage;
    sint32 nVtx;
    D3DTLBUMPVERTEX vtx[544];
} TEXTUREBUCKET;

typedef struct TEXTURE {
    LPDIRECT3DTEXTURE2 tex;
    LPDIRECTDRAWSURFACE4 surface;
    uint32 xoff;
    uint32 yoff;
    uint32 width;
    uint32 height;
    sint32 tpage;
    uint8 bump;
    sint32 bumptpage;
} TEXTURE;

extern TEXTUREBUCKET Bucket[20];
#define FVF (D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW)

extern TEXTURE *Textures;
extern sint32 nTextures;
extern float clip_left;
extern float clip_top;
extern float clip_right;
extern float clip_bottom;
extern sint32 bWaterEffect;

void InsertRoom(ROOM_INFO *r);
void ProcessMeshData(sint32 num_meshes);
void ProcessRoomData(ROOM_INFO *r);

void InitBuckets(void);
void DrawBucket(TEXTUREBUCKET *bucket);
void DrawBuckets(void);
void CalcTriFaceNormal(D3DVECTOR *p1, D3DVECTOR *p2, D3DVECTOR *p3, D3DVECTOR *N);
void FindBucket(sint32 tpage, D3DTLBUMPVERTEX **Vpp, sint32 **nVtxpp);
void PrelightVertsMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r);
void PrelightVertsNonMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r);
void ProjectVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip);
void ProjectWaterVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip);
void ProjectShoreVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip);
void CreateVertexNormals(ROOM_INFO *r);

#endif
