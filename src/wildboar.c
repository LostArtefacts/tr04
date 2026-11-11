#include <stdlib.h>

#include "wildboar.h"
#include "anim.h"
#include "effects.h"
#include "lara.h"
#include "specific/function_stubs.h"

void InitialiseWildboar(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[WILD_BOAR].anim_index + 6;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
