#ifndef SPECIFIC_CMDLINE_H
#define SPECIFIC_CMDLINE_H

#include <windows.h>

#include "../types.h"

void CLSetup(char *cmd);
void CLNoFMV(char *cmd);
uint8 DXSetupDialog(void);
char *MapASCIIToANSI(char *s, char *d);
BOOL CALLBACK DXSetupDlgProc(HWND dlg, UINT message, WPARAM wParam, LPARAM lParam);

extern uint8 start_setup;
extern uint8 fmvs_disabled;
extern uint8 ASCIIToANSITable[7][2];

#endif
