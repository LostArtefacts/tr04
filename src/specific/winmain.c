#include <stdio.h>
#include <stdarg.h>

#include "winmain.h"
#include "../text.h"
#include "function_stubs.h"
#include "registry.h"

sint32 CheckMMXTechnology(void)
{
    uint32 _edx;
    sint32 mmx;

    mmx = 1;
    __try {
        __asm {
            pushad
            mov eax, 1
            cpuid
            mov _edx, edx
            popad
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        mmx = 0;
    }
    if (mmx && _edx & 0x800000) {
        __try {
            __asm {
                emms
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            mmx = 0;
        }
        return mmx;
    }
    return 0;
}

void WinClose(void)
{
    Log(2, "WinClose");
    SaveSettings();
    CloseHandle(App.mutex);
    DXFreeInfo(&App.DXInfo);
    DestroyAcceleratorTable(App.hAccel);
    DXClose();
    Func494400();
}

uint8 WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE *mutex)
{
    HWND window;

    Log(2, "WinRunCheck");
    *mutex = CreateMutex(0, 1, WindowName);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        window = FindWindow(ClassName, WindowName);
        if (window)
            SendMessage(window, WM_ACTIVATE, 0, 0);
        return 1;
    }
    return 0;
}

void WinDisplayString(sint32 x, sint32 y, char *string, ...)
{
    va_list list;
    char buf[4096];

    va_start(list, string);
    vsprintf(buf, string, list);
    PrintString((uint16)x, (uint16)y, 6, buf, 0);
}

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
