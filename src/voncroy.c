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
