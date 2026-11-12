#include "newinv.h"
#include "lara.h"
#include "specific/input.h"
#include "specific/loadsave.h"

sint32 LoadGame(void)
{
    if (S_LoadSave(IN_LOAD, 1) >= 0)
        return 1;
    else
        return -1;
}

sint32 SaveGame(void)
{
    sint32 Save;

    Save = S_LoadSave(IN_SAVE, 1);
    input = 0;
    dbinput = 0;
    if (Save >= 0)
        return 1;
    else
        return -1;
}

sint32 go_and_load_game(void)
{
    return LoadGame();
}
