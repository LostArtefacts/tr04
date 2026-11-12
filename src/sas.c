#include "sas.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "items.h"
#include "lara.h"
#include "lara_states.h"
#include "objnums.h"
#include "sphere.h"
#include "switch.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/input.h"

void InitialiseSas(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[SAS].anim_index + 12;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
