#ifndef SPECIFIC_SWRENDER_H
#define SPECIFIC_SWRENDER_H

#include <windows.h>
#include <d3d.h>
#include "../types.h"

sint32 SW_SelectRasterizer(void *v0, void *v1, void *v2);
sint32 SW_TriSetupGTP(void *v0, void *v1, void *v2);
sint32 SW_TriSetupGT(void *v0, void *v1, void *v2);
sint32 SW_TriSetupG(void *v0, void *v1, void *v2);
sint32 SW_TriSetupGTPFog(void *v0, void *v1, void *v2);
sint32 SW_TriSetupGTFog(void *v0, void *v1, void *v2);
sint32 SW_TriSetupGFog(void *v0, void *v1, void *v2);
void SW_SpanG(void);
void SW_SpanGFogAdd(void);
void SW_SpanGFog(void);
void SW_SpanGFogAlpha(void);
void SW_SpanGAdd(void);
void SW_SpanGAlpha(void);
void SW_SpanGTAlpha(void);
void SW_SpanGTPAlpha(void);
void SW_SpanGT(void);
void SW_SpanGTAdd(void);
void SW_SpanGTKey(void);
void SW_SpanGTP(void);
void SW_SpanGTPKey(void);
void SW_SpanGTPAdd(void);
void SW_SpanGTBilinear(void);
void SW_SpanGTAddBilinear(void);
void SW_SpanGTPBilinear(void);
void SW_SpanGTPAddBilinear(void);
void SW_SpanGTAlphaBilinear(void);
void SW_SpanGTPAlphaBilinear(void);
void SW_SpanGInt(void);
void SW_SpanGAddInt(void);
void SW_SpanGLit(void);
void SW_SpanGTLit(void);
void SW_SpanGTLitPersp(void);
void SW_SpanGTInt(void);
void SW_SpanGTAddInt(void);
void SW_SpanGTKeyInt(void);
void SW_SpanGTPInt(void);
void SW_SpanGTPAddInt(void);
void SW_SpanGTPKeyInt(void);
HRESULT SW_SetRenderStateFlags(D3DRENDERSTATETYPE type, DWORD state);
HRESULT j_SW_DrawPrimitives(D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags);
void SW_D3D_SetupSurface(sint32 w, sint32 h, sint32 mmx);
HMODULE SW_GetKernelModule(void);
FARPROC SW_GetSleepProc(HMODULE module);
void SW_SleepTick(void);
void SW_FreeBuffer(void);
void SW_CopyBuffer(void *dst, void *src, sint32 width);
void SW_Convert32To555(void *dst, void *src, sint32 width);
void SW_Convert32To565(void *dst, void *src, sint32 width);
void SW_Convert32To24(void *dst, void *src, sint32 width);
HRESULT SW_SetSurface(void *surface, sint32 pitch);
sint32 SW_BeginFrame(void);
sint32 SW_EndFrame(void);
sint32 SW_SetColorOpScale(sint32 scale);
HRESULT SW_DrawPrimitives(D3DPRIMITIVETYPE type, DWORD fvf, LPVOID verts, DWORD count, DWORD flags);

#endif
