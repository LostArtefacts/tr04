#include "deathsld.h"
#include "collide.h"
#include "control.h"
#include "lara.h"
#include "lara_states.h"
#include "specific/input.h"
#include "specific/function_stubs.h"
#include "specific/3dmath.h"
#include "anim.h"
#include "sound.h"

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

void ControlDeathSlide(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 c, s, x, y, z;
    sint16 room_number;
    sint32 lara_on_deathslide;
    GAME_VECTOR *old;

    item = &items[item_number];
    if (item->status == ITEM_ACTIVE) {
        if (!(item->flags & IFL_INVISIBLE)) {
            old = (GAME_VECTOR *)item->data;
            item->pos.x_pos = old->x;
            item->pos.y_pos = old->y;
            item->pos.z_pos = old->z;
            if (old->room_number != item->room_number)
                ItemNewRoom(item_number, old->room_number);
            item->status = ITEM_INACTIVE;
            item->current_anim_state = item->goal_anim_state = 1;
            item->anim_number = objects[item->object_number].anim_index;
            item->frame_number = anims[item->anim_number].frame_base;
            RemoveActiveItem(item_number);
            return;
        }
        if (item->current_anim_state == 1) {
            AnimateItem(item);
            return;
        }
        AnimateItem(item);
        if (item->fallspeed < 100)
            item->fallspeed += 5;
        c = phd_cos(item->pos.y_rot);
        s = phd_sin(item->pos.y_rot);
        item->pos.z_pos += item->fallspeed * c >> W2V_SHIFT;
        item->pos.x_pos += item->fallspeed * s >> W2V_SHIFT;
        item->pos.y_pos += item->fallspeed >> 2;
        room_number = item->room_number;
        GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        if (room_number != item->room_number)
            ItemNewRoom(item_number, room_number);
        lara_on_deathslide = (lara_item->current_anim_state == AS_DEATHSLIDE);
        if (lara_on_deathslide) {
            lara_item->pos.x_pos = item->pos.x_pos;
            lara_item->pos.y_pos = item->pos.y_pos;
            lara_item->pos.z_pos = item->pos.z_pos;
        }
        z = item->pos.z_pos + (WALL_L * c >> W2V_SHIFT);
        x = item->pos.x_pos + (WALL_L * s >> W2V_SHIFT);
        y = item->pos.y_pos + (STEP_L >> 2);
        floor = GetFloor(x, y, z, &room_number);
        if (GetHeight(floor, x, y, z) > y + STEP_L && GetCeiling(floor, x, y, z) < y - STEP_L) {
            SoundEffect(280, &item->pos, 0);
        } else {
            if (lara_on_deathslide) {
                lara_item->goal_anim_state = AS_FORWARDJUMP;
                AnimateLara(lara_item);
                lara_item->gravity_status = 1;
                lara_item->speed = item->fallspeed;
                lara_item->fallspeed = item->fallspeed >> 2;
            }
            SoundEffect(281, &item->pos, 0);
            RemoveActiveItem(item_number);
            item->status = ITEM_INACTIVE;
            item->flags -= IFL_INVISIBLE;
        }
    }
}
