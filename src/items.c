#include "items.h"

extern sint16 next_item_free;

void InitialiseItemArray(int num)
{
    int i;
    ITEM_INFO *item;

    next_item_active = NO_ITEM;
    item = &items[level_items];
    next_item_free = (sint16)level_items;
    for (i = level_items + 1; i < num; i++, item++) {
        item->next_item = i;
        item->active = 0;
    }
    item->next_item = NO_ITEM;
}
