#include <string.h>

#include "voncroy.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "camera.h"
#include "lara.h"
#include "specific/3dmath.h"

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
