#include <windows.h>
#include <d3d.h>

#include "specificfx.h"
#include "3dmath.h"
#include "../anim.h"
#include "../objnums.h"
#include "../tomb4fx.h"
#include "function_table.h"
#include "polyinsert.h"
#include "../gameflow.h"
#include "winmain.h"
#include "file.h"
#include "drawroom.h"
#include "../control.h"
#include "../text.h"
#include "../lara.h"
#include "function_stubs.h"
#include "../rope.h"
#include "../camera.h"
#include "../wraith.h"
#include "../effect2.h"
#include "../delstuff.h"

sint32 ShadowTable[42] = {
    4, 1, 5, 5, 1, 6, 6, 1, 2, 6, 2, 7, 8, 4, 9, 9, 4, 5, 9, 5, 10, 10, 5, 6, 10, 6, 11, 11, 6, 7,
    13, 8, 9, 13, 9, 14, 14, 9, 10, 14, 10, 11
};

uint8 TargetGraphColTab[48] = {
    0, 0, 255, 0, 0, 255, 255, 255, 0, 255, 255, 0, 0, 0, 255, 0, 0, 255, 255, 255, 0, 255, 255, 0,
    0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0
};

void setXY4(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 x3, sint32 y3, sint32 x4, sint32 y4, sint32 z, sint16 *clip)
{
    float zv;
    sint32 clip_distance;

    clip_distance = 0;
    zv = f_mpersp / (float)z;
    v->sx = (float)x1;
    v->sy = (float)y1;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x1 < phd_winxmin)
        clip_distance = 1;
    else if (x1 > phd_winxmax)
        clip_distance = 2;
    if (y1 < phd_winymin)
        clip_distance += 4;
    else if (y1 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    clip_distance = 0;
    v->sx = (float)x2;
    v->sy = (float)y2;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x2 < phd_winxmin)
        clip_distance = 1;
    else if (x2 > phd_winxmax)
        clip_distance = 2;
    if (y2 < phd_winymin)
        clip_distance += 4;
    else if (y2 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    clip_distance = 0;
    v->sx = (float)x3;
    v->sy = (float)y3;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x3 < phd_winxmin)
        clip_distance = 1;
    else if (x3 > phd_winxmax)
        clip_distance = 2;
    if (y3 < phd_winymin)
        clip_distance += 4;
    else if (y3 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    v->sx = (float)x4;
    v->sy = (float)y4;
    v->sz = (float)z;
    v->rhw = zv * f_moneopersp;
    clip_distance = 0;
    if (x4 < phd_winxmin)
        clip_distance = 1;
    else if (x4 > phd_winxmax)
        clip_distance = 2;
    if (y4 < phd_winymin)
        clip_distance += 4;
    else if (y4 > phd_winymax)
        clip_distance += 8;
    *clip = clip_distance;
}

void setXY3(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 x3, sint32 y3, sint32 z, sint16 *clip)
{
    float zv;
    sint32 clip_distance;

    clip_distance = 0;
    zv = f_mpersp / (float)z;
    v->sx = (float)x1;
    v->sy = (float)y1;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x1 < phd_winxmin)
        clip_distance = 1;
    else if (x1 > phd_winxmax)
        clip_distance = 2;
    if (y1 < phd_winymin)
        clip_distance += 4;
    else if (y1 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    clip_distance = 0;
    v->sx = (float)x2;
    v->sy = (float)y2;
    v->sz = (float)z;
    v->rhw = f_moneopersp * zv;
    if (x2 < phd_winxmin)
        clip_distance = 1;
    else if (x2 > phd_winxmax)
        clip_distance = 2;
    if (y2 < phd_winymin)
        clip_distance += 4;
    else if (y2 > phd_winymax)
        clip_distance += 8;
    *clip++ = clip_distance;
    v++;
    v->sx = (float)x3;
    v->sy = (float)y3;
    v->sz = (float)z;
    v->rhw = zv * f_moneopersp;
    clip_distance = 0;
    if (x3 < phd_winxmin)
        clip_distance = 1;
    else if (x3 > phd_winxmax)
        clip_distance = 2;
    if (y3 < phd_winymin)
        clip_distance += 4;
    else if (y3 > phd_winymax)
        clip_distance += 8;
    *clip = clip_distance;
}

void setXYZ4(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 z1, sint32 x2, sint32 y2, sint32 z2, sint32 x3, sint32 y3, sint32 z3, sint32 x4, sint32 y4, sint32 z4, sint16 *clip)
{
    float zv, x, y, z;
    sint32 clip_distance;

    clip_distance = 0;
    z = (float)z1;
    y = (float)y1;
    x = (float)x1;
    v->sz = z;
    v->tu = x;
    v->tv = y;
    if (z >= f_mznear) {
        zv = f_mpersp / z;
        if (z > FogEnd) {
            clip_distance = 256;
            v->sz = f_zfar;
        }
        v->sx = zv * x + f_centerx;
        v->sy = zv * y + f_centery;
        v->rhw = f_moneopersp * zv;
        if (phd_winxmin > v->sx)
            clip_distance++;
        else if (phd_winxmax < v->sx)
            clip_distance += 2;
        if (phd_winymin > v->sy)
            clip_distance += 4;
        else if (phd_winymax < v->sy)
            clip_distance += 8;
    } else
        clip_distance = -128;
    v++;
    *clip++ = clip_distance;
    clip_distance = 0;
    z = (float)z2;
    y = (float)y2;
    x = (float)x2;
    v->sz = z;
    v->tu = x;
    v->tv = y;
    if (z >= f_mznear) {
        zv = f_mpersp / z;
        if (z > FogEnd) {
            clip_distance = 256;
            v->sz = f_zfar;
        }
        v->sx = zv * x + f_centerx;
        v->sy = zv * y + f_centery;
        v->rhw = f_moneopersp * zv;
        if (phd_winxmin > v->sx)
            clip_distance++;
        else if (phd_winxmax < v->sx)
            clip_distance += 2;
        if (phd_winymin > v->sy)
            clip_distance += 4;
        else if (phd_winymax < v->sy)
            clip_distance += 8;
    } else
        clip_distance = -128;
    v++;
    *clip++ = clip_distance;
    clip_distance = 0;
    z = (float)z3;
    y = (float)y3;
    x = (float)x3;
    v->sz = z;
    v->tu = x;
    v->tv = y;
    if (z >= f_mznear) {
        zv = f_mpersp / z;
        if (z > FogEnd) {
            clip_distance = 256;
            v->sz = f_zfar;
        }
        v->sx = zv * x + f_centerx;
        v->sy = zv * y + f_centery;
        v->rhw = f_moneopersp * zv;
        if (phd_winxmin > v->sx)
            clip_distance++;
        else if (phd_winxmax < v->sx)
            clip_distance += 2;
        if (phd_winymin > v->sy)
            clip_distance += 4;
        else if (phd_winymax < v->sy)
            clip_distance += 8;
    } else
        clip_distance = -128;
    v++;
    *clip++ = clip_distance;
    clip_distance = 0;
    z = (float)z4;
    y = (float)y4;
    x = (float)x4;
    v->sz = z;
    v->tu = x;
    v->tv = y;
    if (z >= f_mznear) {
        zv = f_mpersp / z;
        if (z > FogEnd) {
            clip_distance = 256;
            v->sz = f_zfar;
        }
        v->sx = zv * x + f_centerx;
        v->sy = zv * y + f_centery;
        v->rhw = zv * f_moneopersp;
        if (phd_winxmin > v->sx)
            clip_distance++;
        else if (phd_winxmax < v->sx)
            clip_distance += 2;
        if (phd_winymin > v->sy)
            clip_distance += 4;
        else if (phd_winymax < v->sy)
            clip_distance += 8;
    } else
        clip_distance = -128;
    *clip = clip_distance;
}

void setXYZ3(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 z1, sint32 x2, sint32 y2, sint32 z2, sint32 x3, sint32 y3, sint32 z3, sint16 *clip)
{
    float zv, x, y, z;
    sint32 clip_distance;

    clip_distance = 0;
    z = (float)z1;
    y = (float)y1;
    x = (float)x1;
    v->sz = z;
    v->tu = x;
    v->tv = y;
    if (z >= f_mznear) {
        zv = f_mpersp / z;
        if (z > FogEnd) {
            clip_distance = 256;
            v->sz = f_zfar;
        }
        v->sx = zv * x + f_centerx;
        v->sy = zv * y + f_centery;
        v->rhw = f_moneopersp * zv;
        if (phd_winxmin > v->sx)
            clip_distance++;
        else if (phd_winxmax < v->sx)
            clip_distance += 2;
        if (phd_winymin > v->sy)
            clip_distance += 4;
        else if (phd_winymax < v->sy)
            clip_distance += 8;
    } else
        clip_distance = -128;
    v++;
    *clip++ = clip_distance;
    clip_distance = 0;
    z = (float)z2;
    y = (float)y2;
    x = (float)x2;
    v->sz = z;
    v->tu = x;
    v->tv = y;
    if (z >= f_mznear) {
        zv = f_mpersp / z;
        if (z > FogEnd) {
            clip_distance = 256;
            v->sz = f_zfar;
        }
        v->sx = zv * x + f_centerx;
        v->sy = zv * y + f_centery;
        v->rhw = f_moneopersp * zv;
        if (phd_winxmin > v->sx)
            clip_distance++;
        else if (phd_winxmax < v->sx)
            clip_distance += 2;
        if (phd_winymin > v->sy)
            clip_distance += 4;
        else if (phd_winymax < v->sy)
            clip_distance += 8;
    } else
        clip_distance = -128;
    v++;
    *clip++ = clip_distance;
    clip_distance = 0;
    z = (float)z3;
    y = (float)y3;
    x = (float)x3;
    v->sz = z;
    v->tu = x;
    v->tv = y;
    if (z >= f_mznear) {
        zv = f_mpersp / z;
        if (z > FogEnd) {
            clip_distance = 256;
            v->sz = f_zfar;
        }
        v->sx = zv * x + f_centerx;
        v->sy = zv * y + f_centery;
        v->rhw = f_moneopersp * zv;
        if (phd_winxmin > v->sx)
            clip_distance++;
        else if (phd_winxmax < v->sx)
            clip_distance += 2;
        if (phd_winymin > v->sy)
            clip_distance += 4;
        else if (phd_winymax < v->sy)
            clip_distance += 8;
    } else
        clip_distance = -128;
    *clip = clip_distance;
}

void DrawBubbles(void)
{
    BUBBLE_STRUCT *bubble;
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    PHD_VECTOR pos;
    sint32 *mptr;
    sint32 *Z;
    sint16 *XY;
    sint16 *offsets;
    float perspz, u1, v1, u2;
    sint32 shade;
    sint32 i, dx, dy, dz, size, x1, y1, x2, y2;

    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
    mptr = phd_mxptr;
    bubble = Bubbles;
    XY = (sint16 *)&scratchpad[0];
    Z = (sint32 *)&scratchpad[256];
    offsets = (sint16 *)&scratchpad[512];
    for (i = 40; i > 0; i--) {
        if (!bubble->size) {
            bubble++;
            continue;
        }
        dx = bubble->pos.x - lara_item->pos.x_pos;
        dy = bubble->pos.y - lara_item->pos.y_pos;
        dz = bubble->pos.z - lara_item->pos.z_pos;
        if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000) {
            bubble->size = 0;
            bubble++;
            continue;
        }
        offsets[0] = (sint16)dx;
        offsets[1] = (sint16)dy;
        offsets[2] = (sint16)dz;
        pos.x = offsets[0] * mptr[M00] + offsets[1] * mptr[M01] + offsets[2] * mptr[M02] + mptr[M03];
        pos.y = offsets[0] * mptr[M10] + offsets[1] * mptr[M11] + offsets[2] * mptr[M12] + mptr[M13];
        pos.z = offsets[0] * mptr[M20] + offsets[1] * mptr[M21] + offsets[2] * mptr[M22] + mptr[M23];
        perspz = f_persp / (float)pos.z;
        XY[0] = (sint16)(pos.x * perspz + f_centerx);
        XY[1] = (sint16)(pos.y * perspz + f_centery);
        Z[0] = pos.z >> W2V_SHIFT;
        if (Z[0] < 32 || Z[0] >= 0x5000) {
            if (Z[0] >= 0x5000)
                bubble->size = 0;
            bubble++;
            continue;
        }
        size = phd_persp * (bubble->size >> 1) / Z[0];
        if (size > 128) {
            bubble->size = 0;
            continue;
        }
        if (size < 4)
            size = 4;
        size >>= 1;
        x1 = XY[0] - size;
        x2 = XY[0] + size;
        y1 = XY[1] - size;
        y2 = XY[1] + size;
        if (x2 < phd_winxmin || x1 >= phd_winxmax || y2 < phd_winymin || y1 >= phd_winymax) {
            bubble++;
            continue;
        }
        sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 13];
        shade = bubble->shade;
        setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
        v[0].color = RGBA(shade, shade, shade, 0xFF);
        v[1].color = RGBA(shade, shade, shade, 0xFF);
        v[2].color = RGBA(shade, shade, shade, 0xFF);
        v[3].color = RGBA(shade, shade, shade, 0xFF);
        v[0].specular = 0xFF000000;
        v[1].specular = 0xFF000000;
        v[2].specular = 0xFF000000;
        v[3].specular = 0xFF000000;
        u1 = sprite->x1;
        u2 = sprite->x2;
        v1 = sprite->y1;
        tex.drawtype = 2;
        tex.flag = 0;
        tex.tpage = sprite->tpage;
        tex.u1 = u1;
        tex.v1 = v1;
        tex.u2 = u2;
        tex.v3 = sprite->y2;
        tex.v2 = v1;
        tex.u3 = u2;
        tex.u4 = u1;
        tex.v4 = sprite->y2;
        AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
        bubble++;
    }
    phd_PopMatrix();
}

void S_DrawFireSparks(sint16 size, sint32 life)
{
    FIRE_SPARKS *sptr;
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    PHD_VECTOR pos;
    sint32 *Z;
    sint16 *XY;
    sint16 *offsets;
    sint32 *mptr;
    float perspz, u1, v1, u2;
    uint32 r, g, b, col;
    sint32 cr, cg, cb, i, s, c, ss, cs, sm, cm;
    sint32 dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;
    sint16 newSize;

    XY = (sint16 *)&scratchpad[0];
    Z = (sint32 *)&scratchpad[256];
    offsets = (sint16 *)&scratchpad[512];
    sptr = &fire_spark[0];
    mptr = phd_mxptr;
    for (i = 0; i < 20; i++, sptr++) {
        if (!sptr->On)
            continue;
        dx = sptr->x >> (2 - size);
        dy = sptr->y >> (2 - size);
        dz = sptr->z >> (2 - size);
        if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000)
            continue;
        offsets[0] = (sint16)dx;
        offsets[1] = (sint16)dy;
        offsets[2] = (sint16)dz;
        pos.x = offsets[0] * mptr[M00] + offsets[1] * mptr[M01] + offsets[2] * mptr[M02] + mptr[M03];
        pos.y = offsets[0] * mptr[M10] + offsets[1] * mptr[M11] + offsets[2] * mptr[M12] + mptr[M13];
        pos.z = offsets[0] * mptr[M20] + offsets[1] * mptr[M21] + offsets[2] * mptr[M22] + mptr[M23];
        perspz = f_persp / (float)pos.z;
        XY[0] = (sint16)(pos.x * perspz + f_centerx);
        XY[1] = (sint16)(pos.y * perspz + f_centery);
        Z[0] = pos.z >> W2V_SHIFT;
        if (Z[0] <= 0 || Z[0] >= 0x5000)
            continue;
        newSize = ((phd_persp * sptr->Size) << 2) / Z[0];
        newSize >>= 2 - size;
        if (newSize > (sptr->Size << 2))
            newSize = sptr->Size << 2;
        else if (newSize < 4)
            newSize = 4;
        if (XY[0] + (newSize >> 1) < phd_winxmin || XY[0] - (newSize >> 1) >= phd_winxmax || XY[1] + (newSize >> 1) < phd_winymin || XY[1] - (newSize >> 1) >= phd_winymax)
            continue;
        if (sptr->Flags & 0x10) {
            s = rcossin_tbl[sptr->RotAng << 1];
            c = rcossin_tbl[(sptr->RotAng << 1) + 1];
            ss = (s * (newSize >> 1)) >> (W2V_SHIFT - 2);
            cs = (c * (newSize >> 1)) >> (W2V_SHIFT - 2);
            sm = (s * (-newSize >> 1)) >> (W2V_SHIFT - 2);
            cm = (c * (-newSize >> 1)) >> (W2V_SHIFT - 2);
            x1 = sm + XY[0] - cm;
            y1 = sm + XY[1] + cm;
            x2 = XY[0] - cm + ss;
            y2 = sm + XY[1] + cs;
            x3 = ss + XY[0] - cs;
            y3 = cs + XY[1] + ss;
            x4 = sm + XY[0] - cs;
            y4 = ss + XY[1] + cm;
            setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);
        } else {
            x1 = XY[0] - (newSize >> 1);
            y1 = XY[1] - (newSize >> 1);
            x2 = XY[0] + (newSize >> 1);
            y2 = XY[1] + (newSize >> 1);
            setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
        }
        sprite = &spriteinfo[sptr->Def];
        if (Z[0] > 0x3000) {
            r = ((0x5000 - Z[0]) * sptr->R) >> 13;
            g = ((0x5000 - Z[0]) * sptr->G) >> 13;
            b = ((0x5000 - Z[0]) * sptr->B) >> 13;
            col = RGBA((r * life) >> 8, (g * life) >> 8, (b * life) >> 8, 0xFF);
        } else {
            cr = (sptr->R * life) >> 8;
            cg = (sptr->G * life) >> 8;
            cb = (sptr->B * life) >> 8;
            col = RGBA(cr, cg, cb, 0xFF);
        }
        v[0].color = col;
        v[1].color = col;
        v[2].color = col;
        v[3].color = col;
        v[0].specular = 0xFF000000;
        v[1].specular = 0xFF000000;
        v[2].specular = 0xFF000000;
        v[3].specular = 0xFF000000;
        u1 = sprite->x1;
        u2 = sprite->x2;
        v1 = sprite->y1;
        tex.drawtype = 2;
        tex.flag = 0;
        tex.tpage = sprite->tpage;
        tex.u1 = u1;
        tex.v1 = v1;
        tex.u2 = u2;
        tex.v3 = sprite->y2;
        tex.v2 = v1;
        tex.u3 = u2;
        tex.u4 = u1;
        tex.v4 = sprite->y2;
        AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
        AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
    }
}

void S_DrawSmokeSparks(void)
{
    SMOKE_SPARKS *sptr;
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    PHD_VECTOR pos;
    sint32 *Z;
    sint16 *XY;
    sint16 *offsets;
    sint32 *mptr;
    float perspz, u1, v1, u2;
    sint32 i, is_mirror, size, col, s, c, ss, cs, sm, cm;
    sint32 dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;

    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
    mptr = phd_mxptr;
    XY = (sint16 *)&scratchpad[0];
    Z = (sint32 *)&scratchpad[256];
    offsets = (sint16 *)&scratchpad[512];
    sptr = &smoke_spark[0];
    is_mirror = 0;
    for (i = 0; i < 32; i++) {
        if (!sptr->On) {
            sptr++;
            continue;
        }
        if (sptr->mirror && !is_mirror)
            is_mirror = 1;
        else
            is_mirror = 0;
        dx = sptr->x - lara_item->pos.x_pos;
        dy = sptr->y - lara_item->pos.y_pos;
        dz = sptr->z - lara_item->pos.z_pos;
        if (is_mirror)
            dz = 2 * gfMirrorZPlane - lara_item->pos.z_pos - sptr->z;
        if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000) {
            if (!is_mirror)
                sptr++;
            continue;
        }
        offsets[0] = (sint16)dx;
        offsets[1] = (sint16)dy;
        offsets[2] = (sint16)dz;
        pos.x = offsets[0] * mptr[M00] + offsets[1] * mptr[M01] + offsets[2] * mptr[M02] + mptr[M03];
        pos.y = offsets[0] * mptr[M10] + offsets[1] * mptr[M11] + offsets[2] * mptr[M12] + mptr[M13];
        pos.z = offsets[0] * mptr[M20] + offsets[1] * mptr[M21] + offsets[2] * mptr[M22] + mptr[M23];
        perspz = f_persp / (float)pos.z;
        XY[0] = (sint16)(pos.x * perspz + f_centerx);
        XY[1] = (sint16)(pos.y * perspz + f_centery);
        Z[0] = pos.z >> W2V_SHIFT;
        if (Z[0] <= 0 || Z[0] >= 0x5000) {
            if (!is_mirror)
                sptr++;
            continue;
        }
        size = ((phd_persp * sptr->Size) << 2) / Z[0];
        if (size > (sptr->Size << 2))
            size = sptr->Size << 2;
        else if (size < 4)
            size = 4;
        if (XY[0] + (size >> 1) < phd_winxmin || XY[0] - (size >> 1) >= phd_winxmax || XY[1] + (size >> 1) < phd_winymin || XY[1] - (size >> 1) >= phd_winymax) {
            if (!is_mirror)
                sptr++;
            continue;
        }
        if (sptr->Flags & 0x10) {
            s = rcossin_tbl[sptr->RotAng << 1];
            c = rcossin_tbl[(sptr->RotAng << 1) + 1];
            ss = (s * (size >> 1)) >> (W2V_SHIFT - 2);
            cs = (c * (size >> 1)) >> (W2V_SHIFT - 2);
            sm = (s * (-size >> 1)) >> (W2V_SHIFT - 2);
            cm = (c * (-size >> 1)) >> (W2V_SHIFT - 2);
            x1 = sm + XY[0] - cm;
            y1 = sm + XY[1] + cm;
            x2 = XY[0] - cm + ss;
            y2 = sm + XY[1] + cs;
            x3 = ss + XY[0] - cs;
            y3 = cs + XY[1] + ss;
            x4 = sm + XY[0] - cs;
            y4 = ss + XY[1] + cm;
            setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);
        } else {
            x1 = XY[0] - (size >> 1);
            y1 = XY[1] - (size >> 1);
            x2 = XY[0] + (size >> 1);
            y2 = XY[1] + (size >> 1);
            setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, Z[0], clipflags);
        }
        sprite = &spriteinfo[sptr->Def];
        if (Z[0] > 0x3000)
            col = ((0x5000 - Z[0]) * sptr->Shade) >> 13;
        else
            col = sptr->Shade;
        v[0].color = RGBA(col, col, col, 0xFF);
        v[1].color = RGBA(col, col, col, 0xFF);
        v[2].color = RGBA(col, col, col, 0xFF);
        v[3].color = RGBA(col, col, col, 0xFF);
        v[0].specular = 0xFF000000;
        v[1].specular = 0xFF000000;
        v[2].specular = 0xFF000000;
        v[3].specular = 0xFF000000;
        u1 = sprite->x1;
        u2 = sprite->x2;
        v1 = sprite->y1;
        tex.drawtype = 2;
        tex.flag = 0;
        tex.tpage = sprite->tpage;
        tex.u1 = u1;
        tex.v1 = v1;
        tex.u2 = u2;
        tex.v3 = sprite->y2;
        tex.v2 = v1;
        tex.u3 = u2;
        tex.u4 = u1;
        tex.v4 = sprite->y2;
        AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
        if (!is_mirror)
            sptr++;
    }
    phd_PopMatrix();
}

void S_PrintShadow(sint32 size, sint16 *box, ITEM_INFO *item, sint32 unknown)
{
    PHD_VECTOR pos;
    FLOOR_INFO *floor;
    sint32 *sXYZ;
    sint32 *hXZ;
    sint32 *hY;
    sint32 sxyz[16 * 3];
    sint32 triA, triB, triC;
    sint32 i, j, x, y, z, xSize, zSize, xDist, zDist;
    sint16 room_number;

    xSize = (sint16)size * (box[1] - box[0]) / 192;
    zSize = (sint16)size * (box[5] - box[4]) / 192;
    xDist = xSize / 4;
    zDist = zSize / 4;
    {
        sint32 hxz[16 * 2];
        sint32 hy[16];

        x = -xDist - (xDist >> 1);
        z = zDist + (zDist >> 1);
        sXYZ = sxyz;
        hXZ = hxz;
        for (i = 0; i < 4; i++, z -= zDist) {
            for (j = 0; j < 4; j++, sXYZ += 3, hXZ += 2, x += xDist) {
                sXYZ[0] = x;
                sXYZ[2] = z;
                hXZ[0] = x;
                hXZ[1] = z;
            }
            x = -xDist - (xDist >> 1);
        }
        phd_PushUnitMatrix();
        if (item == lara_item) {
            pos.x = 0;
            pos.y = 0;
            pos.z = 0;
            GetLaraJointPos(&pos, LM_TORSO);
            room_number = lara_item->room_number;
            floor = GetFloor(pos.x, pos.y, pos.z, &room_number);
        y = GetHeight(floor, pos.x, pos.y, pos.z);
            if (y == NO_HEIGHT)
                y = item->floor;
        } else {
            pos.x = item->pos.x_pos;
            y = item->floor;
            pos.z = item->pos.z_pos;
        }
        y += -16;
        phd_TranslateRel(pos.x, y, pos.z);
        phd_RotY(item->pos.y_rot);
        hXZ = hxz;
        for (i = 0; i < 16; i++, hXZ += 2) {
            x = hXZ[0];
            z = hXZ[1];
            hXZ[0] = (x * phd_mxptr[M00] + z * phd_mxptr[M02] + phd_mxptr[M03]) >> W2V_SHIFT;
            hXZ[1] = (x * phd_mxptr[M20] + z * phd_mxptr[M22] + phd_mxptr[M23]) >> W2V_SHIFT;
        }
        phd_PopMatrix();
        hXZ = hxz;
        hY = hy;
        for (i = 0; i < 16; i++, hXZ += 2, hY++) {
            room_number = item->room_number;
            floor = GetFloor(hXZ[0], item->floor, hXZ[1], &room_number);
            *hY = GetHeight(floor, hXZ[0], item->floor, hXZ[1]);
            if (abs(*hY - item->floor) > 196)
                *hY = item->floor;
        }
        phd_PushMatrix();
        phd_TranslateAbs(pos.x, y, pos.z);
        phd_RotY(item->pos.y_rot);
        sXYZ = sxyz;
        hY = hy;
        for (i = 0; i < 16; i++, sXYZ += 3, hY++) {
            x = sXYZ[0];
            y = *hY - item->floor;
            z = sXYZ[2];
            sXYZ[0] = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z + phd_mxptr[M03]) >> W2V_SHIFT;
            sXYZ[1] = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z + phd_mxptr[M13]) >> W2V_SHIFT;
            sXYZ[2] = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z + phd_mxptr[M23]) >> W2V_SHIFT;
        }
        phd_PopMatrix();
    }
    sXYZ = sxyz;
    for (i = 0; i < 14; i++) {
        TEXTURESTRUCT tex;
        D3DTLVERTEX v[3];

        triA = 3 * ShadowTable[(i * 3) + 0];
        triB = 3 * ShadowTable[(i * 3) + 1];
        triC = 3 * ShadowTable[(i * 3) + 2];
        setXYZ3(v, sXYZ[triA], sXYZ[triA + 1], sXYZ[triA + 2], sXYZ[triB], sXYZ[triB + 1], sXYZ[triB + 2], sXYZ[triC], sXYZ[triC + 1], sXYZ[triC + 2], clipflags);
        v[0].color = 0x4F000000;
        v[1].color = 0x4F000000;
        v[2].color = 0x4F000000;
        if (item->after_death) {
            v[0].color = 0x80000000 - (item->after_death << 24);
            v[1].color = v[0].color;
            v[2].color = v[0].color;
        }
        v[0].specular = 0xFF000000;
        v[1].specular = 0xFF000000;
        v[2].specular = 0xFF000000;
        tex.flag = 0;
        tex.tpage = 0;
        tex.u1 = 0;
        tex.v1 = 0;
        tex.u2 = 0;
        tex.v2 = 0;
        tex.u3 = 0;
        tex.v3 = 0;
        tex.u4 = 0;
        tex.v4 = 0;
        tex.drawtype = 3;
        AddTriSorted(v, 0, 1, 2, &tex, 1);
    }
}

void InitTarget_2(void)
{
    OBJECT_INFO *obj;
    MESH_DATA *mesh;
    D3DTLVERTEX *v;
    sint32 i;

    obj = &objects[TARGET_GRAPHICS];
    if (!obj->loaded)
        return;
    mesh = (MESH_DATA *)meshes[obj->mesh_index];
    targetMeshP = mesh;
    IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB, DDLOCK_READONLY, (void **)&v, 0);
    for (i = 0; i < mesh->nVerts; i++) {
        v[i].sx = (1.0F / 96.0F) * (v[i].sx * 80.0F);
        v[i].sy = (1.0F / 224.0F) * (v[i].sy * 60.0F);
        v[i].color = 0xFF000000;
        v[i].specular = 0xFF000000;
        v[i].sz = 0;
        v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
    }
    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
}

void InitBinoculars(void)
{
    OBJECT_INFO *obj;
    MESH_DATA *mesh;
    D3DTLVERTEX *v;
    sint32 i;

    obj = &objects[BINOCULAR_GRAPHICS];
    if (!obj->loaded)
        return;
    mesh = (MESH_DATA *)meshes[obj->mesh_index];
    binocsMeshP = mesh;
    IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB, DDLOCK_READONLY, (void **)&v, 0);
    for (i = 0; i < mesh->nVerts; i++) {
        v[i].sx = (1.0F / 96.0F) * (v[i].sx * 32.0F);
        v[i].sy = (1.0F / 224.0F) * (v[i].sy * 30.0F);
        v[i].color = 0xFF000000;
        v[i].specular = 0xFF000000;
        v[i].sz = 0;
        v[i].rhw = f_mpersp / f_mznear * f_moneopersp;
    }
    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
}

void DrawBinoculars(void)
{
    MESH_DATA *mesh;
    D3DTLVERTEX *v;
    TEXTURESTRUCT *tex;
    D3DTLVERTEX vtx[256];
    sint16 *clip;
    sint16 *quad;
    sint16 *tri;
    sint32 i, j;
    uint16 drawbak;
    sint16 clipdistance;

    mesh = targetMeshP;
    if (!LaserSight)
        mesh = binocsMeshP;
    IDirect3DVertexBuffer_Lock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB, DDLOCK_READONLY, (void **)&v, 0);
    clip = clipflags;
    for (i = 0; i < mesh->nVerts; i++) {
        clipdistance = 0;
        vtx[i] = v[i];
        vtx[i].sx = vtx[i].sx * (float)(phd_winxmax / 512.0F);
        vtx[i].sy = vtx[i].sy * (float)(phd_winymax / 240.0F);
        vtx[i].sx = f_centerx + vtx[i].sx;
        vtx[i].sy = f_centery + vtx[i].sy;
        if (vtx[i].sx < f_left)
            clipdistance = 1;
        else if (vtx[i].sx > f_right)
            clipdistance = 2;
        if (vtx[i].sy < f_top)
            clipdistance += 4;
        else if (vtx[i].sy > f_bottom)
            clipdistance += 8;
        *clip++ = clipdistance;
    }
    IDirect3DVertexBuffer_Unlock((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB);
    quad = mesh->gt4;
    if (!LaserSight) {
        for (i = 0; i < mesh->ngt4; i++, quad += 6) {
            tex = &textinfo[quad[4] & 0x7FFF];
            drawbak = tex->drawtype;
            tex->drawtype = 0;
            if (quad[5] & 1) {
                vtx[quad[0]].color = 0xFF000000;
                vtx[quad[1]].color = 0xFF000000;
                vtx[quad[2]].color = 0;
                vtx[quad[3]].color = 0;
                tex->drawtype = 3;
            }
            AddQuadSorted(vtx, quad[0], quad[1], quad[2], quad[3], tex, 1);
            tex->drawtype = drawbak;
        }
        tri = mesh->gt3;
        for (i = 0; i < mesh->ngt3; i++, tri += 5) {
            tex = &textinfo[tri[3] & 0x7FFF];
            drawbak = tex->drawtype;
            tex->drawtype = 0;
            if (tri[4] & 1) {
                vtx[tri[0]].color = 0;
                vtx[tri[1]].color = 0xFF000000;
                vtx[tri[2]].color = 0;
                tex->drawtype = 3;
            }
            AddTriSorted(vtx, tri[0], tri[1], tri[2], tex, 1);
            tex->drawtype = drawbak;
        }
    } else {
        for (i = 0; i < mesh->ngt4; i++, quad += 6) {
            tex = &textinfo[quad[4] & 0x7FFF];
            drawbak = tex->drawtype;
            tex->drawtype = 0;
            if (quad[5] & 1) {
                vtx[quad[0]].color = 0xFF000000;
                vtx[quad[1]].color = 0xFF000000;
                vtx[quad[2]].color = 0;
                vtx[quad[3]].color = 0;
                tex->drawtype = 3;
            }
            AddQuadSorted(vtx, quad[0], quad[1], quad[2], quad[3], tex, 1);
            tex->drawtype = drawbak;
        }
        tri = mesh->gt3;
        for (i = 0, j = 0; i < mesh->ngt3; i++, tri += 5) {
            tex = &textinfo[tri[3] & 0x7FFF];
            drawbak = tex->drawtype;
            tex->drawtype = 0;
            if (tri[4] & 1) {
                vtx[tri[0]].color = TargetGraphColTab[j] << 24;
                vtx[tri[1]].color = TargetGraphColTab[j + 1] << 24;
                vtx[tri[2]].color = TargetGraphColTab[j + 2] << 24;
                j += 3;
                tex->drawtype = 3;
            }
            AddTriSorted(vtx, tri[0], tri[1], tri[2], tex, 1);
            tex->drawtype = drawbak;
        }
    }
}

uint8 ClipLine(sint32 *x1, sint32 *y1, sint32 z1, sint32 *x2, sint32 *y2, sint32 z2, sint32 xMin, sint32 yMin, sint32 w, sint32 h)
{
    float clip;

    if (z1 < 20 || z2 < 20)
        return 0;
    if (*x1 < xMin && *x2 < xMin || *y1 < yMin && *y2 < yMin)
        return 0;
    if (*x1 > w && *x2 > w || *y1 > h && *y2 > h)
        return 0;
    if (*x1 > w) {
        clip = ((float)w - *x2) / (float)(*x1 - *x2);
        *x1 = w;
        *y1 = (sint32)((*y1 - *y2) * clip + *y2);
    }
    if (*x2 > w) {
        clip = ((float)w - *x1) / (float)(*x2 - *x1);
        *x2 = w;
        *y2 = (sint32)((*y2 - *y1) * clip + *y1);
    }
    if (*x1 < xMin) {
        clip = ((float)xMin - *x1) / (float)(*x2 - *x1);
        *x1 = xMin;
        *y1 = (sint32)((*y2 - *y1) * clip + *y1);
    }
    if (*x2 < xMin) {
        clip = ((float)xMin - *x2) / (float)(*x1 - *x2);
        *x2 = xMin;
        *y2 = (sint32)((*y1 - *y2) * clip + *y2);
    }
    if (*y1 > h) {
        clip = ((float)h - *y2) / (float)(*y1 - *y2);
        *y1 = h;
        *x1 = (sint32)((*x1 - *x2) * clip + *x2);
    }
    if (*y2 > h) {
        clip = ((float)h - *y1) / (float)(*y2 - *y1);
        *y2 = h;
        *x2 = (sint32)((*x2 - *x1) * clip + *x1);
    }
    if (*y1 < yMin) {
        clip = ((float)yMin - *y1) / (float)(*y2 - *y1);
        *y1 = yMin;
        *x1 = (sint32)((*x2 - *x1) * clip + *x1);
    }
    if (*y2 < yMin) {
        clip = ((float)yMin - *y2) / (float)(*y1 - *y2);
        *y2 = yMin;
        *x2 = (sint32)((*x1 - *x2) * clip + *x2);
    }
    return 1;
}

void ClipCheckPoint(D3DTLVERTEX *v, float x, float y, float z, sint16 *clip)
{
    float perspz;
    sint16 clipdistance;

    clipdistance = 0;
    v->tu = x;
    v->tv = y;
    v->sz = z;
    if (!(v->sz < f_mznear)) {
        perspz = f_mpersp / v->sz;
        if (v->sz > FogEnd) {
            v->sz = f_zfar;
            clipdistance = 256;
        }
        v->sx = perspz * v->tu + f_centerx;
        v->sy = perspz * v->tv + f_centery;
        v->rhw = perspz * f_moneopersp;
        if (v->sx < phd_winxmin)
            clipdistance++;
        else if (phd_winxmax < v->sx)
            clipdistance += 2;
        if (v->sy < phd_winymin)
            clipdistance += 4;
        else if (v->sy > phd_winymax)
            clipdistance += 8;
    } else {
        clipdistance = -128;
    }
    clip[0] = clipdistance;
}

void OutputSky(void)
{
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ALPHABLENDENABLE, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZENABLE, 0);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 0);
    DrawBuckets();
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZENABLE, 1);
    IDirect3DDevice3_SetRenderState(App.dx.lpD3DDevice, D3DRENDERSTATE_ZWRITEENABLE, 1);
    SortPolyList(SortCount, SortList);
    RestoreFPCW(FPCW);
    SW_SetZLimit(0);
    DrawSortList();
    SW_SetZLimit(0.6F);
    MungeFPCW(&FPCW);
    InitBuckets();
    InitialiseSortList();
}

void DrawRope(ROPE_STRUCT *rope)
{
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    sint32 dx, dy, d, b, w, spec, i;
    float u1, v1, u2, v2;
    sint32 x1, y1, z1, x2, y2, z2, x3, y3, x4, y4;

    ProjectRopePoints(rope);
    dx = rope->Coords[1][0] - rope->Coords[0][0];
    dy = rope->Coords[1][1] - rope->Coords[0][1];
    d = SQUARE(dx) + SQUARE(dy);
    d = phd_sqrt(ABS(d));
    dx <<= W2V_SHIFT + 2;
    dy <<= W2V_SHIFT + 2;
    d <<= W2V_SHIFT + 2;
    if (d) {
        d = ((0x1000000 / (d >> 8)) << 8) >> 8;
        b = dx;
        dx = (sint32)(((__int64)-dy * (__int64)d) >> (W2V_SHIFT + 2));
        dy = (sint32)(((__int64)b * (__int64)d) >> (W2V_SHIFT + 2));
    }
    w = 0x60000;
    if (rope->Coords[0][2]) {
        w = 0x60000 * phd_persp / rope->Coords[0][2];
        if (w < 1)
            w = 1;
    }
    w <<= W2V_SHIFT + 2;
    dx = (sint32)(((__int64)dx * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
    dy = (sint32)(((__int64)dy * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
    x1 = rope->Coords[0][0] - dx;
    y1 = rope->Coords[0][1] - dy;
    z1 = rope->Coords[0][2] >> W2V_SHIFT;
    x4 = rope->Coords[0][0] + dx;
    y4 = rope->Coords[0][1] + dy;
    for (i = 0; i < 23; i++) {
        dx = rope->Coords[i + 1][0] - rope->Coords[i][0];
        dy = rope->Coords[i + 1][1] - rope->Coords[i][1];
        d = SQUARE(dx) + SQUARE(dy);
        d = phd_sqrt(ABS(d));
        dx <<= W2V_SHIFT + 2;
        dy <<= W2V_SHIFT + 2;
        d <<= W2V_SHIFT + 2;
        if (d) {
            d = ((0x1000000 / (d >> 8)) << 8) >> 8;
            b = dx;
            dx = (sint32)(((__int64)-dy * (__int64)d) >> (W2V_SHIFT + 2));
            dy = (sint32)(((__int64)b * (__int64)d) >> (W2V_SHIFT + 2));
        }
        w = 0x60000;
        if (rope->Coords[i][2]) {
            w = 0x60000 * phd_persp / rope->Coords[i][2];
            if (w < 3)
                w = 3;
        }
        w <<= W2V_SHIFT + 2;
        dx = (sint32)(((__int64)dx * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
        dy = (sint32)(((__int64)dy * (__int64)w) >> (W2V_SHIFT + 2)) >> (W2V_SHIFT + 2);
        x2 = rope->Coords[i + 1][0] - dx;
        y2 = rope->Coords[i + 1][1] - dy;
        z2 = rope->Coords[i + 1][2] >> W2V_SHIFT;
        x3 = rope->Coords[i + 1][0] + dx;
        y3 = rope->Coords[i + 1][1] + dy;
        if (z1 > f_mznear && z2 > f_mznear) {
            setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, z1, clipflags);
            v[0].color = 0xFF7F7F7F;
            v[1].color = 0xFF7F7F7F;
            v[2].color = 0xFF7F7F7F;
            v[3].color = 0xFF7F7F7F;
            spec = 255;
            if (z1 > 0x3000)
                spec = (255 * (0x5000 - z1)) >> 13;
            v[0].specular = spec << 24;
            v[1].specular = spec << 24;
            v[2].specular = spec << 24;
            v[3].specular = spec << 24;
            sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 16];
            u1 = sprite->x1;
            u2 = sprite->x2;
            v1 = sprite->y1;
            v2 = sprite->y2;
            tex.drawtype = 1;
            tex.flag = 0;
            tex.tpage = sprite->tpage;
            tex.u1 = u1;
            tex.v1 = v1;
            tex.u2 = u1;
            tex.v2 = v2;
            tex.u3 = u2;
            tex.v3 = v2;
            tex.u4 = u2;
            tex.v4 = v1;
            AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
        }
        x1 = x2;
        y1 = y2;
        z1 = z2;
        x4 = x3;
        y4 = y3;
    }
}

void DrawBlood(void)
{
    BLOOD_STRUCT *bptr;
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    PHD_VECTOR pos;
    sint32 *mptr;
    sint32 *Z;
    sint16 *XY;
    sint16 *offsets;
    float perspz, u1, v1, u2;
    uint32 r, col;
    sint32 i, size, s, c, dx, dy, dz, x1, y1, x2, y2, x3, y3, x4, y4;

    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
    mptr = phd_mxptr;
    sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + 15];
    bptr = blood;
    XY = (sint16 *)&scratchpad[0];
    Z = (sint32 *)&scratchpad[256];
    offsets = (sint16 *)&scratchpad[512];
    for (i = 32; i > 0; i--) {
        if (!bptr->On) {
            bptr++;
            continue;
        }
        dx = bptr->x - lara_item->pos.x_pos;
        dy = bptr->y - lara_item->pos.y_pos;
        dz = bptr->z - lara_item->pos.z_pos;
        if (dx < -0x5000 || dx > 0x5000 || dy < -0x5000 || dy > 0x5000 || dz < -0x5000 || dz > 0x5000) {
            bptr++;
            continue;
        }
        offsets[0] = (sint16)dx;
        offsets[1] = (sint16)dy;
        offsets[2] = (sint16)dz;
        pos.x = offsets[0] * mptr[M00] + offsets[1] * mptr[M01] + offsets[2] * mptr[M02] + mptr[M03];
        pos.y = offsets[0] * mptr[M10] + offsets[1] * mptr[M11] + offsets[2] * mptr[M12] + mptr[M13];
        pos.z = offsets[0] * mptr[M20] + offsets[1] * mptr[M21] + offsets[2] * mptr[M22] + mptr[M23];
        perspz = f_persp / (float)pos.z;
        XY[0] = (sint16)(pos.x * perspz + f_centerx);
        XY[1] = (sint16)(pos.y * perspz + f_centery);
        Z[0] = pos.z >> W2V_SHIFT;
        if (Z[0] <= 0 || Z[0] >= 0x5000) {
            bptr++;
            continue;
        }
        size = ((phd_persp * bptr->Size) << 1) / Z[0];
        if (size > (bptr->Size << 1))
            size = bptr->Size << 1;
        else if (size < 4)
            size = 4;
        size <<= 1;
        s = (size * rcossin_tbl[bptr->RotAng << 1]) >> (W2V_SHIFT - 2);
        c = (size * rcossin_tbl[(bptr->RotAng << 1) + 1]) >> (W2V_SHIFT - 2);
        x1 = c + XY[0] - s;
        y1 = XY[1] - c - s;
        x2 = s + c + XY[0];
        y2 = c + XY[1] - s;
        x3 = s + XY[0] - c;
        y3 = s + XY[1] + c;
        x4 = XY[0] - c - s;
        y4 = XY[1] - c + s;
        setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, Z[0], clipflags);
        if (Z[0] > 0x3000) {
            r = (((0x5000 - Z[0]) * bptr->Shade) >> 13) << 16;
            col = r | 0xFF000000;
        } else
            col = RGBA(bptr->Shade, 0, 0, 0xFF);
        v[0].color = col;
        v[1].color = col;
        v[2].color = col;
        v[3].color = col;
        v[0].specular = 0xFF000000;
        v[1].specular = 0xFF000000;
        v[2].specular = 0xFF000000;
        v[3].specular = 0xFF000000;
        tex.drawtype = 2;
        tex.flag = 0;
        u1 = sprite->x1;
        u2 = sprite->x2;
        v1 = sprite->y1;
        tex.tpage = sprite->tpage;
        tex.u1 = u1;
        tex.v1 = v1;
        tex.u2 = u2;
        tex.v2 = v1;
        tex.u3 = u2;
        tex.v3 = sprite->y2;
        tex.u4 = u1;
        tex.v4 = sprite->y2;
        AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
        bptr++;
    }
    phd_PopMatrix();
}

void DrawDrips(void)
{
    DRIP_STRUCT *drip;
    PHD_VECTOR vec;
    D3DTLVERTEX v[2];
    sint32 *Z;
    sint16 *XY;
    sint16 *pos;
    sint32 *mptr;
    float perspz, zf0, zf1;
    sint32 i, x, y, z, x0, y0, z0, x1, y1, z1, r, g, b;

    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
    mptr = phd_mxptr;
    XY = (sint16 *)&scratchpad[0];
    Z = (sint32 *)&scratchpad[256];
    pos = (sint16 *)&scratchpad[512];
    drip = Drips;
    for (i = 0; i < 32; i++, drip++) {
        if (!drip->On)
            continue;
        x = drip->x - lara_item->pos.x_pos;
        pos[0] = x;
        y = drip->y - lara_item->pos.y_pos;
        pos[1] = y;
        z = drip->z - lara_item->pos.z_pos;
        pos[2] = z;
        if (x < -20480 || x > 20480 || y < -20480 || y > 20480 || z < -20480 || z > 20480) {
            drip->On = 0;
            continue;
        }
        vec.x = pos[0] * mptr[M00] + pos[1] * mptr[M01] + pos[2] * mptr[M02] + mptr[M03];
        vec.y = pos[0] * mptr[M10] + pos[1] * mptr[M11] + pos[2] * mptr[M12] + mptr[M13];
        vec.z = pos[0] * mptr[M20] + pos[1] * mptr[M21] + pos[2] * mptr[M22] + mptr[M23];
        perspz = f_persp / (float)vec.z;
        XY[0] = (sint16)(vec.x * perspz + f_centerx);
        XY[1] = (sint16)(vec.y * perspz + f_centery);
        Z[0] = vec.z >> W2V_SHIFT;
        pos[1] -= drip->Yvel >> 6;
        if (room[drip->RoomNumber].flags & ROOM_NOT_INSIDE) {
            pos[0] -= (sint16)(SmokeWindX >> 1);
            pos[2] -= (sint16)(SmokeWindZ >> 1);
        }
        XY += 2;
        Z++;
        vec.x = pos[0] * mptr[M00] + pos[1] * mptr[M01] + pos[2] * mptr[M02] + mptr[M03];
        vec.y = pos[0] * mptr[M10] + pos[1] * mptr[M11] + pos[2] * mptr[M12] + mptr[M13];
        vec.z = pos[0] * mptr[M20] + pos[1] * mptr[M21] + pos[2] * mptr[M22] + mptr[M23];
        perspz = f_persp / (float)vec.z;
        XY[0] = (sint16)(vec.x * perspz + f_centerx);
        XY[1] = (sint16)(vec.y * perspz + f_centery);
        Z[0] = vec.z >> W2V_SHIFT;
        XY -= 2;
        Z--;
        if (Z[0] && Z[0] < 20480) {
            x0 = XY[0];
            y0 = XY[1];
            z0 = Z[0];
            x1 = XY[2];
            y1 = XY[3];
            z1 = Z[1];
            if (ClipLine(&x0, &y0, z0, &x1, &y1, z1, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax)) {
                r = drip->R << 2;
                g = drip->G << 2;
                b = drip->B << 2;
                v[0].sx = (float)x0;
                v[0].sy = (float)y0;
                zf0 = (float)Z[0];
                v[0].sz = zf0;
                zf1 = (float)Z[1];
                v[1].sz = zf1;
                v[1].sx = (float)x1;
                v[1].sy = (float)y1;
                v[0].rhw = f_mpersp / zf0 * f_moneopersp;
                v[0].color = RGBA(r, g, b, 0xFF);
                v[0].specular = 0xFF000000;
                r >>= 1;
                g >>= 1;
                b >>= 1;
                v[1].rhw = f_mpersp / zf1 * f_moneopersp;
                v[1].color = RGBA(r, g, b, 0xFF);
                v[1].specular = 0xFF000000;
                AddLineSorted(v, &v[1], 6);
            }
        } else if (Z[0] >= 20480)
            drip->On = 0;
    }
    phd_PopMatrix();
}

void DrawWraithTrail(ITEM_INFO *item)
{
    WRAITH_STRUCT *wraith;
    PHD_VECTOR pos;
    D3DTLVERTEX v[2];
    sint32 *Z;
    sint16 *XY;
    sint16 *offsets;
    sint32 *mptr;
    float perspz, zf0, zf1;
    sint32 i, j, c0, c1, x0, y0, z0, x1, y1, z1;

    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    mptr = phd_mxptr;
    for (i = 0; i < 5; i++) {
        if (!i)
            phd_RotY(-1092);
        else if (i == 2)
            phd_RotY(1092);
        else if (i == 3)
            phd_RotZ(-1092);
        else if (i == 4)
            phd_RotZ(1092);
        XY = (sint16 *)&scratchpad[0];
        Z = (sint32 *)&scratchpad[256];
        offsets = (sint16 *)&scratchpad[512];
        wraith = (WRAITH_STRUCT *)item->data;
        for (j = 0; j < 8; XY += 2, Z += 2, wraith++, j++) {
            offsets[0] = (sint16)(wraith->pos.x - item->pos.x_pos);
            offsets[1] = (sint16)(wraith->pos.y - item->pos.y_pos);
            offsets[2] = (sint16)(wraith->pos.z - item->pos.z_pos);
            pos.x = offsets[0] * mptr[M00] + offsets[1] * mptr[M01] + offsets[2] * mptr[M02] + mptr[M03];
            pos.y = offsets[0] * mptr[M10] + offsets[1] * mptr[M11] + offsets[2] * mptr[M12] + mptr[M13];
            pos.z = offsets[0] * mptr[M20] + offsets[1] * mptr[M21] + offsets[2] * mptr[M22] + mptr[M23];
            perspz = f_persp / (float)pos.z;
            XY[0] = (sint16)(pos.x * perspz + f_centerx);
            XY[1] = (sint16)(pos.y * perspz + f_centery);
            Z[0] = pos.z >> W2V_SHIFT;
            if (!j || j == 7)
                Z[1] = 0;
            else
                Z[1] = RGBONLY(wraith->r, wraith->g, wraith->b);
        }
        XY = (sint16 *)&scratchpad[0];
        Z = (sint32 *)&scratchpad[256];
        for (j = 7; j > 0; j--, XY += 2, Z += 2) {
            if (Z[0] <= f_mznear || Z[0] >= 20480)
                continue;
            if (Z[0] > 12288) {
                c0 = RGBA(((Z[1] & 0xFF) * (20480 - Z[0])) >> 13, (((Z[1] >> 8) & 0xFF) * (20480 - Z[0])) >> 13, (((Z[1] >> 16) & 0xFF) * (20480 - Z[0])) >> 13, 0xFF);
                c1 = RGBA(((Z[3] & 0xFF) * (20480 - Z[0])) >> 13, (((Z[3] >> 8) & 0xFF) * (20480 - Z[0])) >> 13, (((Z[3] >> 16) & 0xFF) * (20480 - Z[0])) >> 13, 0xFF);
            } else {
                c0 = Z[1];
                c1 = Z[3];
            }
            x0 = XY[0];
            y0 = XY[1];
            z0 = Z[0];
            x1 = XY[2];
            y1 = XY[3];
            z1 = Z[2];
            if (ClipLine(&x0, &y0, z0, &x1, &y1, z1, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax)) {
                v[0].specular = 0xFF000000;
                v[1].specular = 0xFF000000;
                v[0].sx = (float)x0;
                v[0].sy = (float)y0;
                v[0].color = c0;
                v[1].color = c1;
                v[1].sx = (float)x1;
                v[1].sy = (float)y1;
                zf0 = (float)Z[0];
                v[0].sz = zf0;
                zf1 = (float)Z[2];
                v[1].sz = zf1;
                v[0].rhw = f_mpersp / zf0 * f_moneopersp;
                v[1].rhw = f_mpersp / zf1 * f_moneopersp;
                AddLineSorted(v, &v[1], 6);
            }
        }
    }
    phd_PopMatrix();
}

void DrawTrainFloorStrip(sint32 x, sint32 z, TEXTURESTRUCT *tex, sint32 y_and_flags)
{
    SVECTOR *offsets;
    D3DTLVERTEX v[4];
    PHD_VECTOR p1, p2, p3;
    sint32 *mptr;
    sint32 *Z;
    sint16 *XY;
    sint32 i, j, z1, z2, z3, z4, spec;
    sint16 num, x1, y1, x2, y2, x3, y3, x4, y4;

    mptr = phd_mxptr;
    num = 0;
    offsets = (SVECTOR *)&scratchpad[984];
    offsets[0].z = (sint16)z;
    offsets[1].z = (sint16)(z + 512);
    offsets[2].z = (sint16)(z + 1024);
    if (y_and_flags & 0x1000000) {
        offsets[1].z += 1024;
        offsets[2].z += 2048;
    }
    offsets[0].y = ((y_and_flags >> 16) & 0xFF) << 4;
    offsets[1].y = ((y_and_flags >> 8) & 0xFF) << 4;
    offsets[2].y = (y_and_flags & 0xFF) << 4;
    offsets[0].x = (sint16)x;
    offsets[1].x = (sint16)x;
    offsets[2].x = (sint16)x;
    for (i = 2; i > 0; i--) {
        XY = (sint16 *)&scratchpad[0];
        Z = (sint32 *)&scratchpad[492];
        XY -= 6;
        Z -= 3;
        for (j = 41; j > 0; j--) {
            p1.x = (offsets[0].x * mptr[M00] + offsets[0].y * mptr[M01] + offsets[0].z * mptr[M02] + mptr[M03]) >> W2V_SHIFT;
            p1.y = (offsets[0].x * mptr[M10] + offsets[0].y * mptr[M11] + offsets[0].z * mptr[M12] + mptr[M13]) >> W2V_SHIFT;
            p1.z = (offsets[0].x * mptr[M20] + offsets[0].y * mptr[M21] + offsets[0].z * mptr[M22] + mptr[M23]) >> W2V_SHIFT;
            p2.x = (offsets[1].x * mptr[M00] + offsets[1].y * mptr[M01] + offsets[1].z * mptr[M02] + mptr[M03]) >> W2V_SHIFT;
            p2.y = (offsets[1].x * mptr[M10] + offsets[1].y * mptr[M11] + offsets[1].z * mptr[M12] + mptr[M13]) >> W2V_SHIFT;
            p2.z = (offsets[1].x * mptr[M20] + offsets[1].y * mptr[M21] + offsets[1].z * mptr[M22] + mptr[M23]) >> W2V_SHIFT;
            p3.x = (offsets[2].x * mptr[M00] + offsets[2].y * mptr[M01] + offsets[2].z * mptr[M02] + mptr[M03]) >> W2V_SHIFT;
            p3.y = (offsets[2].x * mptr[M10] + offsets[2].y * mptr[M11] + offsets[2].z * mptr[M12] + mptr[M13]) >> W2V_SHIFT;
            p3.z = (offsets[2].x * mptr[M20] + offsets[2].y * mptr[M21] + offsets[2].z * mptr[M22] + mptr[M23]) >> W2V_SHIFT;
            offsets[0].x += 512;
            offsets[1].x += 512;
            offsets[2].x += 512;
            XY += 6;
            Z += 3;
            XY[0] = (sint16)p1.x;
            XY[1] = (sint16)p1.y;
            Z[0] = p1.z;
            XY[2] = (sint16)p2.x;
            XY[3] = (sint16)p2.y;
            Z[1] = p2.z;
            XY[4] = (sint16)p3.x;
            XY[5] = (sint16)p3.y;
            Z[2] = p3.z;
        }
        offsets[0].x -= 512;
        offsets[1].x -= 512;
        offsets[2].x -= 512;
        XY = (sint16 *)&scratchpad[0];
        Z = (sint32 *)&scratchpad[492];
        for (j = num; j < num + 20; j++, XY += 12, Z += 6) {
            z1 = Z[0];
            z2 = Z[2];
            z3 = Z[6];
            z4 = Z[8];
            if (!(z1 | z2 | z3 | z4))
                continue;
            x1 = XY[0];
            y1 = XY[1];
            x2 = XY[4];
            y2 = XY[5];
            x3 = XY[12];
            y3 = XY[13];
            x4 = XY[16];
            y4 = XY[17];
            if (j < 7)
                spec = (j + 1) * 0x101010;
            else if (j < 33)
                spec = 0x808080;
            else
                spec = (40 - j) * 0x101010;
            setXYZ4(v, x1, y1, z1, x2, y2, z2, x4, y4, z4, x3, y3, z3, clipflags);
            spec = ((spec & 0xFF) - 1) << 25;
            v[0].color = 0xFFFFFFFF;
            v[1].color = 0xFFFFFFFF;
            v[2].color = 0xFFFFFFFF;
            v[3].color = 0xFFFFFFFF;
            v[0].specular = spec;
            v[1].specular = spec;
            v[2].specular = spec;
            v[3].specular = spec;
            AddQuadSorted(v, 0, 1, 2, 3, tex, 0);
        }
        num += 20;
    }
}

void DrawTrainStrips(void)
{
    sint16 *ar;

    ar = &aranges[2];
    DrawTrainFloorStrip(-20480, -5120, &textinfo[ar[5]], 0x1101010);
    DrawTrainFloorStrip(-20480, 3072, &textinfo[ar[5]], 0x1101010);
    DrawTrainFloorStrip(-20480, -2048, &textinfo[ar[3]], 0x100800);
    DrawTrainFloorStrip(-20480, 2048, &textinfo[ar[4]], 0x810);
    DrawTrainFloorStrip(-20480, -1024, &textinfo[ar[1]], 0);
    DrawTrainFloorStrip(-20480, 1024, &textinfo[ar[2]], 0);
    DrawTrainFloorStrip(-20480, 0, &textinfo[ar[0]], 0);
}

void DrawSprite(sint32 x, sint32 y, sint32 slot, sint32 col, sint32 size, sint32 z)
{
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    sint32 s;

    s = (sint32)((float)(phd_winwidth / 640.0F) * (size << 1));
    if (z)
        setXY4(v, x - s, y - s, s + x, y - s, x - s, s + y, s + x, s + y, (sint32)(z + f_mznear), clipflags);
    else
        setXY4(v, x - s, y - s, s + x, y - s, x - s, s + y, s + x, s + y, (sint32)f_mzfar, clipflags);
    sprite = &spriteinfo[slot + objects[DEFAULT_SPRITES].mesh_index];
    v[0].specular = 0xFF000000;
    v[3].specular = 0xFF000000;
    v[2].specular = 0xFF000000;
    v[1].specular = 0xFF000000;
    CalcColorSplit(col, &v[0].color);
    v[3].color = v[0].color | 0xFF000000;
    v[2].color = v[0].color | 0xFF000000;
    v[1].color = v[0].color | 0xFF000000;
    tex.drawtype = 2;
    tex.flag = 0;
    tex.tpage = sprite->tpage;
    tex.u1 = sprite->x1;
    tex.v1 = sprite->y1;
    tex.u2 = sprite->x2;
    tex.v2 = sprite->y1;
    tex.u3 = sprite->x2;
    tex.v3 = sprite->y2;
    tex.u4 = sprite->x1;
    tex.v4 = sprite->y2;
    AddQuadSorted(v, 0, 1, 3, 2, &tex, 0);
}

void SetFade(sint32 start, sint32 end)
{
    DoFade = 1;
    FadeVal = start;
    FadeStep = (end - start) >> 3;
    FadeCnt = 0;
    FadeEnd = end;
}

void DoScreenFade(void)
{
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    sint32 a;

    a = FadeVal << 24;
    FadeVal += FadeStep;
    FadeCnt++;
    if (FadeCnt > 8) {
        DoFade = 2;
        a = FadeEnd << 24;
    }
    v[0].sx = 0;
    v[0].sy = 0;
    v[0].sz = 0;
    v[0].rhw = f_moneoznear;
    v[0].color = a;
    v[0].specular = 0xFF000000;
    v[1].sx = (float)(phd_winxmax + 1);
    v[1].sy = (float)phd_winymin;
    v[1].sz = 0;
    v[1].rhw = f_moneoznear;
    v[1].color = a;
    v[1].specular = 0xFF000000;
    v[2].sx = (float)(phd_winxmax + 1);
    v[2].sy = (float)(phd_winymax + 1);
    v[2].sz = 0;
    v[2].rhw = f_moneoznear;
    v[2].color = a;
    v[2].specular = 0xFF000000;
    v[3].sx = (float)phd_winxmin;
    v[3].sy = (float)(phd_winymax + 1);
    v[3].sz = 0;
    v[3].rhw = f_moneoznear;
    v[3].color = a;
    v[3].specular = 0xFF000000;
    tex.drawtype = 3;
    tex.flag = 0;
    tex.tpage = 0;
    clipflags[0] = 0;
    clipflags[1] = 0;
    clipflags[2] = 0;
    clipflags[3] = 0;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawPsxTile(sint32 x_y, sint32 height_width, sint32 color, sint32 u0, sint32 u1)
{
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    float x, y, z, rhw, w, h;
    sint32 col;
    sint32 drawtype;
    sint32 alpha;

    x = (float)((uint32)x_y >> 16);
    y = (float)(x_y & 0xFFFF);
    h = (float)((uint32)height_width >> 16) + 1;
    w = (float)(height_width & 0xFFFF) + 1;
    nPolyType = 6;
    alpha = color & 0xFF000000;
    if (alpha == 0x62000000) {
        drawtype = 3;
        col = color << 24;
    } else {
        drawtype = 2;
        col = color | 0xFF000000;
    }
    z = f_znear;
    rhw = f_moneoznear;
    if (!gfCurrentLevel) {
        z = f_znear + 10;
        rhw = f_moneoznear + 50;
    }
    v[0].sx = x;
    v[0].sy = y;
    v[0].sz = z;
    v[0].rhw = rhw;
    v[0].color = col;
    v[0].specular = 0xFF000000;
    v[1].sx = x + w;
    v[1].sy = y;
    v[1].sz = z;
    v[1].rhw = rhw;
    v[1].color = col;
    v[1].specular = 0xFF000000;
    v[2].sx = x + w;
    v[2].sy = y + h;
    v[2].sz = z;
    v[2].rhw = rhw;
    v[2].color = col;
    v[2].specular = 0xFF000000;
    v[3].sx = x;
    v[3].sy = y + h;
    v[3].sz = z;
    v[3].rhw = rhw;
    v[3].color = col;
    v[3].specular = 0xFF000000;
    tex.drawtype = (uint16)drawtype;
    tex.flag = 0;
    tex.tpage = 0;
    clipflags[0] = 0;
    clipflags[1] = 0;
    clipflags[2] = 0;
    clipflags[3] = 0;
    AddQuadSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawFlash(void)
{
    uint8 r;
    uint8 g;
    uint8 b;

    r = (FlashFadeR * FlashFader) >> 5;
    g = (FlashFadeG * FlashFader) >> 5;
    b = (FlashFadeB * FlashFader) >> 5;
    DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), b | ((g | ((r | (0x62 << 8)) << 8)) << 8), 1, 0);
    DrawPsxTile(0, phd_winwidth | (phd_winheight << 16), b | ((g | ((r | (0xFF << 8)) << 8)) << 8), 2, 0);
}

void DrawBikeSpeedo(sint32 ux, sint32 uy, sint32 vel, sint32 maxVel, sint32 turboVel, sint32 size, sint32 unk)
{
    D3DTLVERTEX v[2];
    sint32 x0, y0, x1, y1;
    sint32 rSize, i;

    ux = (sint32)(448.0F * ((float)phd_winxmax * (1.0F / 512.0F)));
    uy = (sint32)(224.0F * ((float)phd_winymax * (1.0F / 240.0F)));
    rSize = (7 * size) >> 3;
    vel = abs(vel >> 1);
    if (vel) {
        vel += (((rcossin_tbl[(((GlobalCounter & 7) << 13) >> 3) & 0x1FFE] << 2) * ((vel - 4096) >> 5)) >> W2V_SHIFT);
        if (vel < 0)
            vel = 0;
    }
    maxVel >>= 1;
    turboVel >>= 1;
    for (i = -0x4000; i + 0x4000 <= turboVel; i += 2048) {
        x0 = (rSize * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x1 = (size * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y0 = (-(rSize * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y1 = (-(size * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        v[0].sx = (float)(ux + x0);
        v[0].sy = (float)(uy + y0);
        v[0].sz = f_mznear;
        v[0].rhw = f_moneoznear;
        v[1].sx = (float)(ux + x1);
        v[1].sy = (float)(uy + y1);
        v[1].sz = f_mznear;
        v[1].rhw = f_moneoznear;
        v[0].color = v[1].color = i + 0x4000 > maxVel ? 0xFFFF0000 : 0xFFFFFFFF;
        v[1].specular = v[0].specular;
        AddLineSorted(v, &v[1], 6);
    }
    size -= size >> 4;
    x1 = (size * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (-4 * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y0 = (-(-4 * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y1 = (-(size * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    v[0].sx = (float)(ux + x0);
    v[0].sy = (float)(uy + y0);
    v[0].sz = f_mznear;
    v[0].rhw = f_moneoznear;
    v[1].sx = (float)(ux + x1);
    v[1].sy = (float)(uy + y1);
    v[1].sz = f_mznear;
    v[1].rhw = f_moneoznear;
    v[1].color = v[0].color;
    v[1].specular = v[0].specular;
    AddLineSorted(v, &v[1], 6);
}

void S_DrawDarts(ITEM_INFO *item)
{
    D3DTLVERTEX v[2];
    PHD_VECTOR pos;
    sint32 x1, y1, z1, x2, y2, z2, num;
    sint16 XY[2];
    sint16 sx, sy, mxx, mxy, mxz;
    float zv;

    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    pos.z = phd_mxptr[M23];
    pos.x = phd_mxptr[M03];
    pos.y = phd_mxptr[M13];
    zv = f_persp / (float)pos.z;
    XY[0] = (sint16)(pos.x * zv + f_centerx);
    XY[1] = (sint16)(pos.y * zv + f_centery);
    z1 = pos.z >> W2V_SHIFT;
    num = (-96 * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
    mxx = ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * num) >> W2V_SHIFT;
    mxy = (96 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
    mxz = ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * num) >> W2V_SHIFT;
    pos.x = phd_mxptr[M00] * mxx + phd_mxptr[M01] * mxy + phd_mxptr[M02] * mxz + phd_mxptr[M03];
    pos.y = phd_mxptr[M10] * mxx + phd_mxptr[M11] * mxy + phd_mxptr[M12] * mxz + phd_mxptr[M13];
    pos.z = phd_mxptr[M20] * mxx + phd_mxptr[M21] * mxy + phd_mxptr[M22] * mxz + phd_mxptr[M23];
    zv = f_persp / (float)pos.z;
    sx = (sint16)(pos.x * zv + f_centerx);
    sy = (sint16)(pos.y * zv + f_centery);
    z2 = pos.z >> W2V_SHIFT;
    x1 = XY[0];
    y1 = XY[1];
    x2 = sx;
    y2 = sy;
    if (ClipLine(&x1, &y1, z1, &x2, &y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax)) {
        v[0].color = 0xFF000000;
        v[0].specular = 0xFF000000;
        v[1].color = 0xFF783C14;
        v[1].specular = 0xFF000000;
        v[0].sx = (float)x1;
        v[0].sy = (float)y1;
        v[1].sx = (float)x2;
        v[1].sy = (float)y2;
        zv = f_mpersp / (float)z1 * f_moneopersp;
        v[0].rhw = zv;
        v[1].rhw = zv;
        zv = f_a - zv * f_boo;
        v[0].sz = zv;
        v[1].sz = zv;
        AddLineSorted(v, &v[1], 6);
    }
    phd_PopMatrix();
}

void Draw2DSprite(sint32 x, sint32 y, sint32 slot, sint32 unused, sint32 unused2)
{
    SPRITESTRUCT *sprite;
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    sint32 w;
    sint32 h;
    float u1;
    float v1;
    float u2;
    float v2;

    sprite = &spriteinfo[objects[DEFAULT_SPRITES].mesh_index + slot];
    w = (sint32)((float)(sprite->width >> 8) * phd_centerx * (1.0F / 320.0F));
    h = (sint32)((float)(sprite->height >> 8) * phd_centery * (1.0F / 240.0F));
    setXY4(v, (sint32)(float)x, (sint32)(float)y, (sint32)(float)(x + w + 1), (sint32)(float)y, (sint32)(float)(x + w + 1), (sint32)(float)(h + y + 1), (sint32)(float)x, (sint32)(float)(h + y + 1), (sint32)f_mznear, clipflags);
    v[0].specular = 0xFF000000;
    v[1].specular = 0xFF000000;
    v[2].specular = 0xFF000000;
    v[3].specular = 0xFF000000;
    v[0].color = 0xFFFFFFFF;
    v[1].color = 0xFFFFFFFF;
    v[2].color = 0xFFFFFFFF;
    v[3].color = 0xFFFFFFFF;
    u1 = sprite->x1;
    v1 = sprite->y1;
    u2 = sprite->x2;
    v2 = sprite->y2;
    tex.drawtype = 1;
    tex.flag = 0;
    tex.tpage = sprite->tpage;
    tex.u1 = u1;
    tex.v1 = v1;
    tex.u2 = u2;
    tex.v2 = v1;
    tex.u3 = u2;
    tex.v3 = v2;
    tex.u4 = u1;
    tex.v4 = v2;
    AddQuadClippedSorted(v, 0, 1, 2, 3, &tex, 0);
}

void DrawJeepSpeedo(sint32 ux, sint32 uy, sint32 vel, sint32 maxVel, sint32 turboVel, sint32 size, sint32 spriteSlot)
{
    D3DTLVERTEX v[2];
    sint32 x0, y0, x1, y1;
    sint32 rSize, i;

    ux = (sint32)(448.0F * ((float)phd_winxmax * (1.0F / 512.0F)));
    uy = (sint32)(224.0F * ((float)phd_winymax * (1.0F / 240.0F)));
    Draw2DSprite(ux + 24, uy - 16, spriteSlot + 17, RGBONLY(GetRandomDraw() & 0xFF, GetRandomDraw() & 0xFF, GetRandomDraw() & 0xFF), 0);
    rSize = (7 * size) >> 3;
    vel = abs(vel >> 1);
    if (vel) {
        vel += (((rcossin_tbl[(((GlobalCounter & 7) << 13) >> 3) & 0x1FFE] << 2) * ((vel - 4096) >> 5)) >> W2V_SHIFT);
        if (vel < 0)
            vel = 0;
    }
    maxVel >>= 1;
    turboVel >>= 1;
    for (i = -0x4000; i + 0x4000 <= turboVel; i += 2048) {
        x0 = (rSize * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x1 = (size * (rcossin_tbl[(i >> 3) & 0x1FFE] << 2)) >> (W2V_SHIFT - 1);
        x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y0 = (-(rSize * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
        y1 = (-(size * (rcossin_tbl[((i >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
        v[0].sx = (float)(ux + x0);
        v[0].sy = (float)(uy + y0);
        v[0].sz = f_mznear;
        v[0].rhw = f_moneoznear;
        v[1].sx = (float)(ux + x1);
        v[1].sy = (float)(uy + y1);
        v[1].sz = f_mznear;
        v[1].rhw = f_moneoznear;
        v[0].color = v[1].color = i + 0x4000 > maxVel ? 0xFFFF0000 : 0xFFFFFFFF;
        v[1].specular = v[0].specular;
        AddLineSorted(v, &v[1], 6);
    }
    size -= size >> 4;
    x1 = (size * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (-4 * phd_sin(vel - 0x4000)) >> (W2V_SHIFT - 1);
    x0 = (x0 - (x0 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y0 = (-(-4 * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    x1 = (x1 - (x1 >> 2)) * ((float)phd_winxmax * (1.0F / 512.0F));
    y1 = (-(size * phd_cos(vel - 0x4000)) >> W2V_SHIFT) * ((float)phd_winymax * (1.0F / 240.0F));
    v[0].sx = (float)(ux + x0);
    v[0].sy = (float)(uy + y0);
    v[0].sz = f_mznear;
    v[0].rhw = f_moneoznear;
    v[1].sx = (float)(ux + x1);
    v[1].sy = (float)(uy + y1);
    v[1].sz = f_mznear;
    v[1].rhw = f_moneoznear;
    v[1].color = v[0].color;
    v[1].specular = v[0].specular;
    AddLineSorted(v, &v[1], 6);
}
