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
#include "specific/3dmath.h"
#include "camera.h"
#include "lara2gun.h"
#include "gameflow.h"
#include "specific/input.h"
#include "sphere.h"
#include "objects.h"
#include "debris.h"
#include "switch.h"
#include "specific/polyinsert.h"

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

void RifleHandler(sint32 weapon_type)
{
    WEAPON_INFO *winfo;
    PHD_VECTOR pos;

    winfo = &weapons[weapon_type];
    LaraGetNewTarget(winfo);
    if (input & IN_ACTION)
        LaraTargetInfo(winfo);
    AimWeapon(winfo, &lara.left_arm);
    if (lara.left_arm.lock) {
        lara.torso_y_rot = lara.left_arm.y_rot;
        lara.torso_x_rot = lara.left_arm.x_rot;
        if (camera.old_type != LOOK_CAMERA && !BinocularRange) {
            lara.head_y_rot = 0;
            lara.head_x_rot = 0;
        }
    }
    if (weapon_type == WEAPON_REVOLVER)
        AnimatePistols(weapon_type);
    else
        AnimateShotgun(weapon_type);
    if (lara.right_arm.flash_gun) {
        if (weapon_type == WEAPON_SHOTGUN) {
            if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
                TriggerDynamic_MIRROR((GetRandomControl() & 0xFF) + ((rcossin_tbl[(lara_item->pos.y_rot >> 3) & 0x1FFE] << 2) >> 4) + lara_item->pos.x_pos - 128,
                    (GetRandomControl() & 0x7F) + lara_item->pos.y_pos - 575,
                    (GetRandomControl() & 0xFF) + ((rcossin_tbl[((lara_item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) >> 4) + lara_item->pos.z_pos - 128, 12,
                    (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
            else
                TriggerDynamic((GetRandomControl() & 0xFF) + ((rcossin_tbl[(lara_item->pos.y_rot >> 3) & 0x1FFE] << 2) >> 4) + lara_item->pos.x_pos - 128,
                    (GetRandomControl() & 0x7F) + lara_item->pos.y_pos - 575,
                    (GetRandomControl() & 0xFF) + ((rcossin_tbl[((lara_item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) >> 4) + lara_item->pos.z_pos - 128, 12,
                    (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
        } else if (weapon_type == WEAPON_REVOLVER) {
            pos.x = (GetRandomControl() & 0xFF) - 128;
            pos.y = (GetRandomControl() & 0x7F) - 63;
            pos.z = (GetRandomControl() & 0xFF) - 128;
            GetLaraJointPos(&pos, 11);
            if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
                TriggerDynamic_MIRROR(pos.x, pos.y, pos.z, 12, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
            else
                TriggerDynamic(pos.x, pos.y, pos.z, 12, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
        }
    }
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

void FireGrenade(void)
{
    ITEM_INFO *item;
    PHD_VECTOR pos;
    sint16 *ammo;
    FLOOR_INFO *floor;
    sint32 x, y, z, lp;
    sint16 item_number;

    ammo = get_current_ammo_pointer(WEAPON_GRENADE);
    if (!*ammo)
        return;
    lara.has_fired = 1;
    item_number = CreateItem();
    if (item_number == NO_ITEM)
        return;
    item = &items[item_number];
    item->shade = -0x3df0;
    item->object_number = GRENADE;
    item->room_number = lara_item->room_number;
    pos.x = 0;
    pos.y = 276;
    pos.z = 80;
    GetLaraJointPos(&pos, 11);
    item->pos.x_pos = x = pos.x;
    item->pos.y_pos = y = pos.y;
    item->pos.z_pos = z = pos.z;
    floor = GetFloor(pos.x, pos.y, pos.z, &item->room_number);
    if (GetHeight(floor, pos.x, pos.y, pos.z) < pos.y) {
        item->pos.x_pos = lara_item->pos.x_pos;
        item->pos.y_pos = pos.y;
        item->pos.z_pos = lara_item->pos.z_pos;
        item->room_number = lara_item->room_number;
    }
    pos.x = 0;
    pos.y = 1204;
    pos.z = 80;
    GetLaraJointPos(&pos, 11);
    SmokeCountL = 32;
    SmokeWeapon = WEAPON_GRENADE;
    if (lara_item->mesh_bits) {
        for (lp = 0; lp < 5; lp++)
            TriggerGunSmoke(x, y, z, pos.x - x, pos.y - y, pos.z - z, 1, WEAPON_GRENADE, 32);
    }
    InitialiseItem(item_number);
    item->pos.x_rot = lara_item->pos.x_rot + lara.left_arm.x_rot;
    item->pos.y_rot = lara_item->pos.y_rot + lara.left_arm.y_rot;
    item->pos.z_rot = 0;
    if (!lara.left_arm.lock) {
        item->pos.x_rot += lara.torso_x_rot;
        item->pos.y_rot += lara.torso_y_rot;
    }
    item->speed = 128;
    item->fallspeed = (-128 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
    item->current_anim_state = item->pos.x_rot;
    item->goal_anim_state = item->pos.y_rot;
    item->required_anim_state = 0;
    item->hit_points = 120;
    AddActiveItem(item_number);
    if (*ammo != -1)
        (*ammo)--;
    if (lara.grenade_type_carried & 8)
        item->item_flags[0] = 1;
    else if (lara.grenade_type_carried & 0x10)
        item->item_flags[0] = 2;
    else
        item->item_flags[0] = 3;
    savegame.Game.AmmoUsed++;
}

void ControlGrenade(sint16 item_number)
{
    ITEM_INFO **itemlist;
    MESH_INFO **meshlist;
    ITEM_INFO *item, *item2, *target;
    MESH_INFO *mesh;
    FLOOR_INFO *floor;
    PHD_VECTOR pos;
    sint32 abovewater, xv, yv, zv, exploded, rad, i, ox, oy, oz, yrot;
    sint16 new_num, room_number, NumTrigs, j;
    sint16 TriggerItems[8];

    item = &items[item_number];
    if (item->item_flags[1]) {
        if (!--item->item_flags[1]) {
            KillItem(item_number);
            return;
        }
        if (item->item_flags[0] == 3) {
            if (item->item_flags[1] == 1) {
                FlashFader = 32;
                FlashFadeR = 255;
                FlashFadeG = 255;
                FlashFadeB = 255;
                lara.blindTimer = 120;
            } else {
                FlashFadeR = (GetRandomControl() & 0x1F) + 224;
                FlashFadeB = FlashFadeG = FlashFadeR - (GetRandomControl() & 0x1F);
                FlashFader = 32;
            }
            if (IsVolumetric())
                FlashFader = 0;
            TriggerFlashSmoke(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
            TriggerFlashSmoke(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
        } else {
            new_num = CreateItem();
            if (new_num != NO_ITEM) {
                item2 = &items[new_num];
                item2->shade = -0x3DF0;
                item2->object_number = GRENADE;
                item2->room_number = item->room_number;
                item2->pos.x_pos = (GetRandomControl() & 0x1FF) + item->pos.x_pos - 256;
                item2->pos.y_pos = item->pos.y_pos - 256;
                item2->pos.z_pos = (GetRandomControl() & 0x1FF) + item->pos.z_pos - 256;
                InitialiseItem(new_num);
                item2->pos.x_rot = (GetRandomControl() & 0x3FFF) + 0x2000;
                item2->pos.y_rot = GetRandomControl() << 1;
                item2->pos.z_rot = 0;
                item2->speed = 64;
                item2->fallspeed = (-64 * (rcossin_tbl[(item2->pos.x_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
                item2->current_anim_state = item2->pos.x_rot;
                item2->goal_anim_state = item2->pos.y_rot;
                item2->required_anim_state = 0;
                AddActiveItem(new_num);
                item2->status = ITEM_INVISIBLE;
                item2->item_flags[0] = 4;
                item2->item_flags[2] = item->item_flags[2];
                item2->hit_points = 3000;
                if (room[item2->room_number].flags & ROOM_UNDERWATER)
                    item2->hit_points = 1;
            }
        }
        return;
    }
    ox = item->pos.x_pos;
    oy = item->pos.y_pos;
    oz = item->pos.z_pos;
    item->shade = -0x3DF0;
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        abovewater = 0;
        item->fallspeed += (5 - item->fallspeed) >> 1;
        item->speed -= item->speed >> 2;
        if (item->speed) {
            item->pos.z_rot += 182 * ((item->speed >> 4) + 3);
            if (item->required_anim_state)
                item->pos.y_rot += 182 * ((item->speed >> 2) + 3);
            else
                item->pos.x_rot += 182 * ((item->speed >> 2) + 3);
        }
    } else {
        abovewater = 1;
        item->fallspeed += 3;
        if (item->speed) {
            item->pos.z_rot += 182 * ((item->speed >> 2) + 7);
            if (item->required_anim_state)
                item->pos.y_rot += 182 * ((item->speed >> 1) + 7);
            else
                item->pos.x_rot += 182 * ((item->speed >> 1) + 7);
        }
    }
    if (item->speed && abovewater) {
        phd_PushUnitMatrix();
        phd_mxptr[M03] = 0;
        phd_mxptr[M13] = 0;
        phd_mxptr[M23] = 0;
        phd_RotYXZ(item->pos.y_rot + 0x8000, item->pos.x_rot, item->pos.z_rot);
        phd_TranslateRel(0, 0, -64);
        pos.x = phd_mxptr[M03] >> W2V_SHIFT;
        pos.y = phd_mxptr[M13] >> W2V_SHIFT;
        pos.z = phd_mxptr[M23] >> W2V_SHIFT;
        phd_PopMatrix();
        TriggerRocketSmoke(item->pos.x_pos + pos.x, item->pos.y_pos + pos.y, item->pos.z_pos + pos.z, -1);
    }
    zv = ((rcossin_tbl[((item->goal_anim_state >> 3) & 0x1FFE) + 1] << 2) * item->speed) >> W2V_SHIFT;
    item->pos.z_pos += zv;
    xv = ((rcossin_tbl[(item->goal_anim_state >> 3) & 0x1FFE] << 2) * item->speed) >> W2V_SHIFT;
    item->pos.x_pos += xv;
    yv = item->fallspeed;
    item->pos.y_pos += yv;
    if (item->item_flags[0] == 4) {
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        if (GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) < item->pos.y_pos ||
            GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) > item->pos.y_pos)
            item->hit_points = 1;
    } else {
        yrot = item->pos.y_rot;
        item->pos.y_rot = item->goal_anim_state;
        DoProperDetection(item_number, ox, oy, oz, xv, yv, zv);
        item->goal_anim_state = item->pos.y_rot;
        item->pos.y_rot = yrot;
    }
    room_number = item->room_number;
    GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (room[room_number].flags & ROOM_UNDERWATER && abovewater) {
        splash_setup.x = item->pos.x_pos;
        splash_setup.y = room[room_number].maxceiling;
        splash_setup.z = item->pos.z_pos;
        splash_setup.InnerRad = 32;
        splash_setup.InnerSize = 8;
        splash_setup.InnerRadVel = 320;
        splash_setup.InnerYVel = -40 * item->fallspeed;
        splash_setup.pad1 = 48;
        splash_setup.MiddleRad = 32;
        splash_setup.MiddleSize = 480;
        splash_setup.MiddleRadVel = -20 * item->fallspeed;
        splash_setup.MiddleYVel = 32;
        splash_setup.pad2 = 128;
        splash_setup.OuterRad = 544;
        SetupSplash(&splash_setup);
        if (item->item_flags[0] == 4)
            item->hit_points = 1;
    }
    if (item->item_flags[0] == 4)
        TriggerFireFlame(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, -1, 1);
    exploded = 0;
    rad = 0;
    if (item->hit_points) {
        if (!--item->hit_points) {
            rad = 2048;
            exploded = 1;
        } else if (item->hit_points > 118)
            return;
    }
    if (item->item_flags[0] != 3 || !exploded) {
        for (i = 0; i < 2; i++) {
            itemlist = (ITEM_INFO **)&tsv_buffer[0x2000];
            meshlist = (MESH_INFO **)&tsv_buffer[0x3000];
            GetCollidedObjects(item, rad, 1, itemlist, meshlist, 1);
            if (!exploded) {
                if (!*itemlist && !*meshlist)
                    return;
                exploded = 1;
                if (item->item_flags[0] == 3)
                    break;
                rad = 2048;
                continue;
            }
            target = *itemlist;
            while (target) {
                if (target->object_number >= SMASH_OBJECT1 && target->object_number <= SMASH_OBJECT8) {
                    TriggerExplosionSparks(target->pos.x_pos, target->pos.y_pos, target->pos.z_pos, 3, -2, 0, target->room_number);
                    target->pos.y_pos -= 128;
                    TriggerShockwave((PHD_VECTOR *)&target->pos, 0x1300030, 96, 0x18806000, 0);
                    target->pos.y_pos += 128;
                    ExplodeItemNode(target, 0, 0, 128);
                    SmashObject(target - items);
                    KillItem(target - items);
                } else if ((target->object_number == SWITCH_TYPE7 || target->object_number == SWITCH_TYPE8) && !(target->flags & IFL_SWITCH_ONESHOT)) {
                    if (!(target->flags & IFL_CODEBITS) || (target->flags & IFL_CODEBITS) == IFL_CODEBITS) {
                        NumTrigs = (sint16)GetSwitchTrigger(target, TriggerItems, 1);
                        for (j = NumTrigs; j > 0; j--) {
                            AddActiveItem(TriggerItems[j - 1]);
                            items[TriggerItems[j - 1]].status = ITEM_ACTIVE;
                            items[TriggerItems[j - 1]].flags |= IFL_CODEBITS;
                        }
                    } else {
                        sint16 rn;

                        rn = target->room_number;
                        floor = GetFloor(target->pos.x_pos, target->pos.y_pos - 256, target->pos.z_pos, &rn);
                        GetHeight(floor, target->pos.x_pos, target->pos.y_pos - 256, target->pos.z_pos);
                        TestTriggers(trigger_index, 1, target->flags & IFL_CODEBITS);
                    }
                    if (target->object_number == SWITCH_TYPE7)
                        ExplodeItemNode(target, objects[SWITCH_TYPE7].nmeshes - 1, 0, 64);
                    AddActiveItem(target - items);
                    target->status = ITEM_ACTIVE;
                    target->flags |= IFL_SWITCH_ONESHOT | IFL_CODEBITS;
                } else if (objects[target->object_number].intelligent || target->object_number == LARA)
                    DoGrenadeDamageOnBaddie(target, item);
                itemlist++;
                target = *itemlist;
            }
            mesh = *meshlist;
            while (mesh) {
                if (mesh->static_number >= SHATTER0 && mesh->static_number < SHATTER8) {
                    Log(0, "Shatter");
                    TriggerExplosionSparks(mesh->x, mesh->y, mesh->z, 3, -2, 0, item->room_number);
                    mesh->y -= 128;
                    TriggerShockwave((PHD_VECTOR *)&mesh->x, 0xB00028, 64, 0x10806000, 0);
                    mesh->y += 128;
                    ShatterObject(0, mesh, -128, item->room_number, 0);
                    SmashedMeshRoom[SmashedMeshCount] = item->room_number;
                    SmashedMesh[SmashedMeshCount] = mesh;
                    SmashedMeshCount++;
                    mesh->Flags &= ~1;
                }
                meshlist++;
                mesh = *meshlist;
            }
        }
    }
    if (exploded) {
        if (item->item_flags[0] == 3) {
            FlashFader = 32;
            FlashFadeR = 255;
            FlashFadeG = 255;
            FlashFadeB = 255;
            if (IsVolumetric()) {
                FlashFader = 0;
                TriggerFXFogBulb(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 4096, 255, 255, 255, 255, item->room_number);
            }
            TriggerFlashSmoke(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
            TriggerFlashSmoke(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
        } else if (room[item->room_number].flags & ROOM_UNDERWATER)
            TriggerUnderwaterExplosion(item, 0);
        else {
            item->pos.y_pos -= 128;
            TriggerShockwave((PHD_VECTOR *)&item->pos, 0x1300030, 96, 0x18806000, 0);
            item->pos.y_pos += 128;
            TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, item->room_number);
            for (i = 0; i < 2; i++)
                TriggerExplosionSparks(ox, oy, oz, 3, -1, 0, item->room_number);
        }
        AlertNearbyGuards(item);
        SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800000 | SFX_SETPITCH);
        SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
        if (item->item_flags[0] == 1 || item->item_flags[0] == 4)
            KillItem(item_number);
        else {
            item->mesh_bits = 0;
            item->item_flags[1] = item->item_flags[0] == 2 ? 4 : 16;
        }
    }
}

void FireCrossbow(PHD_3DPOS *pos)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    PHD_VECTOR vec;
    sint16 *ammo;
    sint32 h;
    sint16 item_number;

    ammo = get_current_ammo_pointer(WEAPON_CROSSBOW);
    if (!*ammo)
        return;
    lara.has_fired = 1;
    item_number = CreateItem();
    if (item_number != NO_ITEM) {
        item = &items[item_number];
        item->object_number = CROSSBOW_BOLT;
        item->shade = -0x3DF0;
        if (pos) {
            item->room_number = lara_item->room_number;
            item->pos.x_pos = pos->x_pos;
            item->pos.y_pos = pos->y_pos;
            item->pos.z_pos = pos->z_pos;
            InitialiseItem(item_number);
            item->pos.x_rot = pos->x_rot;
            item->pos.y_rot = pos->y_rot;
            item->pos.z_rot = pos->z_rot;
        } else {
            if (*ammo != -1)
                --*ammo;
            vec.x = 0;
            vec.y = 228;
            vec.z = 32;
            GetLaraJointPos(&vec, 11);
            item->room_number = lara_item->room_number;
            floor = GetFloor(vec.x, vec.y, vec.z, &item->room_number);
            h = GetHeight(floor, vec.x, vec.y, vec.z);
            if (h < vec.y) {
                item->pos.x_pos = lara_item->pos.x_pos;
                item->pos.y_pos = vec.y;
                item->pos.z_pos = lara_item->pos.z_pos;
                item->room_number = lara_item->room_number;
            } else {
                item->pos.x_pos = vec.x;
                item->pos.y_pos = vec.y;
                item->pos.z_pos = vec.z;
            }
            InitialiseItem(item_number);
            item->pos.x_rot = lara.left_arm.x_rot + lara_item->pos.x_rot;
            item->pos.y_rot = lara.left_arm.y_rot + lara_item->pos.y_rot;
            item->pos.z_rot = 0;
            if (!lara.left_arm.lock) {
                item->pos.x_rot += lara.torso_x_rot;
                item->pos.y_rot += lara.torso_y_rot;
            }
        }
        item->speed = 512;
        AddActiveItem(item_number);
        if (lara.crossbow_type_carried & 8)
            item->item_flags[0] = 1;
        else if (lara.crossbow_type_carried & 0x10)
            item->item_flags[0] = 2;
        else
            item->item_flags[0] = 3;
        SoundEffect(SFX_LARA_CROSSBOW, 0, SFX_DEFAULT);
        savegame.Game.AmmoUsed++;
    }
}

void CrossbowHitSwitchType78(ITEM_INFO *item, ITEM_INFO *target, sint32 MustHitLastNode)
{
    SPHERE *ptr1;
    sint32 dx, dy, dz, num1, cs, cd, speed, i, j;
    sint16 TriggerItems[8];
    sint16 NumTrigs, room_number;

    if (target->flags & IFL_SWITCH_ONESHOT && target->object_number != SKELETON)
        return;
    if (!MustHitLastNode) {
        num1 = objects[target->object_number].nmeshes;
        cs = num1 - 1;
    } else {
        num1 = GetSpheres(target, Slist, 1);
        cs = -1;
        cd = 0x7FFFFFFF;
        if (target->object_number != SKELETON) {
            ptr1 = Slist;
            for (i = 0; i < num1; i++) {
                dx = ptr1->x - item->pos.x_pos;
                dy = ptr1->y - item->pos.y_pos;
                dz = ptr1->z - item->pos.z_pos;
                dy = SQUARE(dz) - SQUARE(ptr1->r) + SQUARE(dy) + SQUARE(dx);
                if (dy < cd) {
                    cd = dy;
                    cs = i;
                }
                ptr1++;
            }
        } else {
            for (i = 0; i < 8; i++) {
                item->pos.y_pos += ((rcossin_tbl[(-item->pos.x_rot >> 3) & 0x1FFE] << 2) * item->speed) >> 17;
                speed = ((rcossin_tbl[((item->pos.x_rot >> 3) & 0x1FFE) + 1] << 2) * item->speed) >> W2V_SHIFT;
                item->pos.z_pos += ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * speed) >> 17;
                item->pos.x_pos += ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * speed) >> 17;
                ptr1 = Slist;
                for (j = 0; j < num1; j++) {
                    dx = ptr1->x - item->pos.x_pos;
                    dy = ptr1->y - item->pos.y_pos;
                    dz = ptr1->z - item->pos.z_pos;
                    dy = SQUARE(dz) + SQUARE(dy) + SQUARE(dx);
                    if (dy < SQUARE(ptr1->r)) {
                        cs = j;
                        break;
                    }
                    ptr1++;
                }
                if (cs != -1)
                    break;
            }
        }
    }
    if (target->object_number == SKELETON) {
        if (cs != -1 && objects[target->object_number].explodable_meshbits & 1 << cs) {
            ExplodeItemNode(target, cs, 0, 64);
            target->mesh_bits &= ~(1 << cs);
        }
    } else {
        if (cs == num1 - 1) {
            if (target->flags & IFL_CODEBITS && (target->flags & IFL_CODEBITS) != IFL_CODEBITS) {
                room_number = target->room_number;
                GetHeight(GetFloor(target->pos.x_pos, target->pos.y_pos - 256, target->pos.z_pos, &room_number),
                    target->pos.x_pos, target->pos.y_pos - 256, target->pos.z_pos);
                TestTriggers(trigger_index, 1, target->flags & IFL_CODEBITS);
            } else {
                NumTrigs = (sint16)GetSwitchTrigger(target, TriggerItems, 1);
                for (i = 0; i < NumTrigs; i++) {
                    AddActiveItem(TriggerItems[i]);
                    items[TriggerItems[i]].status = ITEM_ACTIVE;
                    items[TriggerItems[i]].flags |= IFL_CODEBITS;
                }
            }
            if (target->object_number == SWITCH_TYPE7)
                ExplodeItemNode(target, objects[SWITCH_TYPE7].nmeshes - 1, 0, 64);
            AddActiveItem(target - items);
            target->flags |= IFL_CODEBITS | IFL_SWITCH_ONESHOT;
            target->status = ITEM_ACTIVE;
        }
    }
}

void ControlCrossbow(sint16 item_number)
{
    ITEM_INFO **itemlist;
    MESH_INFO **meshlist;
    ITEM_INFO *item, *target;
    MESH_INFO *mesh;
    FLOOR_INFO *floor;
    ROOM_INFO *r;
    sint32 abovewater, exploded, collided, rad, speed, i, ox, oy, oz;
    sint16 room_number;

    abovewater = 0;
    exploded = 0;
    collided = 0;
    item = &items[item_number];
    ox = item->pos.x_pos;
    oy = item->pos.y_pos;
    oz = item->pos.z_pos;
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        if (item->speed > 64)
            item->speed -= item->speed >> 4;
        if (GlobalCounter & 1)
            CreateBubble(&item->pos, item->room_number, 4, 7);
    } else
        abovewater = 1;
    item->pos.y_pos += ((rcossin_tbl[(-item->pos.x_rot >> 3) & 0x1FFE] << 2) * item->speed) >> W2V_SHIFT;
    speed = ((rcossin_tbl[((item->pos.x_rot >> 3) & 0x1FFE) + 1] << 2) * item->speed) >> W2V_SHIFT;
    item->pos.z_pos += ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * speed) >> W2V_SHIFT;
    item->pos.x_pos += ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * speed) >> W2V_SHIFT;
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) < item->pos.y_pos ||
        GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) > item->pos.y_pos) {
        item->pos.x_pos = ox;
        item->pos.y_pos = oy;
        item->pos.z_pos = oz;
        if (item->item_flags[0] == 3)
            exploded = 1;
        else {
            ExplodeItemNode(item, 0, 0, 256);
            KillItem(item_number);
            return;
        }
    }
    if (item->room_number != room_number)
        ItemNewRoom(item_number, room_number);
    r = &room[room_number];
    if (r->flags & ROOM_UNDERWATER && abovewater) {
        TriggerSmallSplash(item->pos.x_pos, r->maxceiling, item->pos.z_pos, 8);
        SetupRipple(item->pos.x_pos, room[room_number].maxceiling, item->pos.z_pos, (GetRandomControl() & 7) + 8, 0);
    }
    rad = 128;
    if (exploded)
        rad = 2048;
    i = 0;
    itemlist = (ITEM_INFO **)&tsv_buffer[0x2000];
    meshlist = (MESH_INFO **)&tsv_buffer[0x3000];
    for (; i < 2; i++) {
        GetCollidedObjects(item, rad, 1, itemlist, meshlist, 1);
        if (!*itemlist && !*meshlist)
            break;
        collided = 1;
        if (item->item_flags[0] == 3 && !exploded) {
            exploded = 1;
            rad = 2048;
            continue;
        }
        target = *itemlist;
        while (target) {
            if (exploded) {
                if (target->object_number >= SMASH_OBJECT1 && target->object_number <= SMASH_OBJECT8) {
                    TriggerExplosionSparks(target->pos.x_pos, target->pos.y_pos, target->pos.z_pos, 3, -2, 0, target->room_number);
                    target->pos.y_pos -= 128;
                    TriggerShockwave((PHD_VECTOR *)&target->pos, 0x1300030, 96, 0x18806000, 0);
                    target->pos.y_pos += 128;
                    ExplodeItemNode(target, 0, 0, 128);
                    SmashObject(target - items);
                    KillItem(target - items);
                } else if (target->object_number == SWITCH_TYPE7 || target->object_number == SWITCH_TYPE8)
                    CrossbowHitSwitchType78(item, target, 0);
                else if (objects[target->object_number].intelligent)
                    DoGrenadeDamageOnBaddie(target, item);
            } else if (target->object_number == SWITCH_TYPE7 || target->object_number == SWITCH_TYPE8 || target->object_number == SKELETON)
                CrossbowHitSwitchType78(item, target, 1);
            else if (objects[target->object_number].intelligent) {
                HitTarget(target, (GAME_VECTOR *)&item->pos, weapons[WEAPON_CROSSBOW].damage, 0);
                if (item->item_flags[0] == 2 && !objects[target->object_number].undead)
                    target->poisoned = 1;
            }
            itemlist++;
            target = *itemlist;
        }
        mesh = *meshlist;
        while (mesh) {
            if (mesh->static_number >= SHATTER0 && mesh->static_number < SHATTER8) {
                if (exploded) {
                    TriggerExplosionSparks(mesh->x, mesh->y, mesh->z, 3, -2, 0, item->room_number);
                    mesh->y -= 128;
                    TriggerShockwave((PHD_VECTOR *)&mesh->x, 0xB00028, 64, 0x10806000, 0);
                    mesh->y += 128;
                }
                ShatterObject(0, mesh, -128, item->room_number, 0);
                SmashedMeshRoom[SmashedMeshCount] = item->room_number;
                SmashedMesh[SmashedMeshCount] = mesh;
                SmashedMeshCount++;
                mesh->Flags &= ~1;
            }
            meshlist++;
            mesh = *meshlist;
        }
        break;
    }
    if (exploded) {
        if (room[item->room_number].flags & ROOM_UNDERWATER)
            TriggerUnderwaterExplosion(item, 0);
        else {
            item->pos.y_pos -= 128;
            TriggerShockwave((PHD_VECTOR *)&item->pos, 0x1300030, 96, 0x18806000, 0);
            item->pos.y_pos += 128;
            TriggerExplosionSparks(ox, oy, oz, 3, -2, 0, item->room_number);
            for (i = 0; i < 2; i++)
                TriggerExplosionSparks(ox, oy, oz, 3, -1, 0, item->room_number);
        }
        AlertNearbyGuards(item);
        SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800000 | SFX_SETPITCH);
        SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
    }
    if (collided || exploded) {
        ExplodeItemNode(item, 0, 0, 256);
        KillItem(item_number);
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

void AnimateShotgun(sint32 weapon_type)
{
    ITEM_INFO *item;
    PHD_VECTOR pos;
    static sint32 m16_firing;
    static sint32 harpoon_fired;

    item = &items[lara.weapon_item];
    if (SmokeCountL) {
        if (SmokeWeapon == WEAPON_GRENADE) {
            pos.x = 0;
            pos.y = 180;
            pos.z = 80;
        } else if (SmokeWeapon == WEAPON_SHOTGUN) {
            pos.x = -16;
            pos.y = 228;
            pos.z = 32;
        }
        GetLaraJointPos(&pos, 11);
        if (lara_item->mesh_bits)
            TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
    }
    switch (item->current_anim_state) {
    case 6:
        m16_firing = 0;
        if (harpoon_fired) {
            item->goal_anim_state = 5;
            harpoon_fired = 0;
        } else if (lara.water_status != LW_UNDERWATER)
            item->goal_anim_state = 0;
        else if ((input & IN_ACTION && !lara.target) || lara.left_arm.lock)
            item->goal_anim_state = 8;
        else
            item->goal_anim_state = 7;
        break;
    case 2:
        if (item->frame_number == anims[item->anim_number].frame_base) {
            item->goal_anim_state = 4;
            if (lara.water_status != LW_UNDERWATER && !harpoon_fired) {
                if (input & IN_ACTION) {
                    if (!lara.target || lara.left_arm.lock) {
                        if (weapon_type == WEAPON_GRENADE)
                            FireGrenade();
                        else if (weapon_type == WEAPON_CROSSBOW)
                            FireCrossbow(0);
                        else
                            FireShotgun();
                        item->goal_anim_state = 2;
                    }
                } else if (lara.left_arm.lock)
                    item->goal_anim_state = 0;
            }
            if (item->goal_anim_state != 2 && m16_firing) {
                SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x5000000 | SFX_SETPITCH);
                m16_firing = 0;
            }
        } else if (m16_firing) {
            SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x5000000 | SFX_SETPITCH);
            SoundEffect(78, &lara_item->pos, 0);
        } else if (weapon_type == WEAPON_SHOTGUN && !(input & IN_ACTION) && !lara.left_arm.lock)
            item->goal_anim_state = 4;
        if (item->frame_number - anims[item->anim_number].frame_base == 12 && weapon_type == WEAPON_SHOTGUN)
            TriggerGunShell(1, SHOTGUNSHELL, 4);
        break;
    case 0:
        m16_firing = 0;
        if (harpoon_fired) {
            item->goal_anim_state = 5;
            harpoon_fired = 0;
        } else if (lara.water_status != LW_UNDERWATER) {
            if ((input & IN_ACTION && !lara.target) || lara.left_arm.lock)
                item->goal_anim_state = 2;
            else
                item->goal_anim_state = 4;
        } else
            item->goal_anim_state = 6;
        break;
    }
    AnimateItem(item);
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
