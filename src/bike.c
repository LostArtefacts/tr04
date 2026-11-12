#include "bike.h"
#include "control.h"
#include "lara.h"
#include "lara1gun.h"
#include "lara_states.h"
#include "anim.h"
#include "objnums.h"
#include "box.h"
#include "camera.h"
#include "sphere.h"
#include "delstuff.h"
#include "sound.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "laraflar.h"
#include "newinv.h"
#include "savegame.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/function_stubs.h"
#include "specific/specificfx.h"

void InitialiseBike(sint16 item_number)
{
    ITEM_INFO *item;
    BIKEINFO *bike;

    item = &items[item_number];
    bike = (BIKEINFO *)game_malloc(sizeof(BIKEINFO));
    item->data = bike;
    bike->velocity = 0;
    bike->bike_turn = 0;
    bike->pitch2 = 0;
    bike->move_angle = item->pos.y_rot;
    bike->rot_thing = 0;
    bike->extra_rotation = 0;
    bike->flags2 &= 0xFE;
    bike->light_intensity = 0;
    bike->flags = 0;
    *(sint32 *)&bike->right_front_wheelrot = 0;
    bike->left_wheelrot = 0;
    item->mesh_bits = 1015;
    GlobalBikeItem = item;
}

void BikeCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    BIKEINFO *bike;
    sint16 ang;

    if (l->hit_points < 0 || lara.vehicle != NO_ITEM)
        return;
    item = &items[item_number];
    bike = (BIKEINFO *)item->data;
    if (bike->light_intensity) {
        bike->light_intensity = bike->light_intensity - (bike->light_intensity >> 3) - 1;
        TriggerBikeBeam(item);
    }
    if (GetOnBike(item_number, coll)) {
        lara.vehicle = item_number;
        if (lara.gun_type == WEAPON_FLARE) {
            CreateFlare(FLARE_ITEM, 0);
            undraw_flare_meshes();
            lara.flare_control_left = 0;
            lara.request_gun_type = lara.gun_type = WEAPON_NONE;
            lara.flare_age = 0;
        }
        lara.gun_status = LG_HANDS_BUSY;
        ang = (sint16)phd_atan(item->pos.z_pos - l->pos.z_pos, item->pos.x_pos - l->pos.x_pos);
        ang -= item->pos.y_rot;
        if (ang <= -8190 || ang >= 24570) {
            if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 28;
                GLOBAL_inventoryitemchosen = NO_ITEM;
                savegame.HaveBikeBooster = 1;
            } else {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 9;
            }
            l->current_anim_state = l->goal_anim_state = 9;
        }
        l->frame_number = anims[l->anim_number].frame_base;
        item->hit_points = 1;
        l->pos.x_pos = item->pos.x_pos;
        l->pos.y_pos = item->pos.y_pos;
        l->pos.z_pos = item->pos.z_pos;
        l->pos.y_rot = item->pos.y_rot;
        lara.head_y_rot = 0;
        lara.head_x_rot = 0;
        lara.torso_y_rot = 0;
        lara.torso_x_rot = 0;
        lara.hit_direction = -1;
        AnimateItem(l);
        bike = (BIKEINFO *)item->data;
        bike->unused1 = 0;
        item->flags |= IFL_TRIGGERED;
    } else {
        ObjectCollision(item_number, l, coll);
    }
}

void TriggerBikeBeam(ITEM_INFO *item)
{
    BIKEINFO *bike;
    PHD_VECTOR s;
    PHD_VECTOR d;
    sint32 intensity;

    bike = (BIKEINFO *)item->data;
    s.x = 0;
    s.y = -470;
    s.z = 1836;
    GetJointAbsPosition(item, &s, 0);
    d.x = 0;
    d.y = -470;
    d.z = 20780;
    GetJointAbsPosition(item, &d, 0);
    intensity = bike->light_intensity << 1;
    intensity -= GetRandomControl() & 0xF;
    if (intensity > 0)
        LaraTorch(&s, &d, item->pos.y_rot, intensity);
    else
        bLaraTorch = 0;
}

void DrawBikeExtras(ITEM_INFO *item)
{
    if (lara.vehicle != NO_ITEM)
        DrawBikeSpeedo(phd_winwidth - 64, phd_winheight - 16, ((BIKEINFO *)item->data)->velocity, 0x8000, 0xC000, 32, 0);
    DrawBikeBeam(GlobalBikeItem);
}

void TriggerExhaustSmoke_bike(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 velocity, sint32 thing)
{
    SPARKS *sptr;
    sint32 size, zv;

    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sB = sptr->sG = sptr->sR = 0;
    sptr->dB = 128;
    sptr->dG = sptr->dR = 96;
    if (thing) {
        sptr->dG = sptr->dR = (uint8)((96 * velocity) >> 5);
        sptr->dB = (uint8)((128 * velocity) >> 5);
    }
    sptr->ColFadeSpeed = 4;
    sptr->FadeToBlack = 4;
    sptr->sLife = sptr->Life = (uint8)((GetRandomControl() & 3) - (velocity >> 12) + 20);
    if (sptr->Life < 9)
        sptr->sLife = sptr->Life = 9;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0xF) + x - 8;
    sptr->y = (GetRandomControl() & 0xF) + y - 8;
    sptr->z = (GetRandomControl() & 0xF) + z - 8;
    zv = ((rcossin_tbl[((angle >> 3) & 0x1FFE) + 1] << 2) * velocity) >> 16;
    sptr->Xvel = (sint16)(((rcossin_tbl[(angle >> 3) & 0x1FFE] << 2) * velocity) >> 16);
    sptr->Yvel = -8 - (GetRandomControl() & 7);
    sptr->Zvel = (sint16)zv;
    sptr->Friction = 4;
    if (GetRandomControl() & 1) {
        sptr->Flags = 538;
        sptr->RotAng = GetRandomControl() & 0xFFF;
        if (GetRandomControl() & 1)
            sptr->RotAdd = -24 - (GetRandomControl() & 7);
        else
            sptr->RotAdd = (GetRandomControl() & 7) + 24;
    } else {
        sptr->Flags = 522;
    }
    sptr->Def = (uint8)objects[DEFAULT_SPRITES].mesh_index;
    sptr->Scalar = 1;
    sptr->Gravity = -4 - (GetRandomControl() & 3);
    sptr->MaxYvel = -8 - (GetRandomControl() & 7);
    size = (GetRandomControl() & 7) + (velocity >> 7) + 32;
    sptr->Size = sptr->sSize = size >> 1;
    sptr->dSize = size;
}

void BikeExplode(ITEM_INFO *item)
{
    sint32 i;

    if (room[item->room_number].flags & ROOM_UNDERWATER)
        TriggerUnderwaterExplosion(item, 1);
    else {
        TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);
        for (i = 0; i < 3; i++)
            TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
    }
    ExplodingDeath2(lara.vehicle, -2, 256);
    KillItem(lara.vehicle);
    item->status = ITEM_DEACTIVATED;
    SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
    SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
    lara.vehicle = NO_ITEM;
}

sint32 BikeCheckGetOff(void)
{
    ITEM_INFO *item;
    ITEM_INFO *l;
    BIKEINFO *bike;
    PHD_VECTOR pos;
    sint16 state;

    item = &items[lara.vehicle];
    l = lara_item;
    state = l->current_anim_state;
    if (state == 10 && l->frame_number == anims[l->anim_number].frame_end) {
        l->pos.y_rot -= 0x4000;
        l->anim_number = ANIM_STOP;
        l->frame_number = anims[ANIM_STOP].frame_base;
        l->current_anim_state = l->goal_anim_state = AS_STOP;
        l->pos.x_pos -= 512 * phd_sin(l->pos.y_rot) >> W2V_SHIFT;
        l->pos.z_pos -= 512 * phd_cos(l->pos.y_rot) >> W2V_SHIFT;
        l->pos.x_rot = l->pos.z_rot = 0;
        lara.vehicle = NO_ITEM;
        lara.gun_status = LG_NO_ARMS;
        DashTimer = 120;
    } else if (l->frame_number == anims[l->anim_number].frame_end) {
        bike = (BIKEINFO *)item->data;
        if (state == 20) {
            l->anim_number = ANIM_FASTFALL;
            l->frame_number = anims[ANIM_FASTFALL].frame_base;
            l->current_anim_state = AS_FASTFALL;
            pos.x = 0;
            pos.y = 0;
            pos.z = 0;
            GetLaraJointPos(&pos, LM_HIPS);
            l->pos.x_pos = pos.x;
            l->pos.y_pos = pos.y;
            l->pos.z_pos = pos.z;
            l->fallspeed = item->fallspeed;
            l->gravity_status = 1;
            l->pos.x_rot = l->pos.z_rot = 0;
            l->hit_points = 0;
            lara.gun_status = LG_NO_ARMS;
            item->flags |= IFL_INVISIBLE;
            return 0;
        }
        if (state == 19) {
            l->goal_anim_state = AS_DEATH;
            l->fallspeed = 240;
            l->speed = 0;
            bike->flags |= 0x80;
            return 0;
        }
    }
    return 1;
}

sint32 TestHeight(ITEM_INFO *item, sint32 z, sint32 x, PHD_VECTOR *pos)
{
    FLOOR_INFO *floor;
    sint32 sx, sz, sy, cy, c, h;
    sint16 room_number;

    sz = z * phd_sin(item->pos.x_rot) >> W2V_SHIFT;
    sx = x * phd_sin(item->pos.z_rot) >> W2V_SHIFT;
    pos->y = item->pos.y_pos + sx - sz;
    cy = phd_cos(item->pos.y_rot);
    sy = phd_sin(item->pos.y_rot);
    pos->z = item->pos.z_pos + ((z * cy - x * sy) >> W2V_SHIFT);
    pos->x = item->pos.x_pos + ((x * cy + z * sy) >> W2V_SHIFT);
    room_number = item->room_number;
    floor = GetFloor(pos->x, pos->y, pos->z, &room_number);
    c = GetCeiling(floor, pos->x, pos->y, pos->z);
    if (pos->y < c || c == NO_HEIGHT)
        return NO_HEIGHT;
    h = GetHeight(floor, pos->x, pos->y, pos->z);
    if (pos->y > h)
        pos->y = h;
    return h;
}

sint32 DoDynamics_bike(sint32 height, sint32 fallspeed, sint32 *y, sint32 zero)
{
    sint32 kick;

    if (height > *y) {
        *y += fallspeed;
        if (*y > height - 32) {
            *y = height;
            fallspeed = 0;
        } else if (zero)
            fallspeed += zero;
        else
            fallspeed += 6;
    } else {
        if (!zero) {
            kick = height - *y;
            if (kick < -80)
                kick = -80;
            fallspeed += (kick - fallspeed) >> 4;
            if (*y > height)
                *y = height;
        } else
            return fallspeed;
    }
    return fallspeed;
}

sint32 CanGetOff_bike(sint16 num)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 x, y, z, h, c;
    sint16 yrot, room_number;

    item = &items[lara.vehicle];
    yrot = item->pos.y_rot + 16384;
    x = item->pos.x_pos + (512 * phd_sin(yrot) >> W2V_SHIFT);
    y = item->pos.y_pos;
    z = item->pos.z_pos + (512 * phd_cos(yrot) >> W2V_SHIFT);
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (height_type == BIG_SLOPE || height_type == DIAGONAL || h == NO_HEIGHT)
        return 0;
    if (ABS(h - item->pos.y_pos) > 272)
        return 0;
    c = GetCeiling(floor, x, y, z);
    if (c - item->pos.y_pos > -762 || h - c < 762)
        return 0;
    x = item->pos.x_pos + (512 * phd_sin(yrot) >> (W2V_SHIFT + 2));
    y = item->pos.y_pos;
    z = item->pos.z_pos + (512 * phd_cos(yrot) >> (W2V_SHIFT + 2));
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (height_type == BIG_SLOPE || height_type == DIAGONAL || h == NO_HEIGHT)
        return 0;
    if (ABS(h - item->pos.y_pos) > 80)
        return 0;
    c = GetCeiling(floor, x, y, z);
    if (c - item->pos.y_pos > -762 || h - c < 762)
        return 0;
    return 1;
}

void BikeStart(ITEM_INFO *item, ITEM_INFO *l)
{
    BIKEINFO *bike;

    bike = (BIKEINFO *)item->data;
    lara.gun_status = LG_HANDS_BUSY;
    lara.hit_direction = -1;
    l->current_anim_state = l->goal_anim_state = 15;
    l->anim_number = objects[VEHICLE_EXTRA].anim_index + 18;
    l->frame_number = anims[l->anim_number].frame_base;
    l->gravity_status = 0;
    item->anim_number = objects[MOTORBIKE].anim_index - objects[VEHICLE_EXTRA].anim_index + l->anim_number;
    item->frame_number = anims[item->anim_number].frame_base - anims[l->anim_number].frame_base + l->frame_number;
    item->flags |= IFL_TRIGGERED;
    item->hit_points = 1;
    bike->unused1 = 0;
}
