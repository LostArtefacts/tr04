#ifndef TEXT_H
#define TEXT_H

#include "types.h"

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

typedef struct cvector {
    uint8 b;
    uint8 g;
    uint8 r;
    uint8 a;
} CVECTOR;

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

extern CVECTOR FontShades[10][32];
extern CVECTOR ShadeFromTo[10][2];
extern CHARDEF CharDef[106];
extern CHARDEF CharDefCopy[106];
extern sint32 font_init;
extern uint8 ScaleFlag;
extern uint8 PulseCnt;
extern char AccentTable[46][2];
extern sint32 small_font;
extern sint32 font_height;
extern sint32 GnFrameCounter;

#endif
