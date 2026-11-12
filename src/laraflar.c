#include "laraflar.h"
#include "lara.h"
#include "anim.h"
#include "objnums.h"
#include "delstuff.h"
#include "larafire.h"
#include "lara_states.h"
#include "gameflow.h"
#include "specific/3dmath.h"
#include "draw.h"
#include "effect2.h"
#include "room.h"
#include "sound.h"
#include "specific/function_stubs.h"

sint32 DoFlareLight(PHD_VECTOR *pos, sint32 flare_age)
{
    sint32 x, y, z, random;
    sint32 r, g, b, falloff;

    if (flare_age >= 900 || !flare_age)
        return 0;
    random = GetRandomControl();
    x = pos->x + ((random & 0xf) << 3);
    z = pos->z + ((random >> 5) & 0x78);
    y = pos->y + ((random >> 1) & 0x78) - 256;
    if (flare_age < 4) {
        g = (random & 0x1f) + ((flare_age + 10) << 4);
        r = ((random >> 4) & 0x1f) + (flare_age << 3) + 32;
        b = (flare_age << 4) + ((random >> 8) & 0x1f);
        falloff = (random & 3) + 4 + (flare_age << 2);
        if (falloff > 16)
            falloff -= (random >> 12) & 3;
        TriggerDynamic(x, y, z, falloff, r, g, b);
    } else if (flare_age < 16) {
        r = ((random >> 4) & 0x1f) + (flare_age << 2) + 64;
        g = (random & 0x3f) + (flare_age << 2) + 128;
        b = ((random >> 8) & 0x1f) + (flare_age << 2) + 16;
        falloff = (random & 1) + flare_age + 2;
        TriggerDynamic(x, y, z, falloff, r, g, b);
    } else if (flare_age < 810) {
        r = ((random >> 4) & 0x1f) + 128;
        g = (random & 0x3f) + 192;
        b = ((random >> 8) & 0x3f) + (((random >> 6) & 0x80) >> 2);
        falloff = 16;
        TriggerDynamic(x, y, z, falloff, r, g, b);
    } else if (flare_age < 876) {
        if (random > 0x2000) {
            r = ((random >> 4) & 0x1f) + 128;
            g = (random & 0x3f) + 192;
            b = ((random >> 8) & 0x3f) + (((random >> 6) & 0x80) >> 2);
            falloff = 16;
            TriggerDynamic(x, y, z, falloff, r, g, b);
        } else {
            TriggerDynamic(x, y, z, (GetRandomControl() & 6) + 8, (GetRandomControl() & 0x3f) + 64,
                           (GetRandomControl() & 0x3f) + 192, GetRandomControl() & 0x7f);
            return 0;
        }
    } else {
        TriggerDynamic(x, y, z, 16 - ((flare_age - 876) >> 1), (GetRandomControl() & 0x3f) + 64,
                       (GetRandomControl() & 0x3f) + 192, GetRandomControl() & 0x1f);
        return random & 1;
    }
    return 1;
}

void DoFlareInHand(sint32 flare_age)
{
    PHD_VECTOR pos;

    pos.x = 11;
    pos.y = 32;
    pos.z = 41;
    GetLaraJointPos(&pos, 14);
    lara.left_arm.flash_gun = (sint16)DoFlareLight(&pos, flare_age);
    if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom) {
        pos.z = 2 * gfMirrorZPlane - pos.z;
        DoFlareLight(&pos, flare_age);
    }
    if (lara.flare_age < 900) {
        if (lara.flare_age)
            lara.flare_age++;
    } else if (lara.gun_status == LG_NO_ARMS)
        lara.gun_status = LG_UNDRAW_GUNS;
}

void DrawFlareInAir(ITEM_INFO *item)
{
    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    phd_PutPolygons_train(meshes[objects[FLARE_ITEM].mesh_index], 0);
    phd_PopMatrix();
    if (gfLevelFlags & GF_MIRROR) {
        if (item->room_number == gfMirrorRoom) {
            phd_PushMatrix();
            phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, 2 * gfMirrorZPlane - item->pos.z_pos);
            phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
            phd_PutPolygons_train(meshes[objects[FLARE_ITEM].mesh_index], 0);
            phd_PopMatrix();
        }
    }
}

void set_flare_arm(sint32 frame)
{
    sint32 anim_base;

    anim_base = objects[FLARE_ANIM].anim_index;
    if (frame >= 1) {
        if (frame < 33)
            anim_base += 1;
        else if (frame < 72)
            anim_base += 2;
        else if (frame < 95)
            anim_base += 3;
        else
            anim_base += 4;
    }
    lara.left_arm.anim_number = anim_base;
    lara.left_arm.frame_base = anims[lara.left_arm.anim_number].frame_ptr;
}

void draw_flare(void)
{
    sint16 ani;

    if (lara_item->current_anim_state == AS_FLAREPICKUP || lara_item->current_anim_state == AS_PICKUP) {
        DoFlareInHand(lara.flare_age);
        lara.flare_control_left = 0;
        lara.left_arm.frame_number = 93;
        set_flare_arm(93);
        return;
    } else {
        lara.flare_control_left = 1;
        ani = lara.left_arm.frame_number + 1;
        if (ani < 33 || ani > 94)
            ani = 33;
        else if (ani == 46)
            draw_flare_meshes();
        else if (ani >= 72 && ani <= 93) {
            if (ani == 72) {
                if (room[lara_item->room_number].flags & ROOM_UNDERWATER)
                    SoundEffect(SFX_OBJ_GEM_SMASH, &lara_item->pos, SFX_WATER);
                else
                    SoundEffect(SFX_OBJ_GEM_SMASH, &lara_item->pos, SFX_DEFAULT);
                lara.flare_age = 1;
            }
            DoFlareInHand(lara.flare_age);
        } else if (ani == 94) {
            ready_flare();
            ani = 0;
            DoFlareInHand(lara.flare_age);
        }
    }
    lara.left_arm.frame_number = ani;
    set_flare_arm(ani);
}

void undraw_flare(void)
{
    sint16 ani, ani2;

    ani = lara.left_arm.frame_number;
    ani2 = lara.flare_frame;
    lara.flare_control_left = 1;
    if (lara_item->goal_anim_state == AS_STOP && lara.vehicle == NO_ITEM) {
        if (lara_item->anim_number == ANIM_BREATH) {
            lara_item->anim_number = ANIM_THROWFLARE;
            lara_item->frame_number = lara.flare_frame = ani2 = anims[ANIM_THROWFLARE].frame_base + ani;
        }
        if (lara_item->anim_number == ANIM_THROWFLARE) {
            lara.flare_control_left = 0;
            if (ani2 >= anims[ANIM_THROWFLARE].frame_base + 31) {
                ani2 = 0;
                lara.gun_type = lara.request_gun_type = lara.last_gun_type;
                lara.gun_status = LG_NO_ARMS;
                InitialiseNewWeapon();
                lara.target = NULL;
                lara.left_arm.lock = lara.right_arm.lock = 0;
                lara_item->anim_number = ANIM_STOP;
                lara_item->frame_number = lara.flare_frame = anims[ANIM_STOP].frame_base;
                lara_item->current_anim_state = AS_STOP;
                lara_item->goal_anim_state = AS_STOP;
                return;
            }
            ani2++;
            lara.flare_frame = ani2;
        }
    } else {
        if (lara_item->current_anim_state == AS_STOP && lara.vehicle == NO_ITEM) {
            lara_item->anim_number = ANIM_STOP;
            lara_item->frame_number = anims[ANIM_STOP].frame_base;
        }
    }
    if (ani >= 33 && ani < 72)
        ani = 1;
    if (ani == 0) {
        ani = 1;
    } else if (ani >= 72 && ani < 72 + 23) {
        ani++;
        if (ani == 72 + 23 - 1)
            ani = 1;
    } else if (ani >= 1 && ani < 33) {
        ani++;
        if (ani == 21) {
            CreateFlare(FLARE_ITEM, 1);
            undraw_flare_meshes();
            lara.flare_age = 0;
        } else if (ani == 33) {
            ani = 0;
            lara.gun_type = lara.request_gun_type = lara.last_gun_type;
            lara.gun_status = LG_NO_ARMS;
            InitialiseNewWeapon();
            lara.target = NULL;
            lara.left_arm.lock = lara.right_arm.lock = 0;
            lara.flare_control_left = 0;
            lara.flare_frame = 0;
        }
    } else if (ani >= 95 && ani < 110) {
        ani++;
        if (ani == 110)
            ani = 1;
    }
    if (ani >= 1 && ani < 21)
        DoFlareInHand(lara.flare_age);
    lara.left_arm.frame_number = ani;
    set_flare_arm(lara.left_arm.frame_number);
}

void draw_flare_meshes(void)
{
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[FLARE_ANIM].mesh_index + LM_LHAND * 2];
}

void undraw_flare_meshes(void)
{
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[LARA].mesh_index + LM_LHAND * 2];
}

void ready_flare(void)
{
    lara.gun_status = LG_NO_ARMS;
    lara.left_arm.x_rot = lara.left_arm.y_rot = lara.left_arm.z_rot = 0;
    lara.right_arm.x_rot = lara.right_arm.y_rot = lara.right_arm.z_rot = 0;
    lara.left_arm.lock = lara.right_arm.lock = 0;
    lara.target = NULL;
}

void FlareControl(sint16 item_number)
{
    ITEM_INFO *flare;
    sint32 x, y, z, xv, yv, zv, flare_age;

    flare = &items[item_number];
    if (flare->fallspeed) {
        flare->pos.x_rot += 546;
        flare->pos.z_rot += 910;
    } else {
        flare->pos.x_rot = 0;
        flare->pos.z_rot = 0;
    }
    x = flare->pos.x_pos;
    y = flare->pos.y_pos;
    z = flare->pos.z_pos;
    xv = ((rcossin_tbl[(flare->pos.y_rot >> 3) & 0x1FFE] << 2) * flare->speed) >> W2V_SHIFT;
    flare->pos.x_pos += xv;
    zv = ((rcossin_tbl[((flare->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * flare->speed) >> W2V_SHIFT;
    flare->pos.z_pos += zv;
    if (room[flare->room_number].flags & ROOM_UNDERWATER) {
        flare->fallspeed += (5 - flare->fallspeed) / 2;
        flare->speed += (5 - flare->speed) / 2;
    } else
        flare->fallspeed += 6;
    yv = flare->fallspeed;
    flare->pos.y_pos += yv;
    DoProperDetection(item_number, x, y, z, xv, yv, zv);
    flare_age = (sint32)flare->data & 0x7FFF;
    if (flare_age < 900)
        flare_age++;
    else if (!flare->fallspeed && !flare->speed) {
        KillItem(item_number);
        return;
    }
    if (DoFlareLight((PHD_VECTOR *)&flare->pos, flare_age)) {
        if (gfLevelFlags & GF_MIRROR && flare->room_number == gfMirrorRoom) {
            flare->pos.z_pos = 2 * gfMirrorZPlane - flare->pos.z_pos;
            DoFlareLight((PHD_VECTOR *)&flare->pos, flare_age);
            flare->pos.z_pos = 2 * gfMirrorZPlane - flare->pos.z_pos;
        }
        flare_age |= 0x8000;
    }
    flare->data = (void *)flare_age;
}
