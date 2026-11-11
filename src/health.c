#include <stdio.h>

#include "health.h"
#include "control.h"
#include "deltapak.h"
#include "gameflow.h"
#include "lara.h"
#include "newinv.h"
#include "savegame.h"
#include "spotcam.h"
#include "text.h"
#include "specific/3dmath.h"
#include "specific/loadsave.h"

sint32 FlashIt(void)
{
    if (flash_count) {
        flash_count--;
    } else {
        flash_state ^= 1;
        flash_count = 5;
    }
    return flash_state;
}

void DrawGameInfo(sint32 timed)
{
    sint32 flash_state, seconds;
    char buf[80];

    if (!GLOBAL_playing_cutseq && !bDisableLaraControl && gfGameMode != 1) {
        flash_state = FlashIt();
        DrawHealthBar(flash_state);
        DrawAirBar(flash_state);
        DrawPickups();
        if (DashTimer < 120)
            S_DrawDashBar(100 * DashTimer / 120);
        if (gfLevelFlags & GF_TIMER && savegame.Level.Timer && savegame.Level.Timer < 108000) {
            seconds = savegame.Level.Timer / 30;
            sprintf(buf, "%.2d:%.2d:%.2d", seconds / 60, seconds % 60, (sint32)(334 * (savegame.Level.Timer % 30)) / 100);
            PrintString((uint16)(phd_winwidth >> 1), (uint16)font_height, 0, buf, FF_CENTER);
        }
    }
}
