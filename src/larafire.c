#include "larafire.h"
#include "anim.h"
#include "objnums.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "laraflar.h"
#include "specific/3dmath.h"
#include "box.h"
#include "sound.h"

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
