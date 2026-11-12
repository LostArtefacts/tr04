#include "ahmet.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseAhmet(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[AHMET].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
    item->item_flags[0] = (sint16)(item->pos.x_pos >> 10);
    item->item_flags[1] = (sint16)(item->pos.y_pos >> 8);
    item->item_flags[2] = (sint16)(item->pos.z_pos >> 10);
}
