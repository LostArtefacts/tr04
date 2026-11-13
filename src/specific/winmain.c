#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include "winmain.h"
#include "../text.h"
#include "function_stubs.h"
#include "registry.h"
#include "dxshell.h"
#include "function_table.h"
#include "output.h"
#include "../deltapak.h"
#include "../gameflow.h"
#include "audio.h"
#include "dxsound.h"
#include "cmdline.h"
#include "d3dmatrix.h"
#include "3dmath.h"
#include "lighting.h"
#include "file.h"
#include "gamemain.h"
#include "audio.h"

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

#ifndef TR4_EXE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    DXDISPLAYMODE *dm;
    RECT r;
    HWND desktop;
    HDC hdc;
    DEVMODE devmode;
    char *buf;
    sint32 size;

    start_setup = 0;
    App.mmx = CheckMMXTechnology() != 0;
    App.SetupComplete = 0;
    App.AutoTarget = 0;
    if (WinRunCheck("Tomb Raider - The Last Revelation", "MainGameWindow", &App.mutex))
        return 0;
    FindCDDrive();
    LoadGameflow();
    WinProcessCommandLine(lpCmdLine);
    App.hInstance = hInstance;
    App.WindowClass.hIcon = 0;
    App.WindowClass.lpszMenuName = 0;
    App.WindowClass.lpszClassName = "MainGameWindow";
    App.WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    App.WindowClass.hInstance = hInstance;
    App.WindowClass.style = CS_VREDRAW | CS_HREDRAW;
    App.WindowClass.lpfnWndProc = WinMainWndProc;
    App.WindowClass.cbClsExtra = 0;
    App.WindowClass.cbWndExtra = 0;
    App.WindowClass.hCursor = LoadCursor(App.hInstance, MAKEINTRESOURCE(104));
    if (!RegisterClass(&App.WindowClass)) {
        Log(1, "Unable To Register Window Class");
        return 0;
    }
    r.left = 0;
    r.top = 0;
    r.right = 640;
    r.bottom = 480;
    AdjustWindowRect(&r, WS_CAPTION, 0);
    App.hWnd = CreateWindowEx(WS_EX_APPWINDOW, "MainGameWindow", "Tomb Raider - The Last Revelation", WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, 0, 0, hInstance, 0);
    if (!App.hWnd) {
        Log(1, "Unable To Create Window");
        return 0;
    }
    DXGetInfo(&App.DXInfo, App.hWnd);
    if (start_setup || !LoadSettings()) {
        if (!DXSetupDialog()) {
            free(gfScriptFile);
            free(gfLanguageFile);
            WinClose();
            return 0;
        }
        LoadSettings();
    }
    SetWindowPos(App.hWnd, 0, App.dx.rScreen.left, App.dx.rScreen.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    desktop = GetDesktopWindow();
    hdc = GetDC(desktop);
    App.Desktopbpp = GetDeviceCaps(hdc, BITSPIXEL);
    ReleaseDC(desktop, hdc);
    App.dx.WaitAtBeginScene = 0;
    App.dx.InScene = 0;
    App.fmv = 0;
    dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
    if (!DXCreate(dm->w, dm->h, dm->bpp, App.StartFlags, &App.dx, App.hWnd, WS_CAPTION)) {
        MessageBox(0, SCRIPT_TEXT(TXT_Failed_To_Setup_DirectX), "Tomb Raider IV", 0);
        return 0;
    }
    UpdateWindow(App.hWnd);
    ShowWindow(App.hWnd, nShowCmd);
    if (App.dx.Flags & 1) {
        SetCursor(0);
        ShowCursor(0);
    }
    DXInitKeyboard(App.hWnd, App.hInstance);
    App.hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(101));
    if (!App.SoundDisabled) {
        DXDSCreate();
        ACMInit();
    }
    cutseqpakPtr = 0;
    buf = 0;
    size = LoadFile("data\\cutseq.pak", &buf);
    if (size) {
        cutseqpakPtr = (char *)malloc(*(sint32 *)buf);
        Decompress(cutseqpakPtr, buf + 4, size - 4, *(sint32 *)buf);
        free(buf);
    }
    MainThread.active = 1;
    MainThread.ended = 0;
    MainThread.handle = _beginthreadex(0, 0, GameMain, 0, 0, (unsigned int *)&MainThread.address);
    WinProcMsg();
    MainThread.ended = 1;
    while (MainThread.active) {
    }
    if (cutseqpakPtr)
        free(cutseqpakPtr);
    WinClose();
    desktop = GetDesktopWindow();
    hdc = GetDC(desktop);
    devmode.dmSize = sizeof(DEVMODE);
    devmode.dmBitsPerPel = App.Desktopbpp;
    ReleaseDC(desktop, hdc);
    devmode.dmFields = DM_BITSPERPEL;
    ChangeDisplaySettings(&devmode, 0);
    return 0;
}
#endif

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

#ifndef TR4_EXE
LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static sint32 mouseY, mouseX, mouseB;
    static uint8 closing;

    switch (uMsg) {
    case WM_CREATE:
        resChangeCounter = 0;
        Log(6, "WM_CREATE");
        break;
    case WM_MOVE:
        Log(6, "WM_MOVE");
        DXMove((sint16)lParam, (sint16)((uint32)lParam >> 16));
        break;
    case WM_ACTIVATE:
        Log(6, "WM_ACTIVATE");
        if (!closing) {
            if (App.fmv)
                return 0;
            switch ((sint32)(wParam & 0xFFFF)) {
            case WA_INACTIVE:
                Log(6, "WM_INACTIVE");
                if (App.SetupComplete) {
                    Log(5, "HangGameThread");
                    while (App.dx.InScene) {
                    }
                    App.dx.WaitAtBeginScene = 1;
                    while (!App.dx.InScene) {
                    }
                    SuspendThread((HANDLE)MainThread.handle);
                    Log(5, "Game Thread Suspended");
                }
                break;
            case WA_ACTIVE:
            case WA_CLICKACTIVE:
                Log(6, "WM_ACTIVE");
                if (App.SetupComplete) {
                    ResumeThread((HANDLE)MainThread.handle);
                    App.dx.WaitAtBeginScene = 0;
                    Log(5, "Game Thread Resumed");
                }
                return 0;
            }
            return 0;
        }
        break;
    case WM_CLOSE:
        Log(6, "WM_CLOSE");
        closing = 1;
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        Log(6, "WM_COMMAND");
        WinProcessCommands(wParam & 0xFFFF);
        break;
    case WM_MOUSEMOVE:
        mouseX = (sint16)lParam;
        mouseY = (sint16)((uint32)lParam >> 16);
        mouseB = wParam;
        break;
    case WM_APP:
        FillADPCMBuffer((char *)lParam, wParam);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif

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

void WinProcessCommandLine(char *cmd)
{
    COMMANDLINES *command;
    char *pCommand;
    char *p;
    uint32 i, j, l, num;
    char parameter[20];

    Log(2, "WinProcessCommandLine");
    num = sizeof(commandlines) / sizeof(commandlines[0]);
    for (i = 0; i < num; i++) {
        commandlines[i].code("_INIT");
    }
    for (i = 0; i < strlen(cmd); i++) {
        if (toupper((uint8)cmd[i]))
            cmd[i] = toupper((uint8)cmd[i]);
    }
    for (i = 0; i < num; i++) {
        memset(parameter, 0, sizeof(parameter));
        pCommand = strstr(cmd, commandlines[i].command);
        if (pCommand) {
            if (commandlines[i].needs_parameter) {
                l = strlen(pCommand);
                for (j = 0; j < l; j++, pCommand++) {
                    if (*pCommand == '=') {
                        p = pCommand + 1;
                        l = strlen(p);
                        for (j = 0; j < l; j++, p++) {
                            if (*p != ' ')
                                break;
                        }
                        l = strlen(p);
                        for (j = 0; j < l; j++) {
                            if (j[p] == ' ')
                                break;
                        }
                        strncpy(parameter, p, j);
                        break;
                    }
                }
                commandlines[i].code(parameter);
            } else {
                commandlines[i].code(0);
            }
        }
    }
}

float WinFrameRate(void)
{
    static char first_time;
    static sint32 time;
    static sint32 counter;
    static float fps;
    double t;
    sint32 now;

    if (!(first_time & 1)) {
        first_time |= 1;
        time = clock();
    }
    counter++;
    if (counter == 10) {
        now = clock();
        t = (double)(now - time) * 0.001;
        time = now;
        fps = (float)(counter / t);
        counter = 0;
    }
    App.fps = fps;
    return fps;
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

void j_nullsub_11(void)
{
    nullsub_11();
}

void nullsub_11(void)
{
}

void j_nullsub_12(void)
{
    nullsub_12();
}

void nullsub_12(void)
{
}

void j_nullsub_13(void)
{
    nullsub_13();
}

void nullsub_13(void)
{
}
