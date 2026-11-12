#include "horse.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseHorseman(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[HORSEMAN].anim_index + 8;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 9;
    item->item_flags[0] = -1;
}

void InitialiseHorse(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->anim_number = objects[HORSE].anim_index + 2;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
