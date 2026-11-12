#include <stdlib.h>

#include "missile.h"
#include "anim.h"
#include "debris.h"
#include "control.h"
#include "sound.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

sint32 ExplodeFX(FX_INFO *fx, sint32 NoXZVel, sint16 Num)
{
    sint16 **meshpp;

    meshpp = &meshes[fx->frame_number];
    ShatterItem.YRot = fx->pos.y_rot;
    ShatterItem.meshp = *meshpp;
    ShatterItem.Sphere.x = fx->pos.x_pos;
    ShatterItem.Sphere.y = fx->pos.y_pos;
    ShatterItem.Sphere.z = fx->pos.z_pos;
    ShatterItem.Bit = 0;
    ShatterItem.Flags = fx->flag2 & 0x400;
    ShatterObject(&ShatterItem, 0, Num, fx->room_number, NoXZVel);
    return 1;
}

void ControlBodyPart(sint16 fx_number)
{
    FX_INFO *fx;
    FLOOR_INFO *floor;
    sint32 height, ceiling, ox, oy, oz;
    sint16 room_number;

    fx = &effects[fx_number];
    ox = fx->pos.x_pos;
    oz = fx->pos.z_pos;
    oy = fx->pos.y_pos;
    if (fx->speed)
        fx->pos.x_rot += fx->fallspeed << 5;
    fx->fallspeed += 6;
    fx->pos.y_pos += fx->fallspeed;
    fx->pos.z_pos += (rcossin_tbl[((fx->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * fx->speed >> W2V_SHIFT;
    fx->pos.x_pos += (rcossin_tbl[(fx->pos.y_rot >> 3) & 0x1FFE] << 2) * fx->speed >> W2V_SHIFT;
    room_number = fx->room_number;
    floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
    ceiling = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
    if (fx->pos.y_pos < ceiling) {
        fx->pos.y_pos = ceiling;
        fx->speed -= fx->speed >> 3;
        fx->fallspeed = -fx->fallspeed;
    }
    height = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
    if (fx->pos.y_pos >= height) {
        if (fx->flag2 & 1) {
            fx->pos.x_pos = ox;
            fx->pos.y_pos = oy;
            fx->pos.z_pos = oz;
            if (fx->flag2 & 0x200)
                ExplodeFX(fx, -2, 32);
            else
                ExplodeFX(fx, -1, 32);
            KillEffect(fx_number);
            if (fx->flag2 & 0x800)
                SoundEffect(67, &fx->pos, 0);
            return;
        }
        if (oy > height) {
            fx->pos.y_rot += 32768;
            fx->pos.x_pos = ox;
            fx->pos.z_pos = oz;
        } else if (fx->fallspeed > 32)
            fx->fallspeed = -fx->fallspeed >> 2;
        else
            fx->fallspeed = 0;
        fx->speed -= fx->speed >> 2;
        if (abs(fx->speed) < 4)
            fx->speed = 0;
        fx->pos.y_pos = oy;
    }
    if (!fx->speed) {
        fx->flag1++;
        if (fx->flag1 > 32) {
            KillEffect(fx_number);
            return;
        }
    }
    if (fx->flag2 & 2 && GetRandomControl() & 1)
        DoBloodSplat((GetRandomControl() & 0x3F) + fx->pos.x_pos - 32, (GetRandomControl() & 0x1F) + fx->pos.y_pos - 16,
            (GetRandomControl() & 0x3F) + fx->pos.z_pos - 32, 1, (sint16)(GetRandomControl() << 1), fx->room_number);
    if (room_number != fx->room_number)
        EffectNewRoom(fx_number, room_number);
}
