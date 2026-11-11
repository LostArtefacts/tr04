#include "tomb4fx.h"

void ClearFires(void)
{
    FIRE_LIST *fire;
    sint32 i;

    fire = fires;
    for (i = 0; i < 32; i++, fire++)
        fire->on = 0;
}

void AddFire(sint32 x, sint32 y, sint32 z, sint32 size, sint16 room_number, sint16 fade)
{
    FIRE_LIST *fire;
    sint32 i;

    fire = fires;
    for (i = 0; i < 32; fire++, i++) {
        if (fire->on)
            continue;
        if (fade)
            fire->on = (sint8)fade;
        else
            fire->on = 1;
        fire->x = x;
        fire->y = y;
        fire->z = z;
        fire->size = (sint8)size;
        fire->room_number = room_number;
        break;
    }
}
