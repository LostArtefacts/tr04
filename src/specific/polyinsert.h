#ifndef SPECIFIC_POLYINSERT_H
#define SPECIFIC_POLYINSERT_H

#include <windows.h>
#include <d3d.h>
#include "../types.h"
#include "../3dglodef.h"

typedef struct FVECTOR {
    float x;
    float y;
    float z;
} FVECTOR;

typedef struct FOGBULB_STRUCT {
    FVECTOR WorldPos;
    FVECTOR pos;
    FVECTOR vec;
    float rad;
    float sqrad;
    float inv_sqrad;
    float dist;
    sint32 density;
    sint32 inRange;
    sint32 timer;
    sint32 active;
    sint32 FXRad;
    sint32 room_number;
    sint32 r;
    sint32 g;
    sint32 b;
} FOGBULB_STRUCT;

typedef struct SORTLIST {
    float zVal;
    sint16 drawtype;
    sint16 tpage;
    sint16 nVtx;
    sint16 polytype;
} SORTLIST;

extern char SortBuffer[655360];
extern char *pSortBuffer;
extern SORTLIST *SortList[16384];
extern SORTLIST **pSortList;
extern sint32 SortCount;
extern FOGBULB_STRUCT FXFogBulbs[5];
extern sint32 NumFXFogBulbs;
extern FOGBULB_STRUCT FogBulbs[20];
extern sint32 NumLevelFogBulbs;
extern FOGBULB_STRUCT *ActiveFogBulbs[5];
extern sint32 NumActiveFogBulbs;
extern sint32 rgb80h;
extern sint32 rgbmask;
extern sint32 zero;
extern sint32 nPolyType;
extern sint16 *clipflags;

void AddTriClippedSorted(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, TEXTURESTRUCT *tex, sint32 double_sided);
void AddQuadClippedSorted(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, sint16 v3, TEXTURESTRUCT *tex, sint32 double_sided);
void SubdivideTri(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2, TEXTURESTRUCT *tex, sint32 double_sided, sint32 steps, sint16 *c);
void SubdivideQuad(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v2, D3DTLVERTEX *v3, TEXTURESTRUCT *tex, sint32 double_sided, sint32 steps, sint16 *c);
int DistCompare(const void *a, const void *b);

void CreateFogPos(FOGBULB_STRUCT *FogBulb);
void ControlFXBulb(FOGBULB_STRUCT *FogBulb);
void InitialiseSortList(void);
sint32 IsVolumetric(void);
void ClearFXFogBulbs(void);
void DoSort(sint32 left, sint32 right, SORTLIST **list);
void SortPolyList(sint32 count, SORTLIST **list);
void DrawSortList(void);
void SW_SetZLimit(float limit);
void CreateFXBulbs(void);
void TriggerFXFogBulb(sint32 x, sint32 y, sint32 z, sint32 FXRad, sint32 density, sint32 r, sint32 g, sint32 b, sint32 room_number);
void InitialiseFogBulbs(void);
void AddLineClippedSorted(D3DTLVERTEX *v0, D3DTLVERTEX *v1, sint16 drawtype);
void CalcColorSplit(D3DCOLOR s, D3DCOLOR *d);
void CalcColorSplitMMX(D3DCOLOR s, D3DCOLOR *d);
void AddPrelitMMX(sint32 prelight, D3DCOLOR *color);
void AddTriSubdivide(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, TEXTURESTRUCT *tex, sint32 double_sided);
void AddQuadSubdivide(D3DTLVERTEX *v, sint16 v0, sint16 v1, sint16 v2, sint16 v3, TEXTURESTRUCT *tex, sint32 double_sided);
extern sint32 nPolys;
extern sint32 nClippedPolys;
extern sint32 DrawPrimitiveCnt;
extern sint32 Unk5c0478;
void SubdivideEdge(D3DTLVERTEX *v0, D3DTLVERTEX *v1, D3DTLVERTEX *v, sint16 *c, float tu1, float tv1, float tu2, float tv2, float *tu, float *tv);
void nullsub_10(void);
void j_nullsub_10(void);

#endif
