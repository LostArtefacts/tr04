#include "tomb4fx.h"

void ClearFires(void)
{
    FIRE_LIST *fire;
    sint32 i;

    fire = fires;
    for (i = 0; i < 32; i++, fire++)
        fire->on = 0;
}
