#ifndef SPECIFIC_FUNCTION_TABLE_H
#define SPECIFIC_FUNCTION_TABLE_H

#include <windows.h>
#include <d3d.h>

#include "../types.h"

uint8 _NVisible(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2);
uint8 _Visible(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2);
void SetCullCW(void);
void SetCullCCW(void);

extern uint8 (*IsVisible)(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2);
extern HRESULT (*_EndScene)(void);

#endif
