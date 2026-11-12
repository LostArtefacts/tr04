#include "spotcam.h"
#include "items.h"

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

void SetSplineData(sint32 num, sint32 cam)
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
