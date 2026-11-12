#include <stdio.h>
#include <stdarg.h>

#include "winmain.h"
#include "../text.h"

sint32 MungeFPCW(sint16 *fpcw)
{
    sint32 ret;
    uint16 cw, temp;

    ret = 0;
    __asm {
        fstcw cw
    }
    if (cw & 0x300 || (cw & 0x3F) != 0x3F || cw & 0xC00) {
        __asm {
            mov ax, cw
            and ax, not 0x300
            or ax, 0x3F
            and ax, not 0xC00
            mov temp, ax
            fldcw temp
        }
        ret = 1;
    }
    *fpcw = cw;
    return ret;
}

void RestoreFPCW(sint16 fpcw)
{
    __asm {
        fldcw fpcw
    }
}
