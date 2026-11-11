#include "people.h"
#include "items.h"
#include "lara.h"
#include "effects.h"
#include "specific/function_stubs.h"
#include "room.h"
#include "box.h"
#include "draw.h"
#include "control.h"

sint32 TargetVisible(ITEM_INFO *item, AI_INFO *info)
{
    GAME_VECTOR start, target;
    CREATURE_INFO *creature;
    ITEM_INFO *enemy;
    sint16 true_angle, *bounds;

    creature = (CREATURE_INFO *)item->data;
    if ((enemy = creature->enemy) == NULL)
        return 0;
    if (enemy->hit_points <= 0 || enemy->data == NULL)
        return 0;
    true_angle = info->angle - creature->joint_rotation[2];
    if (true_angle > -0x4000 && true_angle < 0x4000 && info->distance < 0x4000000) {
        start.x = item->pos.x_pos;
        start.y = item->pos.y_pos - STEP_L * 3;
        start.z = item->pos.z_pos;
        start.room_number = item->room_number;
        target.x = enemy->pos.x_pos;
        bounds = GetBestFrame(enemy);
        target.y = enemy->pos.y_pos + ((bounds[3] + bounds[2] * 3) >> 2);
        target.z = enemy->pos.z_pos;
        return LOS(&start, &target);
    }
    return 0;
}

sint16 GunShot(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number)
{
    return NO_ITEM;
}

sint16 GunMiss(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number)
{
    GAME_VECTOR pos;

    pos.x = lara_item->pos.x_pos + (GetRandomControl() - 0x4000) * (WALL_L / 2) / 0x7fff;
    pos.y = lara_item->floor;
    pos.z = lara_item->pos.z_pos + (GetRandomControl() - 0x4000) * (WALL_L / 2) / 0x7fff;
    pos.room_number = lara_item->room_number;
    Richochet(&pos);
    return GunShot(x, y, z, speed, yrot, room_number);
}
