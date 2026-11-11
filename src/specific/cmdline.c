#include <string.h>

#include "cmdline.h"
#include "function_stubs.h"
#include "winmain.h"
#include "../gameflow.h"

void CLSetup(char *cmd)
{
    Log(2, "CLSetup");
    if (cmd)
        start_setup = 0;
    else
        start_setup = 1;
}

void CLNoFMV(char *cmd)
{
    Log(2, "CLNoFMV");
    if (cmd)
        fmvs_disabled = 0;
    else
        fmvs_disabled = 1;
}

uint8 DXSetupDialog(void)
{
    INT_PTR ret;

    ShowCursor(1);
    ret = DialogBoxParam(App.hInstance, MAKEINTRESOURCE(109), 0, DXSetupDlgProc, 0);
    ShowCursor(0);
    if (ret == -1) {
        MessageBox(0, "Unable To Initialise Dialog", "", 0);
        return 0;
    }
    ret = ret != 0;
    return ret;
}

void InitDSDevice(HWND dlg, HWND hwnd)
{
    sint32 i;

    SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
    for (i = 0; i < App.DXInfo.nDSInfo; i++)
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)App.DXInfo.DSInfo[i].About);
    nDSDevice = 0;
    if (!App.DXInfo.nDSInfo) {
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)SCRIPT_TEXT(TXT_No_Sound_Card_Installed));
        EnableWindow(GetDlgItem(dlg, 1018), 0);
        SendMessage(GetDlgItem(dlg, 1018), BM_SETCHECK, 1, 0);
        EnableWindow(hwnd, 0);
    }
    SendMessage(hwnd, CB_SETCURSEL, 0, 0);
}

void InitTFormats(HWND dlg, HWND hwnd)
{
    DXTEXTUREINFO *tex;
    sint32 i;
    sint32 bpp;
    sint32 r;
    sint32 g;
    sint32 b;
    sint32 a;
    char buffer[40];
    uint8 software;

    SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
    EnableWindow(GetDlgItem(dlg, 1006), 1);
    software = SendMessage(GetDlgItem(dlg, 1011), BM_GETCHECK, 0, 0) != 0;
    for (i = 0; i < App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice].nTextureInfos; i++) {
        tex = &App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice].TextureInfos[i];
        r = tex->rbpp;
        a = tex->abpp;
        bpp = tex->bpp;
        b = tex->bbpp;
        g = tex->gbpp;
        wsprintf(buffer, "%d %s RGBA %d%d%d%d", bpp, SCRIPT_TEXT(TXT_Bit), r, g, b, a);
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
        if (software) {
            if (bpp == 32 && r == 8 && b == 8 && g == 8 && a == 8) {
                SendMessage(hwnd, CB_SETCURSEL, i, 0);
                nTFormat = i;
                EnableWindow(GetDlgItem(dlg, 1006), 0);
            }
        } else if (bpp == 16 && r == 5 && b == 5 && g == 5 && a == 1) {
            SendMessage(hwnd, CB_SETCURSEL, i, 0);
            nTFormat = i;
        } else {
            SendMessage(hwnd, CB_SETCURSEL, 0, 0);
            nTFormat = 0;
        }
    }
}


void InitResolution(HWND dlg, HWND hwnd, uint8 resetvms)
{
    DXDISPLAYMODE *dm;
    sint32 i;
    sint32 bpp;
    sint32 w;
    sint32 h;
    sint32 n;
    char buffer[40];
    uint8 software;

    n = 0;
    if (nD3DDevice) {
        SendMessage(GetDlgItem(dlg, 1010), BM_SETCHECK, 1, 0);
        SendMessage(GetDlgItem(dlg, 1011), BM_SETCHECK, 0, 0);
    } else {
        SendMessage(GetDlgItem(dlg, 1010), BM_SETCHECK, 0, 0);
        SendMessage(GetDlgItem(dlg, 1011), BM_SETCHECK, 1, 0);
    }
    software = SendMessage(GetDlgItem(dlg, 1011), BM_GETCHECK, 0, 0) != 0;
    if (resetvms) {
        SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
        for (i = 0; i < App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice].nDisplayModes; i++) {
            dm = &App.DXInfo.DDInfo[nDDDevice].D3DDevices[nD3DDevice].DisplayModes[i];
            w = dm->w;
            h = dm->h;
            bpp = dm->bpp;
            if (bpp > 8) {
                wsprintf(buffer, "%dx%d %d %s", w, h, bpp, SCRIPT_TEXT(TXT_Bit));
                SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
                SendMessage(hwnd, CB_SETITEMDATA, n, i);
                if (software) {
                    if (w == 320 && h == 240 && bpp == 16) {
                        SendMessage(hwnd, CB_SETCURSEL, n, 0);
                        nVMode = n;
                    }
                } else if (w == 640 && h == 480 && bpp == 16) {
                    SendMessage(hwnd, CB_SETCURSEL, n, 0);
                    nVMode = n;
                }
                n++;
            }
        }
    }
    if (!(App.DXInfo.DDInfo[nDDDevice].DDCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)) {
        EnableWindow(GetDlgItem(dlg, 1025), 0);
        SendMessage(GetDlgItem(dlg, 1025), BM_SETCHECK, 0, 0);
    } else {
        EnableWindow(GetDlgItem(dlg, 1025), 1);
    }
    SendMessage(GetDlgItem(dlg, 1012), BM_SETCHECK, Filter, 0);
    if (software) {
        EnableWindow(GetDlgItem(dlg, 1029), 0);
        volumetric_fx = 0;
    } else {
        EnableWindow(GetDlgItem(dlg, 1029), 1);
    }
    SendMessage(GetDlgItem(dlg, 1029), BM_SETCHECK, volumetric_fx, 0);
    if (software) {
        EnableWindow(GetDlgItem(dlg, 1016), 0);
        BumpMap = 0;
    } else {
        EnableWindow(GetDlgItem(dlg, 1016), 1);
    }
    SendMessage(GetDlgItem(dlg, 1016), BM_SETCHECK, BumpMap, 0);
    if (software) {
        EnableWindow(GetDlgItem(dlg, 1014), 0);
        TextLow = 0;
    } else {
        EnableWindow(GetDlgItem(dlg, 1014), 1);
    }
    SendMessage(GetDlgItem(dlg, 1014), BM_SETCHECK, TextLow, 0);
    if (TextLow) {
        SendMessage(GetDlgItem(dlg, 1015), BM_SETCHECK, 1, 0);
        EnableWindow(GetDlgItem(dlg, 1015), 0);
    } else {
        EnableWindow(GetDlgItem(dlg, 1015), 1);
        SendMessage(GetDlgItem(dlg, 1015), BM_SETCHECK, 0, 0);
    }
    if (!BumpMap) {
        SendMessage(GetDlgItem(dlg, 1015), BM_SETCHECK, 0, 0);
        EnableWindow(GetDlgItem(dlg, 1015), 0);
    }
    if (resetvms)
        InitTFormats(dlg, GetDlgItem(dlg, 1006));
}

void InitD3DDevice(HWND dlg, HWND hwnd)
{
    sint32 i;

    SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
    for (i = 0; i < App.DXInfo.DDInfo[nDDDevice].nD3DDevices; i++)
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)App.DXInfo.DDInfo[nDDDevice].D3DDevices[i].About);
    SendMessage(hwnd, CB_SETCURSEL, 1, 0);
    nD3DDevice = 1;
    InitResolution(dlg, GetDlgItem(dlg, 1004), 1);
}

void InitDDDevice(HWND dlg, HWND hwnd)
{
    sint32 i;
    char buffer[256];

    SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
    for (i = 0; i < App.DXInfo.nDDInfo; i++) {
        wsprintf(buffer, "%s - %s (%d.%d.%02d.%04d)", App.DXInfo.DDInfo[i].DDIdentifier.szDescription, App.DXInfo.DDInfo[i].DDIdentifier.szDriver, HIWORD(App.DXInfo.DDInfo[i].DDIdentifier.liDriverVersion.HighPart), LOWORD(App.DXInfo.DDInfo[i].DDIdentifier.liDriverVersion.HighPart), HIWORD(App.DXInfo.DDInfo[i].DDIdentifier.liDriverVersion.LowPart), LOWORD(App.DXInfo.DDInfo[i].DDIdentifier.liDriverVersion.LowPart));
        SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
    }
    SendMessage(hwnd, CB_SETCURSEL, App.DXInfo.nDDInfo - 1, 0);
    nDDDevice = App.DXInfo.nDDInfo - 1;
    InitD3DDevice(dlg, GetDlgItem(dlg, 1003));
}

char *MapASCIIToANSI(char *s, char *d)
{
    char *p;
    sint32 l;
    uint8 c;
    uint8 found;
    int i, j;

    l = strlen(s);
    p = d;
    for (i = 0; i < l; i++) {
        c = *s++;
        if (c >= 0x80) {
            found = 0;
            for (j = 0; j < 7; j++) {
                if (c == ASCIIToANSITable[j][0]) {
                    c = ASCIIToANSITable[j][1];
                    found = 1;
                    break;
                }
            }
            if (!found)
                Log(1, "Reqd : %x", c);
        }
        *d++ = c;
    }
    *d = 0;
    return p;
}
