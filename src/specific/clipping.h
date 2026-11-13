#ifndef SPECIFIC_CLIPPING_H
#define SPECIFIC_CLIPPING_H

#include <windows.h>
#include <d3d.h>
#include "../types.h"
#include "../3dglodef.h"
#include "drawroom.h"

sint32 ZClipper(sint32 n, D3DTLBUMPVERTEX *in, D3DTLBUMPVERTEX *out);
sint32 XYUVGClipper(sint32 n, D3DTLBUMPVERTEX *in);
sint32 visible_zclip(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2);

#endif
