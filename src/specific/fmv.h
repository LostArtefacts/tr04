#ifndef SPECIFIC_FMV_H
#define SPECIFIC_FMV_H

#include <windows.h>
#include <ddraw.h>

#include "../types.h"

typedef struct {
    sint32 pad;
    sint32 num;
    char padfuck[8];
    sint32 num2;
} BINK_STRUCT;

#ifdef TR4_EXE
#define BinkOpenDirectSound _BinkOpenDirectSound
#define BinkSetSoundSystem _BinkSetSoundSystem
#define BinkOpen _BinkOpen
#define BinkDDSurfaceType _BinkDDSurfaceType
#define BinkDoFrame _BinkDoFrame
#define BinkNextFrame _BinkNextFrame
#define BinkWait _BinkWait
#define BinkClose _BinkClose
#define BinkCopyToBuffer _BinkCopyToBuffer
#endif

__declspec(dllimport) void *__stdcall BinkOpenDirectSound(uint32 param);
__declspec(dllimport) sint32 __stdcall BinkSetSoundSystem(void *open, uint32 param);
__declspec(dllimport) BINK_STRUCT *__stdcall BinkOpen(char *name, uint32 flags);
__declspec(dllimport) sint32 __stdcall BinkDDSurfaceType(void *surf);
__declspec(dllimport) sint32 __stdcall BinkDoFrame(BINK_STRUCT *bnk);
__declspec(dllimport) void __stdcall BinkNextFrame(BINK_STRUCT *bnk);
__declspec(dllimport) sint32 __stdcall BinkWait(BINK_STRUCT *bnk);
__declspec(dllimport) void __stdcall BinkClose(BINK_STRUCT *bnk);
__declspec(dllimport) void __stdcall BinkCopyToBuffer(BINK_STRUCT *bnk, void *dest, sint32 destpitch, sint32 destheight, sint32 destx, sint32 desty, sint32 flags);

extern BINK_STRUCT *Bink;
extern LPDIRECTDRAWSURFACE4 BinkSurface;
extern sint32 BinkSurfaceType;
extern sint32 FmvSceneTriggered;

void ShowBinkFrame(void);
sint32 PlayFmvNow(sint32 num);

#endif
