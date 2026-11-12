#include "lara2gun.h"
#include "larafire.h"
#include "anim.h"
#include "sound.h"
#include "specific/input.h"
#include "delstuff.h"
#include "tomb4fx.h"
#include "effect2.h"
#include "savegame.h"
#include "objnums.h"
#include "camera.h"
#include "gameflow.h"
#include "specific/function_stubs.h"

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

void draw_pistol_meshes(sint32 weapon_type)
{
    sint32 mesh_index;

    mesh_index = objects[WeaponObjectMesh(weapon_type)].mesh_index;
    lara.mesh_ptrs[LM_RHAND] = meshes[mesh_index + LM_RHAND * 2];
    lara.holster = LARA_HOLSTERS;
    if (weapon_type != WEAPON_REVOLVER)
        lara.mesh_ptrs[LM_LHAND] = meshes[mesh_index + LM_LHAND * 2];
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

void undraw_pistol_mesh_right(sint32 weapon_type)
{
    WeaponObject(weapon_type);
    lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + LM_RHAND * 2];
    if (weapon_type == WEAPON_PISTOLS)
        lara.holster = LARA_HOLSTERS_PISTOLS;
}

void PistolHandler(sint32 weapon_type)
{
    WEAPON_INFO *winfo;
    PHD_VECTOR pos;

    winfo = &weapons[weapon_type];
    LaraGetNewTarget(winfo);
    if (input & IN_ACTION)
        LaraTargetInfo(winfo);
    AimWeapon(winfo, &lara.left_arm);
    AimWeapon(winfo, &lara.right_arm);
    if (lara.left_arm.lock && !lara.right_arm.lock) {
        lara.torso_y_rot = lara.left_arm.y_rot / 2;
        lara.torso_x_rot = lara.left_arm.x_rot / 2;
        if (camera.old_type != LOOK_CAMERA) {
            lara.head_y_rot = lara.torso_y_rot;
            lara.head_x_rot = lara.torso_x_rot;
        }
    } else if (lara.right_arm.lock && !lara.left_arm.lock) {
        lara.torso_y_rot = lara.right_arm.y_rot / 2;
        lara.torso_x_rot = lara.right_arm.x_rot / 2;
        if (camera.old_type != LOOK_CAMERA) {
            lara.head_y_rot = lara.torso_y_rot;
            lara.head_x_rot = lara.torso_x_rot;
        }
    } else if (lara.left_arm.lock && lara.right_arm.lock) {
        lara.torso_y_rot = (lara.left_arm.y_rot + lara.right_arm.y_rot) / 4;
        lara.torso_x_rot = (lara.left_arm.x_rot + lara.right_arm.x_rot) / 4;
        if (camera.old_type != LOOK_CAMERA) {
            lara.head_y_rot = lara.torso_y_rot;
            lara.head_x_rot = lara.torso_x_rot;
        }
    }
    AnimatePistols(weapon_type);
    if (lara.left_arm.flash_gun || lara.right_arm.flash_gun) {
        pos.x = (GetRandomControl() & 255) - 128;
        pos.y = (GetRandomControl() & 127) - 63;
        pos.z = (GetRandomControl() & 255) - 128;
        GetLaraJointPos(&pos, lara.left_arm.flash_gun ? 11 : 14);
        if ((gfLevelFlags & GF_MIRROR) && lara_item->room_number == gfMirrorRoom)
            TriggerDynamic_MIRROR(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3f) + 192, (GetRandomControl() & 0x1f) + 128, GetRandomControl() & 0x3f);
        else
            TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3f) + 192, (GetRandomControl() & 0x1f) + 128, GetRandomControl() & 0x3f);
    }
}

void AnimatePistols(sint32 weapon_type)
{
    PISTOL_DEF *p;
    WEAPON_INFO *winfo;
    sint16 angles[2];
    PHD_VECTOR pos;
    static sint32 uzi_left;
    static sint32 uzi_right;
    sint16 anil, anir, sound_already;

    sound_already = 0;
    if (lara_item->mesh_bits) {
        if (SmokeCountL) {
            if (SmokeWeapon == WEAPON_PISTOLS) {
                pos.x = 4;
                pos.y = 128;
                pos.z = 40;
            } else if (SmokeWeapon == WEAPON_REVOLVER) {
                pos.x = 16;
                pos.y = 160;
                pos.z = 56;
            } else if (SmokeWeapon == WEAPON_UZI) {
                pos.x = 8;
                pos.y = 140;
                pos.z = 48;
            }
            GetLaraJointPos(&pos, 14);
            TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
        }
        if (SmokeCountR) {
            if (SmokeWeapon == WEAPON_PISTOLS) {
                pos.x = -16;
                pos.y = 128;
                pos.z = 40;
            } else if (SmokeWeapon == WEAPON_REVOLVER) {
                pos.x = -32;
                pos.y = 160;
                pos.z = 56;
            } else if (SmokeWeapon == WEAPON_UZI) {
                pos.x = -16;
                pos.y = 140;
                pos.z = 48;
            }
            GetLaraJointPos(&pos, 11);
            TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountR);
        }
    }
    winfo = &weapons[weapon_type];
    p = &PistolTable[lara.gun_type];
    anir = lara.right_arm.frame_number;
    if (lara.right_arm.lock || ((input & IN_ACTION) && !lara.target)) {
        if (anir >= 0 && anir < p->Draw1Anim2)
            anir++;
        else if (anir == p->Draw1Anim2) {
            if (input & IN_ACTION) {
                if (weapon_type != WEAPON_REVOLVER) {
                    angles[0] = lara.right_arm.y_rot + lara_item->pos.y_rot;
                    angles[1] = lara.right_arm.x_rot;
                    if (FireWeapon(weapon_type, lara.target, lara_item, angles)) {
                        SmokeCountR = 28;
                        SmokeWeapon = weapon_type;
                        TriggerGunShell(1, GUNSHELL, weapon_type);
                        lara.right_arm.flash_gun = winfo->flash_time;
                        SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x2000000 | 4);
                        SoundEffect(winfo->sample_num, &lara_item->pos, 0);
                        sound_already = 1;
                        if (weapon_type == WEAPON_UZI)
                            uzi_right = 1;
                        savegame.Game.AmmoUsed++;
                    }
                }
                anir = p->RecoilAnim;
            } else if (uzi_right) {
                SoundEffect(winfo->sample_num + 1, &lara_item->pos, 0);
                uzi_right = 0;
            }
        } else if (anir >= p->RecoilAnim) {
            if (weapon_type == WEAPON_UZI) {
                SoundEffect(winfo->sample_num, &lara_item->pos, 0);
                uzi_right = 1;
            }
            anir++;
            if (anir == p->RecoilAnim + winfo->recoil_frame)
                anir = p->Draw1Anim2;
        }
    } else {
        if (anir >= p->RecoilAnim)
            anir = p->Draw1Anim2;
        else if (anir > 0 && anir <= p->Draw1Anim2)
            anir--;
        if (uzi_right) {
            SoundEffect(winfo->sample_num + 1, &lara_item->pos, 0);
            uzi_right = 0;
        }
    }
    set_arm_info(&lara.right_arm, anir);
    anil = lara.left_arm.frame_number;
    if (lara.left_arm.lock || ((input & IN_ACTION) && !lara.target)) {
        if (anil >= 0 && anil < p->Draw1Anim2)
            anil++;
        else if (anil == p->Draw1Anim2) {
            if (input & IN_ACTION) {
                angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
                angles[1] = lara.left_arm.x_rot;
                if (FireWeapon(weapon_type, lara.target, lara_item, angles)) {
                    if (weapon_type == WEAPON_REVOLVER) {
                        SmokeCountR = 28;
                        SmokeWeapon = WEAPON_REVOLVER;
                        lara.right_arm.flash_gun = winfo->flash_time;
                    } else {
                        SmokeCountL = 28;
                        SmokeWeapon = weapon_type;
                        TriggerGunShell(0, GUNSHELL, weapon_type);
                        lara.left_arm.flash_gun = winfo->flash_time;
                    }
                    if (!sound_already) {
                        SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x2000000 | 4);
                        SoundEffect(winfo->sample_num, &lara_item->pos, 0);
                    }
                    if (weapon_type == WEAPON_UZI)
                        uzi_left = 1;
                    savegame.Game.AmmoUsed++;
                }
                anil = p->RecoilAnim;
            } else if (uzi_left) {
                SoundEffect(winfo->sample_num + 1, &lara_item->pos, 0);
                uzi_left = 0;
            }
        } else if (anil >= p->RecoilAnim) {
            if (weapon_type == WEAPON_UZI) {
                SoundEffect(winfo->sample_num, &lara_item->pos, 0);
                uzi_left = 1;
            }
            anil++;
            if (anil == p->RecoilAnim + winfo->recoil_frame)
                anil = p->Draw1Anim2;
        }
    } else {
        if (anil >= p->RecoilAnim)
            anil = p->Draw1Anim2;
        else if (anil > 0 && anil <= p->Draw1Anim2)
            anil--;
        if (uzi_left) {
            SoundEffect(winfo->sample_num + 1, &lara_item->pos, 0);
            uzi_left = 0;
        }
    }
    set_arm_info(&lara.left_arm, anil);
}
