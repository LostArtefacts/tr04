#ifndef SPECIFIC_SWRENDER_H
#define SPECIFIC_SWRENDER_H

#include <windows.h>
#include <d3d.h>
#include "../types.h"

HRESULT SW_SetRenderStateFlags(D3DRENDERSTATETYPE type, DWORD state);
HRESULT j_SW_DrawPrimitives(D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags);
void SW_D3D_SetupSurface(sint32 w, sint32 h, sint32 mmx);
HMODULE SW_GetKernelModule(void);
FARPROC SW_GetSleepProc(void);
void SW_FreeBuffer(void);
void SW_CopyBuffer(void *dst, void *src, sint32 width);
void SW_Convert32To555(void *dst, void *src, sint32 width);
void SW_Convert32To565(void *dst, void *src, sint32 width);
void SW_Convert32To24(void *dst, void *src, sint32 width);
HRESULT SW_SetSurface(void *surface, sint32 pitch);
sint32 SW_BeginFrame(void);
sint32 SW_EndFrame(void);
sint32 SW_SetColorOpScale(sint32 scale);

#endif
