#include "lara1gun.h"
#include "larafire.h"
#include "anim.h"
#include "delstuff.h"
#include "objnums.h"
#include "control.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "traps.h"
#include "box.h"
#include "savegame.h"
#include "specific/function_stubs.h"
#include "sound.h"

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

void FireShotgun(void)
{
    sint32 i, fired, scatter, x, y, z, lp;
    PHD_VECTOR pos;
    PHD_ANGLE angles[2];
    PHD_ANGLE dangles[2];

    angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
    angles[1] = lara.left_arm.x_rot;
    if (!lara.left_arm.lock) {
        angles[0] += lara.torso_y_rot;
        angles[1] += lara.torso_x_rot;
    }
    fired = 0;
    if (lara.shotgun_type_carried & 8)
        scatter = 1820;
    else
        scatter = 5460;
    for (i = 0; i < 6; i++) {
        dangles[0] = (PHD_ANGLE)(angles[0] + scatter * (GetRandomControl() - 0x4000) / 0x10000);
        dangles[1] = (PHD_ANGLE)(angles[1] + scatter * (GetRandomControl() - 0x4000) / 0x10000);
        if (FireWeapon(WEAPON_SHOTGUN, lara.target, lara_item, dangles))
            fired = 1;
    }
    if (fired) {
        pos.x = 0;
        pos.y = 228;
        pos.z = 32;
        GetLaraJointPos(&pos, 11);
        x = pos.x;
        y = pos.y;
        z = pos.z;
        pos.x = 0;
        pos.y = 228 + 1280;
        pos.z = 32;
        GetLaraJointPos(&pos, 11);
        SmokeCountL = 32;
        SmokeWeapon = WEAPON_SHOTGUN;
        if (lara_item->mesh_bits) {
            for (lp = 0; lp < 7; lp++)
                TriggerGunSmoke(x, y, z, pos.x - x, pos.y - y, pos.z - z, 1, SmokeWeapon, 32);
            for (lp = 0; lp < 12; lp++)
                nullsub_1(x, y, z, (pos.x - x) << 1, (pos.y - y) << 1, (pos.z - z) << 1);
        }
        lara.right_arm.flash_gun = weapons[WEAPON_SHOTGUN].flash_time;
        SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x1400000 | 4);
        SoundEffect(weapons[WEAPON_SHOTGUN].sample_num, &lara_item->pos, SFX_DEFAULT);
        savegame.Game.AmmoUsed++;
    }
}

void draw_shotgun(sint32 weapon_type)
{
    ITEM_INFO *item;

    if (lara.weapon_item == NO_ITEM) {
        lara.weapon_item = CreateItem();
        item = &items[lara.weapon_item];
        item->object_number = (sint16)WeaponObject(weapon_type);
        if (weapon_type == WEAPON_GRENADE)
            item->anim_number = objects[GRENADE_GUN_ANIM].anim_index;
        else
            item->anim_number = objects[item->object_number].anim_index + 1;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 1;
        item->status = ITEM_ACTIVE;
        item->room_number = 255;
        lara.left_arm.frame_base = lara.right_arm.frame_base = objects[item->object_number].frame_base;
    } else {
        item = &items[lara.weapon_item];
    }
    AnimateItem(item);
    if (!item->current_anim_state || item->current_anim_state == 6)
        ready_shotgun(weapon_type);
    else if (item->frame_number - anims[item->anim_number].frame_base == weapons[weapon_type].draw_frame)
        draw_shotgun_meshes(weapon_type);
    else if (lara.water_status == LW_UNDERWATER)
        item->goal_anim_state = 6;
    lara.left_arm.frame_base = lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
    lara.left_arm.frame_number = lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
    lara.left_arm.anim_number = lara.right_arm.anim_number = item->anim_number;
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

void DoGrenadeDamageOnBaddie(ITEM_INFO *baddie, ITEM_INFO *item)
{
    if ((sint16)(baddie->flags & IFL_CLEARBODY))
        return;
    if (baddie == lara_item && lara_item->hit_points > 0) {
        lara_item->hit_points -= 50;
        if (!(room[item->room_number].flags & ROOM_UNDERWATER) && lara_item->hit_points <= 50)
            LaraBurn();
    } else if (!item->item_flags[2]) {
        baddie->hit_status = 1;
        if ((!objects[baddie->object_number].undead || baddie->object_number == SKELETON || baddie->object_number == MUMMY) && baddie->object_number != AHMET) {
            HitTarget(baddie, 0, 30, 1);
            if (baddie != lara_item) {
                savegame.Game.AmmoHits++;
                if (baddie->hit_points <= 0) {
                    savegame.Level.Kills++;
                    if (baddie->object_number != BABOON_NORMAL && baddie->object_number != BABOON_INV && baddie->object_number != BABOON_SILENT)
                        CreatureDie(baddie - items, 1);
                }
            }
        }
    }
}
