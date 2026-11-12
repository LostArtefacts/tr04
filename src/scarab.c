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

void InitialiseScarabGenerator(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 flag, rot;

    item = &items[item_number];
    flag = item->trigger_flags / 1000;
    rot = item->pos.y_rot;
    item->pos.x_rot = 0x2000;
    item->item_flags[0] = flag & 1;
    item->item_flags[1] = flag & 2;
    item->item_flags[2] = flag & 4;
    item->trigger_flags %= 1000;
    if (!item->item_flags[0]) {
        if (rot > 4096 && rot < 28672)
            item->pos.x_pos -= 512;
        else if (rot < -4096 && rot > -28672)
            item->pos.x_pos += 512;
        if (rot > -8192 && rot < 8192)
            item->pos.z_pos -= 512;
        else if (rot < -20480 || rot > 20480)
            item->pos.z_pos += 512;
    }
}
