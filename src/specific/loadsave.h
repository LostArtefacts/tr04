#ifndef SPECIFIC_LOADSAVE_H
#define SPECIFIC_LOADSAVE_H

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "../types.h"

void S_DrawDashBar(sint32 pos);
void S_DrawAirBar(sint32 pos);
void S_DrawHealthBar(sint32 pos);
void CheckKeyConflicts(void);
sint32 S_LoadSave(sint32 load_or_save, sint32 mono);
void CreateMonoScreen(void);
void FreeMonoScreen(void);
void S_DisplayMonoScreen(void);
sint32 S_DisplayPauseMenu(sint32 reset);
void DoOptions(void);
void DoStatScreen(void);
sint32 GetSaveLoadFiles(void);
sint32 DoLoadSave(sint32 LoadSave);
void S_InitLoadBar(sint32 maxpos);
void DoBar(sint32 x, sint32 y, sint32 width, sint32 height, sint32 pos, sint32 clr1, sint32 clr2);
typedef struct MONOSCREEN_STRUCT {
    LPDIRECT3DTEXTURE2 tex;
    LPDIRECTDRAWSURFACE4 surface;
} MONOSCREEN_STRUCT;

typedef struct savefile_info {
    char name[75];
    char valid;
    sint16 hours;
    sint16 minutes;
    sint16 seconds;
    sint16 days;
    sint32 num;
} SAVEFILE_INFO;

typedef void (*rgbfunc)(uint8 *, uint8 *, uint8 *);

void RGBM_Mono(uint8 *r, uint8 *g, uint8 *b);
void MemBltSurf(void *dest, sint32 x, sint32 y, sint32 w, sint32 h, sint32 dadd, void *source, sint32 x2, sint32 y2, DDSURFACEDESC2 surface, float xsize, float ysize);
LPDIRECTDRAWSURFACE4 CreateTexturePage(sint32 w, sint32 h, sint32 MipMapCount, sint32 *pSrc, rgbfunc RGBM, sint32 format);
void ConvertSurfaceToTextures(LPDIRECTDRAWSURFACE4 surface);
void S_LoadBar(void);
sint32 S_PauseMenu(void);

extern sint32 SoundQuality;
extern sint32 ControlMethod;
extern uint8 MonoScreenOn;
extern MONOSCREEN_STRUCT MonoScreen[5];
extern sint32 nSaves;
extern sint32 pause_selection;
extern sint32 pause_menu;
extern SAVEFILE_INFO SaveGames[15];
extern float loadbar_pos;
extern sint32 loadbar_maxpos;

#endif
