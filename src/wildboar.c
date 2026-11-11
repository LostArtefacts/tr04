#include <stdlib.h>

#include "wildboar.h"
#include "anim.h"
#include "effects.h"
#include "lara.h"
#include "specific/function_stubs.h"

void InitialiseWildboar(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[WILD_BOAR].anim_index + 6;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}

void WildboarControl(sint16 item_number)
{
    ITEM_INFO *item, *target;
    CREATURE_INFO *boar, *baddie;
    AI_INFO info;
    sint32 dx, dz, ldist, dist, max_dist;
    sint16 angle, neckX, neckY, headX, headY;
    int i;

    if (!CreatureActive(item_number))
        return;
    angle = 0;
    neckX = 0;
    neckY = 0;
    headX = 0;
    headY = 0;
    item = &items[item_number];
    boar = (CREATURE_INFO *)item->data;
    if (item->hit_points <= 0) {
        item->hit_points = 0;
        if (item->current_anim_state != 5) {
            item->anim_number = objects[WILD_BOAR].anim_index + 5;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 5;
        }
    } else {
        dz = lara_item->pos.z_pos - item->pos.z_pos;
        dx = lara_item->pos.x_pos - item->pos.x_pos;
        ldist = dx * dx + dz * dz;
        if (item->ai_bits) {
            GetAITarget(boar);
        } else {
            boar->enemy = lara_item;
            max_dist = 0x7FFFFFFF;
            baddie = baddie_slots;
            for (i = 0; i < 5; i++, baddie++) {
                if (baddie->item_num != NO_ITEM && baddie->item_num != item_number) {
                    target = &items[baddie->item_num];
                    if (target->object_number != WILD_BOAR) {
                        dx = target->pos.x_pos - item->pos.x_pos;
                        dz = target->pos.z_pos - item->pos.z_pos;
                        dist = dx * dx + dz * dz;
                        if (dist < max_dist && dist < ldist) {
                            boar->enemy = target;
                            max_dist = dist;
                        }
                    }
                }
            }
        }
        CreatureAIInfo(item, &info);
        GetCreatureMood(item, &info, 1);
        if (item->flags)
            boar->mood = ESCAPE_MOOD;
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, boar->maximum_turn);
        dx = abs(item->item_flags[2] - (sint16)item->pos.x_pos);
        dz = abs(item->item_flags[3] - (sint16)item->pos.z_pos);
        if (info.ahead)
            headY = neckY = info.angle >> 1;
        switch (item->current_anim_state) {
        case 1:
            boar->maximum_turn = 0;
            if (info.ahead && info.distance || item->flags) {
                item->goal_anim_state = 2;
            } else if (GetRandomControl() & 0x7F) {
                headY = neckY = AIGuard(boar) >> 1;
            } else {
                item->goal_anim_state = 3;
            }
            break;
        case 3:
            boar->maximum_turn = 0;
            if (info.ahead && info.distance)
                item->goal_anim_state = 1;
            else if (!(GetRandomControl() & 0x7F))
                item->goal_anim_state = 1;
            break;
        case 2:
            if (info.distance < 0x400000) {
                boar->maximum_turn = 546;
                headX = neckX = -info.distance;
            } else {
                boar->maximum_turn = 1092;
                item->flags = 0;
            }
            if (!item->flags && (dx < 50 && dz < 50 || info.distance < 0x10000 && info.bite)) {
                item->goal_anim_state = 4;
                if (boar->enemy == lara_item) {
                    lara_item->hit_points -= 30;
                    lara_item->hit_status = 1;
                }
                CreatureEffectT(item, &wildboar_bite, 3, item->pos.y_rot, DoBloodSplat);
                item->flags = 1;
            }
            break;
        case 4:
            boar->maximum_turn = 0;
            break;
        }
    }
    CreatureJoint(item, 0, neckX);
    CreatureJoint(item, 1, neckY);
    CreatureJoint(item, 2, headX);
    CreatureJoint(item, 3, headY);
    CreatureAnimation(item_number, angle, 0);
}
