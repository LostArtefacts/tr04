#include <windows.h>

#include "time.h"
#include "function_stubs.h"

extern __int64 counter;
extern __int64 frequency;

void TIME_Reset(void)
{
    QueryPerformanceCounter((LARGE_INTEGER *)&counter);
}

uint8 TIME_Init(void)
{
    __int64 pfq;

    Log(2, "TIME_Init");
    if (!QueryPerformanceFrequency((LARGE_INTEGER *)&pfq))
        return 0;
    frequency = pfq / 60;
    TIME_Reset();
    return 1;
}

int Sync(void)
{
    __int64 PerformanceCount, f;

    QueryPerformanceCounter((LARGE_INTEGER *)&PerformanceCount);
    f = (PerformanceCount - counter) / frequency;
    counter += frequency * f;
    return (int)f;
}
