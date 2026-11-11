#include "deathsld.h"
#include "collide.h"
#include "control.h"
#include "lara.h"
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
