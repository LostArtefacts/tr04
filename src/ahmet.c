#include "ahmet.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "lara.h"
#include "lot.h"
#include "objnums.h"
#include "sound.h"
#include "sphere.h"
#include "switch.h"
#include "tomb4fx.h"
#include "specific/function_stubs.h"

void InitialiseAhmet(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[AHMET].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
    item->item_flags[0] = (sint16)(item->pos.x_pos >> 10);
    item->item_flags[1] = (sint16)(item->pos.y_pos >> 8);
    item->item_flags[2] = (sint16)(item->pos.z_pos >> 10);
}

void ExplodeAhmet(ITEM_INFO *item)
{
    SPHERE *sphere;
    sint32 spheres, i, r, g;

    if (!(wibble & 7)) {
        spheres = GetSpheres(item, Slist, 1);
        sphere = &Slist[((uint32)wibble >> 3) & 1];
        for (i = 0; i < spheres; i += 2, sphere += 2)
            TriggerFireFlame(sphere->x, sphere->y, sphere->z, -1, 1);
    }
    r = (GetRandomControl() & 0x3F) + 192;
    g = (GetRandomControl() & 0x1F) + 96;
    TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 13, r, g, 0);
    SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
}

void ScalesControl(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 flags;
    sint16 numTriggers;
    sint16 itemNos[8];
    sint16 room_number;

    item = &items[item_number];
    if (item->frame_number == anims[item->anim_number].frame_end) {
        if (item->current_anim_state != 1 && !item->item_flags[1]) {
            if (item->current_anim_state == 2) {
                flags = -512;
                RemoveActiveItem(item_number);
                item->status = ITEM_INACTIVE;
            } else {
                flags = -1024;
                item->item_flags[1] = 1;
            }
            room_number = item->room_number;
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            TestTriggers(trigger_index, 1, flags);
        } else if (item->anim_number == objects[item->object_number].anim_index) {
            RemoveActiveItem(item_number);
            item->status = ITEM_INACTIVE;
            item->item_flags[1] = 0;
        } else if (ReTriggerAhmet((sint16)(sint32)lara.GeneralPtr)) {
            for (numTriggers = (sint16)GetSwitchTrigger(item, itemNos, 0); numTriggers > 0; numTriggers--) {
                if (items[itemNos[numTriggers - 1]].object_number != FLAME_EMITTER2) {
                    items[itemNos[numTriggers - 1]].flags = 1024;
                    break;
                }
            }
            item->goal_anim_state = 1;
        }
    }
    AnimateItem(item);
}

sint32 ReTriggerAhmet(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->current_anim_state == 7 && item->frame_number == anims[item->anim_number].frame_end) {
        FlashFadeR = 255;
        FlashFadeG = 64;
        FlashFadeB = 0;
        FlashFader = 32;
        item->pos.x_pos = ((item->item_flags[0] & 0xFFFF) << 10) | 512;
        item->pos.y_pos = (item->item_flags[1] & 0xFFFF) << 8;
        item->pos.z_pos = ((item->item_flags[2] & 0xFFFF) << 10) | 512;
        IsRoomOutside(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        if (item->room_number != IsRoomOutsideNo)
            ItemNewRoom(item_number, IsRoomOutsideNo);
        item->anim_number = objects[AHMET].anim_index;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 1;
        item->hit_points = objects[AHMET].hit_points;
        AddActiveItem(item_number);
        item->flags &= ~IFL_INVISIBLE;
        item->after_death = 0;
        item->collidable = 1;
        item->status = ITEM_ACTIVE;
        EnableBaddieAI(item_number, 1);
        item->trigger_flags = 1;
        return 1;
    }
    return 0;
}
