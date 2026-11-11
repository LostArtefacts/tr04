#include "savegame.h"
#include "gameflow.h"
#include "control.h"
#include <string.h>

sint32 CheckSumValid(sint8 *buffer)
{
    sint8 checksum;
    sint32 i;

    checksum = 0;
    for (i = 0; i < sizeof(SAVEGAME_INFO); i++)
        checksum += *buffer++;
    return !checksum;
}

void sgSaveGame(void)
{
    sint32 level_index;

    level_index = OpenSaveGame(gfCurrentLevel, 1);
    savegame.Game.Timer = GameTimer;
    savegame.CurrentLevel ^= (gfCurrentLevel ^ savegame.CurrentLevel) & 0x7F;
    SaveLevelData(1);
    SaveLaraData();
    SaveHubData(level_index);
    CreateCheckSum();
}

void SaveHubData(sint32 index)
{
    savegame.HubSizes[index] = (uint16)(SGcount - savegame.HubOffsets[index]);
    if (index < 10)
        savegame.HubSizes[index - 9] = savegame.HubSizes[index] + savegame.HubOffsets[index];
}

void WriteSG(void *pointer, sint32 size)
{
    char *data;

    SGcount += size;
    for (data = pointer; size > 0; size--)
        *SGpoint++ = *data++;
}

void sgRestoreGame(void)
{
    OpenSaveGame(savegame.CurrentLevel & 0x7F, 0);
    GameTimer = savegame.Game.Timer;
    gfCurrentLevel = savegame.CurrentLevel & 0x7F;
    RestoreLevelData(1);
    RestoreLaraData(1);
}

void ReadSG(void *pointer, sint32 size)
{
    char *data;

    SGcount += size;
    data = pointer;
    for (; size > 0; size--)
        *(data++) = *(SGpoint++);
}
