#ifndef SPECIFIC_FILE_H
#define SPECIFIC_FILE_H

#include <stdio.h>
#include "../3dglodef.h"

extern TEXTURESTRUCT *AnimatingWaterfalls[3];
extern sint32 AnimatingWaterfallsV[3];

FILE *FileOpen(const char *name);

#endif
