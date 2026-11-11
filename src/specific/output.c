#include "winmain.h"
#include "output.h"
#include "dxshell.h"
#include "function_table.h"
#include "time.h"
#include "../text.h"

sint32 S_DumpScreen(void)
{
    sint32 n;

    n = Sync();
    while (n < 2) {
        while (!Sync())
            ;
        n++;
    }
    GnFrameCounter++;
    _EndScene();
    DXShowFrame();
    App.dx.DoneBlit = 1;
    return n;
}
