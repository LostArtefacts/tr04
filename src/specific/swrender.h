#ifndef SPECIFIC_SWRENDER_H
#define SPECIFIC_SWRENDER_H

#include <windows.h>
#include <d3d.h>
#include "../types.h"

HRESULT SW_SetRenderStateFlags(D3DRENDERSTATETYPE type, DWORD state);
HRESULT j_SW_DrawPrimitives(D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags);
void SW_FreeBuffer(void);
HRESULT SW_SetSurface(void *surface, sint32 pitch);
sint32 SW_BeginFrame(void);
sint32 SW_EndFrame(void);
sint32 SW_SetColorOpScale(sint32 scale);

#endif
