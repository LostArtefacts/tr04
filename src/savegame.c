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

void WriteSG(void *pointer, sint32 size)
{
    char *data;

    SGcount += size;
    for (data = pointer; size > 0; size--)
        *SGpoint++ = *data++;
}

void ReadSG(void *pointer, sint32 size)
{
    char *data;

    SGcount += size;
    data = pointer;
    for (; size > 0; size--)
        *(data++) = *(SGpoint++);
}
