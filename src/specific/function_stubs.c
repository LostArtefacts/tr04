#include <stdarg.h>
#include <stdio.h>

#include "function_stubs.h"

extern sint32 rand_1;
extern sint32 rand_2;

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
