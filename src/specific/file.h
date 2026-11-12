#ifndef SPECIFIC_FILE_H
#define SPECIFIC_FILE_H

#include <stdio.h>
#include "../3dglodef.h"

extern TEXTURESTRUCT *AnimatingWaterfalls[3];
extern sint32 AnimatingWaterfallsV[3];

extern char *FileData;

FILE *FileOpen(const char *name);
void FileClose(FILE *file);
sint32 FileSize(FILE *file);
uint8 LoadCinematic(void);

#endif
