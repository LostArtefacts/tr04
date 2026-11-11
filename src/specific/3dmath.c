#include <math.h>
#include <stdlib.h>

#include "3dmath.h"
#include "d3dmatrix.h"
#include "../lara.h"
#include "function_stubs.h"

extern D3DMATRIX D3DMW2VMatrix;

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

void phd_GenerateW2V(PHD_3DPOS *viewPos)
{
    PHD_VECTOR scalar;
    sint32 sx, cx, sy, cy, sz, cz;
    sint32 x, y, z;

    phd_mxptr = matrix_stack;
    sx = phd_sin(viewPos->x_rot);
    cx = phd_cos(viewPos->x_rot);
    sy = phd_sin(viewPos->y_rot);
    cy = phd_cos(viewPos->y_rot);
    sz = phd_sin(viewPos->z_rot);
    cz = phd_cos(viewPos->z_rot);

    w2v_matrix[M00] = phd_mxptr[M00] = TRIGMULT3(sx, sy, sz) + TRIGMULT2(cy, cz);
    w2v_matrix[M01] = phd_mxptr[M01] = TRIGMULT2(cx, sz);
    w2v_matrix[M02] = phd_mxptr[M02] = TRIGMULT3(sx, cy, sz) - TRIGMULT2(sy, cz);

    w2v_matrix[M10] = phd_mxptr[M10] = TRIGMULT3(sx, sy, cz) - TRIGMULT2(cy, sz);
    w2v_matrix[M11] = phd_mxptr[M11] = TRIGMULT2(cx, cz);
    w2v_matrix[M12] = phd_mxptr[M12] = TRIGMULT3(sx, cy, cz) + TRIGMULT2(sy, sz);

    w2v_matrix[M20] = phd_mxptr[M20] = TRIGMULT2(cx, sy);
    w2v_matrix[M21] = phd_mxptr[M21] = -sx;
    w2v_matrix[M22] = phd_mxptr[M22] = TRIGMULT2(cx, cy);

    if (lara.dpoisoned != lara.poisoned) {
        lara.poisoned += (lara.dpoisoned - lara.poisoned) >> 4;
        if (abs(lara.dpoisoned - lara.poisoned) < 16)
            lara.poisoned = lara.dpoisoned;
    }

    if (lara.poisoned >= 256) {
        x = (lara.poisoned - 256) * ((phd_sin(XSoff1) + phd_sin(XSoff2)) >> 2);
        y = (lara.poisoned - 256) * ((phd_sin(YSoff1) + phd_sin(YSoff2)) >> 2);
        z = (lara.poisoned - 256) * ((phd_sin(ZSoff1) + phd_sin(ZSoff2)) >> 2);
        if (x || y || z) {
            scalar.x = (x >> 12) + 0x4000;
            scalar.y = (y >> 12) + 0x4000;
            scalar.z = (z >> 12) + 0x4000;
            ScaleCurrentMatrix(&scalar);
        }
    }

    w2v_matrix[M03] = matrix_stack[M03] = viewPos->x_pos;
    w2v_matrix[M13] = matrix_stack[M13] = viewPos->y_pos;
    w2v_matrix[M23] = matrix_stack[M23] = viewPos->z_pos;
    w2v_matrix[M10] = matrix_stack[M10] = (sint32)(LfAspectCorrection * (float)matrix_stack[M10]);
    w2v_matrix[M11] = matrix_stack[M11] = (sint32)(LfAspectCorrection * (float)matrix_stack[M11]);
    w2v_matrix[M12] = matrix_stack[M12] = (sint32)(LfAspectCorrection * (float)matrix_stack[M12]);
    SetD3DMatrix(&D3DMW2VMatrix, w2v_matrix);
}

void phd_LookAt(sint32 sx, sint32 sy, sint32 sz, sint32 tx, sint32 ty, sint32 tz, sint16 roll)
{
    PHD_3DPOS viewPos;
    sint32 dx, dy, dz, val;
    sint16 angles[2];

    phd_GetVectorAngles(tx - sx, ty - sy, tz - sz, angles);
    viewPos.x_pos = sx;
    viewPos.y_pos = sy;
    viewPos.z_pos = sz;
    viewPos.x_rot = angles[1];
    viewPos.y_rot = angles[0];
    viewPos.z_rot = roll;
    dx = sx - tx;
    dy = sy - ty;
    dz = sz - tz;
    val = (sint32)sqrt(dx * dx + dz * dz);
    CamRot.x = (mGetAngle(0, 0, val, dy) >> 4) & 0xFFF;
    CamRot.y = (mGetAngle(sz, sx, tz, tx) >> 4) & 0xFFF;
    CamRot.z = 0;
    CamPos.x = sx;
    CamPos.y = sy;
    CamPos.z = sz;
    phd_GenerateW2V(&viewPos);
    S_InitD3DMatrix();
}

sint32 phd_atan(sint32 x, sint32 y)
{
    sint32 octant, n, result;

    if (!x && !y)
        return 0;

    octant = 0;
    if (x < 0) {
        octant += 4;
        x = -x;
    }
    if (y < 0) {
        octant += 2;
        y = -y;
    }
    if (y > x) {
        octant++;
        n = x;
        x = y;
        y = n;
    }
    while ((sint16)y != y) {
        x >>= 1;
        y >>= 1;
    }
    result = phdtan2[octant] + phdtantab[(y << 11) / x];
    if (result < 0)
        result = -result;
    return result;
}

sint32 phd_sqrt(sint32 num)
{
    sint32 base, result, tmp;

    base = 0x40000000;
    result = 0;
    do {
        tmp = result;
        result += base;
        tmp >>= 1;
        if (result > num) {
            result = tmp;
        } else {
            num -= result;
            result = base | tmp;
        }
        base >>= 2;
    } while (base);
    return result;
}

void phd_PushMatrix(void)
{
    sint32 *mx;

    mx = phd_mxptr;
    mx[indices_count + M00] = mx[M00];
    mx[indices_count + M01] = mx[M01];
    mx[indices_count + M02] = mx[M02];
    mx[indices_count + M03] = mx[M03];
    mx[indices_count + M10] = mx[M10];
    mx[indices_count + M11] = mx[M11];
    mx[indices_count + M12] = mx[M12];
    mx[indices_count + M13] = mx[M13];
    mx[indices_count + M20] = mx[M20];
    mx[indices_count + M21] = mx[M21];
    mx[indices_count + M22] = mx[M22];
    mx[indices_count + M23] = mx[M23];
    phd_mxptr += indices_count;
}

void phd_PushUnitMatrix(void)
{
    phd_mxptr += indices_count;
    Func490320();
}

void Func490320(void)
{
    sint32 *mx;

    mx = phd_mxptr;
    mx[M00] = mx[M11] = mx[M22] = 0x4000;
    mx[M01] = mx[M02] = mx[M03] = 0;
    mx[M10] = mx[M12] = mx[M13] = 0;
    mx[M20] = mx[M21] = mx[M23] = 0;
}

sint32 phd_TranslateRel(sint32 x, sint32 y, sint32 z)
{
    sint32 *mptr;

    mptr = phd_mxptr;
    mptr[M03] += mptr[M00] * x + mptr[M01] * y + mptr[M02] * z;
    mptr[M13] += mptr[M10] * x + mptr[M11] * y + mptr[M12] * z;
    mptr[M23] += mptr[M20] * x + mptr[M21] * y + mptr[M22] * z;
    if ((x = ABS(mptr[M03])) > phd_zfar || (y = ABS(mptr[M13])) > phd_zfar || (z = ABS(mptr[M23])) > phd_zfar)
        return 0;
    return 1;
}

void phd_RotX(PHD_ANGLE rx)
{
    sint32 sx, cx, r0, r1;
    sint32 *mptr;

    if (rx) {
        mptr = phd_mxptr;
        sx = phd_sin(rx);
        cx = phd_cos(rx);

        r0 = mptr[M01] * cx + mptr[M02] * sx;
        r1 = mptr[M02] * cx - mptr[M01] * sx;
        mptr[M01] = r0 >> W2V_SHIFT;
        mptr[M02] = r1 >> W2V_SHIFT;

        r0 = mptr[M11] * cx + mptr[M12] * sx;
        r1 = mptr[M12] * cx - mptr[M11] * sx;
        mptr[M11] = r0 >> W2V_SHIFT;
        mptr[M12] = r1 >> W2V_SHIFT;

        r0 = mptr[M21] * cx + mptr[M22] * sx;
        r1 = mptr[M22] * cx - mptr[M21] * sx;
        mptr[M21] = r0 >> W2V_SHIFT;
        mptr[M22] = r1 >> W2V_SHIFT;
    }
}

void phd_RotY(PHD_ANGLE ry)
{
    sint32 sy, cy, r0, r1;
    sint32 *mptr;

    if (ry) {
        mptr = phd_mxptr;
        sy = phd_sin(ry);
        cy = phd_cos(ry);

        r0 = mptr[M00] * cy - mptr[M02] * sy;
        r1 = mptr[M02] * cy + mptr[M00] * sy;
        mptr[M00] = r0 >> W2V_SHIFT;
        mptr[M02] = r1 >> W2V_SHIFT;

        r0 = mptr[M10] * cy - mptr[M12] * sy;
        r1 = mptr[M12] * cy + mptr[M10] * sy;
        mptr[M10] = r0 >> W2V_SHIFT;
        mptr[M12] = r1 >> W2V_SHIFT;

        r0 = mptr[M20] * cy - mptr[M22] * sy;
        r1 = mptr[M22] * cy + mptr[M20] * sy;
        mptr[M20] = r0 >> W2V_SHIFT;
        mptr[M22] = r1 >> W2V_SHIFT;
    }
}

void phd_RotZ(PHD_ANGLE rz)
{
    sint32 sz, cz, r0, r1;
    sint32 *mptr;

    if (rz) {
        mptr = phd_mxptr;
        sz = phd_sin(rz);
        cz = phd_cos(rz);

        r0 = mptr[M00] * cz + mptr[M01] * sz;
        r1 = mptr[M01] * cz - mptr[M00] * sz;
        mptr[M00] = r0 >> W2V_SHIFT;
        mptr[M01] = r1 >> W2V_SHIFT;

        r0 = mptr[M10] * cz + mptr[M11] * sz;
        r1 = mptr[M11] * cz - mptr[M10] * sz;
        mptr[M10] = r0 >> W2V_SHIFT;
        mptr[M11] = r1 >> W2V_SHIFT;

        r0 = mptr[M20] * cz + mptr[M21] * sz;
        r1 = mptr[M21] * cz - mptr[M20] * sz;
        mptr[M20] = r0 >> W2V_SHIFT;
        mptr[M21] = r1 >> W2V_SHIFT;
    }
}
