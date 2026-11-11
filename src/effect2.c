#include "effect2.h"

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
