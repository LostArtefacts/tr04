#include "camera.h"
#include "lara.h"
#include "specific/3dmath.h"

void InitialiseCamera(void)
{
    camera.target.x = last_target.x = lara_item->pos.x_pos;
    camera.target.y = last_target.y = camera.shift = lara_item->pos.y_pos - 1024;
    camera.target.z = last_target.z = lara_item->pos.z_pos;
    camera.target.room_number = last_target.room_number = lara_item->room_number;
    camera.pos.x = camera.target.x;
    camera.pos.y = camera.target.y;
    camera.pos.z = camera.target.z - 100;
    camera.pos.room_number = camera.target.room_number;
    camera.target_distance = 1536;
    camera.item = 0;
    camera.number_frames = 1;
    camera.type = CHASE_CAMERA;
    camera.speed = 1;
    camera.flags = 0;
    camera.bounce = 0;
    camera.number = -1;
    camera.fixed_camera = 0;
    AlterFOV(14560);
    UseForcedFixedCamera = 0;
    CalculateCamera();
}

void FixedCamera(void)
{
    OBJECT_VECTOR *fixed;
    GAME_VECTOR ideal;

    if (UseForcedFixedCamera) {
        ideal.x = ForcedFixedCamera.x;
        ideal.y = ForcedFixedCamera.y;
        ideal.z = ForcedFixedCamera.z;
        ideal.room_number = ForcedFixedCamera.room_number;
    } else {
        fixed = &camera.fixed[camera.number];
        ideal.x = fixed->x;
        ideal.y = fixed->y;
        ideal.z = fixed->z;
        ideal.room_number = fixed->data;
    }
    camera.fixed_camera = 1;
    MoveCamera(&ideal, 1);
    if (camera.timer) {
        camera.timer--;
        if (!camera.timer)
            camera.timer = -1;
    }
}

void LaraTorch(PHD_VECTOR *Soffset, PHD_VECTOR *Eoffset, sint16 yrot, sint32 brightness)
{
    bLaraTorch = 1;
    LaraTorchStart.x = Soffset->x;
    LaraTorchStart.y = Soffset->y;
    LaraTorchStart.z = Soffset->z;
    LaraTorchEnd.x = Eoffset->x;
    LaraTorchEnd.y = Eoffset->y;
    LaraTorchEnd.z = Eoffset->z;
    LaraTorchIntensity = brightness;
    LaraTorchYRot = yrot;
}
