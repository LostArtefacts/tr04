#include "winmain.h"
#include "output.h"
#include "dxshell.h"
#include "function_table.h"
#include "time.h"
#include "../text.h"
#include "d3dmatrix.h"
#include "drawroom.h"

void _InsertRoom(ROOM_INFO *r)
{
    SetD3DViewMatrix();
    InsertRoom(r);
}

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
