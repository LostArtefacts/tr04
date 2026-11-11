#include "bat.h"
#include "anim.h"
#include "box.h"

void InitialiseBat(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[BAT].anim_index + 5;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 6;
}
