#include "lara2gun.h"
#include "larafire.h"
#include "anim.h"
#include "sound.h"
#include "specific/input.h"
#include "delstuff.h"

void draw_pistols(sint32 weapon_type)
{
    sint16 ani;
    PISTOL_DEF *p;

    p = &PistolTable[lara.gun_type];
    ani = lara.left_arm.frame_number + 1;
    if (ani < p->Draw1Anim || ani > p->RecoilAnim - 1)
        ani = p->Draw1Anim;
    else if (ani == p->Draw2Anim) {
        draw_pistol_meshes(weapon_type);
        SoundEffect(6, &lara_item->pos, 0);
    } else if (ani == p->RecoilAnim - 1) {
        ready_pistols(weapon_type);
        ani = 0;
    }
    set_arm_info(&lara.right_arm, ani);
    set_arm_info(&lara.left_arm, ani);
}

void set_arm_info(LARA_ARM *arm, sint32 frame)
{
    sint32 anim_base;
    PISTOL_DEF *p;

    p = &PistolTable[lara.gun_type];
    anim_base = objects[p->ObjectNum].anim_index;
    if (frame < p->Draw1Anim)
        arm->anim_number = anim_base;
    else if (frame < p->Draw2Anim)
        arm->anim_number = anim_base + 1;
    else if (frame < p->RecoilAnim)
        arm->anim_number = anim_base + 2;
    else
        arm->anim_number = anim_base + 3;
    arm->frame_base = anims[arm->anim_number].frame_ptr;
    arm->frame_number = frame;
}

void undraw_pistols(sint32 weapon_type)
{
    PISTOL_DEF *p;
    sint16 anil, anir;

    p = &PistolTable[lara.gun_type];
    anil = lara.left_arm.frame_number;
    if (anil >= p->RecoilAnim) {
        anil = p->Draw1Anim2;
    } else if (anil > 0 && anil < p->Draw1Anim) {
        lara.left_arm.x_rot -= lara.left_arm.x_rot / anil;
        lara.left_arm.y_rot -= lara.left_arm.y_rot / anil;
        anil--;
    } else if (anil == 0) {
        lara.left_arm.x_rot = lara.left_arm.y_rot = lara.left_arm.z_rot = 0;
        anil = p->RecoilAnim - 1;
    } else if (anil > p->Draw1Anim) {
        anil--;
        if (anil == p->Draw2Anim - 1) {
            undraw_pistol_mesh_left(weapon_type);
            SoundEffect(7, &lara_item->pos, 0);
        }
    }
    set_arm_info(&lara.left_arm, anil);
    anir = lara.right_arm.frame_number;
    if (anir >= p->RecoilAnim) {
        anir = p->Draw1Anim2;
    } else if (anir > 0 && anir < p->Draw1Anim) {
        lara.right_arm.x_rot -= lara.right_arm.x_rot / anir;
        lara.right_arm.y_rot -= lara.right_arm.y_rot / anir;
        anir--;
    } else if (anir == 0) {
        lara.right_arm.x_rot = lara.right_arm.y_rot = lara.right_arm.z_rot = 0;
        anir = p->RecoilAnim - 1;
    } else if (anir > p->Draw1Anim) {
        anir--;
        if (anir == p->Draw2Anim - 1) {
            undraw_pistol_mesh_right(weapon_type);
            SoundEffect(7, &lara_item->pos, 0);
        }
    }
    set_arm_info(&lara.right_arm, anir);
    if (anil == p->Draw1Anim && anir == p->Draw1Anim) {
        lara.gun_status = LG_NO_ARMS;
        lara.right_arm.frame_number = lara.left_arm.frame_number = 0;
        lara.target = NULL;
        lara.left_arm.lock = lara.right_arm.lock = 0;
    }
    if (!(input & IN_LOOK)) {
        lara.torso_y_rot = lara.head_y_rot = (lara.left_arm.y_rot + lara.right_arm.y_rot) / 4;
        lara.torso_x_rot = lara.head_x_rot = (lara.left_arm.x_rot + lara.right_arm.x_rot) / 4;
    }
}

void ready_pistols(int weapon_type)
{
    lara.gun_status = LG_READY;
    lara.left_arm.x_rot = lara.left_arm.y_rot = lara.left_arm.z_rot = 0;
    lara.right_arm.x_rot = lara.right_arm.y_rot = lara.right_arm.z_rot = 0;
    lara.left_arm.frame_number = lara.right_arm.frame_number = 0;
    lara.target = NULL;
    lara.left_arm.lock = lara.right_arm.lock = 0;
    lara.left_arm.frame_base = lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;
}

void undraw_pistol_mesh_left(sint32 weapon_type)
{
    if (weapon_type != WEAPON_REVOLVER) {
        WeaponObject(weapon_type);
        lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
        if (weapon_type == WEAPON_PISTOLS)
            lara.holster = LARA_HOLSTERS_PISTOLS;
    }
}
