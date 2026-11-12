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
