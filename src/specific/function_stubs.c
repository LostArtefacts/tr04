#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "function_stubs.h"

extern sint32 rand_1;
extern sint32 rand_2;
extern char *malloc_buffer;
extern sint32 malloc_size;
extern char *malloc_ptr;
extern sint32 malloc_free;
extern sint32 malloc_used;

void other_log(char *format, ...)
{
    va_list arglist;
    char buffer[4096];

    va_start(arglist, format);
    vsprintf(buffer, format, arglist);
    Log(0, buffer);
}

void S_ExitSystem(const char *string)
{
    Log(0, "**** %s ****", string);
}

sint32 GetRandomControl(void)
{
    rand_1 = 0x41C64E6D * rand_1 + 12345;
    return (rand_1 >> 10) & 0x7FFF;
}

sint32 GetRandomDraw(void)
{
    rand_2 = 0x41C64E6D * rand_2 + 12345;
    return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomControl(sint32 seed)
{
    rand_1 = seed;
}

void SeedRandomDraw(sint32 seed)
{
    rand_2 = seed;
}

void init_game_malloc(void)
{
    malloc_buffer = (char *)malloc(MALLOC_SIZE);
    malloc_size = MALLOC_SIZE;
    malloc_ptr = malloc_buffer;
    malloc_free = MALLOC_SIZE;
    malloc_used = 0;
}

void *game_malloc(int size)
{
    char *ptr;

    size = (size + 3) & -4;
    if (size <= malloc_free) {
        ptr = malloc_ptr;
        malloc_free -= size;
        malloc_used += size;
        malloc_ptr += size;
        memset(ptr, 0, size);
        return ptr;
    }
    Log(0, "OUT OF MEMORY");
    return NULL;
}
