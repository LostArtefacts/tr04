#include "d3dmatrix.h"
#include "3dmath.h"

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
