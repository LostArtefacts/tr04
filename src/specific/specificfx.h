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
void InitTarget_2(void);
void InitBinoculars(void);
void DrawFlash(void);
void DoScreenFade(void);
void ClipCheckPoint(D3DTLVERTEX *v, float x, float y, float z, sint16 *clip);

#endif
