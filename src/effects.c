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
#include "tomb4fx.h"
#include "gameflow.h"
#include "scarab.h"

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

sint16 DoBloodSplat(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 direction, sint16 room_number)
{
    if (room[room_number].flags & ROOM_UNDERWATER)
        TriggerUnderwaterBlood(x, y, z, speed);
    else
        TriggerBlood(x, y, z, direction >> 4, speed);
    return -1;
}

void DoLotsOfBlood(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 direction, sint16 room_num, sint32 number)
{
    for (; number > 0; number--)
        DoBloodSplat(x - (GetRandomControl() << 9) / 0x8000 + 256, y - (GetRandomControl() << 9) / 0x8000 + 256, z - (GetRandomControl() << 9) / 0x8000 + 256, speed, direction, room_num);
}

void Splash(ITEM_INFO *item)
{
    sint32 height;
    sint16 room_number;

    room_number = item->room_number;
    GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (room[room_number].flags & ROOM_UNDERWATER) {
        height = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);
        splash_setup.x = item->pos.x_pos;
        splash_setup.y = height;
        splash_setup.z = item->pos.z_pos;
        splash_setup.InnerRad = 32;
        splash_setup.InnerSize = 8;
        splash_setup.InnerRadVel = 320;
        splash_setup.InnerYVel = -40 * item->fallspeed;
        splash_setup.pad1 = 48;
        splash_setup.MiddleRad = 32;
        splash_setup.MiddleSize = 480;
        splash_setup.MiddleRadVel = -20 * item->fallspeed;
        splash_setup.MiddleYVel = 32;
        splash_setup.pad2 = 128;
        splash_setup.OuterRad = 544;
        SetupSplash(&splash_setup);
    }
}

void finish_level_effect(ITEM_INFO *item)
{
    gfLevelComplete = gfCurrentLevel + 1;
}

void turn180_effect(ITEM_INFO *item)
{
    item->pos.y_rot += -0x8000;
    item->pos.x_rot = -item->pos.x_rot;
}

void floor_shake_effect(ITEM_INFO *item)
{
    sint32 x, y, z;

    x = item->pos.x_pos - camera.pos.x;
    y = item->pos.y_pos - camera.pos.y;
    z = item->pos.z_pos - camera.pos.z;
    if (ABS(x) < 16 * WALL_L && ABS(y) < 16 * WALL_L && ABS(z) < 16 * WALL_L)
        camera.bounce = 66 * ((x * x + y * y + z * z) / 256 - WALL_L * WALL_L) / (WALL_L * WALL_L);
}

void SoundFlipEffect(ITEM_INFO *item)
{
    SoundEffect(TriggerTimer, NULL, 0);
    flipeffect = -1;
}

void PoseidonSFX(ITEM_INFO *item)
{
    SoundEffect(238, NULL, 0);
    flipeffect = -1;
}

void ActivateCamera(ITEM_INFO *item)
{
    KeyTriggerActive = 2;
}

void ActivateKey(ITEM_INFO *item)
{
    KeyTriggerActive = 1;
}

void SwapCrowbar(ITEM_INFO *item)
{
    sint16 *tmp;

    tmp = meshes[objects[LARA].mesh_index + 2 * LM_RHAND];
    if (lara.mesh_ptrs[LM_RHAND] == tmp)
        lara.mesh_ptrs[LM_RHAND] = meshes[objects[CROWBAR_ANIM].mesh_index + 2 * LM_RHAND];
    else
        lara.mesh_ptrs[LM_RHAND] = tmp;
}

void ExplosionFX(ITEM_INFO *item)
{
    SoundEffect(105, NULL, 0);
    camera.bounce = -75;
    flipeffect = -1;
}

void LaraLocation(ITEM_INFO *item)
{
    lara.location = TriggerTimer;
    if (TriggerTimer > lara.highest_location)
        lara.highest_location = TriggerTimer;
    flipeffect = -1;
}

void LaraLocationPad(ITEM_INFO *item)
{
    flipeffect = -1;
    lara.locationPad = TriggerTimer;
    lara.location = TriggerTimer;
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

void swap_meshes_with_meshswap1(ITEM_INFO *item)
{
    OBJECT_INFO *obj;
    sint16 *tmp;
    sint32 i;

    obj = &objects[item->object_number];
    for (i = 0; i < obj->nmeshes; i++) {
        tmp = meshes[obj->mesh_index + i];
        meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP1].mesh_index + i];
        meshes[objects[MESHSWAP1].mesh_index + i] = tmp;
    }
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

void ClearScarabsPatch(ITEM_INFO *item)
{
    ClearScarabs();
}

void MeshSwapToPour(ITEM_INFO *item)
{
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[item->item_flags[2]].mesh_index + 2 * LM_LHAND];
}

void MeshSwapFromPour(ITEM_INFO *item)
{
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA_SKIN].mesh_index + 2 * LM_LHAND];
}
