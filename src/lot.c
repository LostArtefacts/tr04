#include <string.h>

#include "lot.h"
#include "camera.h"
#include "lara.h"
#include "objnums.h"
#include "room.h"
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
            creature->LOT.node = (BOX_NODE *)game_malloc(sizeof(BOX_NODE) * number_boxes);
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

void InitialiseSlot(sint16 item_number, int slot)
{
    ITEM_INFO *item;
    CREATURE_INFO *creature;

    creature = &baddie_slots[slot];
    item = &items[item_number];
    item->data = creature;
    creature->item_num = item_number;
    creature->mood = BORED_MOOD;
    memset(creature->joint_rotation, 0, sizeof(creature->joint_rotation));
    creature->alerted = 0;
    creature->head_left = 0;
    creature->head_right = 0;
    creature->reached_goal = 0;
    creature->hurt_by_lara = 0;
    creature->patrol2 = 0;
    creature->jump_ahead = 0;
    creature->monkey_ahead = 0;
    creature->LOT.can_jump = 0;
    creature->LOT.can_monkey = 0;
    creature->LOT.is_jumping = 0;
    creature->LOT.is_monkeying = 0;
    creature->maximum_turn = 182;
    creature->flags = 0;
    creature->enemy = NULL;
    creature->LOT.step = 256;
    creature->LOT.drop = -512;
    creature->LOT.block_mask = 0x4000;
    creature->LOT.fly = 0;
    creature->LOT.zone = BASIC_ZONE;

    switch (item->object_number) {
    case ENEMY_JEEP:
    case VON_CROY:
    case RAGHEAD:
    case SUPER_RAGHEAD:
        creature->LOT.step = 1024;
        creature->LOT.drop = -1024;
        creature->LOT.can_jump = 1;
        creature->LOT.can_monkey = 1;
        creature->LOT.zone = HUMAN_ZONE;
        break;

    case GUIDE:
    case MUMMY:
    case SPHINX:
    case HORSEMAN:
    case SCORPION:
    case TROOPS:
    case KNIGHTS_TEMPLAR:
    case MUTANT:
    case WILD_BOAR:
    case DEMIGOD1:
    case DEMIGOD2:
    case DEMIGOD3:
    case FUCKED_UP_DOG:
        creature->LOT.step = 256;
        creature->LOT.drop = -512;
        creature->LOT.zone = BASIC_ZONE;
        break;

    case SKELETON:
    case SETHA:
        creature->LOT.step = 256;
        creature->LOT.drop = -512;
        creature->LOT.can_jump = 1;
        creature->LOT.zone = SKELLY_ZONE;
        break;

    case CROCODILE:
        creature->LOT.step = 20480;
        creature->LOT.drop = -20480;
        creature->LOT.fly = 32;
        creature->LOT.zone = CROC_ZONE;
        break;

    case BAT:
        creature->LOT.step = 20480;
        creature->LOT.drop = -20480;
        creature->LOT.fly = 16;
        creature->LOT.zone = FLYER_ZONE;
        break;

    case HARPY:
    case BIG_BEETLE:
    case HAMMERHEAD:
        creature->LOT.step = 20480;
        creature->LOT.drop = -20480;
        creature->LOT.fly = 32;
        creature->LOT.zone = FLYER_ZONE;
        break;
    }

    ClearLOT(&creature->LOT);
    if (item_number != lara.item_number)
        CreateZone(item);
    slots_used++;
}

void CreateZone(ITEM_INFO *item)
{
    int i;
    sint16 zone_number, flip_number;
    sint16 *zone, *flip;
    CREATURE_INFO *creature;
    ROOM_INFO *r;
    BOX_NODE *node;

    creature = (CREATURE_INFO *)item->data;
    r = &room[item->room_number];
    item->box_number = r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)].box;

    if (!creature->LOT.fly) {
        zone = ground_zone[creature->LOT.zone][0];
        flip = ground_zone[creature->LOT.zone][1];
        zone_number = zone[item->box_number];
        flip_number = flip[item->box_number];
        node = creature->LOT.node;
        i = 0;
        creature->LOT.zone_count = 0;
        for (; i < number_boxes; i++, zone++, flip++) {
            if (*zone == zone_number || *flip == flip_number) {
                node->box_number = i;
                node++;
                creature->LOT.zone_count++;
            }
        }
    } else {
        creature->LOT.zone_count = 0;
        node = creature->LOT.node;
        for (i = 0; i < number_boxes; i++) {
            node->box_number = i;
            node++;
            creature->LOT.zone_count++;
        }
    }
}

void ClearLOT(LOT_INFO *lot)
{
    int i;
    BOX_NODE *node;

    lot->tail = 2047;
    lot->head = 2047;
    lot->search_number = 0;
    lot->target_box = 2047;
    lot->required_box = 2047;
    node = lot->node;
    for (i = 0; i < number_boxes; i++, node++) {
        node->next_expansion = 2047;
        node->exit_box = 2047;
        node->search_number = 0;
    }
}
