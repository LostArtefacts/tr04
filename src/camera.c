#include "camera.h"
#include "control.h"
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

sint32 mgLOS(GAME_VECTOR *start, GAME_VECTOR *target, sint32 push)
{
    FLOOR_INFO *floor;
    sint32 x, y, z, h, c, cdiff, hdiff, dx, dy, dz, clipped, nc, i;
    sint16 room_number, room_number2;

    dx = (target->x - start->x) >> 3;
    dy = (target->y - start->y) >> 3;
    dz = (target->z - start->z) >> 3;
    x = start->x;
    y = start->y;
    z = start->z;
    room_number2 = start->room_number;
    nc = 0;
    clipped = 0;
    for (i = 0; i < 8; i++) {
        room_number = room_number2;
        floor = GetFloor(x, y, z, &room_number2);
        h = GetHeight(floor, x, y, z);
        c = GetCeiling(floor, x, y, z);
        if (h == NO_HEIGHT || c == NO_HEIGHT || c >= h) {
            if (!nc) {
                x += dx;
                y += dy;
                z += dz;
                continue;
            }
            clipped = 1;
            break;
        }
        if (y > h) {
            hdiff = y - h;
            if (hdiff < push)
                y = h;
            else {
                clipped = 1;
                break;
            }
        }
        if (y < c) {
            cdiff = c - y;
            if (cdiff < push)
                y = c;
            else {
                clipped = 1;
                break;
            }
        }
        nc = 1;
        x += dx;
        y += dy;
        z += dz;
    }
    if (i) {
        x -= dx;
        y -= dy;
        z -= dz;
    }
    target->x = x;
    target->y = y;
    target->z = z;
    GetFloor(x, y, z, &room_number);
    target->room_number = room_number;
    return !clipped;
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
