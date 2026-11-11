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

void sgInitialiseHub(sint32 dont_save_lara)
{
    sint32 i;

    for (i = 0; i < 10; i++) {
        savegame.HubOffsets[i] = 0;
        savegame.HubSizes[i] = 0;
        savegame.HubLevels[i] = 0;
    }
    if (dont_save_lara)
        savegame.HubSavedLara = 0;
    else {
        SaveLaraData();
        savegame.HubSavedLara = 1;
    }
    memset(savegame.Lara.puzzleitems, 0, 12);
    savegame.Lara.puzzleitemscombo = 0;
    savegame.Lara.keyitems = 0;
    savegame.Lara.keyitemscombo = 0;
    savegame.Lara.pickupitems = 0;
    savegame.Lara.pickupitemscombo = 0;
    savegame.Lara.examine1 = 0;
    savegame.Lara.examine2 = 0;
    savegame.Lara.examine3 = 0;
    savegame.Lara.mechanical_scarab = 0;
    savegame.Lara.location = -1;
    savegame.Lara.highest_location = -1;
    savegame.Lara.vehicle = NO_ITEM;
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

void CreateCheckSum(void)
{
    sint8 *ptr;
    sint8 checksum;
    sint32 i;

    ptr = (sint8 *)&savegame;
    savegame.Checksum = 0;
    checksum = 0;
    for (i = 0; i < sizeof(SAVEGAME_INFO); i++)
        checksum += *ptr++;
    savegame.Checksum = -checksum;
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

void sgSaveLevel(void)
{
    sint32 level_index;

    level_index = OpenSaveGame(gfCurrentLevel, 1);
    SaveLevelData(0);
    SaveLaraData();
    SaveHubData(level_index);
}
