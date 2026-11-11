#include "deathsld.h"
#include "collide.h"
#include "control.h"
#include "lara.h"
#include "lara_states.h"
#include "specific/input.h"
#include "specific/function_stubs.h"

void InitialiseDeathSlide(sint16 item_number)
{
    ITEM_INFO *item;
    GAME_VECTOR *old;

    item = &items[item_number];
    old = (GAME_VECTOR *)game_malloc(sizeof(GAME_VECTOR));
    item->data = old;
    old->x = item->pos.x_pos;
    old->y = item->pos.y_pos;
    old->z = item->pos.z_pos;
    old->room_number = item->room_number;
}

void DeathSlideCollision(sint16 item_number, ITEM_INFO *laraitem, COLL_INFO *coll)
{
    ITEM_INFO *item;

    if (!(input & IN_ACTION) || laraitem->gravity_status || lara.gun_status != LG_NO_ARMS || laraitem->current_anim_state != AS_STOP)
        return;
    item = &items[item_number];
    if (item->status != ITEM_INACTIVE)
        return;
    if (TestLaraPosition(DeathSlideBounds, item, laraitem)) {
        AlignLaraPosition(&DeathSlidePosition, item, laraitem);
        lara.gun_status = LG_HANDS_BUSY;
        laraitem->goal_anim_state = AS_DEATHSLIDE;
        do
            AnimateItem(laraitem);
        while (laraitem->current_anim_state != AS_NULL);
        if (!item->active)
            AddActiveItem(item_number);
        item->status = ITEM_ACTIVE;
        item->flags |= IFL_INVISIBLE;
    }
}
