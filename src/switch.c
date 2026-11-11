#include "switch.h"
#include "items.h"
#include "control.h"
#include "objnums.h"
#include "lara.h"
#include "lara_states.h"
#include "specific/input.h"

void SwitchControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->flags |= IFL_CODEBITS;
    if (!TriggerActive(item) && !(item->flags & IFL_INVISIBLE)) {
        if (item->object_number == JUMP_SWITCH)
            item->goal_anim_state = 0;
        else
            item->goal_anim_state = 1;
        item->timer = 0;
    }
    AnimateItem(item);
}

void SwitchCollision2(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!(input & IN_ACTION) || item->status != ITEM_INACTIVE || lara.water_status != LW_UNDERWATER || lara.gun_status != LG_NO_ARMS)
        return;
    if (l->current_anim_state != AS_TREAD)
        return;
    if (!TestLaraPosition(Switch2Bounds, item, l))
        return;
    if (item->current_anim_state == 1 || item->current_anim_state == 0) {
        if (MoveLaraPosition(&Switch2Pos, item, l)) {
            l->fallspeed = 0;
            l->goal_anim_state = AS_SWITCHON;
            do {
                AnimateLara(l);
            } while (l->current_anim_state != AS_SWITCHON);
            l->goal_anim_state = AS_TREAD;
            lara.gun_status = LG_HANDS_BUSY;
            if (item->current_anim_state == 1)
                item->goal_anim_state = 0;
            else
                item->goal_anim_state = 1;
            item->status = ITEM_ACTIVE;
            AddActiveItem(item_number);
            AnimateItem(item);
        }
    }
}
