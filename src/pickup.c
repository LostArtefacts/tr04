#include "pickup.h"
#include "lara.h"
#include "lara_states.h"
#include "anim.h"
#include "room.h"
#include "control.h"
#include "draw.h"
#include "objects.h"
#include "newinv.h"
#include "sound.h"
#include "specific/input.h"

void AnimatingPickUp(sint16 item_number)
{
    if ((items[item_number].trigger_flags & 0x3F) == 2)
        AnimateItem(&items[item_number]);
}

void PuzzleDoneCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll)
{
    if (items[item_num].trigger_flags != 999)
        ObjectCollision(item_num, l, coll);
}

void PuzzleDone(ITEM_INFO *item, sint16 item_number)
{
    item->object_number += 12;
    item->anim_number = objects[item->object_number].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = anims[item->anim_number].current_anim_state;
    item->required_anim_state = 0;
    AddActiveItem(item_number);
    item->flags |= IFL_CODEBITS;
    item->status = ITEM_ACTIVE;
}
