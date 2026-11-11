#include "hammerhead.h"
#include "anim.h"

void InitialiseHammerhead(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[HAMMERHEAD].anim_index + 8;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 0;
}
