#include "dog.h"
#include "anim.h"

void InitialiseDog(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->anim_number = objects[item->object_number].anim_index + 8;
    item->current_anim_state = 1;
    if (item->trigger_flags) {
        item->anim_number = objects[item->object_number].anim_index + 1;
        item->status += ITEM_ACTIVE;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}
