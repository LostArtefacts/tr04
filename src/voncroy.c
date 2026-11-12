#include <string.h>

#include "voncroy.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseVoncroy(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[VON_CROY].anim_index + 11;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 6;
    item->meshswap_meshbits = 0x40080;
    memset(VonCroyCutFlags, 0, 64);
}
