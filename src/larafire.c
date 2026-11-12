#include "larafire.h"
#include "anim.h"
#include "objnums.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "laraflar.h"

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
