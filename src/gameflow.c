#include <stdlib.h>

#include "gameflow.h"
#include "text.h"
#include "specific/3dmath.h"

sint32 DoCredits(void)
{
    char *s;
    sint32 y, num_drawn;

    num_drawn = 0;
    if (!CreditsInit) {
        CreditsStartPos = font_height + phd_winheight;
        CreditsInit = 1;
    }
    y = CreditsStartPos;
    for (s = CreditsTable[0]; s < CreditsTable[71]; s += 22) {
        if (y < font_height + phd_winheight + 1 && y > -font_height) {
            if (*s == '%')
                PrintString((uint16)(phd_winwidth >> 1), (uint16)y, 6, SCRIPT_TEXT(CreditGroups[atoi(s + 1)]), FF_CENTER);
            else if (*s == '0')
                y += font_height;
            else
                PrintString((uint16)(phd_winwidth >> 1), (uint16)y, 2, s, FF_CENTER);
            num_drawn++;
        }
        y += font_height;
    }
    CreditsStartPos--;
    if (!num_drawn)
        CreditsInit = 0;
    return num_drawn;
}
