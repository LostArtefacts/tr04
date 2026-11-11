#ifndef SPECIFIC_CMDLINE_H
#define SPECIFIC_CMDLINE_H

#include <windows.h>

#include "../types.h"

void CLSetup(char *cmd);
void CLNoFMV(char *cmd);
uint8 DXSetupDialog(void);
void InitDSDevice(HWND dlg, HWND hwnd);
void InitTFormats(HWND dlg, HWND hwnd);
void InitResolution(HWND dlg, HWND hwnd, uint8 resetvms);
void InitD3DDevice(HWND dlg, HWND hwnd);
void InitDDDevice(HWND dlg, HWND hwnd);
char *MapASCIIToANSI(char *s, char *d);
BOOL CALLBACK DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam);

extern uint8 start_setup;
extern uint8 fmvs_disabled;
extern sint32 nDSDevice;
extern sint32 nVMode;
extern sint32 nTFormat;
extern sint32 nD3DDevice;
extern sint32 nDDDevice;
extern uint8 ASCIIToANSITable[7][2];
extern uint8 Filter;
extern uint8 TextLow;
extern uint8 BumpMap;
extern uint8 volumetric_fx;

#endif
