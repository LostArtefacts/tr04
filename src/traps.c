#include "traps.h"
#include "effects.h"
#include "items.h"
#include "lara.h"
#include "objnums.h"
#include "control.h"

void LaraBurn(void)
{
    sint16 fx_number;

    if (!lara.burn && !lara.BurnGreen) {
        fx_number = CreateEffect(lara_item->room_number);
        if (fx_number != NO_ITEM) {
            effects[fx_number].object_number = FLAME;
            lara.burn = 1;
        }
    }
}

void LavaBurn(ITEM_INFO *item)
{
    FLOOR_INFO *floor;
    sint16 room_number;

    if (item->hit_points >= 0 && lara.water_status != LW_FLYCHEAT) {
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, 32000, item->pos.z_pos, &room_number);
        if (item->floor == GetHeight(floor, item->pos.x_pos, 32000, item->pos.z_pos)) {
            item->hit_status = 1;
            item->hit_points = -1;
            LaraBurn();
        }
    }
}
