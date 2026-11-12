#include "baboon.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseBaboon(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[BABOON_NORMAL].anim_index + 2;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 6;
    item->item_flags[0] = (sint16)(item->pos.z_pos >> 2 & 0xFFFFFF00 | item->pos.x_pos >> 10);
    item->item_flags[1] = (sint16)(item->pos.y_pos >> 8);
    if (item->object_number == BABOON_NORMAL)
        item->ai_bits = FOLLOW;
}
