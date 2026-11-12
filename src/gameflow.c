#include <stdlib.h>
#include <string.h>

#include "gameflow.h"
#include "text.h"
#include "specific/file.h"
#include "specific/3dmath.h"

void LoadGameflow(void)
{
    char *s;
    char *d;
    STRINGHEADER sh;
    sint32 l, i, j;

    s = 0;
    LoadFile("SCRIPT.DAT", &s);
    gfScriptFile = (uint8 *)s;
    Gameflow = (GAMEFLOW *)s;
    s += sizeof(GAMEFLOW);
    gfExtensions = s;
    s += 40;
    gfFilenameOffset = (uint16 *)s;
    s += sizeof(uint16) * Gameflow->nFileNames;
    gfFilenameWad = s;
    s += Gameflow->FileNameLen;
    gfScriptOffset = (uint16 *)s;
    s += sizeof(uint16) * Gameflow->nLevels;
    gfScriptWad = (uint8 *)s;
    s += Gameflow->ScriptLen;
    for (l = 0;; l++) {
        d = 0;
        if (LoadFile(s, &d))
            break;
        s += strlen(s) + 1;
    }
    gfStringOffset = (uint16 *)d;
    gfLanguageFile = (uint8 *)d;
    Gameflow->Language = l;
    memcpy(&sh, gfStringOffset, sizeof(STRINGHEADER));
    memcpy(gfStringOffset, gfStringOffset + (sizeof(STRINGHEADER) / sizeof(uint16)), TXT_NUM_STRINGS * sizeof(uint16));
    gfStringWad = (char *)(gfStringOffset + TXT_NUM_STRINGS);
    memcpy(gfStringOffset + TXT_NUM_STRINGS,
        gfStringOffset + TXT_NUM_STRINGS + (sizeof(STRINGHEADER) / sizeof(uint16)),
        sh.StringWadLen + (sh.PCStringWadLen + sh.PSXStringWadLen));
    for (i = 0; i < TXT_NUM_STRINGS; i++) {
        s = &gfStringWad[gfStringOffset[i]];
        for (j = 0; s[j]; j++)
            s[j] ^= 0xA5;
    }
    for (i = 0; i < Gameflow->nLevels; i++) {
        s = (char *)&gfScriptWad[gfScriptOffset[i]];
        for (;;) {
            switch ((uint8)*s++) {
            case CMD_FMV:
            case CMD_PLAYCUT:
            case CMD_CUT1:
            case CMD_CUT2:
            case CMD_CUT3:
            case CMD_CUT4:
            case CMD_UVROT:
            case CMD_LEGEND:
            case CMD_ANIMATINGMIP:
            case CMD_RESETHUB:
                s++;
                break;
            case CMD_FOG:
                s += 3;
                break;
            case CMD_TITLE:
            case CMD_LAYER1:
            case CMD_LAYER2:
                s += 4;
                break;
            case CMD_LEVEL:
                gfLevelNames[i] = *s;
                s += 5;
                break;
            case CMD_MIRROR:
                s += 9;
                break;
            case CMD_LENSFLARE:
                s += 25;
                break;
            case CMD_ENDSEQ:
                goto next;
            default:
                s += 2;
                break;
            }
        }
    next:;
    }
}

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
