#include <string.h>

#include "voncroy.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "camera.h"
#include "lara.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/input.h"
#include "specific/function_stubs.h"
#include "tomb4fx.h"
#include "spotcam.h"
#include "sound.h"
#include "lara_states.h"
#include "items.h"
#include "effects.h"
#include "lot.h"
#include "savegame.h"
#include "switch.h"

void SetCutSceneCamera(ITEM_INFO *item)
{
    VONCROY_CUT_DATA *cut;
    sint32 flags;

    bVoncroyCutScene = 1;
    camera.old_type = FIXED_CAMERA;
    camera.type = CHASE_CAMERA;
    camera.speed = 1;
    CalculateCamera();
    actualCameraTarget.x = camera.target.x;
    actualCameraTarget.y = camera.target.y;
    actualCameraTarget.z = camera.target.z;
    actualFOV = CurrentFov;
    actualRoomNumber = camera.pos.room_number;
    camera.underwater = 0;
    actualCameraPos.x = camera.pos.x;
    actualCameraPos.y = camera.pos.y;
    actualCameraPos.z = camera.pos.z;
    cut = &VonCroyCutscenes[VonCroyCutIndices[lara.locationPad]];
    flags = cut->FOV;
    if (flags & 0xFFFF)
        ((void (*)(sint32))AlterFOV)(182 * flags);
    if (flags & 0x40000) {
        camera.pos.x = cut->CameraPos.x;
        camera.pos.y = cut->CameraPos.y;
        camera.pos.z = cut->CameraPos.z;
    } else {
        camera.pos.x += cut->CameraPos.x;
        camera.pos.y += cut->CameraPos.y;
        camera.pos.z += cut->CameraPos.z;
    }
    if (flags & 0x80000) {
        camera.target.x = cut->CameraTarget.x;
        camera.target.y = cut->CameraTarget.y;
        camera.target.z = cut->CameraTarget.z;
    } else {
        camera.target.x += cut->CameraTarget.x;
        camera.target.y += cut->CameraTarget.y;
        camera.target.z += cut->CameraTarget.z;
    }
    if (flags & 0x20000) {
        camera.target.x = item->pos.x_pos;
        camera.target.y = item->pos.y_pos - 256;
        camera.target.z = item->pos.z_pos;
    }
    if (IsRoomOutside(camera.pos.x, camera.pos.y, camera.pos.z) != -2)
        camera.pos.room_number = IsRoomOutsideNo;
    else {
        camera.pos.x = actualCameraPos.x;
        camera.pos.y = actualCameraPos.y;
        camera.pos.z = actualCameraPos.z;
        camera.pos.room_number = (sint16)actualRoomNumber;
    }
    phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, 0);
}

void ClearCutSceneCamera(void)
{
    camera.pos.x = actualCameraPos.x;
    camera.pos.y = actualCameraPos.y;
    camera.pos.z = actualCameraPos.z;
    camera.target.x = actualCameraTarget.x;
    camera.target.y = actualCameraTarget.y;
    bVoncroyCutScene = 0;
    camera.target.z = actualCameraTarget.z;
    camera.pos.room_number = (sint16)actualRoomNumber;
    AlterFOV((sint16)actualFOV);
    camera.speed = 1;
    camera.old_type = FIXED_CAMERA;
    camera.type = CHASE_CAMERA;
}

void GetAIEnemy(CREATURE_INFO *info, sint32 tfl)
{
    AIOBJECT *ai;
    ITEM_INFO *target;
    sint32 i;

    ai = AIObjects;
    for (i = 0; i < nAIObjects; i++, ai++) {
        if (ai->trigger_flags == tfl && ai->room_number != 255) {
            target = &info->ai_target;
            info->enemy = target;
            target->object_number = ai->object_number;
            target->room_number = ai->room_number;
            target->pos.x_pos = ai->x;
            target->pos.y_pos = ai->y;
            target->pos.z_pos = ai->z;
            target->pos.y_rot = ai->y_rot;
            target->flags = ai->flags;
            target->trigger_flags = ai->trigger_flags;
            target->box_number = ai->box_number;
            if (!(target->flags & IFL_TRIGGERED)) {
                target->pos.x_pos += ((rcossin_tbl[(target->pos.y_rot >> 3) & 0x1FFE] << 2) * 256) >> W2V_SHIFT;
                target->pos.z_pos += ((rcossin_tbl[((target->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * 256) >> W2V_SHIFT;
            }
            return;
        }
    }
}

void DoVonCroyCutscene(ITEM_INFO *item, CREATURE_INFO *info)
{
    FLOOR_INFO *floor;
    sint32 h, ang;
    sint16 room_number;

    if (lara.locationPad != 8 && lara.locationPad != 15)
        item->item_flags[3] = lara.locationPad;

    if (lara.locationPad == 20 && lara_item->current_anim_state != AS_SURFTREAD)
        return;

    item->pos.z_rot = 0;

    if (item->item_flags[3] == 8 || item->item_flags[3] == 15) {
        if (lara_item->current_anim_state == AS_HANG || lara_item->current_anim_state == AS_HANGLEFT || lara_item->current_anim_state == AS_HANGRIGHT) {
            S_CDPlay(VonCroyCutTracks[item->item_flags[3]], 0);
            VonCroyCutFlags[item->item_flags[3]] = 1;
            item->item_flags[2] = 2;
        }

        AnimateItem(item);
        return;
    }

    if (!item->trigger_flags) {
        SetFadeClip(24, 1);
        nullsub_1(0);
        ScreenFading = 1;
        dScreenFade = 255;
        ScreenFade = 255;
        ScreenFadeSpeed = 16;
        ScreenFadeBack = 0;
        ScreenFadedOut = 1;
        bDisableLaraControl = 1;
        item->trigger_flags++;
        input = 0;

        if (item->item_flags[3] == 14) {
            item->anim_number = objects[VON_CROY].anim_index + 43;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 27;
            item->goal_anim_state = 30;
        } else {
            item->anim_number = objects[VON_CROY].anim_index + 4;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = item->goal_anim_state = 1;
        }

        GetAIEnemy(info, lara.locationPad);
        item->pos.x_pos = info->enemy->pos.x_pos;
        item->pos.y_pos = info->enemy->pos.y_pos;
        item->pos.z_pos = info->enemy->pos.z_pos;
        ang = phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);

        if (item->item_flags[3] == 14 || item->item_flags[3] == 3) {
            item->pos.y_rot = info->enemy->pos.y_rot;
            info->LOT.is_jumping = 1;
            info->maximum_turn = 0;
        } else if (item->item_flags[3] == 43 || item->item_flags[3] == 53) {
            info->maximum_turn = 0;
            item->pos.y_rot = -0x6000;
        } else
            item->pos.y_rot = ang;

        IsRoomOutsideNo = -1;
        IsRoomOutside(item->pos.x_pos, item->pos.y_pos - 64, item->pos.z_pos);

        if (IsRoomOutsideNo != item->room_number && IsRoomOutsideNo != -1)
            ItemNewRoom(item - items, IsRoomOutsideNo);

        lara_item->pos.y_rot = ang - 0x8000;

        if (lara_item->current_anim_state != AS_SURFTREAD) {
            room_number = lara_item->room_number;
            floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);
            h = GetHeight(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
            lara_item->pos.y_pos = h;
            lara_item->goal_anim_state = AS_STOP;
            lara_item->current_anim_state = AS_STOP;
            lara_item->frame_number = anims[ANIM_STOP].frame_base;
            lara_item->anim_number = ANIM_STOP;
            lara_item->speed = 0;
            lara_item->fallspeed = 0;
            lara_item->gravity_status = 0;
        }
    } else if (item->trigger_flags == 1) {
        if (ScreenFadedOut) {
            SetCutSceneCamera(item);
            ScreenFading = 0;
            SetScreenFadeIn(16);
            item->trigger_flags++;
            S_StartSyncedAudio(VonCroyCutTracks[item->item_flags[3]]);
        }
    } else if (item->trigger_flags == 2) {
        if (input & IN_LOOK && item->item_flags[3] != 43 && item->item_flags[3] != 53) {
            item->meshswap_meshbits &= ~0x8000;
            item->trigger_flags = 3;

            if (item->item_flags[3] != 14)
                item->goal_anim_state = 1;
        } else {
            if (XATrack != VonCroyCutTracks[item->item_flags[3]])
                item->trigger_flags = 3;

            if (item->current_anim_state == 1) {
                if (info->enemy && info->enemy->flags == 36)
                    item->goal_anim_state = 11;
                else
                    item->goal_anim_state = GetRandomControl() % 3 + 8;
            } else if (item->current_anim_state != 27) {
                if (info->enemy && info->enemy->flags == 36)
                    info->enemy->flags = 255;

                item->goal_anim_state = 1;
            }
        }
    } else if (item->trigger_flags == 3) {
        ClearCutSceneCamera();
        S_CDStop();
        S_CDPlay(CurrentAtmosphere, 1);
        IsAtmospherePlaying = 1;
        bDisableLaraControl = 0;
        SetFadeClip(0, 1);
        item->trigger_flags = 0;
        VonCroyCutFlags[item->item_flags[3]] = 1;
        ang = info->enemy->pos.y_rot - item->pos.y_rot;

        if (ang < -1024)
            item->required_anim_state = 35;
        else if (ang > 1024)
            item->required_anim_state = 22;

        item->goal_anim_state = 1;
        item->item_flags[2] = 2;
    }

    AnimateItem(item);

    if (item->current_anim_state == 11) {
        if (item->frame_number == anims[item->anim_number].frame_base + 32)
            item->meshswap_meshbits |= 0x8000;
        else if (item->frame_number == anims[item->anim_number].frame_base + 216)
            item->meshswap_meshbits &= ~0x8000;
    }
}

void InitialiseVoncroy(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[VON_CROY].anim_index + 11;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 6;
    item->meshswap_meshbits = 0x40080;
    memset(VonCroyCutFlags, 0, 64);
}

void VoncroyControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *oEnemy;
    ITEM_INFO *target;
    ITEM_INFO *candidate;
    CREATURE_INFO *VonCroy;
    CREATURE_INFO *baddie;
    FLOOR_INFO *floor;
    static AI_INFO VonCroyAI;
    static AI_INFO VonCroyLaraAI;
    sint32 Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight, dx, dz, dist, max_dist, h, c, i;
    sint16 angle, tilt, torso_x, torso_y, head_y, room_number, jump_ahead, long_jump_ahead, ifl3;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    VonCroy = (CREATURE_INFO *)item->data;
    tilt = 0;
    head_y = 0;
    torso_x = 0;
    torso_y = 0;
    ifl3 = 0;
    room_number = item->room_number;
    Xoffset = (808 * (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
    Zoffset = (808 * (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
    x = item->pos.x_pos + Xoffset;
    y = item->pos.y_pos;
    z = item->pos.z_pos + Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    nearheight = GetHeight(floor, x, y, z);
    room_number = item->room_number;
    x += Xoffset;
    z += Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    midheight = GetHeight(floor, x, y, z);
    room_number = item->room_number;
    x += Xoffset;
    z += Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    farheight = GetHeight(floor, x, y, z);
    if (item->pos.y_pos >= nearheight - 384 || item->pos.y_pos >= midheight + 256 || item->pos.y_pos <= midheight - 256)
        jump_ahead = 0;
    else
        jump_ahead = 1;
    if (item->pos.y_pos >= nearheight - 384 || item->pos.y_pos >= midheight - 384 || item->pos.y_pos >= farheight + 256 || item->pos.y_pos <= farheight - 256)
        long_jump_ahead = 0;
    else
        long_jump_ahead = 1;
    item->ai_bits = FOLLOW;
    GetAITarget(VonCroy);
    target = NULL;
    if (lara.location <= item->item_flags[3]) {
        max_dist = 0x7FFFFFFF;
        baddie = baddie_slots;
        for (i = 0; i < 5; i++, baddie++) {
            if (baddie->item_num != NO_ITEM && baddie->item_num != item_number) {
                candidate = &items[baddie->item_num];
                if (candidate->object_number != VON_CROY) {
                    dx = candidate->pos.x_pos - item->pos.x_pos;
                    dz = candidate->pos.z_pos - item->pos.z_pos;
                    if (abs(dx) <= 5120 && abs(dz) <= 5120) {
                        dist = SQUARE(dx) + SQUARE(dz);
                        if (dist < max_dist) {
                            VonCroy->reached_goal = 0;
                            target = candidate;
                            max_dist = dist;
                            item->item_flags[2] = 0;
                        }
                    }
                }
            }
        }
    }
    oEnemy = VonCroy->enemy;
    if (target)
        VonCroy->enemy = target;
    if (item->anim_number == objects[VON_CROY].anim_index + 36 || item->anim_number == objects[VON_CROY].anim_index + 52) {
        item->pos.x_pos += Xoffset;
        item->pos.z_pos += Zoffset;
        room_number = item->room_number;
        GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number);
        if (item->frame_number == anims[item->anim_number].frame_base + 1)
            CreateZone(item);
        CreatureAIInfo(item, &VonCroyAI);
        item->room_number = room_number;
        item->pos.x_pos -= Xoffset;
        item->pos.z_pos -= Zoffset;
    } else
        CreatureAIInfo(item, &VonCroyAI);
    GetCreatureMood(item, &VonCroyAI, 1);
    CreatureMood(item, &VonCroyAI, 1);
    if (VonCroy->enemy == lara_item)
        VonCroyLaraAI = VonCroyAI;
    else {
        dz = lara_item->pos.z_pos - item->pos.z_pos;
        dx = lara_item->pos.x_pos - item->pos.x_pos;
        VonCroyLaraAI.angle = (sint16)(phd_atan(dz, dx) - item->pos.y_rot);
        if (VonCroyLaraAI.angle > -0x4000 && VonCroyLaraAI.angle < 0x4000)
            VonCroyLaraAI.ahead = 1;
        else
            VonCroyLaraAI.ahead = 0;
        VonCroyLaraAI.enemy_facing = VonCroyLaraAI.angle - lara_item->pos.y_rot + 0x8000;
        if (dz > 32000 || dz < -32000 || dx > 32000 || dx < -32000)
            VonCroyLaraAI.distance = 0x7FFFFFFF;
        else
            VonCroyLaraAI.distance = SQUARE(dx) + SQUARE(dz);
        dx = abs(dx);
        dz = abs(dz);
        if (dx > dz)
            VonCroyLaraAI.x_angle = (sint16)phd_atan(dx + (dz >> 1), item->pos.y_pos - lara_item->pos.y_pos);
        else
            VonCroyLaraAI.x_angle = (sint16)phd_atan(dz + (dx >> 1), item->pos.y_pos - lara_item->pos.y_pos);
    }
    if (VonCroyLaraAI.angle > -6144 && VonCroyLaraAI.angle < 6144 && VonCroyLaraAI.distance < 0x100000)
        VonCroyLaraAI.bite = 1;
    else
        VonCroyLaraAI.bite = 0;
    angle = CreatureTurn(item, VonCroy->maximum_turn);
    if (target) {
        VonCroy->enemy = oEnemy;
        oEnemy = target;
    }
    if (item->item_flags[3] == 43 && savegame.Game.Secrets > 7) {
        VonCroy->reached_goal = 0;
        VonCroy->enemy = NULL;
        item->item_flags[3] = 53;
        item->ai_bits = FOLLOW;
        lara.location = 53;
    }
    if ((lara.locationPad == 9 || lara.locationPad == 10) && item->item_flags[3] == 11)
        lara.locationPad = (sint8)item->item_flags[3];
    else if (lara.locationPad == 10 && item->item_flags[3] == 12 && (item->item_flags[0] || (lara_item->anim_number == objects[LARA].anim_index + 90 && lara_item->frame_number == anims[lara_item->anim_number].frame_end))) {
        lara.locationPad = (sint8)item->item_flags[3];
        item->item_flags[0] = 1;
    } else if (lara.locationPad == 43 && (item->item_flags[3] == 43 || item->item_flags[3] == 53)) {
        lara.locationPad = (sint8)item->item_flags[3];
        lara.location = (sint8)item->item_flags[3];
    } else if (lara.location == 43 && (item->item_flags[3] == 44 || item->item_flags[3] == 54))
        lara.location = (sint8)item->item_flags[3];
    if (!VonCroyCutFlags[item->item_flags[3]] && ((VonCroy->reached_goal && item->item_flags[3] == lara.locationPad && VonCroyCutTracks[item->item_flags[3]] != -1) || item->trigger_flags > 0 || (lara.locationPad >= item->item_flags[3] && !VonCroyCutFlags[lara.locationPad] && VonCroyCutTracks[lara.locationPad] != -1))) {
        torso_y = VonCroyLaraAI.angle >> 1;
        torso_x = VonCroyLaraAI.x_angle >> 1;
        CreatureJoint(item, 0, torso_y);
        CreatureJoint(item, 1, torso_x);
        CreatureJoint(item, 2, torso_y);
        CreatureJoint(item, 3, torso_x);
        DoVonCroyCutscene(item, VonCroy);
        return;
    }
    switch (item->current_anim_state) {
    case 1:
        VonCroy->LOT.is_jumping = 0;
        VonCroy->LOT.is_monkeying = 0;
        VonCroy->flags = 0;
        VonCroy->maximum_turn = 0;
        head_y = VonCroyAI.angle >> 1;
        if (VonCroyAI.ahead) {
            torso_y = VonCroyAI.angle >> 1;
            torso_x = VonCroyAI.x_angle >> 1;
        }
        if (item->required_anim_state) {
            item->goal_anim_state = item->required_anim_state;
        } else if (item->item_flags[2] == 2) {
            if (oEnemy->pos.y_rot - item->pos.y_rot < -1024)
                item->goal_anim_state = 35;
            else if (oEnemy->pos.y_rot - item->pos.y_rot > 1024)
                item->goal_anim_state = 22;
            else {
                item->item_flags[2] = 0;
                if (!oEnemy->flags) {
                    ifl3 = 1;
                    goto reset;
                }
            }
        } else if (lara.location < item->item_flags[3] && VonCroy->reached_goal) {
            item->goal_anim_state = 1;
        } else if (target && VonCroyAI.distance < 0x900000 && item->meshswap_meshbits & 0x40080) {
            item->goal_anim_state = 6;
        } else if (target && VonCroyAI.distance < 0x100000) {
            if (VonCroyAI.bite)
                item->goal_anim_state = 31;
            else if (oEnemy->hit_points > 0 && VonCroyAI.ahead && ABS(oEnemy->pos.y_pos - item->pos.y_pos + 512) < 512)
                item->goal_anim_state = 21;
        } else if (target && oEnemy != lara_item && VonCroyAI.distance > 0x64000) {
            item->goal_anim_state = 2;
        } else if (VonCroy->reached_goal) {
            if (VonCroyAI.distance > 0x4000 && oEnemy->flags && item->item_flags[2] != 6) {
                VonCroy->maximum_turn = 0;
                if (VonCroyAI.ahead)
                    item->required_anim_state = 36;
                else
                    item->required_anim_state = 37;
            } else if (lara.location > item->item_flags[3] || (oEnemy && oEnemy->flags && (lara.locationPad == item->item_flags[3] || (VonCroyCutTracks[item->item_flags[3]] == -1 && lara.location == item->item_flags[3] && VonCroyLaraAI.distance < 0x900000)))) {
                switch (oEnemy->flags) {
                case 10:
                    item->goal_anim_state = 7;
                    break;
                case 8:
                    item->goal_anim_state = 20;
                    break;
                case 2:
                    item->current_anim_state = 29;
                    item->anim_number = objects[VON_CROY].anim_index + 37;
                    item->frame_number = anims[item->anim_number].frame_base;
                    item->pos = oEnemy->pos;
                    ifl3 = 1;
                    goto reset;
                case 4:
                    item->current_anim_state = 26;
                    item->anim_number = objects[VON_CROY].anim_index + 36;
                    item->frame_number = anims[item->anim_number].frame_base;
                    VonCroy->LOT.is_jumping = 1;
                    item->pos = oEnemy->pos;
                    ifl3 = 1;
                    goto reset;
                case 12:
                    VonCroy->maximum_turn = 0;
                    item->anim_number = objects[VON_CROY].anim_index + 22;
                    item->frame_number = anims[item->anim_number].frame_base;
                    item->current_anim_state = 15;
                    if (long_jump_ahead)
                        item->goal_anim_state = 16;
                    else
                        item->goal_anim_state = 15;
                    item->pos = oEnemy->pos;
                    VonCroy->LOT.is_jumping = 1;
                    ifl3 = 1;
                    goto reset;
                case 34:
                    if (lara.location > item->item_flags[3]) {
                        ifl3 = 2;
                        goto reset;
                    }
                    item->goal_anim_state = 32;
                    break;
                case 36:
                    if (lara.location > item->item_flags[3]) {
                        ifl3 = 1;
                        goto reset;
                    }
                    item->goal_anim_state = 1;
                    break;
                case 255:
                    ifl3 = 1;
                    goto reset;
                case 40:
                    if (item->item_flags[2] == 6)
                        item->goal_anim_state = 3;
                    else {
                        item->goal_anim_state = 34;
                        item->pos = oEnemy->pos;
                    }
                    break;
                case 0:
                case 32:
                case 48:
                    goto trig;
                }
            } else if (oEnemy && oEnemy->flags && VonCroyLaraAI.distance >= 0x900000) {
                item->goal_anim_state = 1;
            } else if (!item->item_flags[2]) {
                if (VonCroyLaraAI.angle > 1024)
                    item->goal_anim_state = 35;
                else if (VonCroyLaraAI.angle < -1024)
                    item->goal_anim_state = 22;
                else
                    item->item_flags[2] = 1;
            } else if (item->item_flags[2] != 1) {
                ifl3 = 1;
                goto reset;
            } else if (GetRandomControl() & 0xF)
                item->item_flags[2] = 0;
            else if (VonCroyLaraAI.distance < 0x900000)
                item->goal_anim_state = 14;
            else
                item->goal_anim_state = 13;
        } else {
            if (VonCroyLaraAI.bite)
                item->goal_anim_state = 1;
            else if (VonCroy->monkey_ahead) {
                floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
                h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
                c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
                if (c == h - 1536) {
                    if (item->meshswap_meshbits & 0x40080)
                        item->goal_anim_state = 4;
                    else
                        item->goal_anim_state = 6;
                } else
                    item->goal_anim_state = 2;
            } else if (target && VonCroyAI.distance < 0x900000 && item->meshswap_meshbits & 0x40080) {
                item->goal_anim_state = 6;
            } else if (target && VonCroyAI.distance < 0x100000) {
                if (VonCroyAI.bite)
                    item->goal_anim_state = 31;
                else if (oEnemy->hit_points > 0 && VonCroyAI.ahead && ABS(oEnemy->pos.y_pos - item->pos.y_pos + 512) < 512)
                    item->goal_anim_state = 21;
            } else if ((VonCroyAI.distance > 0x64000 && VonCroyLaraAI.distance < 0x1900000) || lara.location < item->item_flags[3])
                item->goal_anim_state = 2;
        }
        break;
    case 2:
        VonCroy->LOT.is_jumping = 0;
        VonCroy->LOT.is_monkeying = 0;
        VonCroy->maximum_turn = 1092;
        if (VonCroyLaraAI.ahead)
            head_y = VonCroyLaraAI.angle;
        else if (VonCroyAI.ahead)
            head_y = VonCroyAI.angle;
        if (item->required_anim_state)
            item->goal_anim_state = item->required_anim_state;
        else if ((lara.location < item->item_flags[3] && VonCroyLaraAI.distance > 0x1900000) || VonCroyLaraAI.bite)
            item->goal_anim_state = 1;
        else if (VonCroy->monkey_ahead)
            item->goal_anim_state = 1;
        else if (VonCroy->reached_goal) {
            if (oEnemy->flags == 32)
                goto trig;
            item->goal_anim_state = 1;
        } else if (target && (VonCroyAI.distance < 0x200000 || (!(item->meshswap_meshbits & 0x40080) && VonCroyAI.distance < 0x900000)))
            item->goal_anim_state = 1;
        else if (VonCroyAI.distance < 0x64000 && oEnemy->flags != 32)
            item->goal_anim_state = 1;
        else if (VonCroyAI.distance > 0x900000 && lara.location >= item->item_flags[3])
            item->goal_anim_state = 3;
        break;
    case 3:
        if (VonCroyAI.ahead)
            head_y = VonCroyAI.angle;
        if (item->frame_number == anims[item->anim_number].frame_base) {
            VonCroy->LOT.is_jumping = 0;
            VonCroy->maximum_turn = 1456;
        }
        tilt = angle >> 1;
        if (item->item_flags[2] == 6) {
            VonCroy->maximum_turn = 0;
            item->goal_anim_state = 16;
        } else if (lara.location < item->item_flags[3] || jump_ahead || VonCroyLaraAI.bite)
            item->goal_anim_state = 1;
        else if (VonCroy->monkey_ahead)
            item->goal_anim_state = 1;
        else if (VonCroy->reached_goal) {
            if (oEnemy->flags == 32)
                goto trig;
            if (VonCroyAI.distance < 512) {
                if (oEnemy->flags == 40) {
                    VonCroy->maximum_turn = 0;
                    item->pos.y_rot = oEnemy->pos.y_rot;
                    item->goal_anim_state = 16;
                    item->item_flags[2] = 6;
                }
            } else
                item->goal_anim_state = 1;
        } else if (VonCroyAI.distance < 0x64000 && oEnemy->flags != 32 && oEnemy->flags != 40)
            item->goal_anim_state = 1;
        break;
    case 31:
        if (VonCroyAI.ahead) {
            head_y = torso_y = VonCroyAI.angle >> 1;
            torso_x = VonCroyAI.x_angle >> 1;
        }
        VonCroy->maximum_turn = 0;
        CreatureYRot(&item->pos, VonCroyAI.angle, 1092);
        if (oEnemy && oEnemy->flags == 6 && item->frame_number > anims[item->anim_number].frame_base + 21) {
            VonCroy->flags = 1;
            goto trig;
        }
        if (!VonCroy->flags && oEnemy && item->frame_number > anims[item->anim_number].frame_base + 15 && item->frame_number < anims[item->anim_number].frame_base + 26 && ABS(oEnemy->pos.x_pos - item->pos.x_pos) < 512 && ABS(oEnemy->pos.y_pos - item->pos.y_pos) <= 512 && ABS(oEnemy->pos.z_pos - item->pos.z_pos) < 512) {
            oEnemy->hit_points -= 20;
            if (oEnemy->hit_points <= 0)
                item->ai_bits = FOLLOW;
            oEnemy->hit_status = 1;
            VonCroy->flags = 1;
            CreatureEffectT(item, &voncroy_hit, 8, -1, DoBloodSplat);
        }
        break;
    case 21:
        if (VonCroyAI.ahead) {
            head_y = torso_y = VonCroyAI.angle >> 1;
            torso_x = VonCroyAI.x_angle >> 1;
        }
        VonCroy->maximum_turn = 0;
        CreatureYRot(&item->pos, VonCroyAI.angle, 1092);
        if (!VonCroy->flags && oEnemy && item->frame_number > anims[item->anim_number].frame_base + 20 && item->frame_number < anims[item->anim_number].frame_base + 45 && ABS(oEnemy->pos.x_pos - item->pos.x_pos) < 512 && ABS(oEnemy->pos.y_pos - item->pos.y_pos + 768) <= 512 && ABS(oEnemy->pos.z_pos - item->pos.z_pos) < 512) {
            oEnemy->hit_points -= 40;
            if (oEnemy->hit_points <= 0)
                item->ai_bits = FOLLOW;
            oEnemy->hit_status = 1;
            VonCroy->flags = 1;
            CreatureEffectT(item, &voncroy_hit, 2, -1, DoBloodSplat);
        }
        break;
    case 6:
        if (item->frame_number == anims[item->anim_number].frame_base + 28) {
            if (item->meshswap_meshbits & 0x40080)
                item->meshswap_meshbits &= ~0x40080;
            else
                item->meshswap_meshbits |= 0x40080;
        }
        break;
    case 32:
        if (VonCroyAI.ahead) {
            head_y = torso_y = VonCroyAI.angle >> 1;
            torso_x = VonCroyAI.x_angle;
        }
        VonCroy->maximum_turn = 0;
        CreatureYRot(&item->pos, VonCroyAI.angle, 1092);
        if (item->anim_number == objects[VON_CROY].anim_index + 47) {
            if (item->frame_number == anims[item->anim_number].frame_base) {
                ifl3 = 1;
                goto reset;
            }
        } else if (!(GetRandomControl() & 0x1F)) {
            item->goal_anim_state = 1;
            ifl3 = 1;
            goto reset;
        }
        break;
    case 27:
        VonCroy->LOT.is_jumping = 1;
        VonCroy->maximum_turn = 0;
        if (VonCroy->reached_goal) {
            item->goal_anim_state = 30;
            ifl3 = 1;
            goto reset;
        }
        item->goal_anim_state = 28;
        break;
    case 28:
        VonCroy->LOT.is_jumping = 1;
        VonCroy->maximum_turn = 0;
        break;
    case 16:
        if (item->anim_number == objects[VON_CROY].anim_index + 25 || item->frame_number > anims[item->anim_number].frame_base + 5)
            VonCroy->LOT.is_jumping = 1;
        else if (jump_ahead)
            item->goal_anim_state = 15;
        if (item->item_flags[2] == 6)
            item->goal_anim_state = 33;
        break;
    case 34:
        item->item_flags[2] = 6;
        break;
    case 33:
        if (item->anim_number == objects[VON_CROY].anim_index + 52 && item->frame_number == anims[item->anim_number].frame_base)
            ifl3 = 1;
        item->goal_anim_state = 2;
        item->required_anim_state = 3;
        item->item_flags[2] = 0;
        if (ifl3 == -1) {
        trig:
            TestTriggersAtXYZ(VonCroy->ai_target.pos.x_pos, VonCroy->ai_target.pos.y_pos, VonCroy->ai_target.pos.z_pos, VonCroy->ai_target.room_number, 1, 0);
            ifl3 = 1;
        }
    reset:
        if (ifl3) {
            VonCroy->reached_goal = 0;
            VonCroy->enemy = NULL;
            item->item_flags[3] += ifl3;
            item->ai_bits = FOLLOW;
        }
        break;
    case 4:
        VonCroy->maximum_turn = 0;
        if (item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            if (c == h - 1536)
                item->goal_anim_state = 1;
        } else
            item->goal_anim_state = 5;
        break;
    case 5:
        VonCroy->LOT.is_jumping = 1;
        VonCroy->LOT.is_monkeying = 1;
        VonCroy->maximum_turn = 1092;
        if (item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            if (c == h - 1536)
                item->goal_anim_state = 4;
        }
        break;
    case 20:
        if (item->frame_number == anims[item->anim_number].frame_base)
            item->pos = oEnemy->pos;
        else if (item->frame_number == anims[item->anim_number].frame_base + 120)
            goto trig;
        break;
    case 7:
        if (item->frame_number == anims[item->anim_number].frame_base) {
            item->pos = oEnemy->pos;
            if (item->item_flags[3] == 6) {
                VonCroy->maximum_turn = 0;
                item->anim_number = objects[VON_CROY].anim_index + 22;
                item->frame_number = anims[item->anim_number].frame_base;
                item->current_anim_state = 15;
                item->goal_anim_state = 16;
                VonCroy->LOT.is_jumping = 1;
            }
            ifl3 = 1;
            goto reset;
        }
        break;
    case 22:
    case 35:
        VonCroy->maximum_turn = 0;
        if (item->item_flags[2])
            CreatureYRot(&item->pos, oEnemy->pos.y_rot - item->pos.y_rot, 512);
        else
            CreatureYRot(&item->pos, VonCroyLaraAI.angle, 512);
        break;
    case 36:
    case 37:
        VonCroy->maximum_turn = 0;
        MoveCreature3DPos(&item->pos, &oEnemy->pos, 15, oEnemy->pos.y_rot - item->pos.y_rot, 512);
        break;
    }
    CreatureTilt(item, tilt);
    CreatureJoint(item, 0, torso_y);
    CreatureJoint(item, 1, torso_x);
    CreatureJoint(item, 2, head_y);
    CreatureJoint(item, 3, torso_x);
    if (item->current_anim_state >= 15 || item->current_anim_state == 5)
        CreatureAnimation(item_number, angle, 0);
    else {
        switch (CreatureVault(item_number, angle, 2, 260)) {
        case 2:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 29;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 19;
            break;
        case 3:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 28;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 18;
            break;
        case 4:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 27;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 17;
            break;
        case -2:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 42;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 23;
            break;
        case -3:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 41;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 24;
            break;
        case -4:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 35;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 25;
            break;
        }
    }
}

void VoncroyRaceControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *oEnemy;
    CREATURE_INFO *VonCroy;
    FLOOR_INFO *floor;
    AI_INFO info;
    sint32 Xoffset, Zoffset, x, y, z, nearheight, midheight, farheight, dx, dz, distance, ahead, h, c;
    sint16 angle, tilt, torso_x, torso_y, head_y, room_number, jump_ahead, long_jump_ahead, ifl3, iAngle;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    VonCroy = (CREATURE_INFO *)item->data;
    tilt = 0;
    head_y = 0;
    torso_x = 0;
    torso_y = 0;
    ifl3 = 0;
    room_number = item->room_number;
    Xoffset = (808 * (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
    Zoffset = (808 * (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
    x = item->pos.x_pos + Xoffset;
    y = item->pos.y_pos;
    z = item->pos.z_pos + Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    nearheight = GetHeight(floor, x, y, z);
    room_number = item->room_number;
    x += Xoffset;
    z += Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    midheight = GetHeight(floor, x, y, z);
    room_number = item->room_number;
    x += Xoffset;
    z += Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    farheight = GetHeight(floor, x, y, z);
    if (y >= nearheight - 384 || y >= midheight + 256 || y <= midheight - 256)
        jump_ahead = 0;
    else
        jump_ahead = 1;
    if (y >= nearheight - 384 || y >= midheight - 384 || y >= farheight + 256 || y <= farheight - 256)
        long_jump_ahead = 0;
    else
        long_jump_ahead = 1;
    GetAITarget(VonCroy);
    oEnemy = VonCroy->enemy;
    if (item->anim_number == objects[VON_CROY].anim_index + 36 || item->anim_number == objects[VON_CROY].anim_index + 52) {
        item->pos.x_pos += Xoffset;
        item->pos.z_pos += Zoffset;
        room_number = item->room_number;
        GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number);
        if (item->frame_number == anims[item->anim_number].frame_base + 1)
            CreateZone(item);
        CreatureAIInfo(item, &info);
        item->room_number = room_number;
        item->pos.x_pos -= Xoffset;
        item->pos.z_pos -= Zoffset;
    } else
        CreatureAIInfo(item, &info);
    GetCreatureMood(item, &info, 1);
    CreatureMood(item, &info, 1);
    if (VonCroy->enemy == lara_item) {
        iAngle = info.angle;
        distance = info.distance;
    } else {
        dx = lara_item->pos.x_pos - item->pos.x_pos;
        dz = lara_item->pos.z_pos - item->pos.z_pos;
        iAngle = (sint16)(phd_atan(dz, dx) - item->pos.y_rot);
        if (iAngle > -0x4000 && iAngle < 0x4000)
            ahead = 1;
        else
            ahead = 0;
        distance = SQUARE(dx) + SQUARE(dz);
    }
    angle = CreatureTurn(item, VonCroy->maximum_turn);
    switch (item->current_anim_state) {
    case 1:
        VonCroy->LOT.is_jumping = 0;
        VonCroy->LOT.is_monkeying = 0;
        VonCroy->flags = 0;
        VonCroy->maximum_turn = 0;
        head_y = info.angle >> 1;
        if (info.ahead) {
            torso_y = info.angle >> 1;
            torso_x = info.x_angle >> 1;
        }
        if (lara.location < item->item_flags[3] || (XATrack == 80 && (XAFlag == 6 || XAFlag == 5))) {
            item->goal_anim_state = 1;
            break;
        }
        if (VonCroy->reached_goal) {
            if ((!oEnemy || !oEnemy->flags || (distance >= 0x900000 && objects[BAT].loaded)) && lara.location <= item->item_flags[3]) {
                ifl3 = 1;
                goto reset;
            }
            if (item->item_flags[2] != 6)
                item->item_flags[2] = 0;
            switch (oEnemy->flags) {
            case 34:
                if (lara.location > item->item_flags[3]) {
                    ifl3 = 2;
                    goto reset;
                }
                item->goal_anim_state = 32;
                break;
            case 36:
                if (lara.location > item->item_flags[3]) {
                    ifl3 = 1;
                    goto reset;
                }
                item->goal_anim_state = 11;
                break;
            case 6:
                if (lara.location > item->item_flags[3]) {
                    ifl3 = 1;
                    goto reset;
                }
                if (item->meshswap_meshbits & 0x40080)
                    item->goal_anim_state = 6;
                else
                    item->goal_anim_state = 31;
                break;
            case 10:
                if (lara.location > item->item_flags[3]) {
                    ifl3 = 1;
                    goto reset;
                }
                item->goal_anim_state = 7;
                break;
            case 8:
                if (lara.location > item->item_flags[3]) {
                    ifl3 = 1;
                    goto reset;
                }
                item->goal_anim_state = 20;
                break;
            case 0:
            case 32:
                ifl3 = -1;
                goto trig;
            case 2:
                item->current_anim_state = 29;
                item->anim_number = objects[VON_CROY].anim_index + 37;
                item->frame_number = anims[item->anim_number].frame_base;
                item->pos = oEnemy->pos;
                ifl3 = 1;
                goto reset;
            case 4:
                item->current_anim_state = 26;
                item->anim_number = objects[VON_CROY].anim_index + 36;
                item->frame_number = anims[item->anim_number].frame_base;
                VonCroy->LOT.is_jumping = 1;
                item->pos = oEnemy->pos;
                ifl3 = 1;
                goto reset;
            case 40:
                if (item->item_flags[2] == 6)
                    item->goal_anim_state = 3;
                else {
                    item->goal_anim_state = 34;
                    item->pos = oEnemy->pos;
                }
                break;
            }
        } else if (jump_ahead || long_jump_ahead) {
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 22;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 15;
            if (long_jump_ahead)
                item->goal_anim_state = 16;
            else
                item->goal_anim_state = 15;
            VonCroy->LOT.is_jumping = 1;
        } else if (VonCroy->monkey_ahead) {
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            if (c == h - 1536)
                item->goal_anim_state = 4;
            else
                item->goal_anim_state = 2;
        } else if (oEnemy != lara_item || info.distance > 0x64000)
            item->goal_anim_state = 2;
        break;
    case 2:
        VonCroy->LOT.is_jumping = 0;
        VonCroy->LOT.is_monkeying = 0;
        VonCroy->maximum_turn = 1092;
        if (ahead)
            head_y = iAngle;
        else if (info.ahead)
            head_y = info.angle;
        if (!savegame.Level.Timer)
            savegame.Level.Timer = 1;
        if (lara.location < item->item_flags[3])
            item->goal_anim_state = 1;
        else if (jump_ahead || long_jump_ahead) {
            VonCroy->maximum_turn = 0;
            item->goal_anim_state = 1;
        } else if (VonCroy->monkey_ahead)
            item->goal_anim_state = 1;
        else if (VonCroy->reached_goal) {
            if (oEnemy->flags == 32) {
                ifl3 = -1;
                goto trig;
            }
            item->goal_anim_state = 1;
        } else if (info.distance < 0x64000 && oEnemy->flags != 32)
            item->goal_anim_state = 1;
        else if (info.distance > 0x900000)
            item->goal_anim_state = 3;
        break;
    case 3:
        if (info.ahead)
            head_y = info.angle;
        if (item->frame_number == anims[item->anim_number].frame_base) {
            VonCroy->LOT.is_jumping = 0;
            VonCroy->maximum_turn = 1456;
        }
        tilt = angle >> 1;
        if (item->item_flags[2] == 6) {
            VonCroy->maximum_turn = 0;
            item->goal_anim_state = 16;
        } else if (lara.location < item->item_flags[3] || jump_ahead)
            item->goal_anim_state = 1;
        else if (long_jump_ahead) {
            VonCroy->maximum_turn = 0;
            item->goal_anim_state = 16;
        } else if (VonCroy->monkey_ahead)
            item->goal_anim_state = 1;
        else if (VonCroy->reached_goal) {
            if (oEnemy->flags == 32) {
                ifl3 = -1;
                goto trig;
            }
            if (info.distance < 0x4000) {
                if (oEnemy->flags == 40) {
                    VonCroy->maximum_turn = 0;
                    item->pos.y_rot = oEnemy->pos.y_rot;
                    item->goal_anim_state = 16;
                    item->item_flags[2] = 6;
                }
            } else
                item->goal_anim_state = 1;
        } else if (info.distance < 0x64000) {
            if (oEnemy->flags != 32 && oEnemy->flags != 40)
                item->goal_anim_state = 1;
        }
        break;
    case 31:
        if (info.ahead) {
            head_y = torso_y = info.angle >> 1;
            torso_x = info.x_angle >> 1;
        }
        VonCroy->maximum_turn = 0;
        if (abs(info.angle) < 1092)
            item->pos.y_rot += info.angle;
        else if (info.angle < 0)
            item->pos.y_rot -= 1092;
        else
            item->pos.y_rot += 1092;
        if (oEnemy && oEnemy->flags == 6 && item->frame_number > anims[item->anim_number].frame_base + 21) {
            ifl3 = -1;
            goto trig;
        }
        if (!VonCroy->flags && oEnemy) {
            if (item->frame_number > anims[item->anim_number].frame_base + 15 && item->frame_number < anims[item->anim_number].frame_base + 26) {
                if (abs(oEnemy->pos.x_pos - item->pos.x_pos) < 512 && abs(oEnemy->pos.y_pos - item->pos.y_pos) <= 512 && abs(oEnemy->pos.z_pos - item->pos.z_pos) < 512) {
                    oEnemy->hit_points -= 20;
                    oEnemy->hit_status = 1;
                    VonCroy->flags = 1;
                    CreatureEffectT(item, &voncroy_hit, 8, -1, DoBloodSplat);
                }
            }
        }
        break;
    case 6:
        if (item->frame_number == anims[item->anim_number].frame_base + 28) {
            if (item->meshswap_meshbits & 0x40080)
                item->meshswap_meshbits &= ~0x40080;
            else
                item->meshswap_meshbits |= 0x40080;
        }
        break;
    case 15:
        if (item->anim_number == objects[VON_CROY].anim_index + 23)
            item->goal_anim_state = 3;
        break;
    case 16:
        if (item->anim_number == objects[VON_CROY].anim_index + 25 || item->frame_number > anims[item->anim_number].frame_base + 7)
            VonCroy->LOT.is_jumping = 1;
        else if (jump_ahead)
            item->goal_anim_state = 15;
        else if (!objects[BAT].loaded)
            item->goal_anim_state = 3;
        if (item->item_flags[2] == 6)
            item->goal_anim_state = 33;
        break;
    case 34:
        item->item_flags[2] = 6;
        break;
    case 33:
        if (item->anim_number == objects[VON_CROY].anim_index + 52 && item->frame_number == anims[item->anim_number].frame_base)
            ifl3 = 1;
        item->goal_anim_state = 2;
        item->item_flags[2] = 0;
        if (ifl3 == -1) {
        trig:
            if (oEnemy) {
                room_number = oEnemy->room_number;
                floor = GetFloor(oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos, &room_number);
                GetHeight(floor, oEnemy->pos.x_pos, oEnemy->pos.y_pos, oEnemy->pos.z_pos);
                TestTriggers(trigger_index, 1, 0);
                ifl3 = 1;
            }
        } else if (!ifl3)
            break;
    reset:
        VonCroy->reached_goal = 0;
        VonCroy->enemy = 0;
        item->item_flags[3] += ifl3;
        item->ai_bits = FOLLOW;
        break;
    case 4:
        VonCroy->maximum_turn = 0;
        if (item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            if (c == h - 1536)
                item->goal_anim_state = 1;
        } else
            item->goal_anim_state = 5;
        break;
    case 5:
        VonCroy->LOT.is_jumping = 1;
        VonCroy->LOT.is_monkeying = 1;
        VonCroy->maximum_turn = 1092;
        if (item->box_number == VonCroy->LOT.target_box || !VonCroy->monkey_ahead) {
            floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
            h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
            if (c == h - 1536)
                item->goal_anim_state = 4;
        }
        break;
    case 20:
        if (item->frame_number == anims[item->anim_number].frame_base)
            item->pos = oEnemy->pos;
        else if (item->frame_number == anims[item->anim_number].frame_base + 120) {
            ifl3 = -1;
            goto trig;
        }
        break;
    case 22:
    case 35:
        VonCroy->maximum_turn = 0;
        if (item->item_flags[2])
            CreatureYRot(&item->pos, oEnemy->pos.y_rot - item->pos.y_rot, 512);
        else
            CreatureYRot(&item->pos, iAngle, 512);
        break;
    case 36:
    case 37:
        VonCroy->maximum_turn = 0;
        MoveCreature3DPos(&item->pos, &oEnemy->pos, 8, oEnemy->pos.y_rot - item->pos.y_rot, 512);
        break;
    }
    CreatureTilt(item, tilt);
    CreatureJoint(item, 0, torso_y);
    CreatureJoint(item, 1, torso_x);
    CreatureJoint(item, 2, head_y);
    CreatureJoint(item, 3, torso_x);
    if (item->current_anim_state >= 15 || item->current_anim_state == 5)
        CreatureAnimation(item_number, angle, 0);
    else {
        switch (CreatureVault(item_number, angle, 2, 260)) {
        case 2:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 29;
            item->current_anim_state = 19;
            item->frame_number = anims[item->anim_number].frame_base;
            break;
        case 3:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 28;
            item->current_anim_state = 18;
            item->frame_number = anims[item->anim_number].frame_base;
            break;
        case 4:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 27;
            item->current_anim_state = 17;
            item->frame_number = anims[item->anim_number].frame_base;
            break;
        case -2:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 42;
            item->current_anim_state = 23;
            item->frame_number = anims[item->anim_number].frame_base;
            break;
        case -3:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 41;
            item->current_anim_state = 24;
            item->frame_number = anims[item->anim_number].frame_base;
            break;
        case -4:
            VonCroy->maximum_turn = 0;
            item->anim_number = objects[VON_CROY].anim_index + 35;
            item->current_anim_state = 25;
            item->frame_number = anims[item->anim_number].frame_base;
            break;
        }
    }
}
