#include "lara1gun.h"
#include "larafire.h"
#include "anim.h"
#include "delstuff.h"
#include "objnums.h"
#include "control.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "specific/function_stubs.h"

void draw_shotgun_meshes(sint32 weapon_type)
{
    lara.mesh_ptrs[LM_RHAND] = meshes[objects[WeaponObjectMesh(weapon_type)].mesh_index + 2 * LM_RHAND];
    lara.back_gun = WEAPON_NONE;
}

void undraw_shotgun_meshes(sint32 weapon_type)
{
    sint32 obj;

    obj = WeaponObject(weapon_type);
    lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + 2 * LM_RHAND];
    lara.back_gun = obj;
}

void ready_shotgun(int weapon_type)
{
    lara.gun_status = LG_READY;
    lara.left_arm.x_rot = lara.left_arm.y_rot = lara.left_arm.z_rot = 0;
    lara.right_arm.x_rot = lara.right_arm.y_rot = lara.right_arm.z_rot = 0;
    lara.left_arm.frame_number = lara.right_arm.frame_number = 0;
    lara.left_arm.lock = lara.right_arm.lock = 0;
    lara.target = NULL;
    lara.left_arm.frame_base = lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
}

void undraw_shotgun(sint32 weapon_type)
{
    ITEM_INFO *item;

    item = &items[lara.weapon_item];
    if (lara.water_status == LW_SURFACE)
        item->goal_anim_state = 9;
    else
        item->goal_anim_state = 3;
    AnimateItem(item);
    if (item->status == ITEM_DEACTIVATED) {
        lara.gun_status = LG_NO_ARMS;
        lara.target = NULL;
        lara.left_arm.lock = lara.right_arm.lock = 0;
        KillItem(lara.weapon_item);
        lara.weapon_item = NO_ITEM;
        lara.left_arm.frame_number = lara.right_arm.frame_number = 0;
    } else if (item->current_anim_state == 3 && item->frame_number - anims[item->anim_number].frame_base == 21)
        undraw_shotgun_meshes(weapon_type);
    lara.left_arm.frame_base = lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
    lara.left_arm.frame_number = lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
    lara.left_arm.anim_number = lara.right_arm.anim_number = item->anim_number;
}

void TriggerUnderwaterExplosion(ITEM_INFO *item, sint32 vehicle)
{
    sint32 x, y, z, wh, i;

    if (vehicle) {
        x = (GetRandomControl() & 0x1FF) + item->pos.x_pos - 256;
        y = item->pos.y_pos;
        z = (GetRandomControl() & 0x1FF) + item->pos.z_pos - 256;
        TriggerExplosionBubble(x, y, z, item->room_number);
        TriggerExplosionSparks(x, y, z, 2, -1, 1, item->room_number);
        wh = GetWaterHeight(x, y, z, item->room_number);
        if (wh != NO_HEIGHT)
            TriggerSmallSplash(x, wh, z, 8);
    } else {
        TriggerExplosionBubble(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
        TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -2, 1, item->room_number);
        for (i = 0; i < 3; i++)
            TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -1, 1, item->room_number);
        wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
        if (wh != NO_HEIGHT) {
            y = item->pos.y_pos - wh;
            if (y < 2048) {
                splash_setup.x = item->pos.x_pos;
                splash_setup.y = wh;
                splash_setup.z = item->pos.z_pos;
                wh = 2048 - y;
                splash_setup.InnerRadVel = 160;
                splash_setup.MiddleSize = 224;
                splash_setup.OuterRad = 272;
                splash_setup.InnerRad = (sint16)((wh >> 6) + 16);
                splash_setup.InnerSize = (sint16)((wh >> 6) + 12);
                splash_setup.InnerYVel = (sint16)((-512 - wh) << 3);
                splash_setup.pad1 = (sint16)((wh >> 6) + 24);
                splash_setup.MiddleRad = (sint16)((wh >> 6) + 24);
                splash_setup.MiddleRadVel = (sint16)((-768 - wh) << 2);
                splash_setup.MiddleYVel = (sint16)((wh >> 6) + 32);
                splash_setup.pad2 = (sint16)((wh >> 6) + 32);
                SetupSplash(&splash_setup);
            }
        }
    }
}
