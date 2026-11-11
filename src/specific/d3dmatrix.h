#ifndef SPECIFIC_D3DMATRIX_H
#define SPECIFIC_D3DMATRIX_H

#include <windows.h>
#include <d3d.h>

#include "../types.h"

D3DMATRIX *D3DIdentityMatrix(D3DMATRIX *mx);
void SetD3DMatrix(D3DMATRIX *mx, sint32 *imx);
void D3DTransform(D3DVECTOR *vec, D3DMATRIX *mx);
D3DVECTOR *D3DNormalise(D3DVECTOR *vec);

#endif
