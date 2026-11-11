#include "door.h"
#include "box.h"
#include "control.h"
#include "draw.h"

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

void DoorControl(sint16 item_number)
{
    ITEM_INFO *item;
    DOOR_DATA *door;
    sint16 *bounds;

    item = &items[item_number];
    door = (DOOR_DATA *)item->data;
    if (item->trigger_flags == 1) {
        if (item->item_flags[0]) {
            bounds = GetBoundsAccurate(item);
            item->pos.y_pos -= 12;
            item->item_flags[0]--;
            if (item->pos.y_pos < bounds[2] + *(sint32 *)&item->item_flags[2] - 256) {
                item->pos.y_pos = bounds[2] + *(sint32 *)&item->item_flags[2] - 256;
                item->item_flags[0] = 0;
            }
            if (!door->Opened) {
                OpenThatDoor(&door->d1);
                OpenThatDoor(&door->d2);
                OpenThatDoor(&door->d1flip);
                OpenThatDoor(&door->d2flip);
                door->Opened = 1;
            }
        } else {
            if (item->pos.y_pos < *(sint32 *)&item->item_flags[2])
                item->pos.y_pos += 4;
            if (item->pos.y_pos >= *(sint32 *)&item->item_flags[2]) {
                item->pos.y_pos = *(sint32 *)&item->item_flags[2];
                if (door->Opened) {
                    ShutThatDoor(&door->d1);
                    ShutThatDoor(&door->d2);
                    ShutThatDoor(&door->d1flip);
                    ShutThatDoor(&door->d2flip);
                    door->Opened = 0;
                }
            }
        }
    } else {
        if (TriggerActive(item)) {
            if (item->current_anim_state == 0)
                item->goal_anim_state = 1;
            else {
                if (!door->Opened) {
                    OpenThatDoor(&door->d1);
                    OpenThatDoor(&door->d2);
                    OpenThatDoor(&door->d1flip);
                    OpenThatDoor(&door->d2flip);
                    door->Opened = 1;
                }
            }
        } else {
            if (item->current_anim_state == 1)
                item->goal_anim_state = 0;
            else if (door->Opened) {
                ShutThatDoor(&door->d1);
                ShutThatDoor(&door->d2);
                ShutThatDoor(&door->d1flip);
                ShutThatDoor(&door->d2flip);
                door->Opened = 0;
            }
        }
        AnimateItem(item);
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
