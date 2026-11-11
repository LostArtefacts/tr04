#include "debris.h"

sint32 GetFreeDebris(void)
{
    DEBRIS_STRUCT *dptr;
    sint32 eldestage, eldestfree, free;
    int i;

    free = next_debris;
    eldestfree = 0;
    eldestage = -0x4000;
    dptr = &debris[next_debris];
    for (i = 0; i < 256; i++) {
        if (!dptr->On) {
            next_debris = (free + 1) & 0xFF;
            return free;
        }
        if (dptr->Yvel > eldestage) {
            eldestfree = free;
            eldestage = dptr->Yvel;
        }
        if (free == 255) {
            dptr = debris;
            free = 0;
        } else {
            free++;
            dptr++;
        }
    }
    next_debris = (eldestfree + 1) & 0xFF;
    return eldestfree;
}
