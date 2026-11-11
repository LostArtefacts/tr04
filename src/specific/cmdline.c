#include <string.h>

#include "cmdline.h"
#include "function_stubs.h"
#include "winmain.h"

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
