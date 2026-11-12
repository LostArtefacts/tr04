#include <stdlib.h>

#include "scorpion.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "deltapak.h"
#include "effects.h"
#include "gameflow.h"
#include "lara.h"
#include "objnums.h"
#include "room.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseScorpion(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (item->trigger_flags == 1) {
        item->anim_number = objects[SCORPION].anim_index + 7;
        item->current_anim_state = item->goal_anim_state = 8;
    } else {
        item->anim_number = objects[SCORPION].anim_index + 2;
        item->current_anim_state = item->goal_anim_state = 1;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}

void ScorpionControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *enemy;
    FLOOR_INFO *floor;
    CREATURE_INFO *scorpion;
    CREATURE_INFO *cslot;
    AI_INFO info;
    sint32 s, c, x, y, z, h, h2, dist, bestdist, i;
    sint16 angle, room_number, xrot, zrot, target_num, hp;

    if (!CreatureActive(item_number))
        return;
    angle = 0;
    item = &items[item_number];
    scorpion = (CREATURE_INFO *)item->data;
    room_number = item->room_number;
    s = (682 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
    c = (682 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
    y = item->pos.y_pos;
    x = item->pos.x_pos + s;
    z = item->pos.z_pos + c;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (abs(y - h) > 512)
        h = y;
    x = item->pos.x_pos - s;
    z = item->pos.z_pos - c;
    floor = GetFloor(x, y, z, &room_number);
    h2 = GetHeight(floor, x, y, z);
    if (abs(y - h2) > 512)
        h2 = y;
    xrot = (sint16)phd_atan(1364, h2 - h);
    x = item->pos.x_pos - c;
    z = item->pos.z_pos + s;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (abs(y - h) > 512)
        h = y;
    x = item->pos.x_pos + c;
    z = item->pos.z_pos - s;
    floor = GetFloor(x, y, z, &room_number);
    h2 = GetHeight(floor, x, y, z);
    if (abs(y - h2) > 512)
        h2 = y;
    zrot = (sint16)phd_atan(1364, h2 - h);
    if (item->hit_points <= 0) {
        item->hit_points = 0;
        if (item->current_anim_state != 6 && item->trigger_flags > 0 && item->trigger_flags < 7) {
            cutseq_num = 4;
            item->anim_number = objects[SCORPION].anim_index + 5;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 6;
            item->status = ITEM_INVISIBLE;
            scorpion->maximum_turn = 0;
            for (target_num = room[item->room_number].item_number; target_num != NO_ITEM; target_num = enemy->next_item) {
                enemy = &items[target_num];
                if (enemy->object_number == TROOPS && enemy->trigger_flags == 1) {
                    DisableBaddieAI(target_num);
                    KillItem(target_num);
                    enemy->flags |= IFL_CLEARBODY;
                    break;
                }
            }
        } else if (item->current_anim_state != 6 && item->current_anim_state != 7) {
            item->anim_number = objects[SCORPION].anim_index + 5;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 6;
        } else if (cutseq_num == 4) {
            item->frame_number = anims[item->anim_number].frame_end - 1;
            item->status = ITEM_INVISIBLE;
        } else if (item->current_anim_state == 6 && item->status == ITEM_INVISIBLE) {
            item->status = ITEM_ACTIVE;
        }
    } else {
        if (item->ai_bits) {
            GetAITarget(scorpion);
        } else if (scorpion->hurt_by_lara && item->current_anim_state != 8) {
            scorpion->enemy = lara_item;
        } else {
            scorpion->enemy = 0;
            bestdist = 0x7FFFFFFF;
            cslot = baddie_slots;
            for (i = 5; i > 0; i--, cslot++) {
                target_num = cslot->item_num;
                if (target_num != NO_ITEM && target_num != item_number) {
                    enemy = &items[target_num];
                    if (enemy->object_number != LARA && enemy->object_number != SCORPION && (enemy != lara_item || scorpion->hurt_by_lara)) {
                        s = enemy->pos.x_pos - item->pos.x_pos;
                        c = enemy->pos.z_pos - item->pos.z_pos;
                        dist = SQUARE(s) + SQUARE(c);
                        if (dist < bestdist) {
                            scorpion->enemy = enemy;
                            bestdist = dist;
                        }
                    }
                }
            }
        }
        CreatureAIInfo(item, &info);
        enemy = scorpion->enemy;
        if (enemy != lara_item)
            phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);
        GetCreatureMood(item, &info, 1);
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, scorpion->maximum_turn);
        switch (item->current_anim_state) {
        case 1:
            scorpion->maximum_turn = 0;
            scorpion->flags = 0;
            if (info.distance <= 0x1C6E39 && info.bite) {
                scorpion->maximum_turn = 364;
                if (GetRandomControl() & 1 || enemy->object_number == TROOPS && enemy->hit_points <= 15)
                    item->goal_anim_state = 4;
                else
                    item->goal_anim_state = 5;
            } else if (info.distance > 0x1C6E39 || !info.ahead) {
                item->goal_anim_state = 2;
            }
            break;
        case 2:
            scorpion->maximum_turn = 364;
            if (info.distance < 0x1C6E39)
                item->goal_anim_state = 1;
            else if (info.distance > 0xB1A39)
                item->goal_anim_state = 3;
            break;
        case 3:
            scorpion->maximum_turn = 546;
            if (info.distance < 0x1C6E39)
                item->goal_anim_state = 1;
            break;
        case 8:
            scorpion->maximum_turn = 0;
            if (item->frame_number == anims[item->anim_number].frame_end)
                item->trigger_flags++;
            if (enemy && enemy->hit_points <= 0 || item->trigger_flags > 6) {
                item->goal_anim_state = 7;
                enemy->hit_points = 0;
            }
            break;
        case 4:
        case 5:
            hp = lara_item->hit_points;
            scorpion->maximum_turn = 0;
            if (abs(info.angle) < 364)
                item->pos.y_rot += info.angle;
            else if (info.angle < 0)
                item->pos.y_rot -= 364;
            else
                item->pos.y_rot += 364;
            if (!scorpion->flags && enemy && enemy != lara_item && info.distance < 0x1C6E39) {
                enemy->hit_points -= 15;
                if (enemy->hit_points <= 0) {
                    item->goal_anim_state = 7;
                    scorpion->maximum_turn = 0;
                }
                enemy->hit_status = 1;
                scorpion->flags = 1;
                CreatureEffectT(item, &stinger, 10, item->pos.y_rot + 0x8000, DoBloodSplat);
            } else if (!scorpion->flags && item->touch_bits & 0x1B00100) {
                lara_item->hit_points -= 120;
                lara_item->hit_status = 1;
                if (item->current_anim_state == 5) {
                    lara.dpoisoned += 2048;
                    CreatureEffectT(item, &stinger, 10, item->pos.y_rot + 0x8000, DoBloodSplat);
                } else {
                    CreatureEffectT(item, &pincer, 10, item->pos.y_rot + 0x8000, DoBloodSplat);
                }
                scorpion->flags = 1;
                if (hp && lara_item->hit_points <= 0) {
                    CreatureKill(item, 6, 7, 442);
                    scorpion->maximum_turn = 0;
                    return;
                }
            }
            break;
        }
    }
    if (abs(xrot - item->pos.x_rot) < 256)
        item->pos.x_rot = xrot;
    else if (xrot > item->pos.x_rot)
        item->pos.x_rot += 256;
    else if (xrot < item->pos.x_rot)
        item->pos.x_rot -= 256;
    if (abs(zrot - item->pos.z_rot) < 256)
        item->pos.z_rot = zrot;
    else if (zrot > item->pos.z_rot)
        item->pos.z_rot += 256;
    else if (zrot < item->pos.z_rot)
        item->pos.z_rot -= 256;
    if (!cutseq_num)
        CreatureAnimation(item_number, angle, 0);
}

void InitialiseSmlscorp(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[SMALL_SCORPION].anim_index + 2;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
