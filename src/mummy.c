#include "mummy.h"
#include "anim.h"

void InitialiseMummy(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (item->trigger_flags == 2) {
        item->anim_number = objects[MUMMY].anim_index + 12;
        item->current_anim_state = item->goal_anim_state = 8;
        item->status++;
    } else {
        item->anim_number = objects[MUMMY].anim_index + 19;
        item->current_anim_state = item->goal_anim_state = 0;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}
