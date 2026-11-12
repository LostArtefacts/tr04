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

typedef struct {
    sint16 current_anim_state;
    sint16 goal_anim_state;
    sint32 target_distance;
    sint16 target_angle;
    sint16 target_elevation;
    PHD_3DPOS pos;
    PHD_3DPOS pos2;
    PHD_VECTOR t;
} OLD_CAMERA;

extern CAMERA_INFO camera;
extern GAME_VECTOR last_target;
extern sint32 LaserSight;
extern GAME_VECTOR ForcedFixedCamera;
extern char UseForcedFixedCamera;
extern sint32 BinocularRange;
extern sint32 BinocularOn;
extern OLD_CAMERA old_cam;
extern GAME_VECTOR last_ideal;
extern uint8 camerasnaps;
extern sint32 Unk7e71cc;
extern sint32 BinocularOldCamera;
extern sint32 bLaraTorch;
extern PHD_VECTOR LaraTorchStart;
extern PHD_VECTOR LaraTorchEnd;
extern sint32 LaraTorchIntensity;
extern sint32 LaraTorchYRot;

void MoveCamera(GAME_VECTOR *ideal, sint32 speed);
void CalculateCamera(void);
void LaraTorch(PHD_VECTOR *Soffset, PHD_VECTOR *Eoffset, sint16 yrot, sint32 brightness);
void InitialiseCamera(void);
void FixedCamera(void);
sint32 mgLOS(GAME_VECTOR *start, GAME_VECTOR *target, sint32 push);
void ChaseCamera(ITEM_INFO *item);
void CombatCamera(ITEM_INFO *item);
sint32 CameraCollisionBounds(GAME_VECTOR *ideal, sint32 push, sint32 yfirst);
void BinocularCamera(ITEM_INFO *item);

#endif
