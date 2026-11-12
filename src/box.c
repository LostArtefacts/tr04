#include "box.h"
#include "room.h"
#include "lara.h"
#include "control.h"
#include "draw.h"
#include "objnums.h"
#include "anim.h"
#include "sphere.h"
#include "camera.h"
#include "lot.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "effect2.h"
#include "lara_states.h"

#define ESCAPE_DIST (WALL_L * 5)
#define MAX_TILT (ONE_DEGREE * 3)
#define MAX_JOINT_CHANGE (ONE_DEGREE * 3)
#define JOINT_ARC 0x3000

void InitialiseCreature(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->collidable = 1;
    item->data = 0;
}

void CreatureAIInfo(ITEM_INFO *item, AI_INFO *info)
{
    CREATURE_INFO *creature;
    OBJECT_INFO *obj;
    ITEM_INFO *enemy;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    sint16 *zone;
    sint32 x, y, z;
    sint16 ang, state;

    creature = (CREATURE_INFO *)item->data;
    if (!creature)
        return;
    if (item->poisoned) {
        if (!objects[item->object_number].undead && !(wibble & 0x3f) && item->hit_points > 1)
            item->hit_points--;
    }
    enemy = creature->enemy;
    if (!enemy) {
        enemy = lara_item;
        creature->enemy = lara_item;
    }
    zone = ground_zone[creature->LOT.zone][flip_status];
    r = &room[item->room_number];
    floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + ((item->pos.x_pos - r->x) >> WALL_SHIFT) * r->x_size];
    item->box_number = floor->box;
    info->zone_number = zone[item->box_number];
    r = &room[enemy->room_number];
    floor = &r->floor[((enemy->pos.z_pos - r->z) >> WALL_SHIFT) + ((enemy->pos.x_pos - r->x) >> WALL_SHIFT) * r->x_size];
    enemy->box_number = floor->box;
    info->enemy_zone = zone[enemy->box_number];
    if ((boxes[enemy->box_number].overlap_index & creature->LOT.block_mask) || creature->LOT.node[item->box_number].search_number == (creature->LOT.search_number | 0x8000))
        info->enemy_zone |= BLOCKED;
    obj = &objects[item->object_number];
    if (enemy == lara_item)
        ang = lara.move_angle;
    else
        ang = enemy->pos.y_rot;
    z = enemy->pos.z_pos + (14 * enemy->speed * phd_cos(ang) >> W2V_SHIFT) - (obj->pivot_length * phd_cos(item->pos.y_rot) >> W2V_SHIFT) - item->pos.z_pos;
    x = enemy->pos.x_pos + (14 * enemy->speed * phd_sin(ang) >> W2V_SHIFT) - (obj->pivot_length * phd_sin(item->pos.y_rot) >> W2V_SHIFT) - item->pos.x_pos;
    y = item->pos.y_pos - enemy->pos.y_pos;
    ang = phd_atan(z, x);
    if (z > 32000 || z < -32000 || x > 32000 || x < -32000)
        info->distance = 0x7fffffff;
    else if (creature->enemy)
        info->distance = SQUARE(x) + SQUARE(z);
    else
        info->distance = 0x7fffffff;
    info->angle = ang - item->pos.y_rot;
    info->enemy_facing = ang - enemy->pos.y_rot + 0x8000;
    z = abs(z);
    x = abs(x);
    if (enemy == lara_item) {
        state = lara_item->current_anim_state;
        if (state == AS_DUCK || state == AS_DUCKROLL || state == AS_ALL4S || state == AS_CRAWL || state == AS_ALL4TURNL || state == AS_ALL4TURNR)
            y -= 384;
    }
    if (x > z)
        info->x_angle = phd_atan(x + (z >> 1), y);
    else
        info->x_angle = phd_atan(z + (x >> 1), y);
    info->ahead = (info->angle > -0x4000 && info->angle < 0x4000);
    info->bite = (info->ahead && enemy->hit_points > 0 && ABS(enemy->pos.y_pos - item->pos.y_pos) <= 512);
}

sint32 UpdateLOT(LOT_INFO *LOT, sint32 expansion)
{
    BOX_NODE *expand;

    if (LOT->required_box != NO_BOX && LOT->required_box != LOT->target_box) {
        LOT->target_box = LOT->required_box;
        expand = &LOT->node[LOT->required_box];
        if (expand->next_expansion == NO_BOX && LOT->tail != LOT->required_box) {
            expand->next_expansion = LOT->head;
            if (LOT->head == NO_BOX)
                LOT->tail = LOT->target_box;
            LOT->head = LOT->target_box;
        }
        LOT->search_number++;
        expand->search_number = LOT->search_number;
        expand->exit_box = NO_BOX;
    }
    return SearchLOT(LOT, expansion);
}

sint32 SearchLOT(LOT_INFO *LOT, sint32 expansion)
{
    sint32 i, index, done, change, box_number, overlap_flags;
    BOX_NODE *expand, *node;
    BOX_INFO *box;
    sint16 *zone, search_zone;

    zone = ground_zone[LOT->zone][flip_status];
    search_zone = zone[LOT->head];
    for (i = 0; i < expansion; i++) {
        if (LOT->head == NO_BOX) {
            LOT->tail = NO_BOX;
            return 0;
        }
        node = &LOT->node[LOT->head];
        box = &boxes[LOT->head];
        index = box->overlap_index & 0x3fff;
        done = 0;
        do {
            box_number = (uint16)overlap[index++];
            overlap_flags = box_number & ~BOX_NUMBER;
            if (box_number & 0x8000)
                done = 1;
            box_number &= BOX_NUMBER;
            if (!LOT->fly && search_zone != zone[box_number])
                continue;
            change = boxes[box_number].height - box->height;
            if ((change > LOT->step || change < LOT->drop) && (!(overlap_flags & 0x2000) || !LOT->can_monkey))
                continue;
            if ((overlap_flags & 0x800) && !LOT->can_jump)
                continue;
            expand = &LOT->node[box_number];
            if ((node->search_number & 0x7fff) < (expand->search_number & 0x7fff))
                continue;
            if (node->search_number & 0x8000) {
                if ((node->search_number & 0x7fff) == (expand->search_number & 0x7fff))
                    continue;
                expand->search_number = node->search_number;
            } else {
                if ((node->search_number & 0x7fff) == (expand->search_number & 0x7fff) && !(expand->search_number & 0x8000))
                    continue;
                if (boxes[box_number].overlap_index & LOT->block_mask)
                    expand->search_number = node->search_number | 0x8000;
                else {
                    expand->search_number = node->search_number;
                    expand->exit_box = LOT->head;
                }
            }
            if (expand->next_expansion == NO_BOX && box_number != LOT->tail) {
                LOT->node[LOT->tail].next_expansion = (sint16)box_number;
                LOT->tail = (sint16)box_number;
            }
        } while (!done);
        LOT->head = node->next_expansion;
        node->next_expansion = NO_BOX;
    }
    return 1;
}

void TargetBox(LOT_INFO *LOT, sint16 box_number)
{
    BOX_INFO *box;

    box_number &= BOX_NUMBER;
    box = &boxes[box_number];
    LOT->target.z = ((uint32)box->left << WALL_SHIFT) + GetRandomControl() * (((uint32)box->right - (uint32)box->left - 1) >> (15 - WALL_SHIFT)) + WALL_L / 2;
    LOT->target.x = ((uint32)box->top << WALL_SHIFT) + GetRandomControl() * (((uint32)box->bottom - (uint32)box->top - 1) >> (15 - WALL_SHIFT)) + WALL_L / 2;
    LOT->required_box = box_number;
    if (LOT->fly == NO_FLYING)
        LOT->target.y = box->height;
    else
        LOT->target.y = box->height - STEP_L * 3 / 2;
}

sint32 EscapeBox(ITEM_INFO *item, ITEM_INFO *enemy, sint16 box_number)
{
    BOX_INFO *box;
    sint32 x, z;

    box = &boxes[box_number];
    z = (((uint32)box->left + (uint32)box->right) << (WALL_SHIFT - 1)) - enemy->pos.z_pos;
    x = (((uint32)box->top + (uint32)box->bottom) << (WALL_SHIFT - 1)) - enemy->pos.x_pos;
    if (x > -ESCAPE_DIST && x < ESCAPE_DIST && z > -ESCAPE_DIST && z < ESCAPE_DIST)
        return 0;
    if (((z > 0) ^ (item->pos.z_pos > enemy->pos.z_pos)) && ((x > 0) ^ (item->pos.x_pos > enemy->pos.x_pos)))
        return 0;
    return 1;
}

sint32 ValidBox(ITEM_INFO *item, sint16 zone_number, sint16 box_number)
{
    CREATURE_INFO *creature;
    BOX_INFO *box;

    creature = (CREATURE_INFO *)item->data;
    if (!creature->LOT.fly && ground_zone[creature->LOT.zone][flip_status][box_number] != zone_number)
        return 0;
    box = &boxes[box_number];
    if (creature->LOT.block_mask & box->overlap_index)
        return 0;
    if (item->pos.z_pos > box->left << 10 && item->pos.z_pos < box->right << 10 &&
        item->pos.x_pos > box->top << 10 && item->pos.x_pos < box->bottom << 10)
        return 0;
    return 1;
}

void GetCreatureMood(ITEM_INFO *item, AI_INFO *info, sint32 violent)
{
    CREATURE_INFO *creature;
    MOOD_TYPE mood;
    LOT_INFO *LOT;
    ITEM_INFO *enemy;

    if (item->data == NULL)
        return;
    creature = (CREATURE_INFO *)item->data;
    enemy = creature->enemy;
    LOT = &creature->LOT;
    if (LOT->node[item->box_number].search_number == (LOT->search_number | 0x8000))
        LOT->required_box = NO_BOX;
    if (creature->mood != ATTACK_MOOD && LOT->required_box != NO_BOX) {
        if (!ValidBox(item, info->zone_number, LOT->target_box)) {
            if (info->zone_number == info->enemy_zone)
                creature->mood = BORED_MOOD;
            LOT->required_box = NO_BOX;
        }
    }
    mood = creature->mood;
    if (!enemy) {
        creature->mood = BORED_MOOD;
    } else if (enemy->hit_points <= 0 && enemy == lara_item) {
        creature->mood = BORED_MOOD;
    } else if (violent) {
        switch (creature->mood) {
        case ATTACK_MOOD:
            if (info->zone_number != info->enemy_zone)
                creature->mood = BORED_MOOD;
            break;
        case BORED_MOOD:
        case STALK_MOOD:
            if (info->zone_number == info->enemy_zone)
                creature->mood = ATTACK_MOOD;
            else if (item->hit_status)
                creature->mood = ESCAPE_MOOD;
            break;
        case ESCAPE_MOOD:
            if (info->zone_number == info->enemy_zone)
                creature->mood = ATTACK_MOOD;
            break;
        }
    } else {
        switch (creature->mood) {
        case ATTACK_MOOD:
            if (item->hit_status && (GetRandomControl() < 0x800 || info->zone_number != info->enemy_zone))
                creature->mood = STALK_MOOD;
            else if (info->zone_number != info->enemy_zone && info->distance > WALL_L * 6)
                creature->mood = BORED_MOOD;
            break;
        case BORED_MOOD:
        case STALK_MOOD:
            if (creature->alerted && info->zone_number != info->enemy_zone) {
                if (info->distance > WALL_L * 3)
                    creature->mood = STALK_MOOD;
                else
                    creature->mood = BORED_MOOD;
            } else if (info->zone_number == info->enemy_zone) {
                if (info->distance < (WALL_L * 3) * (WALL_L * 3) || (mood == STALK_MOOD && LOT->required_box == NO_BOX))
                    creature->mood = ATTACK_MOOD;
                else
                    creature->mood = STALK_MOOD;
            }
            break;
        case ESCAPE_MOOD:
            if (info->zone_number == info->enemy_zone && GetRandomControl() < 0x100)
                creature->mood = STALK_MOOD;
            break;
        }
    }
    if (mood != creature->mood) {
        if (mood == ATTACK_MOOD)
            TargetBox(LOT, LOT->target_box);
        LOT->required_box = NO_BOX;
    }
}

void CreatureMood(ITEM_INFO *item, AI_INFO *info, sint32 violent)
{
    static sint16 type;
    CREATURE_INFO *creature;
    LOT_INFO *LOT;
    ITEM_INFO *enemy;
    sint16 index, box_number;

    if (item->data == NULL)
        return;
    creature = (CREATURE_INFO *)item->data;
    enemy = creature->enemy;
    LOT = &creature->LOT;
    switch (creature->mood) {
    case ATTACK_MOOD:
        LOT->target.x = enemy->pos.x_pos;
        LOT->target.y = enemy->pos.y_pos;
        LOT->target.z = enemy->pos.z_pos;
        LOT->required_box = enemy->box_number;
        if (LOT->fly != NO_FLYING && lara.water_status == LW_ABOVE_WATER)
            LOT->target.y += GetBestFrame(enemy)[2];
        break;
    case BORED_MOOD:
        box_number = LOT->node[GetRandomControl() * LOT->zone_count >> 15].box_number;
        if (ValidBox(item, info->zone_number, box_number)) {
            if (StalkBox(item, enemy, box_number) && enemy->hit_points > 0 && creature->enemy) {
                TargetBox(LOT, box_number);
                creature->mood = BORED_MOOD;
            } else if (LOT->required_box == NO_BOX) {
                TargetBox(LOT, box_number);
            }
        }
        break;
    case STALK_MOOD:
        if (LOT->required_box == NO_BOX || !StalkBox(item, enemy, LOT->required_box)) {
            box_number = LOT->node[GetRandomControl() * LOT->zone_count >> 15].box_number;
            if (ValidBox(item, info->zone_number, box_number)) {
                if (StalkBox(item, enemy, box_number)) {
                    TargetBox(LOT, box_number);
                } else if (LOT->required_box == NO_BOX) {
                    TargetBox(LOT, box_number);
                    if (info->zone_number != info->enemy_zone)
                        creature->mood = BORED_MOOD;
                }
            }
        }
        break;
    case ESCAPE_MOOD:
        box_number = LOT->node[GetRandomControl() * LOT->zone_count >> 15].box_number;
        if (ValidBox(item, info->zone_number, box_number) && LOT->required_box == NO_BOX) {
            if (EscapeBox(item, enemy, box_number)) {
                TargetBox(LOT, box_number);
            } else if (info->zone_number == info->enemy_zone && StalkBox(item, enemy, box_number) && !violent) {
                TargetBox(LOT, box_number);
                creature->mood = STALK_MOOD;
            }
        }
        break;
    }
    if (LOT->target_box == NO_BOX)
        TargetBox(LOT, item->box_number);
    type = CalculateTarget(&creature->target, item, &creature->LOT);
    creature->jump_ahead = 0;
    creature->monkey_ahead = 0;
    if (LOT->node[item->box_number].exit_box != NO_BOX) {
        index = boxes[item->box_number].overlap_index & 0x3FFF;
        do {
            box_number = overlap[index];
            index++;
        } while (box_number != NO_BOX && !(box_number & 0x8000) && (box_number & 0x7FF) != LOT->node[item->box_number].exit_box);
        if ((box_number & 0x7FF) == LOT->node[item->box_number].exit_box) {
            if (box_number & 0x800)
                creature->jump_ahead = 1;
            if (box_number & 0x2000)
                creature->monkey_ahead = 1;
        }
    }
}

sint32 CalculateTarget(PHD_VECTOR *target, ITEM_INFO *item, LOT_INFO *LOT)
{
    BOX_INFO *box;
    sint32 box_number, box_left, box_right, box_top, box_bottom;
    sint32 left, right, top, bottom, prime_free;
    static uint16 loops;

    UpdateLOT(LOT, 5);
    target->x = item->pos.x_pos;
    target->y = item->pos.y_pos;
    target->z = item->pos.z_pos;
    box_number = item->box_number;
    if (box_number == NO_BOX)
        return 0;
    box = &boxes[box_number];
    left = box->left << WALL_SHIFT;
    right = (box->right << WALL_SHIFT) - 1;
    top = box->top << WALL_SHIFT;
    bottom = (box->bottom << WALL_SHIFT) - 1;
    loops = 0;
    prime_free = 15;
    do {
        loops++;
        box = &boxes[box_number];
        if (LOT->fly == NO_FLYING) {
            if (target->y > box->height)
                target->y = box->height;
        } else if (target->y > box->height - WALL_L) {
            target->y = box->height - WALL_L;
        }
        box_left = box->left << WALL_SHIFT;
        box_right = (box->right << WALL_SHIFT) - 1;
        box_top = box->top << WALL_SHIFT;
        box_bottom = (box->bottom << WALL_SHIFT) - 1;
        if (item->pos.z_pos >= box_left && item->pos.z_pos <= box_right && item->pos.x_pos >= box_top && item->pos.x_pos <= box_bottom) {
            left = box->left << WALL_SHIFT;
            right = (box->right << WALL_SHIFT) - 1;
            top = box->top << WALL_SHIFT;
            bottom = (box->bottom << WALL_SHIFT) - 1;
        } else {
            if (item->pos.z_pos < box_left && prime_free != 2) {
                if ((prime_free & 1) && item->pos.x_pos >= box_top && item->pos.x_pos <= box_bottom) {
                    if (target->z < box_left + 512)
                        target->z = box_left + 512;
                    if (prime_free & 16)
                        return 2;
                    if (box_top > top)
                        top = box_top;
                    if (box_bottom < bottom)
                        bottom = box_bottom;
                    prime_free = 1;
                } else if (prime_free != 1) {
                    target->z = right - 512;
                    if (prime_free != 15)
                        return 2;
                    prime_free = 31;
                }
            } else if (item->pos.z_pos > box_right && prime_free != 1) {
                if ((prime_free & 2) && item->pos.x_pos >= box_top && item->pos.x_pos <= box_bottom) {
                    if (target->z > box_right - 512)
                        target->z = box_right - 512;
                    if (prime_free & 16)
                        return 2;
                    if (box_top > top)
                        top = box_top;
                    if (box_bottom < bottom)
                        bottom = box_bottom;
                    prime_free = 2;
                } else if (prime_free != 2) {
                    target->z = left + 512;
                    if (prime_free != 15)
                        return 2;
                    prime_free = 31;
                }
            }
            if (item->pos.x_pos < box_top && prime_free != 8) {
                if ((prime_free & 4) && item->pos.z_pos >= box_left && item->pos.z_pos <= box_right) {
                    if (target->x < box_top + 512)
                        target->x = box_top + 512;
                    if (prime_free & 16)
                        return 2;
                    if (box_left > left)
                        left = box_left;
                    if (box_right < right)
                        right = box_right;
                    prime_free = 4;
                } else if (prime_free != 4) {
                    target->x = bottom - 512;
                    if (prime_free != 15)
                        return 2;
                    prime_free = 31;
                }
            } else if (item->pos.x_pos > box_bottom && prime_free != 4) {
                if ((prime_free & 8) && item->pos.z_pos >= box_left && item->pos.z_pos <= box_right) {
                    if (target->x > box_bottom - 512)
                        target->x = box_bottom - 512;
                    if (prime_free & 16)
                        return 2;
                    if (box_left > left)
                        left = box_left;
                    if (box_right < right)
                        right = box_right;
                    prime_free = 8;
                } else if (prime_free != 8) {
                    target->x = top + 512;
                    if (prime_free != 15)
                        return 2;
                    prime_free = 31;
                }
            }
        }
        if (box_number == LOT->target_box) {
            if (prime_free & 3) {
                target->z = LOT->target.z;
            } else if (!(prime_free & 16)) {
                if (target->z < box_left + 512)
                    target->z = box_left + 512;
                else if (target->z > box_right - 512)
                    target->z = box_right - 512;
            }
            if (prime_free & 12) {
                target->x = LOT->target.x;
            } else if (!(prime_free & 16)) {
                if (target->x < box_top + 512)
                    target->x = box_top + 512;
                else if (target->x > box_bottom - 512)
                    target->x = box_bottom - 512;
            }
            target->y = LOT->target.y;
            return 1;
        }
        box_number = LOT->node[box_number].exit_box;
        if (box_number != NO_BOX && (boxes[box_number].overlap_index & LOT->block_mask))
            break;
    } while (box_number != NO_BOX);
    if (!(prime_free & 16)) {
        if (target->z < box_left + 512)
            target->z = box_left + 512;
        else if (target->z > box_right - 512)
            target->z = box_right - 512;
    }
    if (!(prime_free & 16)) {
        if (target->x < box_top + 512)
            target->x = box_top + 512;
        else if (target->x > box_bottom - 512)
            target->x = box_bottom - 512;
    }
    if (LOT->fly == NO_FLYING)
        target->y = box->height;
    else
        target->y = box->height - 320;
    return 0;
}

sint32 CreatureCreature(sint16 item_number)
{
    sint16 link, angle;
    sint32 x, z, y_rot, xdistance, zdistance, distance, radius;
    ROOM_INFO *r;
    ITEM_INFO *item;

    item = &items[item_number];
    x = item->pos.x_pos;
    z = item->pos.z_pos;
    y_rot = item->pos.y_rot;
    radius = objects[item->object_number].radius;
    r = &room[item->room_number];
    link = r->item_number;
    do {
        item = &items[link];
        if (link == item_number)
            return 0;
        if (item != lara_item && item->status == ITEM_ACTIVE && item->hit_points > 0) {
            xdistance = abs(item->pos.x_pos - x);
            zdistance = abs(item->pos.z_pos - z);
            if (xdistance > zdistance)
                distance = xdistance + (zdistance >> 1);
            else
                distance = zdistance + (xdistance >> 1);
            if (distance < radius + objects[item->object_number].radius) {
                angle = phd_atan(item->pos.z_pos - z, item->pos.x_pos - x) - y_rot;
                return angle;
            }
        }
        link = item->next_item;
    } while (link != NO_ITEM);
    return 0;
}

sint32 BadFloor(sint32 x, sint32 y, sint32 z, sint32 box_height, sint32 next_height, sint16 room_number, LOT_INFO *LOT)
{
    FLOOR_INFO *floor;
    BOX_INFO *box;

    floor = GetFloor(x, y, z, &room_number);
    if (floor->box == NO_BOX)
        return 1;
    if (LOT->is_jumping)
        return 0;
    box = &boxes[floor->box];
    if (box->overlap_index & LOT->block_mask)
        return 1;
    if (box_height - box->height > LOT->step || box_height - box->height < LOT->drop)
        return 1;
    if (box_height - box->height < -LOT->step && box->height > next_height)
        return 1;
    if (LOT->fly && y > box->height + LOT->fly)
        return 1;
    return 0;
}

void CreatureDie(sint16 item_number, sint32 explode)
{
    ITEM_INFO *item;
    ITEM_INFO *pickup;
    FLOOR_INFO *floor;
    sint16 pickup_number, room_number;

    item = &items[item_number];
    item->hit_points = -16384;
    item->collidable = 0;
    if (explode) {
        if (objects[item->object_number].HitEffect == 1)
            ExplodingDeath2(item_number, -1, 258);
        else
            ExplodingDeath2(item_number, -1, 256);
        KillItem(item_number);
    } else {
        RemoveActiveItem(item_number);
    }
    DisableBaddieAI(item_number);
    item->flags |= IFL_INVISIBLE | IFL_CLEARBODY;
    pickup_number = item->carried_item;
    while (pickup_number != NO_ITEM) {
        pickup = &items[pickup_number];
        if (item->object_number == TROOPS && item->trigger_flags == 1) {
            pickup->pos.x_pos = ((item->pos.x_pos + ((1024 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
            pickup->pos.z_pos = ((item->pos.z_pos + ((1024 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT)) & -512) | 512;
        } else {
            pickup->pos.x_pos = (item->pos.x_pos & -512) | 512;
            pickup->pos.z_pos = (item->pos.z_pos & -512) | 512;
        }
        room_number = item->room_number;
        floor = GetFloor(pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos, &room_number);
        pickup->pos.y_pos = GetHeight(floor, pickup->pos.x_pos, item->pos.y_pos, pickup->pos.z_pos);
        pickup->pos.y_pos -= GetBoundsAccurate(pickup)[3];
        ItemNewRoom(pickup_number, item->room_number);
        pickup->flags |= IFL_TRIGGERED;
        pickup_number = pickup->carried_item;
    }
}

sint16 CreatureTurn(ITEM_INFO *item, sint16 maximum_turn)
{
    CREATURE_INFO *creature;
    ROOM_INFO *r;
    sint32 x, z, feelxplus, feelzplus, feelxminus, feelzminus, feelxmid, feelzmid, feelplus, feelminus, feelmid;
    sint16 angle;

    creature = (CREATURE_INFO *)item->data;
    if (!creature)
        return 0;
    if (!maximum_turn)
        return 0;
    x = item->pos.x_pos;
    z = item->pos.z_pos;
    r = &room[item->room_number];
    feelxplus = x + (512 * phd_sin(item->pos.y_rot + 8190) >> W2V_SHIFT);
    feelzplus = z + (512 * phd_cos(item->pos.y_rot + 8190) >> W2V_SHIFT);
    feelplus = r->floor[((feelzplus - r->z) >> 10) + r->x_size * ((feelxplus - r->x) >> 10)].stopper;
    feelxminus = x + (512 * phd_sin(item->pos.y_rot - 8190) >> W2V_SHIFT);
    feelzminus = z + (512 * phd_cos(item->pos.y_rot - 8190) >> W2V_SHIFT);
    feelminus = r->floor[((feelzminus - r->z) >> 10) + r->x_size * ((feelxminus - r->x) >> 10)].stopper;
    feelxmid = x + (512 * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
    feelzmid = z + (512 * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
    feelmid = r->floor[((feelzmid - r->z) >> 10) + r->x_size * ((feelxmid - r->x) >> 10)].stopper;
    if (feelminus && feelmid) {
        creature->target.x = feelxplus;
        creature->target.z = feelzplus;
    } else if (feelplus && feelmid) {
        creature->target.x = feelxminus;
        creature->target.z = feelzminus;
    } else if (feelplus || feelminus) {
        creature->target.x = feelxmid;
        creature->target.z = feelzmid;
    }
    x = creature->target.x - item->pos.x_pos;
    z = creature->target.z - item->pos.z_pos;
    angle = (sint16)(phd_atan(z, x) - item->pos.y_rot);
    if (angle > 0x4000 || angle < -0x4000) {
        if (SQUARE(x) + SQUARE(z) < SQUARE((item->speed << 14) / maximum_turn))
            maximum_turn >>= 1;
    }
    if (angle > maximum_turn)
        angle = maximum_turn;
    else if (angle < -maximum_turn)
        angle = -maximum_turn;
    item->pos.y_rot += angle;
    return angle;
}

void CreatureTilt(ITEM_INFO *item, sint16 angle)
{
    angle = (angle << 2) - item->pos.z_rot;
    if (angle < -MAX_TILT)
        angle = -MAX_TILT;
    else if (angle > MAX_TILT)
        angle = MAX_TILT;
    item->pos.z_rot += angle;
}

void CreatureJoint(ITEM_INFO *item, sint16 joint, sint16 required)
{
    CREATURE_INFO *creature;
    sint16 change;

    creature = (CREATURE_INFO *)item->data;
    if (!creature)
        return;
    change = required - creature->joint_rotation[joint];
    if (change > MAX_JOINT_CHANGE)
        change = MAX_JOINT_CHANGE;
    else if (change < -MAX_JOINT_CHANGE)
        change = -MAX_JOINT_CHANGE;
    creature->joint_rotation[joint] += change;
    if (creature->joint_rotation[joint] > JOINT_ARC)
        creature->joint_rotation[joint] = JOINT_ARC;
    else if (creature->joint_rotation[joint] < -JOINT_ARC)
        creature->joint_rotation[joint] = -JOINT_ARC;
}

void CreatureUnderwater(ITEM_INFO *item, sint32 depth)
{
    sint32 water_level, floorheight;
    FLOOR_INFO *floor;
    sint16 room_number;

    water_level = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
    if (item->pos.y_pos < water_level + depth) {
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        floorheight = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        item->pos.y_pos = water_level + depth;
        if (item->pos.y_pos > floorheight)
            item->pos.y_pos = floorheight;
        if (item->pos.x_rot > 364)
            item->pos.x_rot -= 364;
        else if (item->pos.x_rot > 0)
            item->pos.x_rot = 0;
    }
}

sint16 CreatureEffect(ITEM_INFO *item, BITE_INFO *bite, sint16 (*generate)(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number))
{
    PHD_VECTOR pos;

    pos.x = bite->x;
    pos.y = bite->y;
    pos.z = bite->z;
    GetJointAbsPosition(item, &pos, bite->mesh_num);
    return generate(pos.x, pos.y, pos.z, item->speed, item->pos.y_rot, item->room_number);
}

sint16 CreatureEffectT(ITEM_INFO *item, BITE_INFO *bite, sint16 damage, sint16 angle, sint16 (*generate)(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number))
{
    PHD_VECTOR pos;

    pos.x = bite->x;
    pos.y = bite->y;
    pos.z = bite->z;
    GetJointAbsPosition(item, &pos, bite->mesh_num);
    return generate(pos.x, pos.y, pos.z, damage, angle, item->room_number);
}

sint32 CreatureVault(sint16 item_number, sint16 angle, sint32 vault, sint32 shift)
{
    ITEM_INFO *item;
    sint32 y, xx, yy, x_floor, y_floor;
    sint16 room_number;

    item = &items[item_number];
    xx = item->pos.z_pos >> WALL_SHIFT;
    yy = item->pos.x_pos >> WALL_SHIFT;
    y = item->pos.y_pos;
    room_number = item->room_number;
    CreatureAnimation(item_number, angle, 0);
    if (item->floor > y + 1152)
        vault = 0;
    else if (item->floor > y + 896)
        vault = -4;
    else if (item->floor > y + 640 && item->object_number == VON_CROY)
        vault = -3;
    else if (item->floor > y + 384 && item->object_number == VON_CROY)
        vault = -2;
    else if (item->pos.y_pos > y - 384)
        return 0;
    else if (item->pos.y_pos > y - 640)
        vault = 2;
    else if (item->pos.y_pos > y - 896)
        vault = 3;
    else if (item->pos.y_pos > y - 1152)
        vault = 4;
    x_floor = item->pos.z_pos >> WALL_SHIFT;
    y_floor = item->pos.x_pos >> WALL_SHIFT;
    if (xx == x_floor) {
        if (yy == y_floor)
            return 0;
        if (yy < y_floor) {
            item->pos.y_rot = 0x4000;
            item->pos.x_pos = (y_floor << WALL_SHIFT) - shift;
        } else {
            item->pos.y_rot = -0x4000;
            item->pos.x_pos = (yy << WALL_SHIFT) + shift;
        }
    } else if (yy == y_floor) {
        if (xx < x_floor) {
            item->pos.y_rot = 0;
            item->pos.z_pos = (x_floor << WALL_SHIFT) - shift;
        } else {
            item->pos.y_rot = -0x8000;
            item->pos.z_pos = (xx << WALL_SHIFT) + shift;
        }
    }
    item->pos.y_pos = item->floor = y;
    if (vault)
        ItemNewRoom(item_number, room_number);
    return vault;
}

void CreatureKill(ITEM_INFO *item, sint16 kill_anim, sint16 kill_state, sint16 lara_anim)
{
    item->anim_number = objects[item->object_number].anim_index + kill_anim;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = kill_state;
    lara_item->anim_number = lara_anim;
    lara_item->frame_number = anims[lara_item->anim_number].frame_base;
    lara_item->current_anim_state = 8;
    lara_item->goal_anim_state = 8;
    lara_item->pos.x_pos = item->pos.x_pos;
    lara_item->pos.y_pos = item->pos.y_pos;
    lara_item->pos.z_pos = item->pos.z_pos;
    lara_item->pos.y_rot = item->pos.y_rot;
    lara_item->pos.x_rot = item->pos.x_rot;
    lara_item->pos.z_rot = item->pos.z_rot;
    lara_item->fallspeed = 0;
    lara_item->gravity_status = 0;
    lara_item->speed = 0;
    if (item->room_number != lara_item->room_number)
        ItemNewRoom(lara.item_number, item->room_number);
    AnimateItem(lara_item);
    lara.gun_status = LG_HANDS_BUSY;
    lara.gun_type = WEAPON_NONE;
    lara.hit_direction = -1;
    lara.air = -1;
    camera.pos.room_number = lara_item->room_number;
    ForcedFixedCamera.x = item->pos.x_pos + ((2048 * phd_sin(item->pos.y_rot)) >> W2V_SHIFT);
    ForcedFixedCamera.y = item->pos.y_pos - 1024;
    ForcedFixedCamera.z = item->pos.z_pos + ((2048 * phd_cos(item->pos.y_rot)) >> W2V_SHIFT);
    ForcedFixedCamera.room_number = item->room_number;
    UseForcedFixedCamera = 1;
}

void AlertAllGuards(sint16 item_number)
{
    ITEM_INFO *target;
    CREATURE_INFO *cinfo;
    sint32 slot;
    sint16 obj_number;

    cinfo = baddie_slots;
    obj_number = items[item_number].object_number;
    for (slot = 0; slot < 5; slot++, cinfo++) {
        if (cinfo->item_num == NO_ITEM)
            continue;
        target = &items[cinfo->item_num];
        if (obj_number == target->object_number && target->status == ITEM_ACTIVE)
            cinfo->alerted = 1;
    }
}

void AlertNearbyGuards(ITEM_INFO *item)
{
    sint32 slot;
    CREATURE_INFO *cinfo;
    ITEM_INFO *target;
    sint32 x, y, z, distance;

    cinfo = baddie_slots;
    for (slot = 0; slot < NUM_SLOTS; slot++, cinfo++) {
        if (cinfo->item_num == NO_ITEM)
            continue;
        target = &items[cinfo->item_num];
        if (target->room_number == item->room_number) {
            cinfo->alerted = 1;
            continue;
        }
        x = (target->pos.x_pos - item->pos.x_pos) >> 6;
        y = (target->pos.y_pos - item->pos.y_pos) >> 6;
        z = (target->pos.z_pos - item->pos.z_pos) >> 6;
        distance = x * x + y * y + z * z;
        if (distance < 8000)
            cinfo->alerted = 1;
    }
}

sint16 AIGuard(CREATURE_INFO *creature)
{
    sint32 random;

    if (items[creature->item_num].ai_bits & MODIFY)
        return 0;
    random = GetRandomControl();
    if (random < 0x100) {
        creature->head_left = 1;
        creature->head_right = 1;
    } else if (random < 0x180) {
        creature->head_left = 1;
        creature->head_right = 0;
    } else if (random < 0x200) {
        creature->head_left = 0;
        creature->head_right = 1;
    }
    if (creature->head_left && creature->head_right)
        return 0;
    else if (creature->head_left)
        return -0x4000;
    else if (creature->head_right)
        return 0x4000;
    return 0;
}

void FindAITargetObject(CREATURE_INFO *creature, sint16 obj_num)
{
    ITEM_INFO *item;
    ITEM_INFO *enemy;
    AIOBJECT *aiObj;
    ROOM_INFO *r;
    sint16 *zone;
    sint16 zone_number, ai_zone;
    sint16 i;

    item = &items[creature->item_num];
    aiObj = AIObjects;
    for (i = 0; i < nAIObjects; i++, aiObj++) {
        if (aiObj->object_number == obj_num && aiObj->trigger_flags == item->item_flags[3] && aiObj->room_number != 255) {
            zone = ground_zone[creature->LOT.zone][flip_status];
            r = &room[item->room_number];
            item->box_number = r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)].box;
            zone_number = zone[item->box_number];
            r = &room[aiObj->room_number];
            aiObj->box_number = r->floor[((aiObj->z - r->z) >> 10) + r->x_size * ((aiObj->x - r->x) >> 10)].box;
            ai_zone = zone[aiObj->box_number];
            if (zone_number == ai_zone) {
                enemy = &creature->ai_target;
                creature->enemy = enemy;
                enemy->object_number = aiObj->object_number;
                enemy->room_number = aiObj->room_number;
                enemy->pos.x_pos = aiObj->x;
                enemy->pos.y_pos = aiObj->y;
                enemy->pos.z_pos = aiObj->z;
                enemy->pos.y_rot = aiObj->y_rot;
                enemy->flags = aiObj->flags;
                enemy->trigger_flags = aiObj->trigger_flags;
                enemy->box_number = aiObj->box_number;
                if (!(enemy->flags & 0x20)) {
                    enemy->pos.x_pos += 256 * phd_sin(enemy->pos.y_rot) >> W2V_SHIFT;
                    enemy->pos.z_pos += 256 * phd_cos(enemy->pos.y_rot) >> W2V_SHIFT;
                }
                break;
            }
        }
    }
}

sint16 SameZone(CREATURE_INFO *creature, ITEM_INFO *target_item)
{
    ITEM_INFO *item;
    ROOM_INFO *r;
    sint16 *zone, zone_number, target_zone;

    zone = ground_zone[creature->LOT.zone][flip_status];
    item = &items[creature->item_num];
    r = &room[item->room_number];
    item->box_number = r->floor[((item->pos.z_pos - r->z) >> 10) + r->x_size * ((item->pos.x_pos - r->x) >> 10)].box;
    zone_number = zone[item->box_number];
    r = &room[target_item->room_number];
    target_item->box_number = r->floor[((target_item->pos.z_pos - r->z) >> 10) + r->x_size * ((target_item->pos.x_pos - r->x) >> 10)].box;
    target_zone = zone[target_item->box_number];
    return zone_number == target_zone;
}

void CreatureYRot(PHD_3DPOS *srcpos, sint16 angle, sint16 angadd)
{
    if (angle > angadd)
        srcpos->y_rot += angadd;
    else if (angle < -angadd)
        srcpos->y_rot -= angadd;
    else
        srcpos->y_rot += angle;
}

sint32 MoveCreature3DPos(PHD_3DPOS *srcpos, PHD_3DPOS *destpos, sint32 velocity, sint16 angdif, sint16 angadd)
{
    sint32 x, y, z, dist;

    x = destpos->x_pos - srcpos->x_pos;
    y = destpos->y_pos - srcpos->y_pos;
    z = destpos->z_pos - srcpos->z_pos;
    dist = phd_sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
    if (velocity >= dist) {
        srcpos->x_pos = destpos->x_pos;
        srcpos->y_pos = destpos->y_pos;
        srcpos->z_pos = destpos->z_pos;
    } else {
        srcpos->x_pos += velocity * x / dist;
        srcpos->y_pos += velocity * y / dist;
        srcpos->z_pos += velocity * z / dist;
    }
    if (angdif > angadd)
        srcpos->y_rot += angadd;
    else if (angdif < -angadd)
        srcpos->y_rot -= angadd;
    else
        srcpos->y_rot = destpos->y_rot;
    return srcpos->x_pos == destpos->x_pos && srcpos->y_pos == destpos->y_pos && srcpos->z_pos == destpos->z_pos && srcpos->y_rot == destpos->y_rot;
}
