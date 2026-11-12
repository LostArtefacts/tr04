#include "demigod.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "specific/function_stubs.h"

void TriggerDemigodMissile(PHD_3DPOS *pos, sint16 room_number, sint16 type)
{
    FX_INFO *fx;
    sint16 fx_number;

    fx_number = CreateEffect(room_number);
    if (fx_number != NO_ITEM) {
        fx = &effects[fx_number];
        fx->pos.x_pos = pos->x_pos;
        fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
        fx->pos.z_pos = pos->z_pos;
        fx->pos.x_rot = pos->x_rot;
        if (type >= 4)
            fx->pos.y_rot = pos->y_rot + (GetRandomControl() & 0x7FF) - 1024;
        else
            fx->pos.y_rot = pos->y_rot;
        fx->pos.z_rot = 0;
        fx->room_number = room_number;
        fx->counter = (sint16)(2 * GetRandomControl() + 0x8000);
        fx->speed = (GetRandomControl() & 0x1F) + 96;
        fx->flag1 = type;
        fx->object_number = BUBBLES;
        if (type >= 4)
            type--;
        fx->frame_number = objects[BUBBLES].mesh_index + type * 2;
    }
}

void InitialiseDemigod(sint16 item_number)
{
    ITEM_INFO *item, *item2;
    sint16 i;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[item->object_number].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->goal_anim_state = 0;
    item->current_anim_state = 0;
    item2 = items;
    for (i = 0; i < level_items; i++, item2++) {
        if (item != item2 && item2->object_number == DEMIGOD3 && !item2->item_flags[0]) {
            item->item_flags[0] = i;
            return;
        }
    }
}
