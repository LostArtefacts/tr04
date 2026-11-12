#include <stdio.h>
#include <stdarg.h>

#include "winmain.h"
#include "../text.h"

void RestoreFPCW(sint16 fpcw)
{
    __asm {
        fldcw fpcw
    }
}
