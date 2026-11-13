#ifndef SPECIFIC_OUTPUT_H
#define SPECIFIC_OUTPUT_H

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "../types.h"
#include "../3dglodef.h"

extern sint32 GlobalAlpha;
extern sint32 GlobalAmbient;
extern D3DTLVERTEX SkinVerts[40][12];
extern sint16 SkinClip[40][12];
extern char SkinVertNums[40][12];
extern char ScratchVertNums[40][12];
extern LPDIRECT3DVERTEXBUFFER DestVB;
extern sint16 *clipflags;

void S_AnimateTextures(sint32 n);
void S_DrawPickup(sint16 object_number);
void phd_PutPolygonsPickup(sint16 *objptr, float x, float y, sint32 color);
void PrelightVerts(sint32 nVerts, D3DTLVERTEX *v, MESH_DATA *mesh);
void CalcVertsColorSplitMMX(sint32 nVerts, D3DTLVERTEX *v);
sint32 S_DumpScreen(void);
void S_InitialisePolyList(void);
void S_OutputPolyList(void);
sint32 S_GetObjectBounds(sint16 *bounds);
void _InsertRoom();
void phd_PutPolygons_train(sint16 *objptr, sint32 clip);
void phd_PutPolygonSkyMesh(sint16 *objptr, sint32 clipstatus);
void StashSkinVertices(sint32 node);
void SkinVerticesToScratch(sint32 node);
HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE4 surf, HBITMAP hbm, sint32 x, sint32 y, sint32 dx, sint32 dy);
HRESULT _LoadBitmap(LPDIRECTDRAWSURFACE4 surf, LPCSTR name);
void do_boot_screen(sint32 language);
void ProjectTrainVerts(sint32 nVerts, D3DTLVERTEX *v, sint16 *clip, sint32 x);
void RenderLoadPic(sint32 unused);
void nullsub_9(void);
void j_nullsub_9(void);

#endif
