#ifndef SPOTCAM_H
#define SPOTCAM_H

#include "types.h"

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
extern SPOTCAM SpotCam[128];
extern sint16 number_spotcams;

void InitialiseSpotCam(sint16 Sequence);

#endif
