#ifndef SPECIFIC_OUTPUT_H
#define SPECIFIC_OUTPUT_H

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "../types.h"

extern sint32 GlobalAlpha;
extern sint32 GlobalAmbient;
extern D3DTLVERTEX SkinVerts[40][12];
extern sint16 SkinClip[40][12];
extern char SkinVertNums[40][12];
extern char ScratchVertNums[40][12];
extern LPDIRECT3DVERTEXBUFFER DestVB;
extern sint16 *clipflags;

void S_AnimateTextures(sint32 n);
sint32 S_DumpScreen(void);
void S_InitialisePolyList(void);
void S_OutputPolyList(void);
sint32 S_GetObjectBounds(sint16 *bounds);
void _InsertRoom();
void phd_PutPolygons_train(sint16 *objptr, sint32 clip);
HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE4 surf, HBITMAP hbm, sint32 x, sint32 y, sint32 dx, sint32 dy);
HRESULT _LoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name);
void do_boot_screen(sint32 language);

#endif
