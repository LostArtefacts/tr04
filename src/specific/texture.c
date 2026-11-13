#include <stdlib.h>
#include "file.h"
#include "winmain.h"
#include "dxshell.h"
#include "drawroom.h"
#include "function_stubs.h"
#include "loadsave.h"

LPDIRECTDRAWSURFACE4 CreateTexturePage(sint32 w, sint32 h, sint32 MipMapCount, sint32 *pSrc, rgbfunc RGBM, sint32 format)
{
    DXTEXTUREINFO *tex;
    LPDIRECTDRAWSURFACE4 tSurf;
    DDSURFACEDESC2 desc;
    sint32 *lS;
    sint32 *lD;
    sint16 *sS;
    sint16 *sD;
    char *cD;
    uint32 c, o, x, y;
    uint8 r, g, b, a;
    sint32 i, xs, ys;
    sint32 *row;
    sint16 *s16;

    memset(&desc, 0, sizeof(DDSURFACEDESC2));
    tSurf = 0;
    desc.dwSize = sizeof(DDSURFACEDESC2);
    desc.dwWidth = w;
    desc.dwHeight = h;
    if (w < 32 || h < 32)
        MipMapCount = 0;
    desc.ddpfPixelFormat = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture].ddpf;
    desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    if (App.dx.Flags & 0x80)
        desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else
        desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    if (MipMapCount) {
        desc.dwFlags |= DDSD_MIPMAPCOUNT;
        desc.dwMipMapCount = MipMapCount + 1;
        desc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
    }
    DXCreateSurface(App.dx.lpDD, &desc, &tSurf);
    DXAttempt(IDirectDrawSurface4_Lock(tSurf, 0, &desc, DDLOCK_NOSYSLOCK, 0));
    if (!format) {
        xs = 256 / w;
        ys = 256 / h;
        cD = (char *)desc.lpSurface;
        row = pSrc;
        for (y = 0; y < desc.dwHeight; y++) {
            lS = row;
            for (x = 0; x < desc.dwWidth; x++) {
                c = *lS;
                r = (uint8)(c >> 16);
                g = (uint8)(c >> 8);
                b = (uint8)c;
                a = (uint8)(c >> 24);
                if (RGBM)
                    RGBM(&r, &g, &b, &a);
                tex = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture];
                o = b >> (8 - tex->bbpp) << tex->bshift | g >> (8 - tex->gbpp) << tex->gshift | a >> (8 - tex->abpp) << tex->ashift | r >> (8 - tex->rbpp) << tex->rshift;
                for (i = tex->bpp; i > 0; i -= 8) {
                    *cD++ = (char)o;
                    o >>= 8;
                }
                lS += xs;
            }
            row += ys << 8;
        }
    } else if (format == 2) {
        xs = 256 / w;
        ys = 256 / h;
        sD = (sint16 *)desc.lpSurface;
        sS = (sint16 *)pSrc;
        for (y = 0; y < desc.dwHeight; y++) {
            s16 = sS;
            for (x = 0; x < desc.dwWidth; x++) {
                *sD++ = *s16;
                s16 += xs;
            }
            sS += ys << 8;
        }
    } else if (format == 1) {
        xs = 256 / w;
        ys = 256 / h;
        lD = (sint32 *)desc.lpSurface;
        row = pSrc;
        for (y = 0; y < desc.dwHeight; y++) {
            lS = row;
            for (x = 0; x < desc.dwWidth; x++) {
                *lD++ = *lS;
                lS += xs;
            }
            row += ys << 8;
        }
    }
    DXAttempt(IDirectDrawSurface4_Unlock(tSurf, 0));
    return tSurf;
}

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
