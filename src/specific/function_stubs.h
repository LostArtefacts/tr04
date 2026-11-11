#ifndef SPECIFIC_FUNCTION_STUBS_H
#define SPECIFIC_FUNCTION_STUBS_H

#include "../3dglodef.h"

void Log(unsigned int type, const char *s, ...);
void *game_malloc(int size);

extern PHD_VECTOR CamPos;
extern PHD_VECTOR CamRot;

#endif
