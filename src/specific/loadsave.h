#ifndef SPECIFIC_LOADSAVE_H
#define SPECIFIC_LOADSAVE_H

#include <windows.h>
#include <ddraw.h>

#include "../types.h"

void S_DrawDashBar(sint32 pos);
void S_DrawAirBar(sint32 pos);
void S_DrawHealthBar(sint32 pos);
void CheckKeyConflicts(void);
sint32 S_LoadSave(sint32 load_or_save, sint32 mono);
void CreateMonoScreen(void);
void S_InitLoadBar(sint32 maxpos);
void DoBar(sint32 x, sint32 y, sint32 width, sint32 height, sint32 pos, sint32 clr1, sint32 clr2);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface);
void S_LoadBar(void);

extern sint32 SoundQuality;
extern sint32 ControlMethod;
extern uint8 MonoScreenOn;
extern float loadbar_pos;
extern sint32 loadbar_maxpos;

#endif
