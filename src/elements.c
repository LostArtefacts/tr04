#include "elements.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "sound.h"
#include "switch.h"
#include "tomb4fx.h"
#include "specific/function_stubs.h"

void InitialiseElementPuzzle(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!item->trigger_flags)
        item->mesh_bits = 80;
    else if (item->trigger_flags == 1)
        item->mesh_bits = 65;
    else if (item->trigger_flags == 2)
        item->mesh_bits = 68;
    else
        item->mesh_bits = 0;
}

void ControlElementPuzzle(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint32 r, g, fade;
    sint16 itemNos[8];
    sint16 itemNum, next, numTriggers;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (item->trigger_flags == 1) {
        SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
        r = (GetRandomControl() & 0x3F) + 192;
        g = (GetRandomControl() & 0x1F) + 96;
        if (item->item_flags[3]) {
            item->item_flags[3]--;
            fade = 255 - GetRandomControl() % ((91 - item->item_flags[3]) << 2);
            if (fade < 1)
                fade = 1;
            if (fade <= 255) {
                r = (r * fade) >> 8;
                g = (g * fade) >> 8;
            } else
                fade = 0;
        } else
            fade = 0;
        AddFire(item->pos.x_pos, item->pos.y_pos - 620, item->pos.z_pos, 1, item->room_number, fade);
        TriggerDynamic(item->pos.x_pos, item->pos.y_pos - 768, item->pos.z_pos, 12, r, g, 0);
    } else if (item->trigger_flags == 3) {
        if (item->item_flags[1] > 90)
            SoundEffect(SFX_JOBY_WIND, &item->pos, SFX_DEFAULT);
        if (item->item_flags[1] < 60) {
            item->item_flags[1]++;
            return;
        }
        item->item_flags[0]++;
        if (item->item_flags[0] == 90) {
            for (numTriggers = GetSwitchTrigger(item, itemNos, 0); numTriggers > 0; numTriggers--) {
                AddActiveItem(itemNos[numTriggers - 1]);
                items[itemNos[numTriggers - 1]].status = ITEM_ACTIVE;
                items[itemNos[numTriggers - 1]].flags |= IFL_CODEBITS;
            }
            KillItem(item_number);
        } else {
            for (itemNum = room[item->room_number].item_number; itemNum != NO_ITEM; itemNum = next) {
                item2 = &items[itemNum];
                next = item2->next_item;
                if (item2->object_number == FLAME_EMITTER2) {
                    if (item->item_flags[0] == 89) {
                        RemoveActiveItem(itemNum);
                        item2->status = ITEM_INACTIVE;
                    } else {
                        item2->item_flags[3] = 255 - GetRandomControl() % (item->item_flags[0] << 2);
                        if (item2->item_flags[3] < 2) {
                            item2->item_flags[3] = 2;
                            RemoveActiveItem(itemNum);
                            item2->status = ITEM_INACTIVE;
                        }
                    }
                } else if (item2->object_number == ELEMENT_PUZZLE && item2->trigger_flags == 1 && !item2->item_flags[3])
                    item2->item_flags[3] = 90;
            }
        }
    }
}
