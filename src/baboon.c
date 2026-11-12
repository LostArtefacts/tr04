#include "baboon.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effects.h"
#include "objnums.h"
#include "room.h"
#include "tomb4fx.h"
#include "specific/function_stubs.h"

void InitialiseBaboon(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[BABOON_NORMAL].anim_index + 2;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 6;
    item->item_flags[0] = (sint16)(item->pos.z_pos >> 2 & 0xFFFFFF00 | item->pos.x_pos >> 10);
    item->item_flags[1] = (sint16)(item->pos.y_pos >> 8);
    if (item->object_number == BABOON_NORMAL)
        item->ai_bits = FOLLOW;
}

void FindCrowbarSwitch(ITEM_INFO *item, sint16 switch_index)
{
    ITEM_INFO *item2;
    sint16 item_num, next;

    for (item_num = room[item->room_number].item_number; item_num != NO_ITEM; item_num = next) {
        item2 = &items[item_num];
        next = item2->next_item;
        if (item2->object_number == CROWBAR_SWITCH)
            item2->item_flags[0] = switch_index;
    }
    item->item_flags[2] = switch_index;
}

void ReTriggerBaboon(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    ExplodeBaboon(item);
    item->pos.x_pos = ((uint16)item->item_flags[0] & 0xFF) << 10 | 0x200;
    item->pos.y_pos = item->item_flags[1] << 8;
    item->pos.z_pos = ((uint16)item->item_flags[0] & 0xFFFFFF00) << 2 | 0x200;
    IsRoomOutside(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (item->room_number != IsRoomOutsideNo)
        ItemNewRoom(item_number, IsRoomOutsideNo);
    item->anim_number = objects[BABOON_NORMAL].anim_index + 2;
    item->frame_number = anims[item->anim_number].frame_base;
    item->goal_anim_state = 6;
    item->current_anim_state = 6;
    item->hit_points = objects[item->object_number].hit_points;
    RemoveActiveItem(item_number);
    item->after_death = 0;
    item->status = ITEM_INVISIBLE;
    item->flags &= ~(IFL_INVISIBLE | IFL_CODEBITS);
    DisableBaddieAI(item_number);
    if (item->object_number != BABOON_NORMAL || !item->trigger_flags) {
        if (item->object_number == BABOON_NORMAL || !item->trigger_flags)
            item->ai_bits = FOLLOW;
        FindCrowbarSwitch(item, 0);
    }
}

void ExplodeBaboon(ITEM_INFO *item)
{
    item->pos.y_pos -= 128;
    TriggerShockwave((PHD_VECTOR *)&item->pos, 0x2000280, -48, 0x28802000, 0);
    TriggerShockwave((PHD_VECTOR *)&item->pos, 0x2000280, -48, 0x28802000, 0x2000);
    TriggerShockwave((PHD_VECTOR *)&item->pos, 0x2000280, -48, 0x28802000, 0x4000);
    TriggerShockwave((PHD_VECTOR *)&item->pos, 0x2000280, -48, 0x28802000, 0x6000);
    item->pos.y_pos += 128;
    FlashFadeR = 255;
    FlashFadeG = 64;
    FlashFadeB = 0;
    FlashFader = 32;
}
