#include <windows.h>
#include <d3d.h>

#include "text.h"
#include "specific/3dmath.h"
#include "specific/drawroom.h"
#include "specific/output.h"
#include "specific/polyinsert.h"
#include "specific/specificfx.h"

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

void UpdatePulseColour(void)
{
    D3DTLVERTEX v;
    uint8 c, r, g, b;
    int i;

    PulseCnt = (PulseCnt + 1) & 0x1F;
    if (PulseCnt >= 16)
        c = -PulseCnt;
    else
        c = PulseCnt;
    c <<= 3;
    CalcColorSplit(RGBONLY(c, c, c), &v.color);
    for (i = 0; i < 16; i++) {
        r = (uint8)CLRR(v.color);
        g = (uint8)CLRG(v.color);
        b = (uint8)CLRB(v.color);
        FontShades[1][i << 1].r = r;
        FontShades[1][i << 1].g = g;
        FontShades[1][i << 1].b = b;
        r = (uint8)CLRR(v.specular);
        g = (uint8)CLRG(v.specular);
        b = (uint8)CLRB(v.specular);
        FontShades[1][(i << 1) + 1].r = r;
        FontShades[1][(i << 1) + 1].g = g;
        FontShades[1][(i << 1) + 1].b = b;
    }
}

sint32 GetStringLength(const char *string, sint16 *top, sint16 *bottom)
{
    sint32 s, accent, length, lowest, highest, y;
    CHARDEF *def;

    accent = 0;
    length = 0;
    lowest = -1024;
    highest = 1024;
    s = (uint8)*string++;
    while (s) {
        if (s == '\n')
            break;
        if (s == ' ') {
            length += (sint32)(8.0F * ((float)(phd_winxmax + 1) * (1.0F / 640.0F)));
        } else if (s == '\t') {
            length += 40;
            if (top && highest > -12)
                highest = -12;
            if (bottom && lowest < 2)
                lowest = 2;
        } else if (s >= 20) {
            if (s < ' ') {
                def = &CharDef[s + 74];
            } else {
                if (s >= 128 && s <= 173) {
                    accent = 1;
                    s = (uint8)AccentTable[s - 128][0];
                }
                def = &CharDef[s - '!'];
            }
            if (ScaleFlag)
                length += def->w - def->w / 4;
            else
                length += def->w;
            y = def->YOffset;
            if (top && y < highest)
                highest = y;
            if (bottom && def->h + y > lowest)
                lowest = def->h + y;
        }
        s = (uint8)*string++;
    }
    if (top) {
        if (accent)
            highest -= 4;
        *top = (sint16)highest;
    }
    if (bottom)
        *bottom = (sint16)lowest;
    return length;
}

void PrintString(uint16 x, uint16 y, uint8 col, const char *string, uint16 flags)
{
    CHARDEF *def;
    CHARDEF *accent;
    sint32 l;
    sint16 bottom, top, bottom2;
    uint16 x2;
    uint8 s, n;

    if (flags & FF_BLINK && GnFrameCounter & 0x10)
        return;
    ScaleFlag = (flags & FF_SMALL) != 0;
    x2 = (uint16)GetStringLength(string, 0, &bottom);
    if (flags & FF_CENTER)
        x2 = x - (x2 >> 1);
    else if (flags & FF_RJUSTIFY)
        x2 = x - x2;
    else
        x2 = x;
    s = *string++;
    while (s) {
        if (s == '\n') {
            if (*string == '\n') {
                bottom = 0;
                y += 16;
            } else {
                l = GetStringLength(string, &top, &bottom2);
                if (flags & FF_CENTER)
                    x2 = x - l / 2;
                else if (flags & FF_RJUSTIFY)
                    x2 = x - l;
                else
                    x2 = x;
                y += bottom - top + 2;
                bottom = bottom2;
            }
        } else if (s == ' ') {
            if (ScaleFlag)
                x2 += 6;
            else
                x2 += (sint16)(8.0F * ((float)(phd_winxmax + 1) * (1.0F / 640.0F)));
        } else if (s == '\t') {
            x2 += 40;
        } else if (s < 20) {
            col = s - 1;
        } else {
            if (s >= 128 && s <= 173) {
                n = s - 128;
                def = &CharDef[AccentTable[n][0] - '!'];
                accent = &CharDef[AccentTable[n][1] - '!'];
                DrawChar(x2, y, col, def);
                if (AccentTable[n][1] != ' ')
                    DrawChar(def->w / 2 + x2 - 3, y + def->YOffset, col, accent);
            } else if (s < ' ') {
                def = &CharDef[s + 74];
                DrawChar(x2, y, col, def);
            } else {
                def = &CharDef[s - '!'];
                DrawChar(x2, y, col, def);
            }
            if (ScaleFlag)
                x2 += def->w - def->w / 4;
            else
                x2 += def->w;
        }
        s = *string++;
    }
    ScaleFlag = 0;
}

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];

void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def)
{
    D3DTLVERTEX v[4];
    TEXTURESTRUCT tex;
    float u1, v1, u2, v2;
    sint32 x1, y1, x2, y2, x3, y3, x4, y4, z, top, bottom;
    sint16 sx, sy;

    sy = y + phd_winymin;
    sx = x + phd_winxmin;
    z = (sint32)f_mznear;
    x1 = sx;
    x2 = x1 + def->w;
    y1 = sy + def->YOffset;
    y2 = y1;
    y3 = def->YOffset + def->h + sy;
    y4 = def->YOffset + def->h + sy;
    if (small_font) {
        y1 = (sint32)((float)y1 * 0.75F);
        y2 = (sint32)((float)y2 * 0.75F);
        y4 = (sint32)((float)y4 * 0.75F);
        y3 = (sint32)((float)y3 * 0.75F);
    }
    setXY4(v, x1, y1, x2, y2, x2, y3, x1, y4, z, clipflags);
    top = *(sint32 *)&FontShades[col][2 * def->TopShade];
    bottom = *(sint32 *)&FontShades[col][2 * def->BottomShade];
    v[0].color = top;
    v[1].color = top;
    v[2].color = bottom;
    v[3].color = bottom;
    top = *(sint32 *)&FontShades[col][2 * def->TopShade + 1];
    bottom = *(sint32 *)&FontShades[col][2 * def->BottomShade + 1];
    v[0].specular = top;
    v[1].specular = top;
    v[2].specular = bottom;
    v[3].specular = bottom;
    u1 = def->u + (1.0F / 512.0F);
    v1 = def->v + (1.0F / 512.0F);
    u2 = 512.0F / (float)(phd_winxmax + 1) * def->w * (1.0F / 256.0F) + def->u - (1.0F / 512.0F);
    v2 = 240.0F / (float)(phd_winymax + 1) * def->h * (1.0F / 256.0F) + def->v - (1.0F / 512.0F);
    tex.u1 = u1;
    tex.v1 = v1;
    tex.u2 = u2;
    tex.v2 = v1;
    tex.u3 = u2;
    tex.v3 = v2;
    tex.u4 = u1;
    tex.v4 = v2;
    tex.drawtype = 1;
    tex.tpage = (uint16)(nTextures - 2);
    tex.flag = 0;
    nPolyType = 4;
    AddQuadClippedSorted(v, 0, 1, 2, 3, &tex, 0);
}
void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
 
 extern CVECTOR FontShades[10][32];
 extern CVECTOR ShadeFromTo[10][2];
