#include "box.h"
#include "room.h"
#include "lara.h"
#include "control.h"
#include "specific/function_stubs.h"

#define ESCAPE_DIST (WALL_L * 5)
#define MAX_TILT (ONE_DEGREE * 3)
#define MAX_JOINT_CHANGE (ONE_DEGREE * 3)
#define JOINT_ARC 0x3000

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
