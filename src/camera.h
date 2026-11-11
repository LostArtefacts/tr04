#ifndef CAMERA_H
#define CAMERA_H

#include "items.h"

#define FOLLOW_CENTRE 1
#define NO_CHUNKY 2

typedef enum {
    CHASE_CAMERA,
    FIXED_CAMERA,
    LOOK_CAMERA,
    COMBAT_CAMERA,
    CINEMATIC_CAMERA,
    HEAVY_CAMERA
} CAMERA_TYPE;

typedef struct camera_info {
    GAME_VECTOR pos;
    GAME_VECTOR target;
    CAMERA_TYPE type;
    CAMERA_TYPE old_type;
    sint32 shift;
    sint32 flags;
    sint32 fixed_camera;
    sint32 number_frames;
    sint32 bounce;
    sint32 underwater;
    sint32 target_distance;
    sint16 target_angle;
    sint16 target_elevation;
    sint16 actual_elevation;
    sint16 actual_angle;
    sint16 number;
    sint16 last;
    sint16 timer;
    sint16 speed;
    ITEM_INFO *item;
    ITEM_INFO *last_item;
    OBJECT_VECTOR *fixed;
    sint32 mike_at_lara;
    PHD_VECTOR mike_pos;
} CAMERA_INFO;

extern CAMERA_INFO camera;
extern GAME_VECTOR ForcedFixedCamera;
extern char UseForcedFixedCamera;
extern sint32 BinocularRange;

#endif
