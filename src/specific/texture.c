#include <stdlib.h>
#include "file.h"
#include "winmain.h"
#include "dxshell.h"
#include "drawroom.h"
#include "function_stubs.h"

void FreeTextures(void)
{
    sint32 i;

    DXAttempt(IDirect3DDevice3_SetTexture(App.dx.lpD3DDevice, 0, 0));
    DXAttempt(IDirect3D3_EvictManagedTextures(App.dx.lpD3D));
    for (i = 0; i < nTextures; i++) {
        if (Textures[i].tex) {
            Log(4, "Released %s @ %x - RefCnt = %d", "Texture", Textures[i].tex, IDirect3DTexture2_Release(Textures[i].tex));
            Textures[i].tex = 0;
        } else
            Log(1, "%s Attempt To Release NULL Ptr", "Texture");
        if (Textures[i].surface) {
            Log(4, "Released %s @ %x - RefCnt = %d", "Surface", Textures[i].surface, IDirectDrawSurface4_Release(Textures[i].surface));
            Textures[i].surface = 0;
        } else
            Log(1, "%s Attempt To Release NULL Ptr", "Surface");
    }
    free(Textures);
    Textures = 0;
}
