#include "drawroom.h"

void InitBuckets(void)
{
    sint32 n;

    for (n = 0; n < 20; n++) {
        Bucket[n].tpage = -1;
        Bucket[n].nVtx = 0;
    }
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
