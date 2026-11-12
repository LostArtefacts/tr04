#include "fmv.h"
#include "dxshell.h"
#include "winmain.h"

void ShowBinkFrame(void)
{
    DDSURFACEDESC2 surf;

    memset(&surf, 0, sizeof(surf));
    surf.dwSize = sizeof(DDSURFACEDESC2);
    DXAttempt(IDirectDrawSurface4_Lock(BinkSurface, 0, &surf, DDLOCK_NOSYSLOCK, 0));
    BinkCopyToBuffer(Bink, surf.lpSurface, surf.lPitch, Bink->num, 0, 0, BinkSurfaceType);
    DXAttempt(IDirectDrawSurface4_Unlock(BinkSurface, 0));
    if (App.dx.Flags & 2)
        DXShowFrame();
}
