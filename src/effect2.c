#include "effect2.h"

void TriggerUnderwaterBlood(sint32 x, sint32 y, sint32 z, sint32 size)
{
    RIPPLE_STRUCT *ripple;
    sint32 n;

    ripple = ripples;
    for (n = 0; n < 16; n++) {
        if (!(ripple->flags & 1)) {
            ripple->flags = 49;
            ripple->life = (GetRandomControl() & 7) - 16;
            ripple->init = 1;
            ripple->size = (uint8)size;
            ripple->x = x + (GetRandomControl() & 0x3F) - 32;
            ripple->y = y;
            ripple->z = z + (GetRandomControl() & 0x3F) - 32;
            return;
        }
        ripple++;
    }
}

void KillAllCurrentItems(sint16 item_number)
{
    KillEverythingFlag = 1;
}

void ClearDynamics(void)
{
    sint32 lp;

    number_dynamics = 0;
    for (lp = 0; lp < MAX_DYNAMICS; lp++)
        dynamics[lp].on = 0;
}
