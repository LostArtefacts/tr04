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
