#include "drawroom.h"

void InitBuckets(void)
{
    sint32 n;

    for (n = 0; n < 20; n++) {
        Bucket[n].tpage = -1;
        Bucket[n].nVtx = 0;
    }
}

void FindBucket(sint32 tpage, D3DTLBUMPVERTEX **Vpp, sint32 **nVtxpp)
{
    TEXTUREBUCKET *bucket;
    sint32 i, nVtx, biggest;

    bucket = Bucket;
    for (i = 0; i < 20; i++, bucket++) {
        if (bucket->tpage == tpage && bucket->nVtx < 512) {
            *Vpp = &bucket->vtx[bucket->nVtx];
            *nVtxpp = &bucket->nVtx;
            return;
        }
        if (bucket->nVtx > 512) {
            DrawBucket(&Bucket[i]);
            bucket->tpage = tpage;
            bucket->nVtx = 0;
            *Vpp = bucket->vtx;
            *nVtxpp = &bucket->nVtx;
            return;
        }
    }
    biggest = 0;
    nVtx = 0;
    bucket = Bucket;
    for (i = 0; i < 20; i++, bucket++) {
        if (bucket->tpage == -1) {
            bucket->tpage = tpage;
            *Vpp = bucket->vtx;
            *nVtxpp = &bucket->nVtx;
            return;
        }
        if (bucket->nVtx > nVtx) {
            nVtx = bucket->nVtx;
            biggest = i;
        }
    }
    DrawBucket(&Bucket[biggest]);
    Bucket[biggest].tpage = tpage;
    Bucket[biggest].nVtx = 0;
    *Vpp = Bucket[biggest].vtx;
    *nVtxpp = &Bucket[biggest].nVtx;
}

void CalcTriFaceNormal(D3DVECTOR *p1, D3DVECTOR *p2, D3DVECTOR *p3, D3DVECTOR *N)
{
    float ux, uy, uz, vx, vy, vz;

    ux = p1->x - p2->x;
    uy = p1->y - p2->y;
    uz = p1->z - p2->z;
    vx = p3->x - p2->x;
    vy = p3->y - p2->y;
    vz = p3->z - p2->z;
    N->x = vz * uy - vy * uz;
    N->y = vx * uz - vz * ux;
    N->z = vy * ux - vx * uy;
}
