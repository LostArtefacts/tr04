#include <windows.h>
#include <stdlib.h>

#include "gamemain.h"
#include "3dmath.h"
#include "../savegame.h"
#include "../gameflow.h"
#include "audio.h"
#include "file.h"
#include "output.h"
#include "function_stubs.h"

void GameClose(void)
{
    Log(2, "GameClose");
    ACMClose();
    FreeLevel();
    if (DestVB) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Dest VB", DestVB, IDirect3DVertexBuffer_Release(DestVB));
        DestVB = 0;
    } else {
        Log(1, "%s Attempt To Release NULL Ptr", "Dest VB");
    }
    free(clipflags);
    if (wav_file_buffer)
        free(wav_file_buffer);
    if (ADPCMBuffer)
        free(ADPCMBuffer);
    free(malloc_buffer);
    free(gfScriptFile);
    free(gfLanguageFile);
}

void init_water_table(void)
{
    sint32 lp, i, j, k, angle;
    sint16 sin;

    srand(121197);
    for (lp = 0; lp < 64; lp++) {
        sin = rcossin_tbl[lp << 7];
        WaterTable[0][lp].shimmer = (sin * 63) >> 15;
        WaterTable[0][lp].choppy = (sin * 16) >> 12;
        WaterTable[0][lp].random = (uint8)GetRandom(WaterTable[0], lp);
        WaterTable[0][lp].abs = 0;
        WaterTable[1][lp].shimmer = (sin * 32) >> 15;
        WaterTable[1][lp].choppy = 0;
        WaterTable[1][lp].random = (uint8)GetRandom(WaterTable[1], lp);
        WaterTable[1][lp].abs = -3;
        WaterTable[2][lp].shimmer = (sin * 64) >> 15;
        WaterTable[2][lp].choppy = 0;
        WaterTable[2][lp].random = (uint8)GetRandom(WaterTable[2], lp);
        WaterTable[2][lp].abs = 0;
        WaterTable[3][lp].shimmer = (sin * 96) >> 15;
        WaterTable[3][lp].choppy = 0;
        WaterTable[3][lp].random = (uint8)GetRandom(WaterTable[3], lp);
        WaterTable[3][lp].abs = 4;
        WaterTable[4][lp].shimmer = (sin * 127) >> 15;
        WaterTable[4][lp].choppy = 0;
        WaterTable[4][lp].random = (uint8)GetRandom(WaterTable[4], lp);
        WaterTable[4][lp].abs = 8;
        for (i = 0, j = 5; i < 4; i++, j += 4) {
            for (k = 0; k < 4; k++) {
                WaterTable[j + k][lp].shimmer = -((sin * water_shimmer[k]) >> 15);
                WaterTable[j + k][lp].choppy = (sin * water_choppy[i]) >> 12;
                WaterTable[j + k][lp].random = (uint8)GetRandom(WaterTable[j + k], lp);
                WaterTable[j + k][lp].abs = water_abs[k];
            }
        }
    }
    for (i = 0; i < 32; i++) {
        angle = 0x10000 * i / 32;
        vert_wibble_table[i] = (float)((phd_sin(angle) * 2) >> W2V_SHIFT);
    }
    for (i = 0; i < 256; i++) {
        angle = 0x10000 * i / 256;
        unused_vert_wibble_table[i] = (float)((phd_sin(angle) * 32) >> W2V_SHIFT);
    }
}

uint16 GetRandom(WATERTAB *w, sint32 n)
{
    sint32 i;
    uint16 random;

    do {
        random = rand() & 0xFC;
        for (i = 0; i < n; i++) {
            if (w[i].random == random)
                break;
        }
        if (i == n)
            return random;
    } while (1);
    return 0;
}

sint32 S_SaveGame(sint32 slot_num)
{
    HANDLE file;
    DWORD bytes;
    sint32 t, days, hours, minutes, seconds;
    char buffer[80], counter[16];

    memset(buffer, 0, sizeof(buffer));
    wsprintf(buffer, "savegame.%d", slot_num);
    file = CreateFile(buffer, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE) {
        memset(buffer, 0, sizeof(buffer));
        wsprintf(buffer, "%s", SCRIPT_TEXT(gfLevelNames[gfCurrentLevel]));
        WriteFile(file, buffer, 75, &bytes, 0);
        WriteFile(file, &SaveCounter, sizeof(sint32), &bytes, 0);
        t = savegame.Game.Timer / 30;
        days = t / 86400;
        hours = t % 86400 / 3600;
        minutes = t / 60 % 60;
        seconds = t % 60;
        WriteFile(file, &days, 2, &bytes, 0);
        WriteFile(file, &hours, 2, &bytes, 0);
        WriteFile(file, &minutes, 2, &bytes, 0);
        WriteFile(file, &seconds, 2, &bytes, 0);
        WriteFile(file, &savegame, sizeof(SAVEGAME_INFO), &bytes, 0);
        CloseHandle(file);
        wsprintf(counter, "%d", SaveCounter);
        SaveCounter++;
        return 1;
    }
    return 0;
}

sint32 S_LoadGame(sint32 slot_num)
{
    HANDLE file;
    DWORD bytes;
    sint32 value, value2, value3;
    char buffer[80];

    wsprintf(buffer, "savegame.%d", slot_num);
    file = CreateFile(buffer, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file != INVALID_HANDLE_VALUE) {
        ReadFile(file, buffer, 75, &bytes, 0);
        ReadFile(file, &value, sizeof(sint32), &bytes, 0);
        ReadFile(file, &value2, sizeof(sint32), &bytes, 0);
        ReadFile(file, &value3, sizeof(sint32), &bytes, 0);
        ReadFile(file, &savegame, sizeof(SAVEGAME_INFO), &bytes, 0);
        CloseHandle(file);
        return 1;
    }
    return 0;
}
