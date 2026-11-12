#ifndef SPECIFIC_POLYINSERT_H
#define SPECIFIC_POLYINSERT_H

#include <windows.h>
#include <d3d.h>
#include "../types.h"

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

int DistCompare(const void *a, const void *b);

void CreateFogPos(FOGBULB_STRUCT *FogBulb);
void ControlFXBulb(FOGBULB_STRUCT *FogBulb);
void InitialiseSortList(void);
sint32 IsVolumetric(void);
void ClearFXFogBulbs(void);
void DoSort(sint32 left, sint32 right, SORTLIST **list);
void SortPolyList(sint32 count, SORTLIST **list);
void CreateFXBulbs(void);
void TriggerFXFogBulb(sint32 x, sint32 y, sint32 z, sint32 FXRad, sint32 density, sint32 r, sint32 g, sint32 b, sint32 room_number);
void InitialiseFogBulbs(void);

#endif
