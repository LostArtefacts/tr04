#include <string.h>

#include "senet.h"
#include "objects.h"
#include "items.h"
#include "deltapak.h"
#include "specific/function_stubs.h"
#include "sound.h"
#include "control.h"
#include "lara_states.h"
#include "collide.h"
#include "tomb4fx.h"
#include "draw.h"
#include "specific/3dmath.h"
#include "specific/output.h"
#include "specific/input.h"
#include "lara.h"
#include "objnums.h"
#include "anim.h"
#include "effect2.h"

void InitialiseSenet(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 i;

    if (senet_item[0])
        return;
    memset(senet_piece, 0, 6);
    memset(&senet_board[1], 0, 16);
    senet_board[0] = 3;
    for (i = 0; i < level_items; i++) {
        item = &items[i];
        if (item->object_number == GAME_PIECE1) {
            senet_item[0] = i;
            SenetTargetX = item->pos.x_pos + 1024;
            SenetTargetZ = item->pos.z_pos;
        } else if (item->object_number == GAME_PIECE2) {
            senet_item[1] = i;
        } else if (item->object_number == GAME_PIECE3) {
            senet_item[2] = i;
        } else if (item->object_number == ENEMY_PIECE) {
            senet_item[item->trigger_flags + 3] = i;
        }
    }
}

void SenetControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (last_throw > 0 && item->trigger_flags != 1)
        MakeMove(item->object_number - GAME_PIECE1, last_throw);
    RemoveActiveItem(item_number);
}

void InitialiseGameStix(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->trigger_flags = NO_ITEM;
    item->data = item->item_flags;
}

void ShockwaveExplosion(ITEM_INFO *item, uint32 col, sint32 speed)
{
    sint32 InnerOuter;

    if (speed < 0)
        InnerOuter = 0x2000280;
    else
        InnerOuter = 0xA00020;
    item->pos.y_pos -= 384;
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0);
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0x2000);
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0x4000);
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0x6000);
    item->pos.y_pos += 384;
}

void ThrowSticks(ITEM_INFO *item)
{
    sint16 rnd;
    sint16 i;

    i = 0;
    last_throw = 0;
    item->trigger_flags = 0;
    for (; i < 4; i++) {
        rnd = GetRandomControl() & 1;
        last_throw += rnd;
        if (rnd)
            item->trigger_flags |= 1 << i;
    }
    if (!last_throw)
        last_throw = 6;
    item->hit_points = 120;
    for (i = 0; i < 3; i++)
        items[senet_item[i]].trigger_flags = 1;
}

void ControlGodHead(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        switch (item->pos.y_rot) {
        case 0:
            item->pos.z_pos &= ~1023;
            break;
        case 0x4000:
            item->pos.x_pos &= ~1023;
            break;
        case -0x8000:
            item->pos.z_pos |= 1023;
            break;
        case -0x4000:
            item->pos.x_pos |= 1023;
            break;
        }
        if (!item->item_flags[0]) {
            if (item->item_flags[1] < 4096) {
                item->item_flags[1] += 128;
            } else {
                item->item_flags[0] = 1;
                item->item_flags[1] = 4096;
                item->item_flags[2] = 210;
            }
        } else if (item->item_flags[2]) {
            item->item_flags[2]--;
        } else if (item->item_flags[1] >= 128) {
            item->item_flags[1] -= 128;
        } else {
            KillItem(item_number);
        }
    }
}
