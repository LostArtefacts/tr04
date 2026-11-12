#ifndef SPECIFIC_OUTPUT_H
#define SPECIFIC_OUTPUT_H

#include "../types.h"

extern sint32 GlobalAlpha;

void S_AnimateTextures(sint32 n);
sint32 S_DumpScreen(void);
void S_OutputPolyList(void);
void _InsertRoom();

#endif
