#include "demigod.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseDemigod(sint16 item_number)
{
    ITEM_INFO *item, *item2;
    sint16 i;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[item->object_number].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->goal_anim_state = 0;
    item->current_anim_state = 0;
    item2 = items;
    for (i = 0; i < level_items; i++, item2++) {
        if (item != item2 && item2->object_number == DEMIGOD3 && !item2->item_flags[0]) {
            item->item_flags[0] = i;
            return;
        }
    }
}
