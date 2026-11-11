#include "traps.h"
#include "effects.h"
#include "items.h"
#include "lara.h"
#include "objnums.h"

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
