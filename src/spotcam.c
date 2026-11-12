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
extern sint32 bFirstLook;
extern sint16 current_sequence;

sint32 Spline(sint32 x, sint32 *knots, sint32 nk);

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

void CalculateSpotCams(void)
{
    SPOTCAM *FirstCam;
    ITEM_INFO *item;
    CAMERA_INFO backup;
    CAMERA_TYPE ctype;
    sint32 cpx, cpy, cpz, ctx, cty, ctz, cspeed, cfov, croll, next_spline_camera, cunt;
    sint32 cs, cp, clen, tlen, cx, cy, cz, lx, ly, lz, sp, i, j;
    sint16 spline_cnt;

    if (bDisableLaraControl) {
        lara_item->hit_points = (sint16)LaraHealth;
        lara.air = (sint16)LaraAir;
    }
    FirstCam = &SpotCam[first_camera];
    if (FirstCam->flags & 8)
        spline_cnt = current_camera_cnt + 2;
    else
        spline_cnt = 4;
    cpx = Spline(current_spline_position, camera_xposition, spline_cnt);
    cpy = Spline(current_spline_position, camera_yposition, spline_cnt);
    cpz = Spline(current_spline_position, camera_zposition, spline_cnt);
    ctx = Spline(current_spline_position, camera_xtarget, spline_cnt);
    cty = Spline(current_spline_position, camera_ytarget, spline_cnt);
    ctz = Spline(current_spline_position, camera_ztarget, spline_cnt);
    cspeed = Spline(current_spline_position, camera_speed, spline_cnt);
    croll = Spline(current_spline_position, camera_roll, spline_cnt);
    cfov = Spline(current_spline_position, camera_fov, spline_cnt);
    if (SpotCam[current_spline_camera].flags & 0x1000 && CameraFade != current_spline_camera)
        CameraFade = current_spline_camera;
    if (SpotCam[current_spline_camera].flags & 0x2000 && CameraFade != current_spline_camera)
        CameraFade = current_spline_camera;
    if (FirstCam->flags & 8) {
        sp = 0;
        cp = 0;
        cs = 0x2000;
        lx = lara_item->pos.x_pos;
        ly = lara_item->pos.y_pos;
        lz = lara_item->pos.z_pos;
        for (i = 0; i < 8; i++) {
            tlen = 0x10000;
            for (j = 0; j < 8; j++) {
                cx = Spline(sp, camera_xposition, spline_cnt) - lx;
                cy = Spline(sp, camera_yposition, spline_cnt) - ly;
                cz = Spline(sp, camera_zposition, spline_cnt) - lz;
                clen = phd_sqrt(SQUARE(cx) + SQUARE(cy) + SQUARE(cz));
                if (clen <= tlen) {
                    cp = sp;
                    tlen = clen;
                }
                sp += cs;
                if (sp > 0x10000)
                    break;
            }
            cs >>= 1;
            sp = cp - ((cs >> 1) << 2);
            if (sp < 0)
                sp = 0;
        }
        current_spline_position += (cp - current_spline_position) >> 5;
        if (FirstCam->flags & 1) {
            if (ABS(cp - current_spline_position) > 0x8000)
                current_spline_position = cp;
        }
        if (current_spline_position < 0)
            current_spline_position = 0;
        else if (current_spline_position > 0x10000)
            current_spline_position = 0x10000;
    } else if (!spotcam_timer)
        current_spline_position += cspeed;
    if (!(input & IN_LOOK))
        bFirstLook = 0;
    if (!(FirstCam->flags & 0x200) && input & IN_LOOK && gfGameMode != 1) {
        if (FirstCam->flags & 8) {
            if (!bFirstLook) {
                camera.old_type = FIXED_CAMERA;
                bFirstLook = 1;
            }
            CalculateCamera();
        } else {
            SetFadeClip(0, 1);
            bUseSpotCam = 0;
            bDisableLaraControl = 0;
            camera.speed = 1;
            AlterFOV(LastFov);
            CalculateCamera();
            bCheckTrigger = 0;
        }
    } else {
        camera.pos.x = cpx;
        camera.pos.y = cpy;
        camera.pos.z = cpz;
        if (FirstCam->flags & 0x28) {
            ctx = lara_item->pos.x_pos;
            cty = lara_item->pos.y_pos;
            ctz = lara_item->pos.z_pos;
        }
        if (SpotCam[current_spline_camera].flags & 2) {
            item = &items[SpotCam[current_spline_camera].timer];
            if (item) {
                ctx = item->pos.x_pos;
                cty = item->pos.y_pos;
                ctz = item->pos.z_pos;
            }
        }
        camera.target.x = ctx;
        camera.target.y = cty;
        camera.target.z = ctz;
        if (IsRoomOutside(cpx, cpy, cpz) != -2)
            camera.pos.room_number = IsRoomOutsideNo;
        else {
            camera.pos.room_number = SpotCam[current_spline_camera].room_number;
            GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &camera.pos.room_number);
        }
        AlterFOV((sint16)cfov);
        phd_LookAt(camera.pos.x, camera.pos.y, camera.pos.z, camera.target.x, camera.target.y, camera.target.z, (sint16)croll);
        if (bCheckTrigger) {
            ctype = camera.type;
            camera.type = HEAVY_CAMERA;
            if (gfCurrentLevel)
                TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
            else {
                TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 0, 0);
                TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
            }
            camera.type = ctype;
            bCheckTrigger = 0;
        }
        if (FirstCam->flags & 8)
            bTrackCamInit = 1;
        else if (current_spline_position > 0x10000 - cspeed) {
            if (SpotCam[current_spline_camera].flags & 0x4000)
                bCheckTrigger = 1;
            if (SpotCam[current_spline_camera].flags & 0x100) {
                if (spotcam_timer)
                    spotcam_timer--;
                else
                    spotcam_timer = SpotCam[current_spline_camera].timer >> 4;
            }
            if (!spotcam_timer) {
                current_spline_position = 0;
                if (current_spline_camera == first_camera)
                    next_spline_camera = last_camera;
                else
                    next_spline_camera = current_spline_camera - 1;
                cunt = 1;
                if (spline_from_camera) {
                    spline_from_camera = 0;
                    next_spline_camera = first_camera - 1;
                } else {
                    if (SpotCam[current_spline_camera].flags & 0x800)
                        bDisableLaraControl = 0;
                    if (SpotCam[current_spline_camera].flags & 0x400) {
                        SetFadeClip(16, 1);
                        bDisableLaraControl = 1;
                    }
                    cunt = 0;
                    if (SpotCam[current_spline_camera].flags & 0x80) {
                        next_spline_camera = first_camera + (SpotCam[current_spline_camera].timer & 0xF);
                        current_spline_camera = (sint16)next_spline_camera;
                        SetSplineData_inl(cunt, next_spline_camera);
                        cunt = 1;
                    }
                    SetSplineData_inl(cunt, next_spline_camera);
                    cunt++;
                }
                next_spline_camera++;
                while (cunt < 4) {
                    if (FirstCam->flags & 4) {
                        if (next_spline_camera > last_camera)
                            next_spline_camera = first_camera;
                    } else if (next_spline_camera > last_camera)
                        next_spline_camera = last_camera;
                    SetSplineData_inl(cunt, next_spline_camera);
                    next_spline_camera++;
                    cunt++;
                }
                current_spline_camera++;
                if (current_spline_camera > last_camera) {
                    if (FirstCam->flags & 4) {
                        spotcam_loopcnt++;
                        current_spline_camera = first_camera;
                    } else if (!(FirstCam->flags & 0x40) && !spline_to_camera) {
                        current_spline_camera--;
                        SetSplineData_inl(0, current_spline_camera - 1);
                        SetSplineData_inl(1, current_spline_camera);
                        memcpy(&backup, &camera, sizeof(CAMERA_INFO));
                        camera.old_type = FIXED_CAMERA;
                        camera.type = CHASE_CAMERA;
                        camera.speed = 1;
                        CalculateCamera();
                        camera_fov[2] = CurrentFov;
                        InitialCameraPosition.x = camera.pos.x;
                        InitialCameraPosition.y = camera.pos.y;
                        InitialCameraPosition.z = camera.pos.z;
                        InitialCameraTarget.x = camera.target.x;
                        InitialCameraTarget.y = camera.target.y;
                        InitialCameraTarget.z = camera.target.z;
                        camera_xposition[2] = camera.pos.x;
                        camera_yposition[2] = camera.pos.y;
                        camera_zposition[2] = camera.pos.z;
                        camera_xtarget[2] = camera.target.x;
                        camera_ytarget[2] = camera.target.y;
                        camera_ztarget[2] = camera.target.z;
                        camera_roll[2] = 0;
                        camera_speed[2] = camera_speed[1];
                        camera_xposition[3] = camera.pos.x;
                        camera_yposition[3] = camera.pos.y;
                        camera_zposition[3] = camera.pos.z;
                        camera_xtarget[3] = camera.target.x;
                        camera_ytarget[3] = camera.target.y;
                        camera_ztarget[3] = camera.target.z;
                        camera_fov[3] = CurrentFov;
                        camera_speed[3] = camera_speed[1] >> 1;
                        memcpy(&camera, &backup, sizeof(CAMERA_INFO));
                        phd_LookAt(backup.pos.x, backup.pos.y, backup.pos.z, backup.target.x, backup.target.y, backup.target.z, (sint16)croll);
                        spline_to_camera = 1;
                    } else {
                        if (bCheckTrigger) {
                            ctype = camera.type;
                            camera.type = HEAVY_CAMERA;
                            if (gfCurrentLevel)
                                TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
                            else {
                                TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 0, 0);
                                TestTriggersAtXYZ(camera.pos.x, camera.pos.y, camera.pos.z, camera.pos.room_number, 1, 0);
                            }
                            camera.type = ctype;
                            bCheckTrigger = 0;
                        }
                        SetFadeClip(0, 1);
                        bUseSpotCam = 0;
                        bDisableLaraControl = 0;
                        bCheckTrigger = 0;
                        camera.old_type = FIXED_CAMERA;
                        camera.type = CHASE_CAMERA;
                        camera.speed = 1;
                        if (FirstCam->flags & 0x40) {
                            camera.pos.x = InitialCameraPosition.x;
                            camera.pos.y = InitialCameraPosition.y;
                            camera.pos.z = InitialCameraPosition.z;
                            camera.target.x = InitialCameraTarget.x;
                            camera.target.y = InitialCameraTarget.y;
                            camera.target.z = InitialCameraTarget.z;
                            camera.pos.room_number = InitialCameraRoom;
                        }
                        AlterFOV(LastFov);
                    }
                }
            }
        }
    }
}
