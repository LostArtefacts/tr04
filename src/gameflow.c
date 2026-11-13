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
#include "control.h"
#include "deltapak.h"
#include "health.h"
#include "items.h"
#include "lara.h"
#include "lot.h"
#include "sound.h"
#include "spotcam.h"
#include "tomb4fx.h"
#include "specific/audio.h"
#include "specific/dxshell.h"
#include "specific/dxsound.h"
#include "specific/function_table.h"
#include "specific/gamemain.h"
#include "specific/input.h"
#include "specific/loadsave.h"
#include "specific/polyinsert.h"
#include "specific/specificfx.h"
#include "specific/winmain.h"

extern sint32 framecount;

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

static __inline sint32 TitleOptions(void)
{
    __int64 i, flag;
    sint32 ret, ret2, ii, n, n2, load, num, y;

    ret = 0;
    if (load_or_new) {
        ret2 = load_or_new;
        if (DoFade == 2) {
            gfLevelComplete = (uint8)gfLevelComplete_bak;
            gfLevelComplete_bak = 0;
            load_or_new = 0;
            return ret2;
        }
        input = 0;
        dbinput = 0;
    }
    if (bDoCredits) {
        if (DoCredits())
            return 0;
        bDoCredits = 0;
    }
    if (!always0) {
        switch (menu_to_display) {
        case 1:
            y = font_height + phd_winymin;
            PrintString(phd_centerx, y, 6, SCRIPT_TEXT(TXT_Select_Level), FF_CENTER);
            y += font_height;
            if (Gameflow->nLevels < 10) {
                n2 = 1;
                num = Gameflow->nLevels - 1;
            } else {
                i = selected_option;
                n = 0;
                num = 10;
                while (i) {
                    i >>= 1;
                    n++;
                }
                n2 = n - 9;
                if (n2 < 1)
                    n2 = 1;
                else if (n2 > 1) {
                    PrintString(32, font_height + y, 6, "\x18", 0);
                    PrintString(phd_winxmax - 48, font_height + y, 6, "\x18", 0);
                }
                if (n != Gameflow->nLevels - 1) {
                    PrintString(32, y + 10 * font_height, 6, "\x1a", 0);
                    PrintString(phd_winxmax - 48, y + 10 * font_height, 6, "\x1a", 0);
                }
            }
            for (ii = n2; ii < num + n2; ii++) {
                n = ii - 1;
                y += font_height;
                PrintString(phd_centerx, y, (selected_option & ((__int64)1 << n)) ? 1 : 2, SCRIPT_TEXT(gfLevelNames[ii]), FF_CENTER);
            }
            flag = (__int64)1 << (Gameflow->nLevels - 2);
            break;
        case 2:
            if (Gameflow->LoadSaveEnabled) {
                load = DoLoadSave(IN_LOAD);
                if (load >= 0) {
                    S_LoadGame(load);
                    ret = 2;
                }
                break;
            }
            SoundEffect(SFX_LARA_NO, 0, SFX_ALWAYS);
            menu_to_display = 0;
        case 0:
            y = phd_winymax - 4 * font_height;
            ShowTitle();
            Chris_Menu = 0;
            PrintString(phd_centerx, y, (selected_option & 1) ? 1 : 2, SCRIPT_TEXT(TXT_New_Game), FF_CENTER);
            PrintString(phd_centerx, y + font_height, (selected_option & 2) ? 1 : 2, SCRIPT_TEXT(TXT_Load_Game), FF_CENTER);
            PrintString(phd_centerx, y + 2 * font_height, (selected_option & 4) ? 1 : 2, SCRIPT_TEXT(TXT_Options), FF_CENTER);
            PrintString(phd_centerx, y + 3 * font_height, (selected_option & 8) ? 1 : 2, SCRIPT_TEXT(TXT_Exit), FF_CENTER);
            flag = 8;
            break;
        case 3:
            DoOptions();
            break;
        }
        if (menu_to_display < 2) {
            if (dbinput & IN_FORWARD) {
                if (selected_option > 1)
                    selected_option >>= 1;
                SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
            }
            if (dbinput & IN_BACK) {
                if (selected_option < flag)
                    selected_option <<= 1;
                SoundEffect(SFX_MENU_CHOOSE, 0, SFX_ALWAYS);
            }
        }
        if (dbinput & IN_DESELECT && menu_to_display > 0) {
            menu_to_display = 0;
            selected_option = selected_option_bak;
            S_SoundStopAllSamples();
            SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
        }
        if (dbinput & IN_SELECT && !keymap[DIK_LALT] && menu_to_display < 2) {
            SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
            switch (menu_to_display) {
            case 1:
                gfLevelComplete = 0;
                i = selected_option;
                while (i) {
                    i >>= 1;
                    gfLevelComplete++;
                }
                ret = 3;
                break;
            case 0:
                switch (selected_option) {
                case 1:
                    if (Gameflow->PlayAnyLevel) {
                        selected_option_bak = selected_option;
                        menu_to_display = 1;
                    } else {
                        gfLevelComplete = 1;
                        ret = 3;
                    }
                    break;
                case 2:
                    GetSaveLoadFiles();
                    selected_option_bak = selected_option;
                    menu_to_display = 2;
                    break;
                case 3:
                case 5:
                case 6:
                case 7:
                    break;
                case 4:
                    selected_option_bak = selected_option;
                    menu_to_display = 3;
                    break;
                case 8:
                    ret = 4;
                    break;
                }
                break;
            }
        }
    }
    if (thread_ended)
        return 4;
    if (ret) {
        load_or_new = ret;
        gfLevelComplete_bak = gfLevelComplete;
        gfLevelComplete = 0;
        ret = 0;
        SetFade(0, 255);
    }
    return ret;
}

void DoTitle(uint8 Name, uint8 Audio)
{
    Unk7fe114 = 0;
    SetFade(255, 0);
    num_fmvs = 0;
    fmv_to_play[1] = 0;
    fmv_to_play[0] = 0;
    sgInitialiseHub(1);
    memset(&savegame.Level, 0, sizeof(STATS));
    savegame.HaveBikeBooster = 0;
    memset(&savegame.Game, 0, sizeof(STATS));
    S_LoadLevelFile(Name);
    GLOBAL_lastinvitem = -1;
    InitSpotCamSequences();
    title_controls_locked_out = 0;
    InitialiseFXArray(1);
    InitialiseLOTarray(1);
    SetFogColor(gfFog.r, gfFog.g, gfFog.b);
    ClearFXFogBulbs();
    InitialisePickUpDisplay();
    nullsub_1();
    SOUND_Stop();
    S_CDPlay(Audio, 1);
    IsAtmospherePlaying = 0;
    nullsub_1(1);
    InitialiseCamera();
    if (!bDoCredits) {
        InitialiseSpotCam(1);
        ScreenFadedOut = 0;
        ScreenFade = 0;
        dScreenFade = 0;
        ScreenFadeBack = 0;
        ScreenFadeSpeed = 8;
        ScreenFading = 0;
    } else {
        cutseq_num = 28;
        SetFadeClip(32, 1);
        ScreenFadedOut = 1;
        ScreenFade = 255;
        dScreenFade = 255;
        S_CDPlay(98, 1);
    }
    bUseSpotCam = 1;
    lara_item->mesh_bits = 0;
    gfGameMode = 1;
    gfLevelComplete = 0;
    nFrames = 2;
    gfStatus = ControlPhase(2, 0);
    while (!gfStatus) {
        S_InitialisePolyList();
        gfStatus = TitleOptions();
        if (gfStatus)
            break;
        handle_cutseq_triggering(Name);
        nFrames = DrawPhaseGame();
        gfStatus = ControlPhase(nFrames, 0);
    }
    S_SoundStopAllSamples();
    S_CDStop();
    bUseSpotCam = 0;
    bDisableLaraControl = 0;
    if (gfLevelComplete == 1 && gfStatus != 2)
        PlayFmvNow(12);
    if (gfStatus != 4)
        RenderLoadPic(0);
    input = 0;
}

void DoLevel(uint8 Name, uint8 Audio)
{
    sint32 gamestatus;

    gamestatus = 0;
    SetFade(255, 0);
    if (gfGameMode != 4)
        memset(&savegame.Level, 0, sizeof(STATS));
    S_LoadLevelFile(Name);
    SetFogColor(gfFog.r, gfFog.g, gfFog.b);
    InitialiseFXArray(1);
    InitialiseLOTarray(1);
    ClearFXFogBulbs();
    InitSpotCamSequences();
    InitialisePickUpDisplay();
    nullsub_1();
    SOUND_Stop();
    bDisableLaraControl = 0;
    if (gfGameMode == 4) {
        sgRestoreGame();
        gfRequiredStartPos = 0;
        gfInitialiseGame = 0;
        gfFog.r = savegame.fog_colour.r;
        gfFog.g = savegame.fog_colour.g;
        gfFog.b = savegame.fog_colour.b;
        if (IsVolumetric())
            SetFogColor(gfFog.r, gfFog.g, gfFog.b);
    } else {
        if (gfInitialiseGame) {
            GameTimer = 0;
            gfRequiredStartPos = 0;
            gfInitialiseGame = 0;
            CutSceneTriggered = 0;
            FmvSceneTriggered = 0;
        } else
            sgRestoreLevel();
        if (gfLevelFlags & GF_REMOVEAMULET)
            lara.questitems &= ~1;
        savegame.Level.Timer = 0;
        CurrentAtmosphere = Audio;
    }
    S_CDPlay(CurrentAtmosphere, 1);
    IsAtmospherePlaying = 1;
    ScreenFadedOut = 0;
    ScreenFading = 0;
    ScreenFadeBack = 0;
    dScreenFade = 255;
    ScreenFade = 255;
    if (gfCutNumber && !(CutSceneTriggered & (1 << gfCutNumber))) {
        cutseq_num = gfCutNumber;
        gfCutNumber = 0;
        ScreenFadedOut = 1;
    } else {
        cutseq_num = 0;
        gfCutNumber = 0;
        SetScreenFadeIn(16);
    }
    InitialiseCamera();
    bUseSpotCam = 0;
    gfGameMode = 0;
    gfLevelComplete = 0;
    nFrames = 2;
    framecount = 0;
    gfStatus = ControlPhase(2, 0);
    dbinput = 0;
    while (!gfStatus) {
        S_InitialisePolyList();
        if (gfLegendTime && !DestFadeScreenHeight && !FadeScreenHeight && !cutseq_num) {
            PrintString(phd_winwidth >> 1, phd_winymax - font_height, 2, SCRIPT_TEXT(gfLegend), 0x8000);
            gfLegendTime--;
        }
        nFrames = DrawPhaseGame();
        handle_cutseq_triggering(Name);
        if (DEL_playingamefmv) {
            DEL_playingamefmv = 0;
            S_CDStop();
            PlayFmvNow(7);
            DelsHandyTeleportLara(54179, -8192, 50899, -32703);
        }
        if (gfLevelComplete) {
            gfStatus = 3;
            break;
        }
        gfStatus = ControlPhase(nFrames, 0);
        if (gfStatus && !gamestatus) {
            if (lara_item->hit_points >= 0)
                break;
            gamestatus = gfStatus;
            SetFade(0, 255);
            gfStatus = 0;
        }
        if (gamestatus) {
            gfStatus = 0;
            if (DoFade == 2)
                gfStatus = gamestatus;
        }
    }
    S_SoundStopAllSamples();
    S_CDStop();
    if (gfStatus == 3) {
        if (fmv_to_play[0] & 0x80) {
            if ((fmv_to_play[0] & 0x7F) == 9 && gfLevelComplete != 10)
                fmv_to_play[0] = 0;
            if ((fmv_to_play[0] & 0x7F) == 8 && gfLevelComplete != 22)
                fmv_to_play[0] = 0;
        }
        if (!fmv_to_play[0] || PlayFmvNow(fmv_to_play[0] & 0x7F) != 2) {
            if (fmv_to_play[1])
                PlayFmvNow(fmv_to_play[1] & 0x7F);
        }
    }
    num_fmvs = 0;
    fmv_to_play[1] = 0;
    fmv_to_play[0] = 0;
    lara.examine3 = 0;
    lara.examine2 = 0;
    lara.examine1 = 0;
    RenderLoadPic(0);
    if (gfStatus == 3) {
        if (gfLevelComplete == 39) {
            input = 0;
            reset_flag = 0;
            gfStatus = 1;
            bDoCredits = 1;
            return;
        }
        if (gfLevelComplete == 25 && skipped_level)
            lara.vehicle = -1;
    }
    input = 0;
    reset_flag = 0;
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
