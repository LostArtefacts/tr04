#ifndef SPECIFIC_SPECIFICFX_H
#define SPECIFIC_SPECIFICFX_H

#include "../types.h"
#include "../3dglodef.h"

void DrawPsxTile(sint32 x_y, sint32 height_width, sint32 color, sint32 u0, sint32 u1);
void SetUpLensFlare(sint32 x, sint32 y, sint32 z, GAME_VECTOR *lfobj);
void DrawJeepSpeedo(sint32 ux, sint32 uy, sint32 vel, sint32 maxVel, sint32 turboVel, sint32 size, sint32 spriteSlot);

#endif
