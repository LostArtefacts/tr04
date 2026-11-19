#include "hammerhead.h"
#include "anim.h"
#include "control.h"
#include "effects.h"
#include "lara.h"
#include "specific/3dmath.h"

void InitialiseHammerhead(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[HAMMERHEAD].anim_index + 8;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 0;
}

void HammerheadControl(sint16 item_number)
{
    ITEM_INFO *item;
    CREATURE_INFO *shark;
    AI_INFO info, larainfo;
    sint32 dx, dz;
    sint16 angle, joint;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    shark = (CREATURE_INFO *)item->data;
    if (item->hit_points <= 0) {
        item->hit_points = 0;
        if (item->current_anim_state != 5) {
            item->anim_number = objects[HAMMERHEAD].anim_index + 4;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 5;
        }
        CreatureFloat(item_number);
    } else {
        if (item->ai_bits)
            GetAITarget(shark);
        else if (shark->hurt_by_lara)
            shark->enemy = lara_item;
        CreatureAIInfo(item, &info);
        if (shark->enemy == lara_item) {
            larainfo.angle = info.angle;
            larainfo.ahead = info.ahead;
            larainfo.distance = info.distance;
        } else {
            dz = lara_item->pos.z_pos - item->pos.z_pos;
            dx = lara_item->pos.x_pos - item->pos.x_pos;
            larainfo.angle = (sint16)(phd_atan(dz, dx) - item->pos.y_rot);
            if (larainfo.angle > -0x4000 && larainfo.angle < 0x4000)
                larainfo.ahead = 1;
            else
                larainfo.ahead = 0;
            larainfo.distance = SQUARE(dx) + SQUARE(dz);
        }
        GetCreatureMood(item, &info, 1);
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, shark->maximum_turn);
        joint = angle << 1;
        switch (item->current_anim_state) {
        case 0:
            item->goal_anim_state = 1;
            shark->flags = 0;
            break;
        case 1:
            shark->maximum_turn = 1274;
            if (info.distance > 0x100000)
                item->goal_anim_state = 2;
            else if (info.distance < 0x718E4)
                item->goal_anim_state = 3;
            break;
        case 2:
            if (info.distance < 0x100000)
                item->goal_anim_state = 1;
            break;
        case 3:
            if (!shark->flags && item->touch_bits & 0x3400) {
                lara_item->hit_points -= 120;
                lara_item->hit_status = 1;
                CreatureEffect(item, &hammerhead_bite, DoBloodSplat);
                shark->flags = 1;
            }
            break;
        }
        CreatureTilt(item, 0);
        CreatureJoint(item, 0, -joint);
        CreatureJoint(item, 1, -joint);
        CreatureJoint(item, 2, -joint);
        CreatureJoint(item, 3, joint);
        if (item->current_anim_state != 6) {
            CreatureAnimation(item_number, angle, 0);
            CreatureUnderwater(item, 341);
        } else
            AnimateItem(item);
    }
}
