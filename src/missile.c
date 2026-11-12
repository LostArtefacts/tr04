#include "missile.h"
#include "anim.h"
#include "debris.h"

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
