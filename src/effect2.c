#include "effect2.h"

RIPPLE_STRUCT *SetupRipple(sint32 x, sint32 y, sint32 z, sint32 size, sint32 flags)
{
    RIPPLE_STRUCT *ripple;
    sint32 n;

    ripple = ripples;
    for (n = 0; n < 16; n++) {
        if (!(ripple->flags & 1)) {
            ripple->flags = (char)flags | 1;
            ripple->life = (GetRandomControl() & 0xF) + 48;
            ripple->size = (uint8)size;
            ripple->init = 1;
            ripple->x = x;
            ripple->y = y;
            ripple->z = z;
            if (flags & 0x40) {
                ripple->x += (GetRandomControl() & 0x7F) - 64;
                ripple->z += (GetRandomControl() & 0x7F) - 64;
            }
            return ripple;
        }
        ripple++;
    }
    return ripple;
}

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
