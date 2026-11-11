#include "box.h"
#include "room.h"
#include "lara.h"
#include "specific/function_stubs.h"

#define ESCAPE_DIST (WALL_L * 5)
#define MAX_JOINT_CHANGE (ONE_DEGREE * 3)
#define JOINT_ARC 0x3000

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
