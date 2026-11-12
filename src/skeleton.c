#include "skeleton.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseSkeleton(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (!item->trigger_flags) {
        item->anim_number = objects[SKELETON].anim_index;
        item->current_anim_state = item->goal_anim_state = 0;
    } else if (item->trigger_flags == 1) {
        item->anim_number = objects[SKELETON].anim_index + 37;
        item->current_anim_state = item->goal_anim_state = 20;
    } else if (item->trigger_flags == 2) {
        item->anim_number = objects[SKELETON].anim_index + 34;
        item->current_anim_state = item->goal_anim_state = 19;
    } else if (item->trigger_flags == 3) {
        item->anim_number = objects[SKELETON].anim_index + 46;
        item->current_anim_state = item->goal_anim_state = 25;
        item->status += ITEM_ACTIVE;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}
