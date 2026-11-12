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
