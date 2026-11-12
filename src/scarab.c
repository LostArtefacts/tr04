#include "scarab.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/output.h"

void InitialiseScarab(sint16 item_number)
{
    ITEM_INFO *item;

    InitialiseCreature(item_number);
    item = &items[item_number];
    item->anim_number = objects[BIG_BEETLE].anim_index + 3;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
