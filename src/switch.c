#include "switch.h"
#include "items.h"
#include "control.h"
#include "objnums.h"
#include "anim.h"
#include "camera.h"
#include "specific/3dmath.h"
#include "lara.h"
#include "lara_states.h"
#include "specific/input.h"

void TestTriggersAtXYZ(sint32 x, sint32 y, sint32 z, sint16 room_number, sint16 heavy, sint16 flags)
{
    FLOOR_INFO *floor;

    floor = GetFloor(x, y, z, &room_number);
    GetHeight(floor, x, y, z);
    TestTriggers(trigger_index, heavy, flags);
}

void SwitchControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->flags |= IFL_CODEBITS;
    if (!TriggerActive(item) && !(item->flags & IFL_INVISIBLE)) {
        if (item->object_number == JUMP_SWITCH)
            item->goal_anim_state = 0;
        else
            item->goal_anim_state = 1;
        item->timer = 0;
    }
    AnimateItem(item);
}

void SwitchCollision2(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!(input & IN_ACTION) || item->status != ITEM_INACTIVE || lara.water_status != LW_UNDERWATER || lara.gun_status != LG_NO_ARMS)
        return;
    if (l->current_anim_state != AS_TREAD)
        return;
    if (!TestLaraPosition(Switch2Bounds, item, l))
        return;
    if (item->current_anim_state == 1 || item->current_anim_state == 0) {
        if (MoveLaraPosition(&Switch2Pos, item, l)) {
            l->fallspeed = 0;
            l->goal_anim_state = AS_SWITCHON;
            do {
                AnimateLara(l);
            } while (l->current_anim_state != AS_SWITCHON);
            l->goal_anim_state = AS_TREAD;
            lara.gun_status = LG_HANDS_BUSY;
            if (item->current_anim_state == 1)
                item->goal_anim_state = 0;
            else
                item->goal_anim_state = 1;
            item->status = ITEM_ACTIVE;
            AddActiveItem(item_number);
            AnimateItem(item);
        }
    }
}

void UnderwaterSwitchCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 flag;

    item = &items[item_number];
    if (((input & IN_ACTION) && lara.water_status == LW_UNDERWATER && l->current_anim_state == AS_TREAD && l->anim_number == ANIM_TREAD && lara.gun_status == LG_NO_ARMS && item->current_anim_state == 0) || (lara.IsMoving && lara.GeneralPtr == (void *)item_number)) {
        flag = 0;
        if (TestLaraPosition(UnderwaterSwitchBounds, item, l)) {
            if (!MoveLaraPosition(&UnderwaterSwitchPos, item, l))
                lara.GeneralPtr = (void *)item_number;
            else
                flag = 1;
        } else {
            l->pos.y_rot ^= 0x8000;
            if (TestLaraPosition(UnderwaterSwitchBounds2, item, l)) {
                if (MoveLaraPosition(&UnderwaterSwitchPos2, item, l))
                    flag = 1;
                else
                    lara.GeneralPtr = (void *)item_number;
            }
            l->pos.y_rot ^= 0x8000;
        }
        if (flag) {
            l->current_anim_state = AS_SWITCHON;
            l->anim_number = ANIM_WATERSWITCH;
            l->frame_number = anims[ANIM_WATERSWITCH].frame_base;
            l->fallspeed = 0;
            lara.IsMoving = 0;
            lara.gun_status = LG_HANDS_BUSY;
            item->goal_anim_state = 1;
            item->status = ITEM_ACTIVE;
            AddActiveItem(item_number);
            ForcedFixedCamera.x = item->pos.x_pos - ((1024 * phd_sin(item->pos.y_rot + 16380)) >> W2V_SHIFT);
            ForcedFixedCamera.y = item->pos.y_pos - 1024;
            ForcedFixedCamera.z = item->pos.z_pos - ((1024 * phd_cos(item->pos.y_rot + 16380)) >> W2V_SHIFT);
            ForcedFixedCamera.room_number = item->room_number;
        }
    }
}

void JumpSwitchCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if ((input & IN_ACTION) && lara.gun_status == LG_NO_ARMS && (l->current_anim_state == AS_REACH || l->current_anim_state == AS_UPJUMP) && l->gravity_status && l->fallspeed > 0 && !item->current_anim_state) {
        if (TestLaraPosition(JumpSwitchBounds, item, l)) {
            AlignLaraPosition(&JumpSwitchPos, item, l);
            l->current_anim_state = AS_SWITCHON;
            l->anim_number = ANIM_LEAPSWITCH;
            l->frame_number = anims[ANIM_LEAPSWITCH].frame_base;
            l->fallspeed = 0;
            l->gravity_status = 0;
            lara.gun_status = LG_HANDS_BUSY;
            item->goal_anim_state = 1;
            item->status = ITEM_ACTIVE;
            AddActiveItem(item_number);
        }
    }
}

void FullBlockSwitchControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->anim_number == objects[item->object_number].anim_index + 2 && CurrentSequence < 3 && !item->item_flags[0]) {
        item->item_flags[0] = 1;
        Sequences[CurrentSequence] = (uint8)item->trigger_flags;
        CurrentSequence++;
        if (CurrentSequence == 3 && SequenceUsed[SequenceResults[Sequences[0]][Sequences[1]][Sequences[2]]])
            CurrentSequence++;
    } else if (CurrentSequence >= 4) {
        item->item_flags[0] = 0;
        item->goal_anim_state = 1;
        item->status = ITEM_INACTIVE;
        CurrentSequence++;
        if (CurrentSequence >= 7)
            CurrentSequence = 0;
    }
    AnimateItem(item);
}
