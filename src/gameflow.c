#include <stdlib.h>
#include <string.h>

#include "gameflow.h"
#include "text.h"
#include "specific/file.h"
#include "specific/3dmath.h"
#include "specific/fmv.h"
#include "specific/output.h"
#include "savegame.h"
#include "newinv.h"
#include "camera.h"
#include "draw.h"

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

void DoGameflow(void)
{
    uint8 *gf;
    uint8 n;

    PlayFmvNow(0);
    do_boot_screen(Gameflow->Language);
    num_fmvs = 0;
    fmv_to_play[0] = fmv_to_play[1] = 0;
    gfCurrentLevel = !Gameflow->TitleEnabled;
    gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];
    while (1) {
        switch (n = *gf++) {
        case CMD_LAYER1:
            gfLayer1Col.r = (uint8)(LightningRGBs[0] = LightningRGB[0] = *gf++);
            gfLayer1Col.g = (uint8)(LightningRGBs[1] = LightningRGB[1] = *gf++);
            gfLayer1Col.b = (uint8)(LightningRGBs[2] = LightningRGB[2] = *gf++);
            gfLayer1Vel = *gf++;
            break;
        case CMD_LAYER2:
            gfLayer2Col.r = (uint8)(LightningRGBs[0] = LightningRGB[0] = *gf++);
            gfLayer2Col.g = (uint8)(LightningRGBs[1] = LightningRGB[1] = *gf++);
            gfLayer2Col.b = (uint8)(LightningRGBs[2] = LightningRGB[2] = *gf++);
            gfLayer2Vel = *gf++;
            break;
        case CMD_FOG:
            gfFog.r = *gf++;
            gfFog.g = *gf++;
            gfFog.b = *gf++;
            break;
        case CMD_ANIMATINGMIP:
            gfMips[gfNumMips++] = *gf++;
            break;
        case CMD_UVROT:
            gfUVRotate = *gf++;
            break;
        case CMD_LEGEND:
            gfLegend = *gf++;
            if (gfGameMode != 4)
                gfLegendTime = 150;
            break;
        case CMD_LENSFLARE:
            gfLensFlare.x = ((gf[1] << 8) | gf[0]) << 8;
            gfLensFlare.y = ((sint16)(gf[3] << 8) | gf[2]) << 8;
            gfLensFlare.z = ((gf[5] << 8) | gf[4]) << 8;
            gfLensFlareColour.r = gf[6];
            gfLensFlareColour.g = gf[7];
            gfLensFlareColour.b = gf[8];
            gf += 9;
            break;
        case CMD_CAMERA:
            load_cam.x = (((((gf[3] << 8) | gf[2]) << 8) | gf[1]) << 8) | gf[0];
            load_cam.y = (((((gf[7] << 8) | gf[6]) << 8) | gf[5]) << 8) | gf[4];
            load_cam.z = (((((gf[11] << 8) | gf[10]) << 8) | gf[9]) << 8) | gf[8];
            load_target.x = (((((gf[15] << 8) | gf[14]) << 8) | gf[13]) << 8) | gf[12];
            load_target.y = (((((gf[19] << 8) | gf[18]) << 8) | gf[17]) << 8) | gf[16];
            load_target.z = (((((gf[23] << 8) | gf[22]) << 8) | gf[21]) << 8) | gf[20];
            load_roomnum = gf[24];
            gf += 25;
            break;
        case CMD_MIRROR:
            gfMirrorRoom = gf[0];
            gfMirrorZPlane = (((((gf[4] << 8) | gf[3]) << 8) | gf[2]) << 8) | gf[1];
            gf += 5;
            break;
        case CMD_CUT1:
            gfResidentCut[0] = *gf++;
            break;
        case CMD_CUT2:
            gfResidentCut[1] = *gf++;
            break;
        case CMD_CUT3:
            gfResidentCut[2] = *gf++;
            break;
        case CMD_CUT4:
            gfResidentCut[3] = *gf++;
            break;
        case CMD_PLAYCUT:
            gfCutNumber = *gf++;
            break;
        case CMD_RESETHUB:
            gfResetHubDest = *gf++;
            break;
        case CMD_FMV:
            fmv_to_play[num_fmvs++] = *gf++;
            break;
        case CMD_LEVEL:
            gfLevelFlags = gf[1] | (gf[2] << 8);
            if (gfLevelFlags & GF_NOLEVEL) {
                gfStatus = 999;
                gfCurrentLevel++;
            } else
                DoLevel(gf[3], gf[4]);
            gfLegendTime = 0;
            LaserSight = 0;
            BinocularRange = 0;
            *(sint32 *)gfResidentCut = 0;
            gfUVRotate = 0;
            gfNumMips = 0;
            gfMirrorRoom = 255;
            gfFog.b = 0;
            gfFog.g = 0;
            gfFog.r = 0;
            gfFog.a = 0;
            switch (gfStatus) {
            case 1:
                gfInitialiseGame = 1;
                gfCurrentLevel = Gameflow->TitleEnabled ? 0 : 1;
                break;
            case 3:
                if ((gfLevelFlags & GF_RESETHUB && gfLevelComplete == gfResetHubDest) || skipped_level) {
                    sgInitialiseHub(0);
                    skipped_level = 0;
                } else
                    sgSaveLevel();
                if (Gameflow->DemoDisc || Gameflow->nLevels == 2)
                    gfCurrentLevel = 0;
                else if (gfLevelComplete > Gameflow->nLevels)
                    gfCurrentLevel = 0;
                else
                    gfCurrentLevel = gfLevelComplete;
                break;
            case 2:
                gfGameMode = 4;
                gfCurrentLevel = savegame.CurrentLevel & 0x7F;
                break;
            case 4:
                return;
            }
            gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];
            break;
        case CMD_TITLE:
            gfLevelFlags = gf[0] | (gf[1] << 8);
            DoTitle(gf[2], gf[3]);
            *(sint32 *)gfResidentCut = 0;
            gfUVRotate = 0;
            gfNumMips = 0;
            gfMirrorRoom = 255;
            switch (gfStatus) {
            case 2:
                gfGameMode = 4;
                gfCurrentLevel = savegame.CurrentLevel & 0x7F;
                break;
            case 3:
                gfGameMode = 0;
                gfCurrentLevel = gfLevelComplete;
                gfInitialiseGame = 1;
                break;
            case 4:
                return;
            }
            gf = &gfScriptWad[gfScriptOffset[gfCurrentLevel]];
            break;
        case CMD_ENDSEQ:
            break;
        default:
            if (n >= 0x93 && n <= 0x9E)
                n -= 82;
            else if (n >= 0x9F && n <= 0xAA)
                n -= 122;
            else if (n >= 0xAB && n <= 0xAE)
                n -= 78;
            else if (n >= 0xAF && n <= 0xB1)
                n -= 59;
            else if (n >= 0xB2 && n <= 0xC1)
                n -= 101;
            else if (n >= 0xC2 && n <= 0xD1)
                n += 111;
            else if (n >= 0xD2 && n <= 0xD9)
                n -= 113;
            inventry_objects_list[n].objname = gf[0] | (gf[1] << 8);
            inventry_objects_list[n].yoff = gf[2] | (gf[3] << 8);
            inventry_objects_list[n].scale1 = gf[4] | (gf[5] << 8);
            inventry_objects_list[n].yrot = gf[6] | (gf[7] << 8);
            inventry_objects_list[n].xrot = gf[8] | (gf[9] << 8);
            inventry_objects_list[n].zrot = gf[10] | (gf[11] << 8);
            inventry_objects_list[n].flags = gf[12] | (gf[13] << 8);
            gf += 14;
            break;
        }
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
