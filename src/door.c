#include "door.h"
#include "box.h"
#include "control.h"

void ShutThatDoor(DOORPOS_DATA *d)
{
    FLOOR_INFO *floor;
    CREATURE_INFO *cinfo;
    sint16 slot;

    floor = d->floor;
    if (floor) {
        floor->box = NO_BOX;
        floor->floor = floor->ceiling = NO_HEIGHT >> 8;
        floor->index = 0;
        floor->pit_room = floor->sky_room = NO_ROOM;
        if (d->block != NO_BOX) {
            boxes[d->block].overlap_index |= BLOCKED;
            cinfo = baddie_slots;
            for (slot = 0; slot < NUM_SLOTS; slot++, cinfo++)
                cinfo->LOT.target_box = NO_BOX;
        }
    }
}

void OpenThatDoor(DOORPOS_DATA *d)
{
    CREATURE_INFO *cinfo;
    sint16 slot;

    if (d->floor) {
        *d->floor = d->data;
        if (d->block != NO_BOX) {
            boxes[d->block].overlap_index &= ~BLOCKED;
            cinfo = baddie_slots;
            for (slot = 0; slot < NUM_SLOTS; slot++, cinfo++)
                cinfo->LOT.target_box = NO_BOX;
        }
    }
}

void PushPullKickDoorControl(sint16 item_number)
{
    ITEM_INFO *item;
    DOOR_DATA *door;

    item = &items[item_number];
    door = (DOOR_DATA *)item->data;
    if (!door->Opened) {
        OpenThatDoor(&door->d1);
        OpenThatDoor(&door->d2);
        OpenThatDoor(&door->d1flip);
        OpenThatDoor(&door->d2flip);
        door->Opened = 1;
    }
    AnimateItem(item);
}
