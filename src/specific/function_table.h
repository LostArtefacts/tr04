#ifndef SPECIFIC_FUNCTION_TABLE_H
#define SPECIFIC_FUNCTION_TABLE_H

#include <windows.h>
#include <d3d.h>

#include "../types.h"
#include "../3dglodef.h"

uint8 _NVisible(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2);
uint8 _Visible(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2);
void SetCullCW(void);
void SetCullCCW(void);
void SetFogColor(sint32 r, sint32 g, sint32 b);

extern uint8 (*IsVisible)(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2);
extern HRESULT (*_EndScene)(void);
extern void (*AddQuadSorted)(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, sint16 v3, TEXTURESTRUCT *tex, sint32 double_sided);

#endif
