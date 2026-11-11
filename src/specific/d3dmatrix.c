#include <math.h>

#include "d3dmatrix.h"
#include "3dmath.h"
#include "dxshell.h"
#include "winmain.h"

void SetD3DViewMatrix(void)
{
    sint32 *mptr;

    mptr = phd_mxptr;
    D3DIdentityMatrix(&D3DMView);
    D3DMView._11 = (float)mptr[M00] / 16384;
    D3DMView._12 = (float)mptr[M10] / 16384;
    D3DMView._13 = (float)mptr[M20] / 16384;
    D3DMView._21 = (float)mptr[M01] / 16384;
    D3DMView._22 = (float)mptr[M11] / 16384;
    D3DMView._23 = (float)mptr[M21] / 16384;
    D3DMView._31 = (float)mptr[M02] / 16384;
    D3DMView._32 = (float)mptr[M12] / 16384;
    D3DMView._33 = (float)mptr[M22] / 16384;
    D3DMView._41 = (float)(mptr[M03] >> W2V_SHIFT);
    D3DMView._42 = (float)(mptr[M13] >> W2V_SHIFT);
    D3DMView._43 = (float)(mptr[M23] >> W2V_SHIFT);
    DXAttempt(IDirect3DDevice3_SetTransform(App.dx.lpD3DDevice, D3DTRANSFORMSTATE_VIEW, &D3DMView));
}

void SetD3DMatrix(D3DMATRIX *mx, sint32 *imx)
{
    D3DIdentityMatrix(mx);
    mx->_11 = (float)imx[M00] / 16384;
    mx->_12 = (float)imx[M10] / 16384;
    mx->_13 = (float)imx[M20] / 16384;
    mx->_21 = (float)imx[M01] / 16384;
    mx->_22 = (float)imx[M11] / 16384;
    mx->_23 = (float)imx[M21] / 16384;
    mx->_31 = (float)imx[M02] / 16384;
    mx->_32 = (float)imx[M12] / 16384;
    mx->_33 = (float)imx[M22] / 16384;
    mx->_41 = (float)(imx[M03] >> W2V_SHIFT);
    mx->_42 = (float)(imx[M13] >> W2V_SHIFT);
    mx->_43 = (float)(imx[M23] >> W2V_SHIFT);
}

void S_InitD3DMatrix(void)
{
    D3DIdentityMatrix(&D3DMWorld);
    D3DIdentityMatrix(&D3DMProjection);
    D3DMProjection._22 = -1;
    DXAttempt(IDirect3DDevice3_SetTransform(App.dx.lpD3DDevice, D3DTRANSFORMSTATE_WORLD, &D3DMWorld));
    DXAttempt(IDirect3DDevice3_SetTransform(App.dx.lpD3DDevice, D3DTRANSFORMSTATE_PROJECTION, &D3DMProjection));
}

D3DVECTOR *D3DNormalise(D3DVECTOR *vec)
{
    float val;

    if (vec->x != 0 || vec->y != 0 || vec->z != 0) {
        val = 1.0F / sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
        vec->x = val * vec->x;
        vec->y = val * vec->y;
        vec->z = val * vec->z;
    }
    return vec;
}

D3DMATRIX *D3DIdentityMatrix(D3DMATRIX *mx)
{
    mx->_11 = 1;
    mx->_12 = 0;
    mx->_13 = 0;
    mx->_14 = 0;
    mx->_21 = 0;
    mx->_22 = 1;
    mx->_23 = 0;
    mx->_24 = 0;
    mx->_31 = 0;
    mx->_32 = 0;
    mx->_33 = 1;
    mx->_34 = 0;
    mx->_41 = 0;
    mx->_42 = 0;
    mx->_43 = 0;
    mx->_44 = 1;
    return mx;
}
