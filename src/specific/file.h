#ifndef SPECIFIC_FILE_H
#define SPECIFIC_FILE_H

#include <stdio.h>
#include "../3dglodef.h"

extern TEXTURESTRUCT *AnimatingWaterfalls[3];
extern sint32 AnimatingWaterfallsV[3];

extern char *FileData;
extern sint32 number_cameras;

FILE *FileOpen(const char *name);
void FileClose(FILE *file);
sint32 FileSize(FILE *file);
uint8 LoadCinematic(void);
sint32 LoadFile(const char *name, char **dest);
uint8 LoadSoundEffects(void);
uint8 LoadCameras(void);

#endif
