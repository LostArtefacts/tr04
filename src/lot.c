#include "lot.h"
#include "camera.h"
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

int EnableBaddieAI(sint16 item_number, int Always)
{
    int x, y, z;
    int worstslot, slot;
    int worstdist, dist;
    ITEM_INFO *item;
    CREATURE_INFO *creature;

    item = &items[item_number];
    if (item->data)
        return 1;

    if (slots_used < NUM_SLOTS) {
        creature = baddie_slots;
        for (slot = 0; slot < NUM_SLOTS; slot++, creature++) {
            if (creature->item_num == NO_ITEM) {
                InitialiseSlot(item_number, slot);
                return 1;
            }
        }
    }

    if (!Always) {
        x = (item->pos.x_pos - camera.pos.x) >> 8;
        y = (item->pos.y_pos - camera.pos.y) >> 8;
        z = (item->pos.z_pos - camera.pos.z) >> 8;
        worstdist = x * x + y * y + z * z;
    } else {
        worstdist = 0;
    }

    worstslot = -1;
    creature = baddie_slots;
    for (slot = 0; slot < NUM_SLOTS; slot++, creature++) {
        item = &items[creature->item_num];
        x = (item->pos.x_pos - camera.pos.x) >> 8;
        y = (item->pos.y_pos - camera.pos.y) >> 8;
        z = (item->pos.z_pos - camera.pos.z) >> 8;
        dist = x * x + y * y + z * z;
        if (dist > worstdist) {
            worstdist = dist;
            worstslot = slot;
        }
    }

    if (worstslot >= 0) {
        items[baddie_slots[worstslot].item_num].status = ITEM_INVISIBLE;
        DisableBaddieAI(baddie_slots[worstslot].item_num);
        InitialiseSlot(item_number, worstslot);
        return 1;
    }
    return 0;
}
