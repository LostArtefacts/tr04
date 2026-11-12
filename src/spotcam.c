#include "spotcam.h"
#include "items.h"
#include "camera.h"
#include "control.h"
#include "lara.h"
#include "gameflow.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/input.h"

extern PHD_VECTOR InitialCameraPosition;
extern PHD_VECTOR InitialCameraTarget;
extern sint32 CameraFade;
extern sint32 LaraHealth;
extern sint32 LaraAir;
extern sint32 spline_to_camera;
extern sint32 spline_from_camera;
extern sint32 bCheckTrigger;
extern sint32 current_spline_position;
extern sint16 current_spline_camera;
extern sint16 current_camera_cnt;
extern sint16 first_camera;
extern sint16 last_camera;
extern sint16 InitialCameraRoom;
extern sint16 LastFov;
extern sint16 spotcam_timer;
extern sint16 spotcam_loopcnt;
extern sint16 current_sequence;

void InitSpotCamSequences(void)
{
    sint32 s, cc, ce, i;

    bTrackCamInit = 0;
    if (number_spotcams) {
        ce = 0;
        s = SpotCam[0].sequence;
        cc = 1;
        for (i = 1; i < number_spotcams; i++) {
            if (SpotCam[i].sequence == s)
                cc++;
            else {
                CameraCnt[ce] = cc;
                SpotRemap[s] = ce;
                s = SpotCam[i].sequence;
                cc = 1;
                ce++;
            }
        }
        CameraCnt[ce] = cc;
        SpotRemap[s] = ce;
    }
}

static __inline void SetSplineData_inl(sint32 num, sint32 cam)
{
    ITEM_INFO *item;

    camera_xposition[num] = SpotCam[cam].x;
    camera_yposition[num] = SpotCam[cam].y;
    camera_zposition[num] = SpotCam[cam].z;
    camera_xtarget[num] = SpotCam[cam].tx;
    camera_ytarget[num] = SpotCam[cam].ty;
    camera_ztarget[num] = SpotCam[cam].tz;
    if (SpotCam[cam].flags & 0x10) {
        camera_xtarget[num] = LaraFixedPosition.x;
        camera_ytarget[num] = LaraFixedPosition.y;
        camera_ztarget[num] = LaraFixedPosition.z;
    } else {
        camera_xtarget[num] = SpotCam[cam].tx;
        camera_ytarget[num] = SpotCam[cam].ty;
        camera_ztarget[num] = SpotCam[cam].tz;
    }
    if (SpotCam[cam].flags & 2) {
        item = &items[SpotCam[cam].timer];
        if (item) {
            camera_xtarget[num] = item->pos.x_pos;
            camera_ytarget[num] = item->pos.y_pos;
            camera_ztarget[num] = item->pos.z_pos;
        }
    }
    camera_roll[num] = SpotCam[cam].roll;
    camera_fov[num] = SpotCam[cam].fov;
    camera_speed[num] = SpotCam[cam].speed;
}

void InitialiseSpotCam(sint16 Sequence)
{
    SPOTCAM *s;
    ITEM_INFO *item;
    sint32 next_spline_camera, cunt, i;

    if (bTrackCamInit && Sequence == LastSequence) {
        bTrackCamInit = 0;
        return;
    }
    LastFov = CurrentFov;
    CameraFade = -1;
    LastSequence = Sequence;
    bTrackCamInit = 0;
    spotcam_timer = 0;
    spotcam_loopcnt = 0;
    bDisableLaraControl = 0;
    LaraHealth = lara_item->hit_points;
    LaraAir = lara.air;
    InitialCameraTarget.x = camera.target.x;
    InitialCameraTarget.y = camera.target.y;
    InitialCameraTarget.z = camera.target.z;
    InitialCameraPosition.x = camera.pos.x;
    InitialCameraPosition.y = camera.pos.y;
    InitialCameraPosition.z = camera.pos.z;
    InitialCameraRoom = camera.pos.room_number;
    LaraFixedPosition.x = lara_item->pos.x_pos;
    LaraFixedPosition.y = lara_item->pos.y_pos;
    LaraFixedPosition.z = lara_item->pos.z_pos;
    current_sequence = Sequence;
    current_spline_camera = 0;
    for (i = 0; i < SpotRemap[Sequence]; i++)
        current_spline_camera += CameraCnt[i];
    current_camera_cnt = CameraCnt[SpotRemap[Sequence]];
    first_camera = current_spline_camera;
    last_camera = first_camera + current_camera_cnt - 1;
    current_spline_position = 0;
    spline_to_camera = 0;
    s = &SpotCam[current_spline_camera];
    if (s->flags & 0x400 || gfGameMode == 1) {
        bDisableLaraControl = 1;
        if (gfGameMode != 1)
            SetFadeClip(16, 1);
    }
    if (s->flags & 8) {
        spline_from_camera = 0;
        cunt = 0;
        SetSplineData_inl(cunt, first_camera);
        cunt++;
        next_spline_camera = first_camera;
        for (i = 0; i < current_camera_cnt; i++) {
            SetSplineData_inl(cunt, next_spline_camera);
            cunt++;
            next_spline_camera++;
        }
        SetSplineData_inl(cunt, last_camera);
    } else if (s->flags & 1) {
        spline_from_camera = 0;
        cunt = 0;
        next_spline_camera = current_spline_camera;
        camera_xposition[cunt] = SpotCam[next_spline_camera].x;
        camera_yposition[cunt] = SpotCam[next_spline_camera].y;
        camera_zposition[cunt] = SpotCam[next_spline_camera].z;
        camera_xtarget[cunt] = SpotCam[next_spline_camera].tx;
        camera_ytarget[cunt] = SpotCam[next_spline_camera].ty;
        camera_ztarget[cunt] = SpotCam[next_spline_camera].tz;
        if (SpotCam[next_spline_camera].flags & 0x10) {
            camera_xtarget[cunt] = LaraFixedPosition.x;
            camera_ytarget[cunt] = LaraFixedPosition.y;
            camera_ztarget[cunt] = LaraFixedPosition.z;
        } else {
            camera_xtarget[cunt] = SpotCam[next_spline_camera].tx;
            camera_ytarget[cunt] = SpotCam[next_spline_camera].ty;
            camera_ztarget[cunt] = SpotCam[next_spline_camera].tz;
        }
        if (SpotCam[next_spline_camera].flags & 2) {
            item = &items[SpotCam[next_spline_camera].timer];
            if (item) {
                camera_xtarget[cunt] = item->pos.x_pos;
                camera_ytarget[cunt] = item->pos.y_pos;
                camera_ztarget[cunt] = item->pos.z_pos;
            }
        }
        camera_roll[cunt] = SpotCam[next_spline_camera].roll;
        camera_fov[cunt] = SpotCam[next_spline_camera].fov;
        camera_speed[cunt] = SpotCam[next_spline_camera].speed;
        cunt++;
        while (cunt < 4) {
            if (next_spline_camera > last_camera)
                next_spline_camera = first_camera;
            SetSplineData(cunt, next_spline_camera);
            cunt++;
            next_spline_camera++;
        }
        current_spline_camera++;
        if (current_spline_camera > last_camera)
            current_spline_camera = first_camera;
        if (s->flags & 0x4000)
            bCheckTrigger = 1;
    } else {
        spline_from_camera = 1;
        camera_xtarget[0] = InitialCameraTarget.x;
        camera_ytarget[0] = InitialCameraTarget.y;
        camera_ztarget[0] = InitialCameraTarget.z;
        camera_xposition[0] = InitialCameraPosition.x;
        camera_yposition[0] = InitialCameraPosition.y;
        camera_zposition[0] = InitialCameraPosition.z;
        camera_roll[0] = 0;
        camera_fov[0] = CurrentFov;
        camera_xposition[1] = camera_xposition[0];
        camera_yposition[1] = camera_yposition[0];
        camera_zposition[1] = camera_zposition[0];
        camera_xtarget[1] = camera_xtarget[0];
        camera_ytarget[1] = camera_ytarget[0];
        camera_ztarget[1] = camera_ztarget[0];
        camera_roll[1] = camera_roll[0];
        camera_fov[1] = camera_fov[0];
        camera_speed[1] = camera_speed[0];
        cunt = current_spline_camera;
        SetSplineData(2, cunt);
        cunt++;
        if (cunt > last_camera)
            cunt = first_camera;
        SetSplineData(3, cunt);
    }
}

void SetSplineData(sint32 num, sint32 cam)
{
    SetSplineData_inl(num, cam);
}
