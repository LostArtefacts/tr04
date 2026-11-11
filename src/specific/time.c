#include <windows.h>

#include "time.h"

extern __int64 counter;
extern __int64 frequency;

void TIME_Reset(void)
{
    QueryPerformanceCounter((LARGE_INTEGER *)&counter);
}
