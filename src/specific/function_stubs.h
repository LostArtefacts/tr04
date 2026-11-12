#ifndef SPECIFIC_FUNCTION_STUBS_H
#define SPECIFIC_FUNCTION_STUBS_H

#include "../3dglodef.h"

#define MALLOC_SIZE 5000000

void Log(unsigned int type, const char *s, ...);
void other_log(char *format, ...);
void S_ExitSystem(const char *string);
sint32 GetRandomControl(void);
sint32 GetRandomDraw(void);
void SeedRandomControl(sint32 seed);
void SeedRandomDraw(sint32 seed);
void init_game_malloc(void);
void *game_malloc();

extern char *malloc_buffer;
extern PHD_VECTOR CamPos;
extern PHD_VECTOR CamRot;

#endif
