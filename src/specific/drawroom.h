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

extern TEXTUREBUCKET Bucket[20];
extern sint32 bWaterEffect;

void InsertRoom(ROOM_INFO *r);

void InitBuckets(void);
void DrawBucket(TEXTUREBUCKET *bucket);
void DrawBuckets(void);
void CalcTriFaceNormal(D3DVECTOR *p1, D3DVECTOR *p2, D3DVECTOR *p3, D3DVECTOR *N);
void FindBucket(sint32 tpage, D3DTLBUMPVERTEX **Vpp, sint32 **nVtxpp);
void PrelightVertsMMX(sint32 nVerts, D3DTLVERTEX *v, ROOM_INFO *r);

#endif
