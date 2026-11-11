#include "3dmath.h"

void SetupZRange(sint32 znear, sint32 zfar)
{
    f_zfar = (float)zfar;
    f_znear = (float)znear;
    phd_zfar = zfar;
    phd_znear = znear;
    f_perspoznear = f_persp / f_znear;
    f_mznear = (float)(znear >> W2V_SHIFT);
    f_mzfar = (float)(zfar >> W2V_SHIFT);
    f_mperspoznear = f_mpersp / f_mznear;
    f_moneoznear = mone / f_mznear;
    f_b = f_mzfar * f_mznear * 0.99F / (f_mznear - f_mzfar);
    f_a = 0.005F - f_b / f_mznear;
    f_b = -f_b;
    f_boo = f_b / mone;
}

void ScaleCurrentMatrix(PHD_VECTOR *vec)
{
    phd_mxptr[M00] = (phd_mxptr[M00] * vec->x) >> W2V_SHIFT;
    phd_mxptr[M10] = (phd_mxptr[M10] * vec->x) >> W2V_SHIFT;
    phd_mxptr[M20] = (phd_mxptr[M20] * vec->x) >> W2V_SHIFT;
    phd_mxptr[M01] = (phd_mxptr[M01] * vec->y) >> W2V_SHIFT;
    phd_mxptr[M11] = (phd_mxptr[M11] * vec->y) >> W2V_SHIFT;
    phd_mxptr[M21] = (phd_mxptr[M21] * vec->y) >> W2V_SHIFT;
    phd_mxptr[M02] = (phd_mxptr[M02] * vec->z) >> W2V_SHIFT;
    phd_mxptr[M12] = (phd_mxptr[M12] * vec->z) >> W2V_SHIFT;
    phd_mxptr[M22] = (phd_mxptr[M22] * vec->z) >> W2V_SHIFT;
}

void InitWindow(sint32 x, sint32 y, sint32 w, sint32 h, sint32 znear, sint32 zfar, sint32 fov, sint32 a, sint32 b)
{
    phd_winxmin = (sint16)x;
    phd_winwidth = w;
    phd_winxmax = (sint16)(w - 1);
    phd_winheight = h;
    phd_winymax = (sint16)(h - 1);
    phd_winymin = (sint16)y;
    phd_centerx = w / 2;
    phd_centery = h / 2;
    phd_znear = znear << W2V_SHIFT;
    phd_zfar = zfar << W2V_SHIFT;
    f_centerx = (float)phd_centerx;
    f_centery = (float)phd_centery;
    AlterFOV((sint16)(182 * fov));
    SetupZRange(phd_znear, phd_zfar);
    phd_right = phd_winxmax;
    phd_bottom = phd_winymax;
    phd_left = x;
    phd_top = y;
    f_left = (float)phd_winxmin;
    f_top = (float)phd_winymin;
    f_right = (float)(1 + phd_winxmax);
    f_bottom = (float)(1 + phd_winymax);
    phd_mxptr = matrix_stack;
}

uint32 mGetAngle(sint32 x, sint32 z, sint32 x1, sint32 z1)
{
    sint32 dx, dz, octant, swap, angle;

    dx = x1 - x;
    dz = z1 - z;
    if (!dx && !dz)
        return 0;

    octant = 0;
    if (dx < 0) {
        octant = 4;
        dx = -dx;
    }
    if (dz < 0) {
        octant += 2;
        dz = -dz;
    }
    if (dz > dx) {
        octant++;
        swap = dx;
        dx = dz;
        dz = swap;
    }
    while ((sint16)dz != dz) {
        dx >>= 1;
        dz >>= 1;
    }
    angle = phdtan2[octant] + phdtantab[(dz << 11) / dx];
    if (angle < 0)
        angle = -angle;
    return -angle & 0xFFFF;
}
