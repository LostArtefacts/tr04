#ifndef SPECIFIC_DRAWROOM_H
#define SPECIFIC_DRAWROOM_H

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

void InsertRoom(ROOM_INFO *r);

void InitBuckets(void);

#endif
