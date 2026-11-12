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

__declspec(dllimport) void __stdcall BinkCopyToBuffer(BINK_STRUCT *bnk, void *dest, sint32 destpitch, sint32 destheight, sint32 destx, sint32 desty, sint32 flags);

extern BINK_STRUCT *Bink;
extern LPDIRECTDRAWSURFACE4 BinkSurface;
extern sint32 BinkSurfaceType;

void ShowBinkFrame(void);

#endif
