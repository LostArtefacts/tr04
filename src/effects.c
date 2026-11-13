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
#include "deltapak.h"
#include "lot.h"
#include "lara_states.h"
#include "text.h"
#include "savegame.h"
#include "specific/polyinsert.h"
#include "specific/function_table.h"

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

void SoundEffects(void)
{
    sint32 i;
    OBJECT_VECTOR *sound;

    sound = sound_effects;
    for (i = number_sound_effects; i > 0; i--, sound++) {
        if (!flip_status && (sound->flags & UNFLIPFLAG) == 0)
            continue;
        else if (flip_status && (sound->flags & FLIPFLAG) == 0)
            continue;
        SoundEffect(sound->data, (PHD_3DPOS *)&sound->x, 0);
    }
    if (flipeffect != -1)
        (*effect_routines[flipeffect])(NULL);
    SOUND_EndScene();
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

void WadeSplash(ITEM_INFO *item, sint32 water, sint32 depth)
{
    sint16 *bounds;
    sint16 room_number;

    room_number = item->room_number;
    GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (!(room[room_number].flags & ROOM_UNDERWATER))
        return;
    bounds = GetBestFrame(item);
    if (item->pos.y_pos + bounds[2] > water || item->pos.y_pos + bounds[3] < water)
        return;
    if (item->fallspeed > 0 && depth < 474 && !SplashCount) {
        splash_setup.x = item->pos.x_pos;
        splash_setup.y = water;
        splash_setup.z = item->pos.z_pos;
        splash_setup.InnerRad = 16;
        splash_setup.InnerSize = 12;
        splash_setup.InnerRadVel = 160;
        splash_setup.InnerYVel = -72 * item->fallspeed;
        splash_setup.pad1 = 24;
        splash_setup.MiddleRad = 24;
        splash_setup.MiddleSize = 224;
        splash_setup.MiddleRadVel = -36 * item->fallspeed;
        splash_setup.MiddleYVel = 32;
        splash_setup.pad2 = 32;
        splash_setup.OuterRad = 272;
        SetupSplash(&splash_setup);
        SplashCount = 16;
    } else if (!(wibble & 0xF) && (!(GetRandomControl() & 0xF) || item->current_anim_state != AS_STOP)) {
        if (item->current_anim_state != AS_STOP)
            SetupRipple(item->pos.x_pos, water, item->pos.z_pos, (GetRandomControl() & 0xF) + 112, 18);
        else
            SetupRipple(item->pos.x_pos, water, item->pos.z_pos, (GetRandomControl() & 0xF) + 112, 16);
    }
}

void WaterFall(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 x, y, z, ang;

    item = &items[item_number];
    x = item->pos.x_pos - lara_item->pos.x_pos;
    y = item->pos.y_pos - lara_item->pos.y_pos;
    z = item->pos.z_pos - lara_item->pos.z_pos;
    if (x < -0x4000 || x > 0x4000 || z < -0x4000 || z > 0x4000 || y < -0x4000 || y > 0x4000)
        return;
    nullsub_1(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, &item->il);
    ang = ((item->pos.y_rot >> 4) & 4095) << 1;
    z = (rcossin_tbl[ang + 1] * 544) >> 12;
    x = (rcossin_tbl[ang] * 544) >> 12;
    if ((wibble & 12) == 0)
        TriggerWaterfallMist(item->pos.x_pos + x, item->pos.y_pos, item->pos.z_pos + z, item->pos.y_rot >> 4);
    SoundEffect(79, &item->pos, 0);
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

void RubbleFX(ITEM_INFO *item)
{
    ITEM_INFO *eq;

    eq = find_a_fucking_item(EARTHQUAKE);
    if (eq) {
        AddActiveItem(eq - items);
        eq->status = ITEM_ACTIVE;
        eq->flags |= IFL_CODEBITS;
    } else {
        camera.bounce = -150;
    }
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

void GhostTrap(ITEM_INFO *item)
{
    ITEM_INFO *wraith;
    sint16 nex;

    nex = next_item_active;
    if (next_item_active != NO_ITEM) {
        while (nex != NO_ITEM) {
            wraith = &items[nex];
            if (wraith->object_number == WRAITH3 && !wraith->hit_points) {
                wraith->hit_points = item - items;
                break;
            }
            nex = wraith->next_active;
        }
    }
    flipeffect = -1;
}

void KillActiveBaddies(ITEM_INFO *item)
{
    ITEM_INFO *target_item;
    sint16 item_num;

    item_num = next_item_active;
    while (item_num != NO_ITEM) {
        target_item = &items[item_num];
        if (objects[target_item->object_number].intelligent) {
            target_item->status = ITEM_INVISIBLE;
            if (item != (ITEM_INFO *)0xABCDEF) {
                RemoveActiveItem(item_num);
                DisableBaddieAI(item_num);
                target_item->flags |= IFL_INVISIBLE;
            }
        }
        item_num = target_item->next_active;
    }
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

void SetFog(ITEM_INFO *item)
{
    sint32 col;
    uint16 timer;
    sint32 r;
    sint32 g;
    sint32 b;

    GlobalFogOff = 0;
    timer = TriggerTimer;
    if (IsVolumetric()) {
        if (timer != 100) {
            col = FogTableColor[timer];
            r = CLRR(col);
            g = CLRG(col);
            b = CLRB(col);
            SetFogColor(r, g, b);
            savegame.fog_colour.r = r;
            gfFog.r = r;
            savegame.fog_colour.g = g;
            savegame.fog_colour.b = b;
            gfFog.g = g;
            gfFog.b = b;
        } else {
            GlobalFogOff = 1;
        }
    }
    flipeffect = -1;
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

void swap_meshes_with_meshswap2(ITEM_INFO *item)
{
    OBJECT_INFO *obj;
    sint16 *tmp;
    sint32 i;

    obj = &objects[item->object_number];
    for (i = 0; i < obj->nmeshes; i++) {
        tmp = meshes[obj->mesh_index + i];
        meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP2].mesh_index + i];
        meshes[objects[MESHSWAP2].mesh_index + i] = tmp;
    }
}

void swap_meshes_with_meshswap3(ITEM_INFO *item)
{
    OBJECT_INFO *obj;
    sint16 *tmp;
    sint32 i;

    obj = &objects[item->object_number];
    for (i = 0; i < obj->nmeshes; i++) {
        tmp = meshes[obj->mesh_index + i];
        meshes[obj->mesh_index + i] = meshes[objects[MESHSWAP3].mesh_index + i];
        if (item == lara_item)
            lara.mesh_ptrs[i] = meshes[objects[MESHSWAP3].mesh_index + i * 2];
        meshes[objects[MESHSWAP3].mesh_index + i * 2] = tmp;
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
