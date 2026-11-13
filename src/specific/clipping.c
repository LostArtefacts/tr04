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

sint32 XYUVGClipper(sint32 n, D3DTLBUMPVERTEX *in)
{
    D3DTLBUMPVERTEX *v1;
    D3DTLBUMPVERTEX *v2;
    D3DTLBUMPVERTEX output[8];
    float cr1, cg1, cb1, ca1, sr1, sg1, sb1, sa1;
    float cr2, cg2, cb2, ca2, sr2, sg2, sb2, sa2;
    float clipper, f;
    sint32 nPoints, r, g, b, a, i;

    v2 = &in[n - 1];
    ca2 = (float)(v2->color >> 24);
    cr2 = (float)CLRR(v2->color);
    cg2 = (float)CLRG(v2->color);
    cb2 = (float)CLRB(v2->color);
    sa2 = (float)(v2->specular >> 24);
    sr2 = (float)CLRR(v2->specular);
    sg2 = (float)CLRG(v2->specular);
    sb2 = (float)CLRB(v2->specular);
    nPoints = 0;
    for (i = 0; i < n; i++) {
        v1 = v2;
        ca1 = ca2;
        cr1 = cr2;
        cg1 = cg2;
        cb1 = cb2;
        sr1 = sr2;
        sg1 = sg2;
        sb1 = sb2;
        sa1 = sa2;
        v2 = &in[i];
        ca2 = (float)(v2->color >> 24);
        cr2 = (float)CLRR(v2->color);
        cg2 = (float)CLRG(v2->color);
        cb2 = (float)CLRB(v2->color);
        sa2 = (float)(v2->specular >> 24);
        sr2 = (float)CLRR(v2->specular);
        sg2 = (float)CLRG(v2->specular);
        sb2 = (float)CLRB(v2->specular);
        if (v1->sx < f_left) {
            if (v2->sx < f_left)
                continue;
            clipper = (f_left - v2->sx) / (v1->sx - v2->sx);
            output[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            output[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            output[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            output[nPoints].sx = f_left;
            output[nPoints].sy = (v1->sy - v2->sy) * clipper + v2->sy;
            nPoints++;
        } else if (v1->sx > f_right) {
            if (v2->sx > f_right)
                continue;
            clipper = (f_right - v2->sx) / (v1->sx - v2->sx);
            output[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            output[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            output[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            output[nPoints].sx = f_right;
            output[nPoints].sy = (v1->sy - v2->sy) * clipper + v2->sy;
            nPoints++;
        }
        if (v2->sx < f_left) {
            clipper = (f_left - v2->sx) / (v1->sx - v2->sx);
            output[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            output[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            output[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            output[nPoints].sx = f_left;
            output[nPoints].sy = (v1->sy - v2->sy) * clipper + v2->sy;
            nPoints++;
        } else if (v2->sx > f_right) {
            clipper = (f_right - v2->sx) / (v1->sx - v2->sx);
            output[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            output[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            output[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            output[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            output[nPoints].sx = f_right;
            output[nPoints].sy = (v1->sy - v2->sy) * clipper + v2->sy;
            nPoints++;
        } else {
            output[nPoints].sx = v2->sx;
            output[nPoints].sy = v2->sy;
            output[nPoints].sz = v2->sz;
            output[nPoints].rhw = v2->rhw;
            output[nPoints].tu = v2->tu;
            output[nPoints].tv = v2->tv;
            output[nPoints].color = v2->color;
            output[nPoints].specular = v2->specular;
            nPoints++;
        }
    }
    if (nPoints < 3)
        return 0;
    n = nPoints;
    v2 = &output[n - 1];
    ca2 = (float)(v2->color >> 24);
    cr2 = (float)CLRR(v2->color);
    cg2 = (float)CLRG(v2->color);
    cb2 = (float)CLRB(v2->color);
    sa2 = (float)(v2->specular >> 24);
    sr2 = (float)CLRR(v2->specular);
    sg2 = (float)CLRG(v2->specular);
    sb2 = (float)CLRB(v2->specular);
    nPoints = 0;
    for (i = 0; i < n; i++) {
        v1 = v2;
        ca1 = ca2;
        cr1 = cr2;
        cg1 = cg2;
        cb1 = cb2;
        sr1 = sr2;
        sg1 = sg2;
        sb1 = sb2;
        sa1 = sa2;
        v2 = &output[i];
        ca2 = (float)(v2->color >> 24);
        cr2 = (float)CLRR(v2->color);
        cg2 = (float)CLRG(v2->color);
        cb2 = (float)CLRB(v2->color);
        sa2 = (float)(v2->specular >> 24);
        sr2 = (float)CLRR(v2->specular);
        sg2 = (float)CLRG(v2->specular);
        sb2 = (float)CLRB(v2->specular);
        if (v1->sy < f_top) {
            if (v2->sy < f_top)
                continue;
            clipper = (f_top - v2->sy) / (v1->sy - v2->sy);
            in[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            in[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            in[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            in[nPoints].sx = (v1->sx - v2->sx) * clipper + v2->sx;
            in[nPoints].sy = f_top;
            nPoints++;
        } else if (v1->sy > f_bottom) {
            if (v2->sy > f_bottom)
                continue;
            clipper = (f_bottom - v2->sy) / (v1->sy - v2->sy);
            in[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            in[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            in[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            in[nPoints].sx = (v1->sx - v2->sx) * clipper + v2->sx;
            in[nPoints].sy = f_bottom;
            nPoints++;
        }
        if (v2->sy < f_top) {
            clipper = (f_top - v2->sy) / (v1->sy - v2->sy);
            in[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            in[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            in[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            in[nPoints].sx = (v1->sx - v2->sx) * clipper + v2->sx;
            in[nPoints].sy = f_top;
            nPoints++;
        } else if (v2->sy > f_bottom) {
            clipper = (f_bottom - v2->sy) / (v1->sy - v2->sy);
            in[nPoints].rhw = (v1->rhw - v2->rhw) * clipper + v2->rhw;
            in[nPoints].tu = (v1->tu - v2->tu) * clipper + v2->tu;
            in[nPoints].tv = (v1->tv - v2->tv) * clipper + v2->tv;
            f = (ca1 - ca2) * clipper + ca2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (cr1 - cr2) * clipper + cr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (cg1 - cg2) * clipper + cg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (cb1 - cb2) * clipper + cb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].color = b | ((g | ((r | (a << 8)) << 8)) << 8);
            f = (sa1 - sa2) * clipper + sa2 + 12582912.0F;
            a = *(sint32 *)&f & 0x3FFFFF;
            f = (sr1 - sr2) * clipper + sr2 + 12582912.0F;
            r = *(sint32 *)&f & 0x3FFFFF;
            f = (sg1 - sg2) * clipper + sg2 + 12582912.0F;
            g = *(sint32 *)&f & 0x3FFFFF;
            f = (sb1 - sb2) * clipper + sb2 + 12582912.0F;
            b = *(sint32 *)&f & 0x3FFFFF;
            in[nPoints].specular = b | ((g | ((r | (a << 8)) << 8)) << 8);
            in[nPoints].sx = (v1->sx - v2->sx) * clipper + v2->sx;
            in[nPoints].sy = f_bottom;
            nPoints++;
        } else {
            in[nPoints].sx = v2->sx;
            in[nPoints].sy = v2->sy;
            in[nPoints].sz = v2->sz;
            in[nPoints].rhw = v2->rhw;
            in[nPoints].tu = v2->tu;
            in[nPoints].tv = v2->tv;
            in[nPoints].color = v2->color;
            in[nPoints].specular = v2->specular;
            nPoints++;
        }
    }
    if (nPoints < 3)
        return 0;
    return nPoints;
}


sint32 visible_zclip(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2)
{
    float a, b, c;

    a = (v2->tu * v0->sz - v2->sz * v0->tu) * v1->tv;
    b = (v2->sz * v0->tv - v2->tv * v0->sz) * v1->tu;
    c = (v2->tv * v0->tu - v2->tu * v0->tv) * v1->sz;
    if (a + b + c < 0)
        return 1;
    return 0;
}
