#include "items.h"
#include "effect2.h"
#include "lara.h"
#include "room.h"

extern sint16 next_item_free;
extern uint8 InItemControlLoop;
extern sint16 ItemNewRooms[256][2];
extern sint16 ItemNewRoomNo;

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

void KillItem(sint16 item_num)
{
    sint16 linknum;
    ITEM_INFO *item;

    if (InItemControlLoop) {
        ItemNewRooms[ItemNewRoomNo][0] = item_num | 0x8000;
        ItemNewRoomNo++;
        return;
    }

    DetatchSpark(item_num, 128);
    item = &items[item_num];
    item->active = 0;
    item->really_active = 0;
    linknum = next_item_active;
    if (linknum == item_num) {
        next_item_active = item->next_active;
    } else {
        for (; linknum != NO_ITEM; linknum = items[linknum].next_active) {
            if (items[linknum].next_active == item_num) {
                items[linknum].next_active = item->next_active;
                break;
            }
        }
    }

    if (item->room_number != 255) {
        linknum = room[item->room_number].item_number;
        if (linknum == item_num) {
            room[item->room_number].item_number = item->next_item;
        } else {
            for (; linknum != NO_ITEM; linknum = items[linknum].next_item) {
                if (items[linknum].next_item == item_num) {
                    items[linknum].next_item = item->next_item;
                    break;
                }
            }
        }
    }

    if (item == lara.target)
        lara.target = NULL;
    if (item_num < level_items) {
        item->flags |= IFL_CLEARBODY;
    } else {
        item->next_item = next_item_free;
        next_item_free = item_num;
    }
}

sint16 CreateItem(void)
{
    sint16 item_num;

    item_num = next_item_free;
    if (item_num == NO_ITEM)
        return item_num;
    items[item_num].flags = 0;
    next_item_free = items[item_num].next_item;
    return item_num;
}
