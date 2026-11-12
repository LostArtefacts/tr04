#include "croc.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "draw.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "room.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseCroc(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        item->anim_number = objects[CROCODILE].anim_index + 12;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 8;
    } else {
        item->anim_number = objects[CROCODILE].anim_index;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 1;
    }
}

void InitialiseLocustEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    switch (item->pos.y_rot) {
    case 0:
        item->pos.z_pos += 512;
        break;
    case 16384:
        item->pos.x_pos += 512;
        break;
    case -32768:
        item->pos.z_pos -= 512;
        break;
    case -16384:
        item->pos.x_pos -= 512;
        break;
    }
}
