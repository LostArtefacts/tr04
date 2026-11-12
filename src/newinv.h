#ifndef NEWINV_H
#define NEWINV_H

#include "types.h"

sint32 LoadGame(void);
sint32 SaveGame(void);
void DEL_picked_up_object(sint16 objnum);
sint32 have_i_got_object(sint16 object_number);
sint32 go_and_load_game(void);
sint32 go_and_save_game(void);
void combine_PuzzleItem1(sint32 flag);
void combine_PuzzleItem2(sint32 flag);
void combine_PuzzleItem3(sint32 flag);
void combine_PuzzleItem4(sint32 flag);
void combine_PuzzleItem5(sint32 flag);
void combine_PuzzleItem6(sint32 flag);
void combine_PuzzleItem7(sint32 flag);
void combine_PuzzleItem8(sint32 flag);
void combine_KeyItem1(sint32 flag);
void combine_KeyItem2(sint32 flag);
void combine_KeyItem3(sint32 flag);
void combine_KeyItem4(sint32 flag);
void combine_KeyItem5(sint32 flag);
void combine_KeyItem6(sint32 flag);
void combine_KeyItem7(sint32 flag);

extern sint32 GLOBAL_enterinventory;
extern sint32 GLOBAL_inventoryitemchosen;

#endif
