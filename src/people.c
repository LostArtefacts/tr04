#include "people.h"
#include "items.h"
#include "lara.h"
#include "effects.h"
#include "specific/function_stubs.h"
#include "room.h"
#include "box.h"
#include "draw.h"
#include "control.h"
#include "objnums.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "sound.h"

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

sint32 Targetable(ITEM_INFO *item, AI_INFO *info)
{
    GAME_VECTOR start, target;
    CREATURE_INFO *creature;
    ITEM_INFO *enemy;
    sint16 *bounds;

    creature = (CREATURE_INFO *)item->data;
    if ((enemy = creature->enemy) == NULL)
        return 0;
    if (enemy->hit_points <= 0 || enemy->data == NULL)
        return 0;
    if (info->ahead && (info->distance < 0x4000000 || item->object_number == SETHA)) {
        start.x = item->pos.x_pos;
        bounds = GetBestFrame(item);
        start.y = item->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);
        start.z = item->pos.z_pos;
        start.room_number = item->room_number;
        target.x = enemy->pos.x_pos;
        bounds = GetBestFrame(enemy);
        target.y = enemy->pos.y_pos + ((bounds[3] + 3 * bounds[2]) >> 2);
        target.z = enemy->pos.z_pos;
        return LOS(&start, &target);
    }
    return 0;
}

sint16 GunShot(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number)
{
    return NO_ITEM;
}

sint16 GunHit(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number)
{
    PHD_VECTOR pos;

    pos.z = 0;
    pos.y = 0;
    pos.x = 0;
    GetJointAbsPosition(lara_item, &pos, (25 * GetRandomControl()) / 0x7FFF);
    DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 3, lara_item->pos.y_rot, lara_item->room_number);
    SoundEffect(50, &lara_item->pos, 0);
    return GunShot(x, y, z, speed, yrot, room_number);
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

sint32 ShotLara(ITEM_INFO *item, AI_INFO *info, BITE_INFO *gun, sint16 extra_rotation, sint32 damage)
{
    ITEM_INFO *enemy;
    CREATURE_INFO *creature;
    PHD_VECTOR pos;
    sint32 hit, targetable, random, distance;

    creature = (CREATURE_INFO *)item->data;
    enemy = creature->enemy;
    if (info->distance <= 0x4000000 && Targetable(item, info)) {
        distance = ((rcossin_tbl[(info->enemy_facing >> 3) & 0x1FFE] << 2) * enemy->speed >> W2V_SHIFT) * 0x4000000 / 300;
        distance = info->distance + SQUARE(distance);
        if (distance > 0x4000000)
            hit = 0;
        else {
            random = (0x4000000 - info->distance) / 3276 + 0x2000;
            hit = GetRandomControl() < random;
        }
        targetable = 1;
    } else {
        targetable = 0;
        hit = 0;
    }
    if (damage) {
        if (enemy == lara_item) {
            if (hit) {
                CreatureEffect(item, gun, GunHit);
                lara_item->hit_points -= (sint16)damage;
                lara_item->hit_status = 1;
            } else if (targetable)
                CreatureEffect(item, gun, GunMiss);
        } else {
            CreatureEffect(item, gun, GunShot);
            if (hit) {
                enemy->hit_points += (sint16)(damage / -10);
                enemy->hit_status = 1;
                random = GetRandomControl() & 0xF;
                if (random > 14)
                    random = 0;
                pos.x = pos.y = pos.z = 0;
                GetJointAbsPosition(enemy, &pos, random);
                DoBloodSplat(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 4, enemy->pos.y_rot, enemy->room_number);
            }
        }
    }
    return targetable;
}
