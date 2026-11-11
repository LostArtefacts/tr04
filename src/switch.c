#include "switch.h"
#include "items.h"
#include "control.h"
#include "objnums.h"

void SwitchControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->flags |= IFL_CODEBITS;
    if (!TriggerActive(item) && !(item->flags & IFL_INVISIBLE)) {
        if (item->object_number == JUMP_SWITCH)
            item->goal_anim_state = 0;
        else
            item->goal_anim_state = 1;
        item->timer = 0;
    }
    AnimateItem(item);
}
