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
