#include "guide.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseGuide(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[GUIDE].anim_index + 4;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
    if (objects[WRAITH1].loaded) {
        item->meshswap_meshbits = 0;
        item->item_flags[1] = 2;
    } else
        item->meshswap_meshbits = 0x40000;
}
