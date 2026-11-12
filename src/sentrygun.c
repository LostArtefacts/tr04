#include "sentrygun.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseAutogun(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->item_flags[0] = 0;
    item->item_flags[1] = 768;
    item->item_flags[2] = 0;
}
