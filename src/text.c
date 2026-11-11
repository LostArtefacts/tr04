#include <windows.h>
#include <d3d.h>

#include "text.h"
#include "specific/3dmath.h"

void CalcColorSplit(D3DCOLOR s, D3DCOLOR *d);

void InitFont(void)
{
    D3DTLVERTEX v;
    uint16 r, g, b;
    sint16 h, w, yoff;
    uint8 fr, fg, fb, tr, tg, tb;
    int i, j;

    for (i = 0; i < 10; i++) {
        fr = ShadeFromTo[i][0].r;
        fg = ShadeFromTo[i][0].g;
        fb = ShadeFromTo[i][0].b;
        tr = ShadeFromTo[i][1].r;
        tg = ShadeFromTo[i][1].g;
        tb = ShadeFromTo[i][1].b;
        for (j = 0; j < 16; j++) {
            r = ((tr * j) >> 4) + ((fr * (16 - j)) >> 4);
            g = ((tg * j) >> 4) + ((fg * (16 - j)) >> 4);
            b = ((tb * j) >> 4) + ((fb * (16 - j)) >> 4);
            if (r > 255)
                r = 255;
            if (g > 255)
                g = 255;
            if (b > 255)
                b = 255;
            CalcColorSplit(RGBONLY(b, g, r), &v.color);
            r = CLRR((sint32)v.color);
            g = CLRG((sint32)v.color);
            b = CLRB((sint32)v.color);
            FontShades[i][j << 1].r = (uint8)r;
            FontShades[i][j << 1].g = (uint8)g;
            FontShades[i][j << 1].b = (uint8)b;
            FontShades[i][j << 1].a = 0xFF;
            r = CLRR((sint32)v.specular);
            g = CLRG((sint32)v.specular);
            b = CLRB((sint32)v.specular);
            FontShades[i][(j << 1) + 1].r = (uint8)r;
            FontShades[i][(j << 1) + 1].g = (uint8)g;
            FontShades[i][(j << 1) + 1].b = (uint8)b;
            FontShades[i][(j << 1) + 1].a = 0xFF;
        }
    }
    if (font_init) {
        for (i = 0; i < 106; i++) {
            CharDefCopy[i].h = CharDef[i].h;
            CharDefCopy[i].w = CharDef[i].w;
            CharDefCopy[i].YOffset = CharDef[i].YOffset;
        }
        font_init = 0;
    }
    for (i = 0; i < 106; i++) {
        h = (sint16)((float)CharDefCopy[i].h * (float)phd_winymax * (1.0F / 240.0F));
        w = (sint16)((float)CharDefCopy[i].w * (float)phd_winxmax * (1.0F / 512.0F));
        yoff = (sint16)((float)CharDefCopy[i].YOffset * (float)phd_winymax * (1.0F / 240.0F));
        CharDef[i].h = h;
        CharDef[i].w = w;
        CharDef[i].YOffset = yoff;
    }
    font_height = (sint32)((float)phd_winymax * 0.075F);
}
