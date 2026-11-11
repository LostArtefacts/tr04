#include "jean.h"
#include "anim.h"
#include "control.h"
#include "lara.h"
#include "specific/function_stubs.h"

void InitialiseJeanYves(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->anim_number = objects[JEAN_YVES].anim_index;
    item->goal_anim_state = 1;
    item->current_anim_state = 1;
    item->frame_number = anims[item->anim_number].frame_base;
}

void JeanYvesControl(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 random;

    item = &items[item_number];
    if (item->trigger_flags < lara.highest_location) {
        if (lara.highest_location > 3)
            lara.highest_location = 3;
        random = (GetRandomControl() & 3) + 4 * lara.highest_location;
        item->anim_number = objects[JEAN_YVES].anim_index + random;
        item->current_anim_state = item->goal_anim_state = random + 1;
        item->frame_number = anims[item->anim_number].frame_base;
        item->trigger_flags = lara.highest_location;
    } else {
        if (GetRandomControl() & 3)
            random = (GetRandomControl() & 1) + 1;
        else
            random = 3 * (GetRandomControl() & 1);
        item->goal_anim_state = random + ((sint8)item->current_anim_state - 1 & 0xC) + 1;
    }
    AnimateItem(item);
}
