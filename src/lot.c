#include "lot.h"
#include "specific/function_stubs.h"

extern int slots_used;

void InitialiseLOTarray(int allocmem)
{
    int i;
    CREATURE_INFO *creature;

    if (allocmem)
        baddie_slots = (CREATURE_INFO *)game_malloc(NUM_SLOTS * sizeof(CREATURE_INFO));
    creature = baddie_slots;
    for (i = 0; i < NUM_SLOTS; i++, creature++) {
        creature->item_num = NO_ITEM;
        if (allocmem)
            creature->LOT.node = (BOX_NODE *)game_malloc(sizeof(BOX_NODE) * num_boxes);
    }
    slots_used = 0;
}

void DisableBaddieAI(sint16 item_number)
{
    ITEM_INFO *item;
    CREATURE_INFO *creature;

    item = &items[item_number];
    creature = (CREATURE_INFO *)item->data;
    item->data = NULL;
    if (creature) {
        creature->item_num = NO_ITEM;
        slots_used--;
    }
}
