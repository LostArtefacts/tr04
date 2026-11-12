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

sint32 go_and_save_game(void)
{
    return SaveGame();
}

void combine_PuzzleItem1(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x3;
    lara.puzzleitems[0] = 1;
}

void combine_PuzzleItem2(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC;
    lara.puzzleitems[1] = 1;
}

void combine_PuzzleItem3(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x30;
    lara.puzzleitems[2] = 1;
}

void combine_PuzzleItem4(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC0;
    lara.puzzleitems[3] = 1;
}

void combine_PuzzleItem5(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x300;
    lara.puzzleitems[4] = 1;
}

void combine_PuzzleItem6(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC00;
    lara.puzzleitems[5] = 1;
}

void combine_PuzzleItem7(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x3000;
    lara.puzzleitems[6] = 1;
}

void combine_PuzzleItem8(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC000;
    lara.puzzleitems[7] = 1;
}

void combine_KeyItem1(sint32 flag)
{
    lara.keyitems |= 0x1;
    lara.keyitemscombo &= ~0x3;
}

void combine_KeyItem2(sint32 flag)
{
    lara.keyitems |= 0x2;
    lara.keyitemscombo &= ~0xC;
}

void combine_KeyItem3(sint32 flag)
{
    lara.keyitems |= 0x4;
    lara.keyitemscombo &= ~0x30;
}

void combine_KeyItem4(sint32 flag)
{
    lara.keyitems |= 0x8;
    lara.keyitemscombo &= ~0xC0;
}

void combine_KeyItem5(sint32 flag)
{
    lara.keyitems |= 0x10;
    lara.keyitemscombo &= ~0x300;
}

void combine_KeyItem6(sint32 flag)
{
    lara.keyitems |= 0x20;
    lara.keyitemscombo &= ~0xC00;
}

void combine_KeyItem7(sint32 flag)
{
    lara.keyitems |= 0x40;
    lara.keyitemscombo &= ~0x3000;
}

void combine_KeyItem8(sint32 flag)
{
    lara.keyitems |= 0x80;
    lara.keyitemscombo &= ~0xC000;
}

void combine_PickupItem1(sint32 flag)
{
    lara.pickupitems |= 0x1;
    lara.pickupitemscombo &= ~0x3;
}

void combine_PickupItem2(sint32 flag)
{
    lara.pickupitems |= 0x2;
    lara.pickupitemscombo &= ~0xC;
}

void combine_PickupItem3(sint32 flag)
{
    lara.pickupitems |= 0x4;
    lara.pickupitemscombo &= ~0x30;
}

void combine_PickupItem4(sint32 flag)
{
    lara.pickupitems |= 0x8;
    lara.pickupitemscombo &= ~0xC0;
}
