#include "elements.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"

void InitialiseElementPuzzle(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!item->trigger_flags)
        item->mesh_bits = 80;
    else if (item->trigger_flags == 1)
        item->mesh_bits = 65;
    else if (item->trigger_flags == 2)
        item->mesh_bits = 68;
    else
        item->mesh_bits = 0;
}
