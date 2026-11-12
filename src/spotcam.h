#ifndef SPOTCAM_H
#define SPOTCAM_H

#include "3dglodef.h"

#define SP_FLYBYONESHOT 0x8000

typedef struct SPOTCAM {
    sint32 x;
    sint32 y;
    sint32 z;
    sint32 tx;
    sint32 ty;
    sint32 tz;
    uint8 sequence;
    uint8 camera;
    sint16 fov;
    sint16 roll;
    sint16 timer;
    sint16 speed;
    sint16 flags;
    sint16 room_number;
    sint16 pad;
} SPOTCAM;

extern sint32 bDisableLaraControl;
extern sint32 bUseSpotCam;
extern sint16 LastSequence;
extern SPOTCAM SpotCam[128];
extern sint16 number_spotcams;
extern sint32 bTrackCamInit;
extern uint8 SpotRemap[8];
extern uint8 CameraCnt[8];
extern PHD_VECTOR LaraFixedPosition;
extern sint32 camera_xposition[18];
extern sint32 camera_yposition[18];
extern sint32 camera_zposition[18];
extern sint32 camera_xtarget[18];
extern sint32 camera_ytarget[18];
extern sint32 camera_ztarget[18];
extern sint32 camera_roll[18];
extern sint32 camera_fov[18];
extern sint32 camera_speed[18];

void InitialiseSpotCam(sint16 Sequence);
void InitSpotCamSequences(void);
void SetSplineData(sint32 num, sint32 cam);

#endif
