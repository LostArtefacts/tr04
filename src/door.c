#include "door.h"
#include "box.h"
#include "control.h"
#include "draw.h"
#include "anim.h"
#include "objnums.h"
#include "lara.h"
#include "lara_states.h"
#include "specific/input.h"

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

void PushPullKickDoorCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 pull;
    sint32 goin;

    item = &items[item_num];
    if (((input & IN_ACTION) && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && item->status != ITEM_ACTIVE && !l->gravity_status && lara.gun_status == LG_NO_ARMS) || (lara.IsMoving && lara.GeneralPtr == (void *)item_num)) {
        pull = 0;
        if (l->room_number == item->room_number) {
            item->pos.y_rot ^= 0x8000;
            pull = 1;
        }
        if (TestLaraPosition(PushPullKickDoorBounds, item, l)) {
            goin = 0;
            if (pull) {
                if (MoveLaraPosition(&PullDoorPos, item, l)) {
                    l->anim_number = ANIM_PULLDOOR;
                    l->frame_number = anims[ANIM_PULLDOOR].frame_base;
                    item->goal_anim_state = 3;
                    goin = 1;
                } else
                    lara.GeneralPtr = (void *)item_num;
            } else {
                if (item->object_number < KICK_DOOR1) {
                    if (MoveLaraPosition(&PushDoorPos, item, l)) {
                        l->anim_number = ANIM_PUSHDOOR;
                        l->frame_number = anims[ANIM_PUSHDOOR].frame_base;
                        item->goal_anim_state = 2;
                        goin = 1;
                    } else
                        lara.GeneralPtr = (void *)item_num;
                } else {
                    if (MoveLaraPosition(&KickDoorPos, item, l)) {
                        l->anim_number = ANIM_KICKDOOR;
                        l->frame_number = anims[ANIM_KICKDOOR].frame_base;
                        item->goal_anim_state = 2;
                        goin = 1;
                    } else
                        lara.GeneralPtr = (void *)item_num;
                }
            }
            if (goin) {
                AddActiveItem(item_num);
                item->status = ITEM_ACTIVE;
                l->current_anim_state = AS_CONTROLLED;
                l->goal_anim_state = AS_STOP;
                lara.IsMoving = 0;
                lara.gun_status = LG_HANDS_BUSY;
            }
        } else if (lara.IsMoving && lara.GeneralPtr == (void *)item_num) {
            lara.IsMoving = 0;
            lara.gun_status = LG_NO_ARMS;
        }
        if (pull)
            item->pos.y_rot ^= 0x8000;
    } else if (!item->current_anim_state)
        DoorCollision(item_num, l, coll);
}

void DoubleDoorCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_num];
    if (((input & IN_ACTION) && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && item->status != ITEM_ACTIVE && !l->gravity_status && lara.gun_status == LG_NO_ARMS) || (lara.IsMoving && lara.GeneralPtr == (void *)item_num)) {
        item->pos.y_rot ^= 0x8000;
        if (TestLaraPosition(PushPullKickDoorBounds, item, l)) {
            if (MoveLaraPosition(&DoubleDoorPos, item, l)) {
                l->anim_number = ANIM_TWODOOR;
                l->frame_number = anims[ANIM_TWODOOR].frame_base;
                l->current_anim_state = AS_TWODOOR;
                AddActiveItem(item_num);
                item->status = ITEM_ACTIVE;
                lara.IsMoving = 0;
                lara.gun_status = LG_HANDS_BUSY;
                lara.head_y_rot = 0;
                lara.head_x_rot = 0;
                lara.torso_y_rot = 0;
                lara.torso_x_rot = 0;
            } else
                lara.GeneralPtr = (void *)item_num;
        } else if (lara.IsMoving && lara.GeneralPtr == (void *)item_num) {
            lara.IsMoving = 0;
            lara.gun_status = LG_NO_ARMS;
        }
        item->pos.y_rot ^= 0x8000;
    }
}

void UnderwaterDoorCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_num];
    if (((input & IN_ACTION) && item->status != ITEM_ACTIVE && l->current_anim_state == AS_TREAD && lara.water_status == LW_UNDERWATER && lara.gun_status == LG_NO_ARMS) || (lara.IsMoving && lara.GeneralPtr == (void *)item_num)) {
        l->pos.y_rot ^= 0x8000;
        if (TestLaraPosition(UnderwaterDoorBounds, item, l)) {
            if (MoveLaraPosition(&UnderwaterDoorPos, item, l)) {
                l->anim_number = ANIM_WATERDOOR;
                l->frame_number = anims[ANIM_WATERDOOR].frame_base;
                l->current_anim_state = AS_CONTROLLED;
                l->fallspeed = 0;
                item->status = ITEM_ACTIVE;
                AddActiveItem(item_num);
                item->goal_anim_state = 1;
                AnimateItem(item);
                lara.IsMoving = 0;
                lara.gun_status = LG_HANDS_BUSY;
            } else
                lara.GeneralPtr = (void *)item_num;
        } else if (lara.IsMoving && lara.GeneralPtr == (void *)item_num) {
            lara.IsMoving = 0;
            lara.gun_status = LG_NO_ARMS;
        }
        l->pos.y_rot ^= 0x8000;
    } else if (item->status == ITEM_ACTIVE)
        ObjectCollision(item_num, l, coll);
}
