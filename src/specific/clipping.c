#include "clipping.h"
#include "3dmath.h"
#include "../text.h"

sint32 ZClipper(sint32 n, D3DTLBUMPVERTEX *in, D3DTLBUMPVERTEX *out)
{
    D3DTLBUMPVERTEX *pIn;
    D3DTLBUMPVERTEX *pOut;
    D3DTLBUMPVERTEX *last;
    float lastZ, inZ, dz, iR, iG, iB, iA, sR, sG, sB, sA, f;
    sint32 nPoints, r, g, b, a;

    pIn = in;
    last = &in[n - 1];
    pOut = out;
    for (nPoints = 0; n--; last = pIn++) {
        lastZ = f_mznear - last->sz;
        inZ = f_mznear - pIn->sz;
        if ((*(sint32 *)&lastZ | *(sint32 *)&inZ) >= 0)
            continue;
        if ((*(sint32 *)&lastZ ^ *(sint32 *)&inZ) < 0) {
            dz = inZ / (last->sz - pIn->sz);
            pOut->sx = ((last->tx - pIn->tx) * dz + pIn->tx) * f_mperspoznear + f_centerx;
            pOut->sy = ((last->ty - pIn->ty) * dz + pIn->ty) * f_mperspoznear + f_centery;
            pOut->rhw = f_moneoznear;
            pOut->tu = ((last->tu - pIn->tu) * dz + pIn->tu) * pOut->rhw;
            pOut->tv = ((last->tv - pIn->tv) * dz + pIn->tv) * pOut->rhw;
            iA = (float)(pIn->color >> 24);
            iR = (float)CLRR(pIn->color);
            iG = (float)CLRG(pIn->color);
            iB = (float)CLRB(pIn->color);
            f = ((float)(last->color >> 24) - iA) * dz + iA + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = ((float)CLRR(last->color) - iR) * dz + iR + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = ((float)CLRG(last->color) - iG) * dz + iG + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = ((float)CLRB(last->color) - iB) * dz + iB + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            pOut->color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            sA = (float)(pIn->specular >> 24);
            sR = (float)CLRR(pIn->specular);
            sG = (float)CLRG(pIn->specular);
            sB = (float)CLRB(pIn->specular);
            f = ((float)(last->specular >> 24) - sA) * dz + sA + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = ((float)CLRR(last->specular) - sR) * dz + sR + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = ((float)CLRG(last->specular) - sG) * dz + sG + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = ((float)CLRB(last->specular) - sB) * dz + sB + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            pOut->specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            pOut++;
            nPoints++;
        }
        if (*(sint32 *)&inZ < 0) {
            pOut->sx = pIn->sx;
            pOut->sy = pIn->sy;
            pOut->rhw = pIn->rhw;
            pOut->tu = pIn->tu * pIn->rhw;
            pOut->tv = pIn->tv * pIn->rhw;
            pOut->color = pIn->color;
            pOut->specular = pIn->specular;
            pOut++;
            nPoints++;
        }
    }
    if (nPoints < 3)
        nPoints = 0;
    return nPoints;
}
