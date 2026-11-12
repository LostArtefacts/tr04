#include "scorpion.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseScorpion(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (item->trigger_flags == 1) {
        item->anim_number = objects[SCORPION].anim_index + 7;
        item->current_anim_state = item->goal_anim_state = 8;
    } else {
        item->anim_number = objects[SCORPION].anim_index + 2;
        item->current_anim_state = item->goal_anim_state = 1;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}
