#ifndef SPECIFIC_OUTPUT_H
#define SPECIFIC_OUTPUT_H

#include "../types.h"

extern sint32 GlobalAlpha;
extern sint32 GlobalAmbient;

void S_AnimateTextures(sint32 n);
sint32 S_DumpScreen(void);
void S_InitialisePolyList(void);
void S_OutputPolyList(void);
sint32 S_GetObjectBounds(sint16 *bounds);
void _InsertRoom();

#endif
