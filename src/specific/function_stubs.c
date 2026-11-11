#include <stdarg.h>
#include <stdio.h>

#include "function_stubs.h"

void other_log(char *format, ...)
{
    va_list arglist;
    char buffer[4096];

    va_start(arglist, format);
    vsprintf(buffer, format, arglist);
    Log(0, buffer);
}
