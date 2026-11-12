#include "larafire.h"
#include "anim.h"
#include "objnums.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "laraflar.h"
#include "specific/3dmath.h"
#include "box.h"
#include "sound.h"
#include "savegame.h"
#include "effects.h"
#include "control.h"
#include "sphere.h"
#include "camera.h"
#include "flmtorch.h"
#include "gameflow.h"
#include "delstuff.h"
#include "lara_states.h"
#include "specific/input.h"

void LaraGun(void)
{
    sint16 state;

    if (lara.left_arm.flash_gun > 0)
        lara.left_arm.flash_gun--;
    if (lara.right_arm.flash_gun > 0)
        lara.right_arm.flash_gun--;
    if (lara.gun_type == WEAPON_TORCH) {
        DoFlameTorch();
        return;
    }
    if (lara_item->hit_points <= 0)
        lara.gun_status = LG_NO_ARMS;
    else if (lara.gun_status == LG_NO_ARMS) {
        if (input & IN_DRAW)
            lara.request_gun_type = lara.last_gun_type;
        else if (input & IN_FLARE && !(gfLevelFlags & GF_YOUNGLARA)) {
            if (lara_item->current_anim_state == AS_DUCK && lara_item->anim_number != ANIM_DUCKBREATHE)
                return;
            if (lara.gun_type == WEAPON_FLARE) {
                if (!lara.left_arm.frame_number)
                    lara.gun_status = LG_UNDRAW_GUNS;
            } else if (lara.num_flares) {
                if (lara.num_flares != -1)
                    lara.num_flares--;
                lara.request_gun_type = WEAPON_FLARE;
            }
        }
        if (input & IN_DRAW || lara.request_gun_type != lara.gun_type) {
            state = lara_item->current_anim_state;
            if ((state == AS_DUCK || state == AS_DUCKROTL || state == AS_DUCKROTR) &&
                (lara.request_gun_type == WEAPON_SHOTGUN || lara.request_gun_type == WEAPON_CROSSBOW || lara.request_gun_type == WEAPON_GRENADE)) {
                if (lara.gun_type == WEAPON_FLARE)
                    lara.request_gun_type = WEAPON_FLARE;
            } else if (lara.request_gun_type != WEAPON_FLARE && (lara.vehicle != NO_ITEM || (lara.water_status != LW_ABOVE_WATER &&
                (lara.water_status != LW_WADE || lara.water_surface_dist <= -weapons[lara.gun_type].gun_height)))) {
                lara.last_gun_type = lara.request_gun_type;
                if (lara.gun_type != WEAPON_FLARE)
                    lara.gun_type = lara.request_gun_type;
                else
                    lara.request_gun_type = WEAPON_FLARE;
            } else {
                if (lara.gun_type == WEAPON_FLARE) {
                    CreateFlare(FLARE_ITEM, 0);
                    undraw_flare_meshes();
                    lara.flare_control_left = 0;
                    lara.flare_age = 0;
                }
                lara.gun_type = lara.request_gun_type;
                InitialiseNewWeapon();
                lara.gun_status = LG_DRAW_GUNS;
                lara.right_arm.frame_number = 0;
                lara.left_arm.frame_number = 0;
            }
        }
    } else if (lara.gun_status == LG_READY) {
        if (input & IN_DRAW || lara.request_gun_type != lara.gun_type || (lara.water_status != LW_ABOVE_WATER &&
            (lara.water_status != LW_WADE || lara.water_surface_dist < -weapons[lara.gun_type].gun_height)))
            lara.gun_status = LG_UNDRAW_GUNS;
    } else if (lara.gun_status == LG_HANDS_BUSY && input & IN_FLARE && lara_item->current_anim_state == AS_ALL4S && lara_item->anim_number == ANIM_ALL4S)
        lara.request_gun_type = WEAPON_FLARE;
    switch (lara.gun_status) {
    case LG_DRAW_GUNS:
        if (lara.gun_type != WEAPON_FLARE && lara.gun_type != WEAPON_NONE)
            lara.last_gun_type = lara.gun_type;
        switch (lara.gun_type) {
        case WEAPON_PISTOLS:
        case WEAPON_REVOLVER:
        case WEAPON_UZI:
            if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA && camera.type != HEAVY_CAMERA)
                camera.type = COMBAT_CAMERA;
            draw_pistols(lara.gun_type);
            break;
        case WEAPON_SHOTGUN:
        case WEAPON_GRENADE:
        case WEAPON_CROSSBOW:
            if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA && camera.type != HEAVY_CAMERA)
                camera.type = COMBAT_CAMERA;
            draw_shotgun(lara.gun_type);
            break;
        case WEAPON_FLARE:
            draw_flare();
            break;
        default:
            lara.gun_status = LG_NO_ARMS;
            break;
        }
        break;
    case LG_FLARE:
        draw_flare();
        break;
    case LG_UNDRAW_GUNS:
        lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA].mesh_index + LM_HEAD * 2];
        switch (lara.gun_type) {
        case WEAPON_PISTOLS:
        case WEAPON_REVOLVER:
        case WEAPON_UZI:
            undraw_pistols(lara.gun_type);
            break;
        case WEAPON_SHOTGUN:
        case WEAPON_GRENADE:
        case WEAPON_CROSSBOW:
            undraw_shotgun(lara.gun_type);
            break;
        case WEAPON_FLARE:
            undraw_flare();
            break;
        }
        break;
    case LG_READY:
        if (input & IN_ACTION)
            lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA_SCREAM].mesh_index + LM_HEAD * 2];
        else
            lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA].mesh_index + LM_HEAD * 2];
        if (camera.type != CINEMATIC_CAMERA && camera.type != LOOK_CAMERA && camera.type != HEAVY_CAMERA)
            camera.type = COMBAT_CAMERA;
        if (input & IN_ACTION) {
            if (!*get_current_ammo_pointer(lara.gun_type)) {
                SoundEffect(48, &lara_item->pos, SFX_DEFAULT);
                lara.request_gun_type = WEAPON_PISTOLS;
                return;
            }
        }
        switch (lara.gun_type) {
        case WEAPON_PISTOLS:
        case WEAPON_UZI:
            PistolHandler(lara.gun_type);
            break;
        case WEAPON_REVOLVER:
        case WEAPON_SHOTGUN:
        case WEAPON_GRENADE:
        case WEAPON_CROSSBOW:
            RifleHandler(lara.gun_type);
            break;
        }
        break;
    case LG_NO_ARMS:
        if (lara.gun_type == WEAPON_FLARE) {
            if (lara.vehicle == NO_ITEM && !CheckForHoldingState(lara_item->current_anim_state))
                lara.flare_control_left = 0;
            else if (!lara.flare_control_left) {
                lara.flare_control_left = 1;
                lara.left_arm.frame_number = 95;
            } else if (lara.left_arm.frame_number) {
                lara.left_arm.frame_number++;
                if (lara.left_arm.frame_number == 110)
                    lara.left_arm.frame_number = 0;
            }
            DoFlareInHand(lara.flare_age);
            set_flare_arm(lara.left_arm.frame_number);
        }
        break;
    case LG_HANDS_BUSY:
        if (lara.gun_type == WEAPON_FLARE && lara.mesh_ptrs[LM_LHAND] == meshes[objects[FLARE_ANIM].mesh_index + LM_LHAND * 2]) {
            lara.flare_control_left = lara.vehicle != NO_ITEM || CheckForHoldingState(lara_item->current_anim_state);
            DoFlareInHand(lara.flare_age);
            set_flare_arm(lara.left_arm.frame_number);
        }
        break;
    }
}

sint32 CheckForHoldingState(sint32 state)
{
    sint16 *holds;

    holds = HoldStates;
    while (*holds >= 0) {
        if (state == *holds)
            return 1;
        holds++;
    }
    return 0;
}

void InitialiseNewWeapon(void)
{
    lara.left_arm.frame_number = lara.right_arm.frame_number = 0;
    lara.left_arm.x_rot = lara.left_arm.y_rot = lara.left_arm.z_rot = 0;
    lara.right_arm.x_rot = lara.right_arm.y_rot = lara.right_arm.z_rot = 0;
    lara.target = NULL;
    lara.left_arm.lock = lara.right_arm.lock = 0;
    lara.left_arm.flash_gun = lara.right_arm.flash_gun = 0;
    switch (lara.gun_type) {
    case WEAPON_PISTOLS:
    case WEAPON_UZI:
        lara.left_arm.frame_base = lara.right_arm.frame_base = objects[PISTOLS_ANIM].frame_base;
        if (lara.gun_status != LG_NO_ARMS)
            draw_pistol_meshes(lara.gun_type);
        break;
    case WEAPON_REVOLVER:
    case WEAPON_SHOTGUN:
    case WEAPON_GRENADE:
        lara.left_arm.frame_base = lara.right_arm.frame_base = objects[WeaponObject(lara.gun_type)].frame_base;
        if (lara.gun_status != LG_NO_ARMS)
            draw_shotgun_meshes(lara.gun_type);
        break;
    case WEAPON_FLARE:
        lara.left_arm.frame_base = lara.right_arm.frame_base = objects[FLARE_ANIM].frame_base;
        if (lara.gun_status != LG_NO_ARMS)
            draw_flare_meshes();
        break;
    default:
        lara.left_arm.frame_base = lara.right_arm.frame_base = anims[lara_item->anim_number].frame_ptr;
        break;
    }
}

void LaraTargetInfo(WEAPON_INFO *winfo)
{
    GAME_VECTOR src, target;
    ITEM_INFO *item;
    sint16 ang[2];

    item = lara.target;
    if (!item) {
        lara.right_arm.lock = 0;
        lara.left_arm.lock = 0;
        lara.target_angles[1] = 0;
        lara.target_angles[0] = 0;
        return;
    }
    src.x = 0;
    src.y = 0;
    src.z = 0;
    GetLaraJointPos((PHD_VECTOR *)&src, 11);
    src.x = lara_item->pos.x_pos;
    src.z = lara_item->pos.z_pos;
    src.room_number = lara_item->room_number;
    find_target_point(item, &target);
    phd_GetVectorAngles(target.x - src.x, target.y - src.y, target.z - src.z, ang);
    ang[0] -= lara_item->pos.y_rot;
    ang[1] -= lara_item->pos.x_rot;
    if (LOS(&src, &target)) {
        if (ang[0] >= winfo->lock_angles[0] && ang[0] <= winfo->lock_angles[1] && ang[1] >= winfo->lock_angles[2] && ang[1] <= winfo->lock_angles[3]) {
            lara.right_arm.lock = 1;
            lara.left_arm.lock = 1;
            lara.target_angles[0] = ang[0];
            lara.target_angles[1] = ang[1];
            return;
        }
        if (lara.left_arm.lock) {
            if (ang[0] < winfo->left_angles[0] || ang[0] > winfo->left_angles[1] ||
                ang[1] < winfo->left_angles[2] || ang[1] > winfo->left_angles[3])
                lara.left_arm.lock = 0;
        }
        if (lara.right_arm.lock) {
            if (ang[0] < winfo->right_angles[0] || ang[0] > winfo->right_angles[1] ||
                ang[1] < winfo->right_angles[2] || ang[1] > winfo->right_angles[3]) {
                lara.right_arm.lock = 0;
                lara.target_angles[0] = ang[0];
                lara.target_angles[1] = ang[1];
                return;
            }
        }
    } else {
        lara.right_arm.lock = 0;
        lara.left_arm.lock = 0;
    }
    lara.target_angles[0] = ang[0];
    lara.target_angles[1] = ang[1];
}

void find_target_point(ITEM_INFO *item, GAME_VECTOR *target)
{
    sint32 x, y, z, c, s;
    sint16 *bounds;

    bounds = GetBestFrame(item);
    x = (bounds[0] + bounds[1]) / 2;
    y = (bounds[2] + (bounds[3] - bounds[2]) / 3);
    z = (bounds[4] + bounds[5]) / 2;
    c = phd_cos(item->pos.y_rot);
    s = phd_sin(item->pos.y_rot);
    target->x = item->pos.x_pos + ((x * c + z * s) >> W2V_SHIFT);
    target->y = item->pos.y_pos + y;
    target->z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);
    target->room_number = item->room_number;
}

sint32 FireWeapon(sint32 weapon_type, ITEM_INFO *target, ITEM_INFO *src, sint16 *angles)
{
    SPHERE *sptr;
    sint16 *ammo;
    sint32 r, i, nSpheres, bestdist, best;
    sint16 room_number;
    WEAPON_INFO *winfo;

    bum_view.x_pos = 0;
    bum_view.y_pos = 0;
    bum_view.z_pos = 0;
    GetLaraJointPos((PHD_VECTOR *)&bum_view, 11);
    ammo = get_current_ammo_pointer(weapon_type);
    if (*ammo) {
        if (*ammo != -1)
            (*ammo)--;
        winfo = &weapons[weapon_type];
        bum_view.x_pos = src->pos.x_pos;
        bum_view.z_pos = src->pos.z_pos;
        r = (GetRandomControl() - 0x4000) * winfo->shot_accuracy / 0x10000;
        bum_view.x_rot = angles[1] + (sint16)r;
        r = (GetRandomControl() - 0x4000) * winfo->shot_accuracy / 0x10000;
        bum_view.y_rot = angles[0] + (sint16)r;
        bum_view.z_rot = 0;
        phd_GenerateW2V(&bum_view);
        nSpheres = GetSpheres(target, Slist, 0);
        best = -1;
        bestdist = 0x7FFFFFFF;
        for (i = 0; i < nSpheres; i++) {
            sptr = &Slist[i];
            r = sptr->r;
            if (ABS(sptr->x) < r && ABS(sptr->y) < r && sptr->z > r && SQUARE(sptr->x) + SQUARE(sptr->y) <= SQUARE(r)) {
                if (sptr->z - r < bestdist) {
                    bestdist = sptr->z - r;
                    best = i;
                }
            }
        }
        lara.has_fired = 1;
        bum_vsrc.x = bum_view.x_pos;
        bum_vsrc.y = bum_view.y_pos;
        bum_vsrc.z = bum_view.z_pos;
        room_number = src->room_number;
        GetFloor(bum_view.x_pos, bum_view.y_pos, bum_view.z_pos, &room_number);
        bum_vsrc.room_number = room_number;
        if (best >= 0) {
            savegame.Game.AmmoHits++;
            bum_vdest.x = bum_vsrc.x + (bestdist * phd_mxptr[M20] >> W2V_SHIFT);
            bum_vdest.y = bum_vsrc.y + (bestdist * phd_mxptr[M21] >> W2V_SHIFT);
            bum_vdest.z = bum_vsrc.z + (bestdist * phd_mxptr[M22] >> W2V_SHIFT);
            if (!GetTargetOnLOS(&bum_vsrc, &bum_vdest, 0, 1))
                HitTarget(target, &bum_vdest, winfo->damage, 0);
            return 1;
        }
        bum_vdest.x = bum_vsrc.x + (0x5000 * phd_mxptr[M20] >> W2V_SHIFT);
        bum_vdest.y = bum_vsrc.y + (0x5000 * phd_mxptr[M21] >> W2V_SHIFT);
        bum_vdest.z = bum_vsrc.z + (0x5000 * phd_mxptr[M22] >> W2V_SHIFT);
        GetTargetOnLOS(&bum_vsrc, &bum_vdest, 0, 1);
        return -1;
    }
    return 0;
}

void HitTarget(ITEM_INFO *item, GAME_VECTOR *hitpos, sint32 damage, sint32 grenade)
{
    OBJECT_INFO *obj;

    obj = &objects[item->object_number];
    item->hit_status = 1;
    if (item->data && item != lara_item)
        ((CREATURE_INFO *)item->data)->hurt_by_lara = 1;
    if (hitpos && obj->HitEffect) {
        if (obj->HitEffect == 1) {
            if (item->object_number == SUPER_RAGHEAD && (item->current_anim_state == 8 || GetRandomControl() & 1) && (lara.gun_type == WEAPON_PISTOLS || lara.gun_type == WEAPON_SHOTGUN || lara.gun_type == WEAPON_UZI)) {
                SoundEffect(SFX_BAD_SWORD_RICO, &item->pos, SFX_DEFAULT);
                TriggerRicochetSpark(hitpos, lara_item->pos.y_rot, 3, 0);
                return;
            }
            DoBloodSplat(hitpos->x, hitpos->y, hitpos->z, (GetRandomControl() & 3) + 3, item->pos.y_rot, item->room_number);
        } else if (obj->HitEffect == 3)
            TriggerRicochetSpark(hitpos, lara_item->pos.y_rot, 3, 0);
        else if (obj->HitEffect == 2)
            TriggerRicochetSpark(hitpos, lara_item->pos.y_rot, 3, -5);
    }
    if (!obj->undead || grenade || item->hit_points == -16384) {
        if (item->hit_points > 0 && item->hit_points <= damage)
            savegame.Level.Kills++;
        item->hit_points -= (sint16)damage;
    }
}

sint32 WeaponObject(sint32 weapon_type)
{
    switch (weapon_type) {
    case WEAPON_REVOLVER:
        return SIXSHOOTER_ANIM;
    case WEAPON_UZI:
        return UZI_ANIM;
    case WEAPON_SHOTGUN:
        return SHOTGUN_ANIM;
    case WEAPON_GRENADE:
        return GRENADE_GUN_ANIM;
    case WEAPON_CROSSBOW:
        return CROSSBOW_ANIM;
    default:
        return PISTOLS_ANIM;
    }
}

sint32 WeaponObjectMesh(sint32 weapon_type)
{
    switch (weapon_type) {
    case WEAPON_REVOLVER:
        if (lara.sixshooter_type_carried & 4)
            return LARA_REVOLVER_LASER;
        else
            return SIXSHOOTER_ANIM;
    case WEAPON_UZI:
        return UZI_ANIM;
    case WEAPON_SHOTGUN:
        return SHOTGUN_ANIM;
    case WEAPON_GRENADE:
        return GRENADE_GUN_ANIM;
    case WEAPON_CROSSBOW:
        if (lara.crossbow_type_carried & 4)
            return LARA_CROSSBOW_LASER;
        else
            return CROSSBOW_ANIM;
    default:
        return PISTOLS_ANIM;
    }
}

void DoProperDetection(sint16 item_number, sint32 x, sint32 y, sint32 z, sint32 xv, sint32 yv, sint32 zv)
{
    ITEM_INFO *item;
    sint16 room_number;
    sint32 ceiling, height, oldtype, oldonobj, oldheight;
    sint32 bs, yang, xs;
    FLOOR_INFO *floor;

    item = &items[item_number];
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    oldheight = GetHeight(floor, x, y, z);
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    height = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (item->pos.y_pos >= height) {
        bs = 0;
        oldtype = height_type;
        if ((oldtype == BIG_SLOPE || oldtype == DIAGONAL) && oldheight < height) {
            yang = (sint32)((uint16)item->pos.y_rot);
            if (tiltyoff < 0) {
                if (yang >= 0x8000)
                    bs = 1;
            } else if (tiltyoff > 0) {
                if (yang <= 0x8000)
                    bs = 1;
            }
            if (tiltxoff < 0) {
                if (yang >= 0x4000 && yang <= 0xc000)
                    bs = 1;
            } else if (tiltxoff > 0) {
                if (yang <= 0x4000 || yang >= 0xc000)
                    bs = 1;
            }
        }
        if (y > height + 32 && !bs && ((item->pos.x_pos ^ x) & ~0x3FF || (item->pos.z_pos ^ z) & ~0x3FF)) {
            if ((item->pos.x_pos ^ x) & ~0x3FF && (item->pos.z_pos ^ z) & ~0x3FF)
                xs = abs(x - item->pos.x_pos) < abs(z - item->pos.z_pos);
            else
                xs = 1;
            if ((item->pos.x_pos ^ x) & ~0x3FF && xs) {
                if (xv <= 0)
                    item->pos.y_rot = -item->pos.y_rot;
                else
                    item->pos.y_rot = -item->pos.y_rot;
            } else
                item->pos.y_rot = 0x8000 - item->pos.y_rot;
            item->pos.x_pos = x;
            item->pos.y_pos = y;
            item->pos.z_pos = z;
            item->speed >>= 1;
        } else if (oldtype != BIG_SLOPE && oldtype != DIAGONAL) {
            if (item->fallspeed > 0) {
                if (item->fallspeed > 16) {
                    if (item->object_number == GRENADE)
                        item->fallspeed = (item->fallspeed >> 1) - item->fallspeed;
                    else {
                        item->fallspeed = -(item->fallspeed >> 2);
                        if (item->fallspeed < -100)
                            item->fallspeed = -100;
                    }
                } else {
                    item->fallspeed = 0;
                    if (item->object_number == GRENADE) {
                        item->speed--;
                        item->required_anim_state = 1;
                        item->pos.x_rot = 0;
                    } else
                        item->speed -= 3;
                    if (item->speed < 0)
                        item->speed = 0;
                }
            }
            item->pos.y_pos = height;
        } else {
            item->speed -= item->speed >> 2;
            if (tiltyoff < 0 && abs(tiltyoff) - abs(tiltxoff) >= 2) {
                if ((uint16)item->pos.y_rot > 32768) {
                    item->pos.y_rot = -1 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed -= (sint16)(2 * tiltyoff);
                        if ((uint16)item->pos.y_rot > 16384 && (uint16)item->pos.y_rot < 49152) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot < 16384)
                                item->pos.y_rot = 16384;
                        } else if ((uint16)item->pos.y_rot < 16384) {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot > 16384)
                                item->pos.y_rot = 16384;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            } else if (tiltyoff > 0 && abs(tiltyoff) - abs(tiltxoff) >= 2) {
                if ((uint16)item->pos.y_rot < 32768) {
                    item->pos.y_rot = -1 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed += (sint16)(2 * tiltyoff);
                        if ((uint16)item->pos.y_rot > 49152 || (uint16)item->pos.y_rot < 16384) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot < 49152)
                                item->pos.y_rot = -16384;
                        } else if ((uint16)item->pos.y_rot < 49152) {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot > 49152)
                                item->pos.y_rot = -16384;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            } else if (tiltxoff < 0 && abs(tiltxoff) - abs(tiltyoff) >= 2) {
                if ((uint16)item->pos.y_rot > 16384 && (uint16)item->pos.y_rot < 49152) {
                    item->pos.y_rot = 32767 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed -= (sint16)(2 * tiltxoff);
                        if ((uint16)item->pos.y_rot < 32768) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot > 61440)
                                item->pos.y_rot = 0;
                        } else {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot < 4096)
                                item->pos.y_rot = 0;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            } else if (tiltxoff > 0 && abs(tiltxoff) - abs(tiltyoff) >= 2) {
                if ((uint16)item->pos.y_rot > 49152 || (uint16)item->pos.y_rot < 16384) {
                    item->pos.y_rot = 32767 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed += (sint16)(2 * tiltxoff);
                        if ((uint16)item->pos.y_rot > 32768) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot < 32768)
                                item->pos.y_rot = -32768;
                        } else if ((uint16)item->pos.y_rot < 32768) {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot > 32768)
                                item->pos.y_rot = -32768;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            } else if (tiltyoff < 0 && tiltxoff < 0) {
                if ((uint16)item->pos.y_rot > 24576 && (uint16)item->pos.y_rot < 57344) {
                    item->pos.y_rot = -16385 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed -= (sint16)(tiltxoff + tiltyoff);
                        if ((uint16)item->pos.y_rot > 8192 && (uint16)item->pos.y_rot < 40960) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot < 8192)
                                item->pos.y_rot = 8192;
                        } else if (item->pos.y_rot != 8192) {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot > 8192)
                                item->pos.y_rot = 8192;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            } else if (tiltyoff < 0 && tiltxoff > 0) {
                if ((uint16)item->pos.y_rot > 40960 || (uint16)item->pos.y_rot < 8192) {
                    item->pos.y_rot = 16383 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed += (sint16)(tiltxoff - tiltyoff);
                        if ((uint16)item->pos.y_rot < 57344 && (uint16)item->pos.y_rot > 24576) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot < 24576)
                                item->pos.y_rot = 24576;
                        } else if (item->pos.y_rot != 24576) {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot > 24576)
                                item->pos.y_rot = 24576;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            } else if (tiltyoff > 0 && tiltxoff > 0) {
                if ((uint16)item->pos.y_rot > 57344 || (uint16)item->pos.y_rot < 24576) {
                    item->pos.y_rot = -16385 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed += (sint16)(tiltxoff + tiltyoff);
                        if ((uint16)item->pos.y_rot < 8192 || (uint16)item->pos.y_rot > 40960) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot < 40960)
                                item->pos.y_rot = -24576;
                        } else if ((uint16)item->pos.y_rot != 40960) {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot > 40960)
                                item->pos.y_rot = -24576;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            } else if (tiltyoff > 0 && tiltxoff < 0) {
                if ((uint16)item->pos.y_rot > 8192 && (uint16)item->pos.y_rot < 40960) {
                    item->pos.y_rot = 16383 - item->pos.y_rot;
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                } else {
                    if (item->speed < 32) {
                        item->speed += (sint16)(tiltyoff - tiltxoff);
                        if ((uint16)item->pos.y_rot < 24576 || (uint16)item->pos.y_rot > 57344) {
                            item->pos.y_rot -= 4096;
                            if ((uint16)item->pos.y_rot < 57344)
                                item->pos.y_rot = -8192;
                        } else if ((uint16)item->pos.y_rot != 57344) {
                            item->pos.y_rot += 4096;
                            if ((uint16)item->pos.y_rot > 57344)
                                item->pos.y_rot = -8192;
                        }
                    }
                    if (item->fallspeed > 0)
                        item->fallspeed = -(item->fallspeed >> 1);
                    else
                        item->fallspeed = 0;
                }
            }
            item->pos.x_pos = x;
            item->pos.y_pos = y;
            item->pos.z_pos = z;
        }
    } else {
        if (yv >= 0) {
            room_number = item->room_number;
            floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
            oldheight = GetHeight(floor, item->pos.x_pos, y, item->pos.z_pos);
            oldonobj = OnObject;
            room_number = item->room_number;
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            if (item->pos.y_pos >= oldheight && oldonobj) {
                if (item->fallspeed > 0) {
                    if (item->fallspeed > 16) {
                        if (item->object_number == GRENADE)
                            item->fallspeed = (item->fallspeed >> 1) - item->fallspeed;
                        else {
                            item->fallspeed = -(item->fallspeed >> 2);
                            if (item->fallspeed < -100)
                                item->fallspeed = -100;
                        }
                    } else {
                        item->fallspeed = 0;
                        if (item->object_number == GRENADE) {
                            item->required_anim_state = 1;
                            item->pos.x_rot = 0;
                            item->speed--;
                        } else
                            item->speed -= 3;
                        if (item->speed < 0)
                            item->speed = 0;
                    }
                }
                item->pos.y_pos = oldheight;
            }
        }
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        ceiling = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        if (item->pos.y_pos < ceiling) {
            if (y < ceiling
                && (((x >> WALL_SHIFT) != (item->pos.x_pos >> WALL_SHIFT))
                    || ((z >> WALL_SHIFT) != (item->pos.z_pos >> WALL_SHIFT)))) {
                if ((x & (~(WALL_L - 1))) != (item->pos.x_pos & (~(WALL_L - 1)))) {
                    if (xv <= 0)
                        item->pos.y_rot = 0x4000 + (0xc000 - item->pos.y_rot);
                    else
                        item->pos.y_rot = 0xc000 + (0x4000 - item->pos.y_rot);
                } else {
                    item->pos.y_rot = 0x8000 - item->pos.y_rot;
                }
                if (item->object_number == GRENADE)
                    item->speed -= item->speed >> 3;
                else
                    item->speed >>= 1;
                item->pos.x_pos = x;
                item->pos.y_pos = y;
                item->pos.z_pos = z;
            } else
                item->pos.y_pos = ceiling;
            if (item->fallspeed < 0)
                item->fallspeed = -item->fallspeed;
        }
    }
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (room_number != item->room_number)
        ItemNewRoom(item_number, room_number);
}

sint16 *get_current_ammo_pointer(sint32 weapon_type)
{
    sint16 *ammo;

    switch (weapon_type) {
    case WEAPON_SHOTGUN:
        if (lara.shotgun_type_carried & 8)
            ammo = &lara.num_shotgun_ammo1;
        else
            ammo = &lara.num_shotgun_ammo2;
        break;
    case WEAPON_REVOLVER:
        ammo = &lara.num_revolver_ammo;
        break;
    case WEAPON_UZI:
        ammo = &lara.num_uzi_ammo;
        break;
    case WEAPON_GRENADE:
        if (lara.grenade_type_carried & 8)
            ammo = &lara.num_grenade_ammo1;
        else if (lara.grenade_type_carried & 0x10)
            ammo = &lara.num_grenade_ammo2;
        else
            ammo = &lara.num_grenade_ammo3;
        break;
    case WEAPON_CROSSBOW:
        if (lara.crossbow_type_carried & 8)
            ammo = &lara.num_crossbow_ammo1;
        else if (lara.crossbow_type_carried & 0x10)
            ammo = &lara.num_crossbow_ammo2;
        else
            ammo = &lara.num_crossbow_ammo3;
        break;
    default:
        ammo = &lara.num_pistols_ammo;
        break;
    }
    return ammo;
}
