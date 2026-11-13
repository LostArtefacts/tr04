#ifndef SPECIFIC_SWRENDER_H
#define SPECIFIC_SWRENDER_H

#include <windows.h>
#include <d3d.h>

HRESULT SW_SetRenderStateFlags(D3DRENDERSTATETYPE type, DWORD state);
HRESULT j_SW_DrawPrimitives(D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags);
void SW_FreeBuffer(void);

#endif
