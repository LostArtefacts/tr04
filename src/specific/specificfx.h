#ifndef SPECIFIC_SPECIFICFX_H
#define SPECIFIC_SPECIFICFX_H

#include <windows.h>
#include <d3d.h>

#include "../types.h"
#include "../3dglodef.h"

extern sint32 DoFade;
extern sint32 FadeVal;
extern sint32 FadeStep;
extern sint32 FadeCnt;
extern sint32 FadeEnd;
extern MESH_DATA *targetMeshP;
extern MESH_DATA *binocsMeshP;

void DrawRope(void *rope);
void DrawPsxTile(sint32 x_y, sint32 height_width, sint32 color, sint32 u0, sint32 u1);
void SetUpLensFlare(sint32 x, sint32 y, sint32 z, GAME_VECTOR *lfobj);
void DrawBikeSpeedo(sint32 ux, sint32 uy, sint32 vel, sint32 maxVel, sint32 turboVel, sint32 size, sint32 unk);
void DrawJeepSpeedo(sint32 ux, sint32 uy, sint32 vel, sint32 maxVel, sint32 turboVel, sint32 size, sint32 spriteSlot);
void SetFade(sint32 start, sint32 end);
void DrawTrainFloorStrip(sint32 x, sint32 z, TEXTURESTRUCT *tex, sint32 y_and_flags);
void setXY4(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 x3, sint32 y3, sint32 x4, sint32 y4, sint32 z, sint16 *clip);
void setXY3(D3DTLVERTEX *v, sint32 x1, sint32 y1, sint32 x2, sint32 y2, sint32 x3, sint32 y3, sint32 z, sint16 *clip);
void InitTarget_2(void);
void InitBinoculars(void);
void DrawFlash(void);
void DrawBinoculars(void);
void DoScreenFade(void);
void ClipCheckPoint(D3DTLVERTEX *v, float x, float y, float z, sint16 *clip);
void OutputSky(void);
void DrawSprite(sint32 x, sint32 y, sint32 slot, sint32 col, sint32 size, sint32 z);
void Draw2DSprite(sint32 x, sint32 y, sint32 slot, sint32 unused, sint32 unused2);
void DrawTrainStrips(void);
void S_DrawDarts(struct item_info *item);

void S_DrawFireSparks(sint32 size, sint32 life);
uint8 ClipLine(sint32 *x1, sint32 *y1, sint32 z1, sint32 *x2, sint32 *y2, sint32 z2, sint32 xMin, sint32 yMin, sint32 w, sint32 h);

#endif
