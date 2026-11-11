#ifndef TEXT_H
#define TEXT_H

#include "3dglodef.h"

#define RGBONLY(r, g, b) ((b) | (((g) | ((r) << 8)) << 8))
#define CLRR(clr) (((clr) >> 16) & 0xFF)
#define CLRG(clr) (((clr) >> 8) & 0xFF)
#define CLRB(clr) ((clr) & 0xFF)

enum font_flags {
    FF_SMALL = 0x1000,
    FF_BLINK = 0x2000,
    FF_RJUSTIFY = 0x4000,
    FF_CENTER = 0x8000
};

typedef struct chardef {
    float u;
    float v;
    sint16 w;
    sint16 h;
    sint16 YOffset;
    sint8 TopShade;
    sint8 BottomShade;
} CHARDEF;

void InitFont(void);
void UpdatePulseColour(void);
void DrawChar(sint16 x, sint16 y, uint16 col, CHARDEF *def);
void PrintString(uint16 x, uint16 y, uint8 col, const char *string, uint16 flags);
sint32 GetStringLength(const char *string, sint16 *top, sint16 *bottom);

extern CVECTOR FontShades[10][32];
extern CVECTOR ShadeFromTo[10][2];
extern CHARDEF CharDef[106];
extern CHARDEF CharDefCopy[106];
extern sint32 font_init;
extern uint8 ScaleFlag;
extern uint8 PulseCnt;
extern uint8 AccentTable[46][2];
extern sint32 small_font;
extern sint32 font_height;
extern sint32 GnFrameCounter;

#endif
