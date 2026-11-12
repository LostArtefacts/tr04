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

sint32 GetFreeScarab(void)
{
    SCARAB_STRUCT *fx;
    sint32 free, i;

    free = next_scarab;
    fx = &Scarabs[free];
    for (i = 0; i < 128; i++) {
        if (!fx->On) {
            next_scarab = (free + 1) & 0x7F;
            return free;
        }
        if (free == 127) {
            fx = Scarabs;
            free = 0;
        } else {
            free++;
            fx++;
        }
    }
    return -1;
}

void ClearScarabs(void)
{
    SCARAB_STRUCT *fx;
    sint32 i;

    fx = Scarabs;
    for (i = 128; i > 0; i--, fx++)
        fx->On = 0;
    next_scarab = 0;
    flipeffect = -1;
}
