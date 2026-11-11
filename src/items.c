#include "items.h"
#include "anim.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "room.h"
#include "specific/function_stubs.h"

extern sint16 next_item_free;
extern sint16 next_fx_free;
extern uint8 InItemControlLoop;
extern sint16 ItemNewRooms[256][2];
extern sint16 ItemNewRoomNo;

void InitialiseItemArray(int num)
{
    int i;
    ITEM_INFO *item;

    next_item_active = NO_ITEM;
    item = &items[level_items];
    next_item_free = (sint16)level_items;
    for (i = level_items + 1; i < num; i++, item++) {
        item->next_item = i;
        item->active = 0;
    }
    item->next_item = NO_ITEM;
}

void KillItem(sint16 item_num)
{
    sint16 linknum;
    ITEM_INFO *item;

    if (InItemControlLoop) {
        ItemNewRooms[ItemNewRoomNo][0] = item_num | 0x8000;
        ItemNewRoomNo++;
        return;
    }

    DetatchSpark(item_num, 128);
    item = &items[item_num];
    item->active = 0;
    item->really_active = 0;
    linknum = next_item_active;
    if (linknum == item_num) {
        next_item_active = item->next_active;
    } else {
        for (; linknum != NO_ITEM; linknum = items[linknum].next_active) {
            if (items[linknum].next_active == item_num) {
                items[linknum].next_active = item->next_active;
                break;
            }
        }
    }

    if (item->room_number != 255) {
        linknum = room[item->room_number].item_number;
        if (linknum == item_num) {
            room[item->room_number].item_number = item->next_item;
        } else {
            for (; linknum != NO_ITEM; linknum = items[linknum].next_item) {
                if (items[linknum].next_item == item_num) {
                    items[linknum].next_item = item->next_item;
                    break;
                }
            }
        }
    }

    if (item == lara.target)
        lara.target = NULL;
    if (item_num < level_items) {
        item->flags |= IFL_CLEARBODY;
    } else {
        item->next_item = next_item_free;
        next_item_free = item_num;
    }
}

sint16 CreateItem(void)
{
    sint16 item_num;

    item_num = next_item_free;
    if (item_num == NO_ITEM)
        return item_num;
    items[item_num].flags = 0;
    next_item_free = items[item_num].next_item;
    return item_num;
}

void InitialiseItem(sint16 item_num)
{
    ITEM_INFO *item;
    ROOM_INFO *r;
    FLOOR_INFO *floor;

    item = &items[item_num];
    item->anim_number = objects[item->object_number].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = anims[item->anim_number].current_anim_state;
    item->required_anim_state = 0;
    item->pos.x_rot = item->pos.z_rot = 0;
    item->speed = item->fallspeed = 0;
    item->active = 0;
    item->status = ITEM_INACTIVE;
    item->gravity_status = 0;
    item->hit_status = 0;
    item->looked_at = 0;
    item->dynamic_light = 0;
    item->ai_bits = 0;
    item->really_active = 0;
    item->item_flags[0] = item->item_flags[1] = item->item_flags[2] = item->item_flags[3] = 0;
    item->hit_points = objects[item->object_number].hit_points;
    item->poisoned = 0;
    item->collidable = 1;
    item->timer = 0;

    if (item->object_number == SIXSHOOTER_ITEM || item->object_number == CROSSBOW_ITEM || item->object_number == SHOTGUN_ITEM)
        item->mesh_bits = 1;
    else
        item->mesh_bits = -1;
    if (item->object_number == SARCOPHAGUS_CUT)
        item->mesh_bits = 5;
    if (item->object_number == HORUS_STATUE)
        item->mesh_bits = 1607;

    item->touch_bits = 0;
    item->after_death = 0;
    item->fired_weapon = 0;
    item->data = NULL;

    if (item->flags & IFL_INVISIBLE) {
        item->status = ITEM_INVISIBLE;
        item->flags -= IFL_INVISIBLE;
    } else if (objects[item->object_number].intelligent)
        item->status = ITEM_INVISIBLE;

    if ((item->flags & IFL_CODEBITS) == IFL_CODEBITS) {
        item->flags -= IFL_CODEBITS;
        item->flags |= IFL_REVERSE;
        AddActiveItem(item_num);
        item->status = ITEM_ACTIVE;
    }

    r = &room[item->room_number];
    item->next_item = r->item_number;
    r->item_number = item_num;
    floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];
    item->floor = floor->floor << 8;
    item->box_number = floor->box;

    if (objects[item->object_number].initialise)
        objects[item->object_number].initialise(item_num);

    item->il.ambient = r->ambient;
    item->il.room_number = item->il.fcnt = -1;
    item->il.nPrevLights = item->il.nCurrentLights = item->il.RoomChange = 0;
    item->il.pCurrentLights = item->il.CurrentLights;
    item->il.pPrevLights = item->il.PrevLights;
}


void RemoveActiveItem(sint16 item_num)
{
    sint16 linknum;

    if (!items[item_num].active)
        return;
    items[item_num].active = 0;
    linknum = next_item_active;
    if (linknum == item_num) {
        next_item_active = items[item_num].next_active;
        return;
    }
    for (; linknum != NO_ITEM; linknum = items[linknum].next_active) {
        if (items[linknum].next_active == item_num) {
            items[linknum].next_active = items[item_num].next_active;
            return;
        }
    }
}

void RemoveDrawnItem(sint16 item_num)
{
    sint16 linknum;
    ITEM_INFO *item;

    item = &items[item_num];
    linknum = room[item->room_number].item_number;
    if (linknum == item_num) {
        room[item->room_number].item_number = item->next_item;
    } else {
        for (; linknum != NO_ITEM; linknum = items[linknum].next_item) {
            if (items[linknum].next_item == item_num) {
                items[linknum].next_item = item->next_item;
                return;
            }
        }
    }
}

void AddActiveItem(sint16 item_num)
{
    ITEM_INFO *item;

    item = &items[item_num];
    item->flags |= IFL_TRIGGERED;
    if (objects[item->object_number].control == NULL) {
        item->status = ITEM_INACTIVE;
        return;
    }
    if (item->active == 1)
        return;
    item->active = 1;
    item->next_active = next_item_active;
    next_item_active = item_num;
}

void ItemNewRoom(sint16 item_num, sint16 room_number)
{
    sint16 linknum;
    ROOM_INFO *r;
    ITEM_INFO *item;

    if (InItemControlLoop) {
        ItemNewRooms[ItemNewRoomNo][0] = item_num;
        ItemNewRooms[ItemNewRoomNo][1] = room_number;
        ItemNewRoomNo++;
        return;
    }

    item = &items[item_num];
    if (item->room_number != 255) {
        r = &room[item->room_number];
        linknum = r->item_number;
        if (linknum == item_num) {
            r->item_number = item->next_item;
        } else {
            for (; linknum != NO_ITEM; linknum = items[linknum].next_item) {
                if (items[linknum].next_item == item_num) {
                    items[linknum].next_item = item->next_item;
                    break;
                }
            }
        }
    }
    item->room_number = room_number;
    r = &room[room_number];
    item->next_item = r->item_number;
    r->item_number = item_num;
}

void InitialiseFXArray(int allocmem)
{
    int i;
    FX_INFO *fx;

    if (allocmem)
        effects = (FX_INFO *)game_malloc(24 * sizeof(FX_INFO));
    next_fx_active = NO_ITEM;
    next_fx_free = 0;
    fx = effects;
    for (i = 1; i < 24; i++, fx++)
        fx->next_fx = i;
    fx->next_fx = NO_ITEM;
}

sint16 CreateEffect(sint16 room_num)
{
    FX_INFO *fx;
    ROOM_INFO *r;
    sint16 fx_num;

    fx_num = next_fx_free;
    if (fx_num == NO_ITEM)
        return fx_num;
    fx = &effects[fx_num];
    next_fx_free = fx->next_fx;
    r = &room[room_num];
    fx->room_number = room_num;
    fx->next_fx = r->fx_number;
    r->fx_number = fx_num;
    fx->next_active = next_fx_active;
    next_fx_active = fx_num;
    fx->shade = 0x4210;
    return fx_num;
}

void KillEffect(sint16 fx_num)
{
    sint16 linknum;
    FX_INFO *fx;

    if (InItemControlLoop) {
        ItemNewRooms[ItemNewRoomNo][0] = fx_num | 0x8000;
        ItemNewRoomNo++;
        return;
    }

    DetatchSpark(fx_num, 64);
    fx = &effects[fx_num];
    linknum = next_fx_active;
    if (linknum == fx_num) {
        next_fx_active = fx->next_active;
    } else {
        for (; linknum != NO_ITEM; linknum = effects[linknum].next_active) {
            if (effects[linknum].next_active == fx_num) {
                effects[linknum].next_active = fx->next_active;
                break;
            }
        }
    }
    linknum = room[fx->room_number].fx_number;
    if (linknum == fx_num) {
        room[fx->room_number].fx_number = fx->next_fx;
    } else {
        for (; linknum != NO_ITEM; linknum = effects[linknum].next_fx) {
            if (effects[linknum].next_fx == fx_num) {
                effects[linknum].next_fx = fx->next_fx;
                break;
            }
        }
    }
    fx->next_fx = next_fx_free;
    next_fx_free = fx_num;
}

void EffectNewRoom(sint16 fx_num, sint16 room_num)
{
    sint16 linknum;
    ROOM_INFO *r;
    FX_INFO *fx;

    if (InItemControlLoop) {
        ItemNewRooms[ItemNewRoomNo][0] = fx_num;
        ItemNewRooms[ItemNewRoomNo][1] = room_num;
        ItemNewRoomNo++;
        return;
    }

    fx = &effects[fx_num];
    r = &room[fx->room_number];
    linknum = r->fx_number;
    if (linknum == fx_num) {
        r->fx_number = fx->next_fx;
    } else {
        for (; linknum != NO_ITEM; linknum = effects[linknum].next_fx) {
            if (effects[linknum].next_fx == fx_num) {
                effects[linknum].next_fx = fx->next_fx;
                break;
            }
        }
    }
    fx->room_number = room_num;
    r = &room[room_num];
    fx->next_fx = r->fx_number;
    r->fx_number = fx_num;
}
