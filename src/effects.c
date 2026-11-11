#include "effects.h"
#include "lara.h"
#include "sound.h"
#include "camera.h"
#include "control.h"
#include "hair.h"
#include "anim.h"
#include "delstuff.h"
#include "objnums.h"
#include "effect2.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "draw.h"
#include "room.h"

sint32 ItemNearLara(PHD_3DPOS *pos, sint32 distance)
{
    sint32 x, y, z;
    sint16 *bounds;

    x = pos->x_pos - lara_item->pos.x_pos;
    y = pos->y_pos - lara_item->pos.y_pos;
    z = pos->z_pos - lara_item->pos.z_pos;
    if (x < -distance || x > distance || z < -distance || z > distance || y < -WALL_L * 3 || y > WALL_L * 3)
        return 0;
    if (x * x + z * z > distance * distance)
        return 0;
    bounds = GetBoundsAccurate(lara_item);
    if (y < bounds[2] || y > bounds[3] + 100)
        return 0;
    return 1;
}

void Richochet(GAME_VECTOR *pos)
{
    sint32 angle;

    angle = (mGetAngle(pos->z, pos->x, lara_item->pos.z_pos, lara_item->pos.x_pos) >> 4) & 4095;
    TriggerRicochetSpark(pos, angle, 3, 0);
    SoundEffect(SFX_LARA_RICOCHET, (PHD_3DPOS *)pos, SFX_DEFAULT);
}

void DoLotsOfBlood(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 direction, sint16 room_num, sint32 number)
{
    for (; number > 0; number--)
        DoBloodSplat(x - (GetRandomControl() << 9) / 0x8000 + 256, y - (GetRandomControl() << 9) / 0x8000 + 256, z - (GetRandomControl() << 9) / 0x8000 + 256, speed, direction, room_num);
}

void turn180_effect(ITEM_INFO *item)
{
    item->pos.y_rot += -0x8000;
    item->pos.x_rot = -item->pos.x_rot;
}

void ExplosionFX(ITEM_INFO *item)
{
    SoundEffect(105, NULL, 0);
    camera.bounce = -75;
    flipeffect = -1;
}

void lara_hands_free(ITEM_INFO *item)
{
    lara.gun_status = LG_NO_ARMS;
}

void draw_right_gun(ITEM_INFO *item)
{
    sint16 *tmp;

    tmp = lara.mesh_ptrs[LM_RTHIGH];
    lara.mesh_ptrs[LM_RTHIGH] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_RTHIGH * 2];
    meshes[objects[PISTOLS_ANIM].mesh_index + LM_RTHIGH * 2] = tmp;
    tmp = lara.mesh_ptrs[LM_RHAND];
    lara.mesh_ptrs[LM_RHAND] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_RHAND * 2];
    meshes[objects[PISTOLS_ANIM].mesh_index + LM_RHAND * 2] = tmp;
}

void draw_left_gun(ITEM_INFO *item)
{
    sint16 *tmp;

    tmp = lara.mesh_ptrs[LM_LTHIGH];
    lara.mesh_ptrs[LM_LTHIGH] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_LTHIGH * 2];
    meshes[objects[PISTOLS_ANIM].mesh_index + LM_LTHIGH * 2] = tmp;
    tmp = lara.mesh_ptrs[LM_LHAND];
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_LHAND * 2];
    meshes[objects[PISTOLS_ANIM].mesh_index + LM_LHAND * 2] = tmp;
}

void shoot_right_gun(ITEM_INFO *item)
{
    lara.right_arm.flash_gun = 3;
}

void shoot_left_gun(ITEM_INFO *item)
{
    lara.left_arm.flash_gun = 3;
}

void invisibility_on(ITEM_INFO *item)
{
    item->status = ITEM_INVISIBLE;
}

void invisibility_off(ITEM_INFO *item)
{
    item->status = ITEM_ACTIVE;
}

void reset_hair(ITEM_INFO *item)
{
    InitialiseHair();
}
