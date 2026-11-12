#include <stdio.h>
#include <stdarg.h>

#include "winmain.h"
#include "../text.h"
#include "function_stubs.h"
#include "registry.h"
#include "dxshell.h"
#include "function_table.h"
#include "output.h"
#include "d3dmatrix.h"
#include "3dmath.h"
#include "lighting.h"
#include "file.h"
#include "gamemain.h"

void ClearSurfaces(void)
{
    D3DRECT r;

    r.x1 = App.dx.rViewport.left;
    r.y1 = App.dx.rViewport.top;
    r.x2 = App.dx.rViewport.left + App.dx.rViewport.right;
    r.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
    if (App.dx.Flags & 0x80)
        DXAttempt(IDirect3DViewport3_Clear2(App.dx.lpViewport, 1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));
    else
        SW_ClearBuffer(App.dx.lpD3DDevice, 1, &r, D3DCLEAR_TARGET, 0, 1.0F, 0);
    S_DumpScreen();
    if (App.dx.Flags & 0x80)
        DXAttempt(IDirect3DViewport3_Clear2(App.dx.lpViewport, 1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));
    else
        SW_ClearBuffer(App.dx.lpD3DDevice, 1, &r, D3DCLEAR_TARGET, 0, 1.0F, 0);
    S_DumpScreen();
}

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

void WinProcMsg(void)
{
    MSG msg;

    Log(2, "WinProcMsg");
    do {
        GetMessage(&msg, 0, 0, 0);
        if (!TranslateAccelerator(App.hWnd, App.hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } while (!thread_ended && msg.message != WM_QUIT);
}

void WinProcessCommands(sint32 cmd)
{
    switch (cmd) {
    case 40001:
    case 40002: {
        sint32 odm;

        if (LevelLoadingThread.active || App.fmv)
            return;
        Log(5, "Change Video Mode");
        Log(5, "HangGameThread");
        while (App.dx.InScene) {
        }
        App.dx.WaitAtBeginScene = 1;
        while (!App.dx.InScene) {
        }
        SuspendThread((HANDLE)MainThread.handle);
        Log(5, "Game Thread Suspended");
        odm = App.DXInfo.nDisplayMode;
        if (cmd == 40001) {
            App.DXInfo.nDisplayMode++;
            if (App.DXInfo.nDisplayMode >= G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes)
                App.DXInfo.nDisplayMode = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes - 1;
            while (G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes[App.DXInfo.nDisplayMode].bpp != G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes[odm].bpp) {
                App.DXInfo.nDisplayMode++;
                if (App.DXInfo.nDisplayMode >= G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes) {
                    App.DXInfo.nDisplayMode = odm;
                    break;
                }
            }
        } else {
            App.DXInfo.nDisplayMode--;
            if (App.DXInfo.nDisplayMode < 0)
                App.DXInfo.nDisplayMode = 0;
            while (G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes[App.DXInfo.nDisplayMode].bpp != G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes[odm].bpp) {
                App.DXInfo.nDisplayMode--;
                if (App.DXInfo.nDisplayMode < 0) {
                    App.DXInfo.nDisplayMode = odm;
                    break;
                }
            }
        }
        if (odm != App.DXInfo.nDisplayMode) {
            FreeD3DLights();
            if (!DXChangeVideoMode()) {
                App.DXInfo.nDisplayMode = odm;
                DXChangeVideoMode();
            }
            HWInitialise();
            CreateD3DLights();
            InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
            InitFont();
            S_InitD3DMatrix();
            SetD3DViewMatrix();
        }
        ResumeThread((HANDLE)MainThread.handle);
        App.dx.WaitAtBeginScene = 0;
        Log(5, "Game Thread Resumed");
        resChangeCounter = 120;
        break;
    }
    case 8:
        if (App.fmv || !(G_dxinfo->DDInfo[G_dxinfo->nDD].DDCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) || LevelLoadingThread.active)
            return;
        Log(6, "KA_ALTENTER");
        Log(5, "HangGameThread");
        while (App.dx.InScene) {
        }
        App.dx.WaitAtBeginScene = 1;
        while (!App.dx.InScene) {
        }
        SuspendThread((HANDLE)MainThread.handle);
        Log(5, "Game Thread Suspended");
        FreeD3DLights();
        DXToggleFullScreen();
        HWInitialise();
        CreateD3DLights();
        S_InitD3DMatrix();
        SetD3DViewMatrix();
        ResumeThread((HANDLE)MainThread.handle);
        App.dx.WaitAtBeginScene = 0;
        Log(5, "Game Thread Resumed");
        if (App.dx.Flags & 1) {
            SetCursor(0);
            ShowCursor(0);
        } else {
            SetCursor(LoadCursor(App.hInstance, MAKEINTRESOURCE(104)));
            ShowCursor(1);
        }
        break;
    }
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
