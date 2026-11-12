#include "troops.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseTroop(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (item->trigger_flags == 1) {
        item->anim_number = objects[TROOPS].anim_index + 27;
        item->current_anim_state = item->goal_anim_state = 16;
    } else {
        item->anim_number = objects[TROOPS].anim_index + 12;
        item->current_anim_state = item->goal_anim_state = 1;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}
