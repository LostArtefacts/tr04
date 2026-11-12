#include "wraith.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "specific/function_stubs.h"

void InitialiseWraith(sint16 item_number)
{
    ITEM_INFO *item;
    WRAITH_STRUCT *data;
    sint32 i;

    item = &items[item_number];
    data = (WRAITH_STRUCT *)game_malloc(8 * sizeof(WRAITH_STRUCT));
    item->data = data;
    item->item_flags[0] = 0;
    item->hit_points = 0;
    item->speed = WraithSpeed;
    for (i = 0; i < 8; i++) {
        data->pos.x = item->pos.x_pos;
        data->pos.y = item->pos.y_pos;
        data->pos.z = item->pos.z_pos;
        data->zv = 0;
        data->yv = 0;
        data->xv = 0;
        data->b = 0;
        data->g = 0;
        data->r = 0;
        data++;
    }
}
