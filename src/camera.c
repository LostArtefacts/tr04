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

void ChaseCamera(ITEM_INFO *item)
{
    FLOOR_INFO *floor;
    GAME_VECTOR ideal;
    GAME_VECTOR ideals[5];
    GAME_VECTOR temp[2];
    sint32 distance, dx, dz, farthest, farthestnum, h, c, i;
    sint16 angle;

    if (!camera.target_elevation)
        camera.target_elevation = -1820;
    camera.target_elevation += item->pos.x_rot;
    if (camera.target_elevation > 15470)
        camera.target_elevation = 15470;
    else if (camera.target_elevation < -15470)
        camera.target_elevation = -15470;
    distance = ((rcossin_tbl[((camera.target_elevation >> 3) & 0x1FFE) + 1] << 2) * camera.target_distance) >> W2V_SHIFT;
    floor = GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);
    h = GetHeight(floor, camera.target.x, camera.target.y, camera.target.z);
    c = GetCeiling(floor, camera.target.x, camera.target.y, camera.target.z);
    if (c + 16 > h - 16 && h != NO_HEIGHT && c != NO_HEIGHT) {
        camera.target.y = (c + h) >> 1;
        camera.target_elevation = 0;
    } else if (camera.target.y > h - 16 && h != NO_HEIGHT) {
        camera.target.y = h - 16;
        camera.target_elevation = 0;
    } else if (camera.target.y < c + 16 && c != NO_HEIGHT) {
        camera.target.y = c + 16;
        camera.target_elevation = 0;
    }
    GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);
    {
        sint16 room_number;
        sint32 wx, wy, wz;

        wx = camera.target.x;
        wy = camera.target.y;
        wz = camera.target.z;
        room_number = camera.target.room_number;
        floor = GetFloor(wx, wy, wz, &room_number);
        h = GetHeight(floor, wx, wy, wz);
        c = GetCeiling(floor, wx, wy, wz);
        if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT) {
            camera.target.x = last_target.x;
            camera.target.y = last_target.y;
            camera.target.z = last_target.z;
            camera.target.room_number = last_target.room_number;
        }
    }
    for (i = 0; i < 5; i++)
        ideals[i].y = (((rcossin_tbl[(camera.target_elevation >> 3) & 0x1FFE] << 2) * camera.target_distance) >> W2V_SHIFT) + camera.target.y;
    farthest = 0x7FFFFFFF;
    farthestnum = 0;
    for (i = 0; i < 5; i++) {
        if (!i)
            angle = item->pos.y_rot + camera.target_angle;
        else
            angle = (i - 1) << 14;
        ideals[i].x = camera.target.x - (((rcossin_tbl[(angle >> 3) & 0x1FFE] << 2) * distance) >> W2V_SHIFT);
        ideals[i].z = camera.target.z - (((rcossin_tbl[((angle >> 3) & 0x1FFE) + 1] << 2) * distance) >> W2V_SHIFT);
        ideals[i].room_number = camera.target.room_number;
        if (mgLOS(&camera.target, &ideals[i], 200)) {
            temp[0].x = ideals[i].x;
            temp[0].y = ideals[i].y;
            temp[0].z = ideals[i].z;
            temp[0].room_number = ideals[i].room_number;
            temp[1].x = camera.pos.x;
            temp[1].y = camera.pos.y;
            temp[1].z = camera.pos.z;
            temp[1].room_number = camera.pos.room_number;
            if (!i) {
                farthestnum = 0;
                break;
            }
            if (mgLOS(&temp[0], &temp[1], 0)) {
                dx = SQUARE(camera.pos.x - ideals[i].x);
                dz = SQUARE(camera.pos.z - ideals[i].z);
                dz += dx;
                if (dz < farthest) {
                    farthest = dz;
                    farthestnum = i;
                }
            }
        } else if (!i) {
            temp[0].x = ideals[i].x;
            temp[0].y = ideals[i].y;
            temp[0].z = ideals[i].z;
            temp[0].room_number = ideals[i].room_number;
            temp[1].x = camera.pos.x;
            temp[1].y = camera.pos.y;
            temp[1].z = camera.pos.z;
            temp[1].room_number = camera.pos.room_number;
            dx = SQUARE(camera.target.x - ideals[i].x);
            dz = SQUARE(camera.target.z - ideals[i].z);
            dz += dx;
            if (dz > 0x90000) {
                farthestnum = 0;
                break;
            }
        }
    }
    ideal.x = ideals[farthestnum].x;
    ideal.y = ideals[farthestnum].y;
    ideal.z = ideals[farthestnum].z;
    ideal.room_number = ideals[farthestnum].room_number;
    CameraCollisionBounds(&ideal, 384, 1);
    if (camera.old_type == FIXED_CAMERA)
        camera.speed = 1;
    MoveCamera(&ideal, camera.speed);
}

void CombatCamera(ITEM_INFO *item)
{
    FLOOR_INFO *floor;
    GAME_VECTOR ideal;
    GAME_VECTOR ideals[9];
    GAME_VECTOR temp[2];
    sint32 distance, dx, dz, farthest, farthestnum, h, c, i;
    sint16 angle;

    camera.target.z = item->pos.z_pos;
    camera.target.x = item->pos.x_pos;
    if (lara.target) {
        camera.target_angle = item->pos.y_rot + lara.target_angles[0];
        camera.target_elevation = item->pos.x_rot + lara.target_angles[1];
    } else {
        camera.target_angle = item->pos.y_rot + lara.head_y_rot + lara.torso_y_rot;
        camera.target_elevation = item->pos.x_rot + lara.head_x_rot + lara.torso_x_rot - 2730;
    }
    floor = GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);
    h = GetHeight(floor, camera.target.x, camera.target.y, camera.target.z);
    c = GetCeiling(floor, camera.target.x, camera.target.y, camera.target.z);
    if (c + 64 > h - 64 && h != NO_HEIGHT && c != NO_HEIGHT) {
        camera.target.y = (c + h) >> 1;
        camera.target_elevation = 0;
    } else if (camera.target.y > h - 64 && h != NO_HEIGHT) {
        camera.target.y = h - 64;
        camera.target_elevation = 0;
    } else if (camera.target.y < c + 64 && c != NO_HEIGHT) {
        camera.target.y = c + 64;
        camera.target_elevation = 0;
    }
    GetFloor(camera.target.x, camera.target.y, camera.target.z, &camera.target.room_number);
    {
        sint16 room_number;
        sint32 wx, wy, wz;

        wx = camera.target.x;
        wy = camera.target.y;
        wz = camera.target.z;
        room_number = camera.target.room_number;
        floor = GetFloor(wx, wy, wz, &room_number);
        h = GetHeight(floor, wx, wy, wz);
        c = GetCeiling(floor, wx, wy, wz);
        if (wy < c || wy > h || c >= h || h == NO_HEIGHT || c == NO_HEIGHT) {
            camera.target.x = last_target.x;
            camera.target.y = last_target.y;
            camera.target.z = last_target.z;
            camera.target.room_number = last_target.room_number;
        }
    }
    camera.target_distance = 1536;
    distance = ((rcossin_tbl[((camera.target_elevation >> 3) & 0x1FFE) + 1] << 2) * 1536) >> W2V_SHIFT;
    for (i = 0; i < 5; i++)
        ideals[i].y = (((rcossin_tbl[(camera.target_elevation >> 3) & 0x1FFE] << 2) * 1536) >> W2V_SHIFT) + camera.target.y;
    farthest = 0x7FFFFFFF;
    farthestnum = 0;
    for (i = 0; i < 5; i++) {
        if (!i)
            angle = camera.target_angle;
        else
            angle = (i - 1) << 14;
        ideals[i].x = camera.target.x - (((rcossin_tbl[(angle >> 3) & 0x1FFE] << 2) * distance) >> W2V_SHIFT);
        ideals[i].z = camera.target.z - (((rcossin_tbl[((angle >> 3) & 0x1FFE) + 1] << 2) * distance) >> W2V_SHIFT);
        ideals[i].room_number = camera.target.room_number;
        if (mgLOS(&camera.target, &ideals[i], 200)) {
            temp[0].x = ideals[i].x;
            temp[0].y = ideals[i].y;
            temp[0].z = ideals[i].z;
            temp[0].room_number = ideals[i].room_number;
            temp[1].x = camera.pos.x;
            temp[1].y = camera.pos.y;
            temp[1].z = camera.pos.z;
            temp[1].room_number = camera.pos.room_number;
            if (!i) {
                farthestnum = 0;
                break;
            }
            if (mgLOS(&temp[0], &temp[1], 0)) {
                dx = SQUARE(camera.pos.x - ideals[i].x);
                dz = SQUARE(camera.pos.z - ideals[i].z);
                dz += dx;
                if (dz < farthest) {
                    farthest = dz;
                    farthestnum = i;
                }
            }
        } else if (!i) {
            temp[0].x = ideals[i].x;
            temp[0].y = ideals[i].y;
            temp[0].z = ideals[i].z;
            temp[0].room_number = ideals[i].room_number;
            temp[1].x = camera.pos.x;
            temp[1].y = camera.pos.y;
            temp[1].z = camera.pos.z;
            temp[1].room_number = camera.pos.room_number;
            dx = SQUARE(camera.target.x - ideals[i].x);
            dz = SQUARE(camera.target.z - ideals[i].z);
            dz += dx;
            if (dz > 0x90000) {
                farthestnum = 0;
                break;
            }
        }
    }
    ideal.x = ideals[farthestnum].x;
    ideal.y = ideals[farthestnum].y;
    ideal.z = ideals[farthestnum].z;
    ideal.room_number = ideals[farthestnum].room_number;
    CameraCollisionBounds(&ideal, 384, 1);
    if (camera.old_type == FIXED_CAMERA)
        camera.speed = 1;
    MoveCamera(&ideal, camera.speed);
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
