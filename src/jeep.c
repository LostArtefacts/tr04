#include "jeep.h"
#include "bike.h"
#include "control.h"
#include "lara.h"
#include "lara1gun.h"
#include "lara_states.h"
#include "anim.h"
#include "objnums.h"
#include "box.h"
#include "sound.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "laraflar.h"
#include "lot.h"
#include "laramisc.h"
#include "traps.h"
#include "effects.h"
#include "debris.h"
#include "draw.h"
#include "camera.h"
#include "sphere.h"
#include "newinv.h"
#include "specific/input.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/function_stubs.h"
#include "specific/specificfx.h"

void InitialiseJeep(sint16 item_number)
{
    ITEM_INFO *item;
    JEEPINFO *jeep;

    item = &items[item_number];
    item->data = game_malloc(sizeof(JEEPINFO));
    jeep = (JEEPINFO *)item->data;
    jeep->velocity = 0;
    jeep->unused1 = 0;
    jeep->turn_rate = 0;
    jeep->camera_angle = 0;
    jeep->extra_rotation = 0;
    jeep->move_angle = item->pos.y_rot;
    jeep->pitch2 = 0;
    jeep->flags = 0;
    jeep->gear = 0;
    *(sint32 *)&jeep->right_front_wheelrot = 0;
    *(sint32 *)&jeep->left_back_wheelrot = 0;
    item->mesh_bits = 0x1BFFF;
}

void JeepCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    JEEPINFO *jeep;
    sint16 item_num, ang;

    if (l->hit_points < 0 || lara.vehicle != NO_ITEM)
        return;
    item = &items[item_number];
    if (GetOnJeep(item_number, coll)) {
        lara.vehicle = item_number;
        if (lara.gun_type == WEAPON_FLARE) {
            CreateFlare(FLARE_ITEM, 0);
            undraw_flare_meshes();
            lara.flare_control_left = 0;
            lara.request_gun_type = lara.gun_type = WEAPON_NONE;
            lara.flare_age = 0;
        }
        lara.gun_status = LG_HANDS_BUSY;
        for (item_num = room[item->room_number].item_number; item_num != NO_ITEM; item_num = item2->next_item) {
            item2 = &items[item_num];
            if (item2->object_number == ENEMY_JEEP) {
                EnableBaddieAI(item_num, 1);
                item2->status = ITEM_ACTIVE;
                AddActiveItem(item_num);
                break;
            }
        }
        ang = (sint16)phd_atan(item->pos.z_pos - l->pos.z_pos, item->pos.x_pos - l->pos.x_pos);
        ang -= item->pos.y_rot;
        if (ang > -8190 && ang < 24570)
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 18;
        else
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 9;
        l->current_anim_state = l->goal_anim_state = 9;
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
        jeep = (JEEPINFO *)item->data;
        jeep->unused1 = 0;
        jeep->gear = 0;
        item->flags |= IFL_TRIGGERED;
        CurrentAtmosphere = 98;
        IsAtmospherePlaying = 1;
        S_CDPlay(98, 1);
    } else {
        ObjectCollision(item_number, l, coll);
    }
}

sint32 GetOnJeep(sint16 item_number, COLL_INFO *coll)
{
    ITEM_INFO *item;
    ITEM_INFO *l;
    FLOOR_INFO *floor;
    sint32 h;
    sint16 room_number, ang;
    uint16 rot;

    item = &items[item_number];
    l = lara_item;
    if (input & IN_ACTION || GLOBAL_inventoryitemchosen == PUZZLE_ITEM1) {
        if (!(sint16)(item->flags & IFL_INVISIBLE) && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->gravity_status) {
            if (ABS(item->pos.y_pos - l->pos.y_pos) < 256 && TestBoundsCollide(item, l, 100)) {
                room_number = item->room_number;
                floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
                h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
                if (h >= NO_HEIGHT + 512) {
                    h = phd_atan(item->pos.z_pos - l->pos.z_pos, item->pos.x_pos - l->pos.x_pos);
                    ang = (sint16)(h - item->pos.y_rot);
                    rot = l->pos.y_rot - item->pos.y_rot;
                    if (ang > -8190 && ang < 24570) {
                        if (rot <= 8190 || rot >= 24570)
                            return 0;
                        if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1) {
                            GLOBAL_inventoryitemchosen = NO_ITEM;
                            return 1;
                        }
                    } else {
                        if (rot <= 40950 || rot >= 57330)
                            return 0;
                        if (GLOBAL_inventoryitemchosen == PUZZLE_ITEM1) {
                            GLOBAL_inventoryitemchosen = NO_ITEM;
                            return 1;
                        }
                    }
                    if (have_i_got_object(PUZZLE_ITEM1))
                        GLOBAL_enterinventory = PUZZLE_ITEM1;
                }
            }
        }
    }
    return 0;
}

void DrawJeepExtras(ITEM_INFO *item)
{
    JEEPINFO *jeep;

    if (lara.vehicle == NO_ITEM)
        return;
    jeep = (JEEPINFO *)item->data;
    DrawJeepSpeedo(phd_winwidth - 64, phd_winheight - 16, jeep->velocity, 0x6000, 0x8000, 32, jeep->gear);
}

void JeepControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *l;
    JEEPINFO *jeep;
    FLOOR_INFO *floor;
    PHD_VECTOR flPos, frPos, fmPos;
    PHD_VECTOR pos;
    sint32 front_left, front_right, front_mid;
    sint32 hitWall, h, driving, killed, pitch, oldY, smokeVel;
    sint16 room_number, wheelRot, xRot, zRot;
    static uint8 ExhaustSmokeVel;

    l = lara_item;
    driving = -1;
    killed = 0;
    item = &items[lara.vehicle];
    jeep = (JEEPINFO *)item->data;
    hitWall = JeepDynamics(item);
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    front_left = TestHeight(item, 550, -256, &flPos);
    front_right = TestHeight(item, 550, 256, &frPos);
    front_mid = TestHeight(item, -600, 0, &fmPos);
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    TestTriggers(trigger_index, 1, 0);
    TestTriggers(trigger_index, 0, 0);
    if (lara_item->hit_points <= 0) {
        killed = 1;
        input &= ~(IN_FORWARD | IN_BACK | IN_LEFT | IN_RIGHT | IN_LSTEP | IN_RSTEP);
    }
    if (jeep->flags)
        hitWall = 0;
    else if (lara_item->current_anim_state != 9)
        driving = UserControl_jeep(item, h, &pitch);
    else {
        driving = -1;
        hitWall = 0;
    }
    if (!jeep->velocity && !jeep->unused1) {
        if (driving != -1)
            SoundEffect(SFX_JEEP_IDLE, &item->pos, 0);
        jeep->pitch2 = 0;
    } else {
        jeep->pitch2 = pitch;
        if (pitch < -0x8000)
            jeep->pitch2 = -0x8000;
        else if (pitch > 0xA000)
            jeep->pitch2 = 0xA000;
        SoundEffect(SFX_JEEP_MOVE, &item->pos, (jeep->pitch2 << 8) + 0x1000004);
    }
    item->floor = h;
    wheelRot = (sint16)(jeep->velocity >> 2);
    jeep->right_front_wheelrot -= wheelRot;
    jeep->left_front_wheelrot -= wheelRot;
    jeep->left_back_wheelrot -= wheelRot;
    jeep->right_back_wheelrot -= wheelRot;
    oldY = item->pos.y_pos;
    item->fallspeed = (sint16)DoDynamics_jeep(h, item->fallspeed, &item->pos.y_pos, 0);
    h = (frPos.y + flPos.y) >> 1;
    if (fmPos.y < front_mid) {
        if (h < (front_left + front_right) >> 1) {
            xRot = (sint16)phd_atan(137, oldY - item->pos.y_pos);
            if (jeep->velocity < 0)
                xRot = -xRot;
        } else
            xRot = (sint16)phd_atan(550, item->pos.y_pos - h);
    } else if (h >= (front_left + front_right) >> 1)
        xRot = (sint16)phd_atan(1100, front_mid - h);
    else
        xRot = (sint16)phd_atan(550, front_mid - item->pos.y_pos);
    zRot = (sint16)phd_atan(256, h - flPos.y);
    item->pos.x_rot += (xRot - item->pos.x_rot) >> 2;
    item->pos.z_rot += (zRot - item->pos.z_rot) >> 2;
    if (!(jeep->flags & 0x80)) {
        if (room_number != item->room_number) {
            ItemNewRoom(lara.vehicle, room_number);
            ItemNewRoom(lara.item_number, room_number);
        }
        l->pos.x_pos = item->pos.x_pos;
        l->pos.y_pos = item->pos.y_pos;
        l->pos.z_pos = item->pos.z_pos;
        l->pos.y_rot = item->pos.y_rot;
        l->pos.x_rot = item->pos.x_rot;
        l->pos.z_rot = item->pos.z_rot;
        AnimateJeep(item, hitWall, killed);
        AnimateItem(l);
        item->anim_number = l->anim_number - objects[VEHICLE_EXTRA].anim_index + objects[JEEP].anim_index;
        item->frame_number = anims[item->anim_number].frame_base - anims[l->anim_number].frame_base + l->frame_number;
        camera.target_elevation = -5460;
        camera.target_distance = 2048;
        if (!jeep->gear)
            jeep->camera_angle -= jeep->camera_angle >> 3;
        else if (jeep->gear == 1)
            jeep->camera_angle += (0x7F42 - jeep->camera_angle) >> 3;
        camera.target_angle = (sint16)jeep->camera_angle;
        if (jeep->flags & 0x40 && item->pos.y_pos == item->floor) {
            lara_item->mesh_bits = 0;
            lara_item->hit_points = 0;
            lara_item->flags |= IFL_INVISIBLE;
            JeepExplode(item);
            return;
        }
    }
    if (l->current_anim_state == 9 || l->current_anim_state == 10)
        ExhaustSmokeVel = 0;
    else {
        pos.x = 80;
        pos.y = 0;
        pos.z = -500;
        GetJointAbsPosition(item, &pos, 11);
        if (item->speed > 32) {
            if (item->speed < 64)
                TriggerExhaustSmoke_jeep(pos.x, pos.y, pos.z, (uint16)(item->pos.y_rot + 0x8000), 64 - item->speed, 1);
        } else {
            if (ExhaustSmokeVel < 16) {
                smokeVel = ((GetRandomControl() & 0x10) + (GetRandomControl() & 7) + 2 * ExhaustSmokeVel) << 6;
                ExhaustSmokeVel++;
            } else if (!(GetRandomControl() & 3))
                smokeVel = ((GetRandomControl() & 0x10) + (GetRandomControl() & 0xF)) << 6;
            else
                smokeVel = 0;
            TriggerExhaustSmoke_jeep(pos.x, pos.y, pos.z, (uint16)(item->pos.y_rot + 0x8000), smokeVel, 0);
        }
    }
    JeepCheckGetOut();
}

void TriggerExhaustSmoke_jeep(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 velocity, sint32 thing)
{
    SPARKS *sptr;
    sint32 size, zv;

    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sB = sptr->sG = sptr->sR = 0;
    sptr->dB = 32;
    sptr->dG = sptr->dR = 16;
    if (thing) {
        sptr->dG = sptr->dR = (uint8)((16 * velocity) >> 5);
        sptr->dB = (uint8)((32 * velocity) >> 5);
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
    sptr->Scalar = 1;
    sptr->Gravity = -4 - (GetRandomControl() & 3);
    sptr->MaxYvel = -8 - (GetRandomControl() & 7);
    size = (GetRandomControl() & 7) + (velocity >> 7) + 32;
    sptr->Size = sptr->sSize = size >> 1;
    sptr->dSize = size;
}

void JeepExplode(ITEM_INFO *item)
{
    sint32 i;

    if (room[item->room_number].flags & ROOM_UNDERWATER)
        TriggerUnderwaterExplosion(item, 1);
    else {
        TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);
        for (i = 0; i < 3; i++)
            TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
    }
    ExplodingDeath2(lara.vehicle, -1, 256);
    KillItem(lara.vehicle);
    item->status = ITEM_DEACTIVATED;
    SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
    SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
    lara.vehicle = NO_ITEM;
}

sint32 JeepCheckGetOut(void)
{
    ITEM_INFO *item;

    item = lara_item;
    if (item->current_anim_state == 10 && item->frame_number == anims[item->anim_number].frame_end) {
        item->pos.y_rot += 0x4000;
        item->anim_number = ANIM_STOP;
        item->frame_number = anims[ANIM_STOP].frame_base;
        item->current_anim_state = item->goal_anim_state = AS_STOP;
        item->pos.x_pos -= 512 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
        item->pos.z_pos -= 512 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
        item->pos.x_rot = item->pos.z_rot = 0;
        lara.vehicle = NO_ITEM;
        lara.gun_status = LG_NO_ARMS;
        CurrentAtmosphere = 110;
        IsAtmospherePlaying = 1;
        S_CDPlay(110, 1);
    }
    return 1;
}

sint32 DoDynamics_jeep(sint32 height, sint32 fallspeed, sint32 *y, sint32 zero)
{
    sint32 kick;

    if (height > *y) {
        *y += fallspeed;
        if (*y > height - 32) {
            *y = height;
            if (fallspeed > 150)
                lara_item->hit_points += 150 - fallspeed;
            fallspeed = 0;
        } else if (zero)
            fallspeed += zero + (zero >> 1);
        else
            fallspeed += 9;
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

sint32 JeepDynamics(ITEM_INFO *item)
{
    JEEPINFO *jeep;
    FLOOR_INFO *floor;
    PHD_VECTOR pos, newPos;
    PHD_VECTOR flPos, frPos, blPos, brPos, fmPos, flPos2, frPos2, blPos2, brPos2, fmPos2;
    sint32 front_left, front_right, back_left, back_right, front_mid;
    sint32 h, speed, anim, s, s2;
    sint16 ang, vel, shift, shift2, room_number;

    dont_exit_jeep = 0;
    jeep = (JEEPINFO *)item->data;
    front_left = TestHeight(item, 550, -256, &flPos);
    front_right = TestHeight(item, 550, 256, &frPos);
    back_left = TestHeight(item, -600, -256, &blPos);
    back_right = TestHeight(item, -600, 256, &brPos);
    front_mid = TestHeight(item, -600, 0, &fmPos);
    pos.x = item->pos.x_pos;
    pos.y = item->pos.y_pos;
    pos.z = item->pos.z_pos;
    if (blPos.y > back_left)
        blPos.y = back_left;
    if (brPos.y > back_right)
        brPos.y = back_right;
    if (flPos.y > front_left)
        flPos.y = front_left;
    if (frPos.y > front_right)
        frPos.y = front_right;
    if (fmPos.y > front_mid)
        fmPos.y = front_mid;
    if (item->pos.y_pos > item->floor - 8) {
        if (jeep->turn_rate < -182)
            jeep->turn_rate += 182;
        else if (jeep->turn_rate > 182)
            jeep->turn_rate -= 182;
        else
            jeep->turn_rate = 0;
        item->pos.y_rot += (sint16)(jeep->extra_rotation + jeep->turn_rate);
        ang = item->pos.y_rot - jeep->move_angle;
        vel = (sint16)(728 - ((3 * jeep->velocity) >> 11));
        if (!(input & IN_ACTION) && jeep->velocity > 0)
            vel -= vel >> 2;
        if (ang < -273) {
            if (ang < -13650) {
                item->pos.y_pos -= 41;
                item->fallspeed = -6 - (GetRandomControl() & 3);
                jeep->turn_rate = 0;
                jeep->velocity -= jeep->velocity >> 3;
            }
            if (ang < -16380)
                jeep->move_angle = item->pos.y_rot + 0x3FFC;
            else
                jeep->move_angle -= vel;
        } else if (ang > 273) {
            if (ang > 13650) {
                item->pos.y_pos -= 41;
                item->fallspeed = -6 - (GetRandomControl() & 3);
                jeep->turn_rate = 0;
                jeep->velocity -= jeep->velocity >> 3;
            }
            if (ang > 16380)
                jeep->move_angle = item->pos.y_rot - 0x3FFC;
            else
                jeep->move_angle += vel;
        } else
            jeep->move_angle = item->pos.y_rot;
    } else {
        if (jeep->turn_rate < -91)
            jeep->turn_rate += 91;
        else if (jeep->turn_rate > 91)
            jeep->turn_rate -= 91;
        else
            jeep->turn_rate = 0;
        item->pos.y_rot += (sint16)(jeep->extra_rotation + jeep->turn_rate);
        jeep->move_angle += (sint16)(item->pos.y_rot - jeep->move_angle) >> 5;
    }
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (item->pos.y_pos >= h)
        speed = (item->speed * (rcossin_tbl[((item->pos.x_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
    else
        speed = item->speed;
    item->pos.z_pos += (speed * (rcossin_tbl[((jeep->move_angle >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
    item->pos.x_pos += (speed * (rcossin_tbl[(jeep->move_angle >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
    if (item->pos.y_pos >= h) {
        s = (100 * (rcossin_tbl[(item->pos.x_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
        if (ABS(s) > 16) {
            dont_exit_jeep = 1;
            if (s < 0)
                jeep->velocity += ((s + 16) * (s + 16)) >> 1;
            else
                jeep->velocity -= ((s - 16) * (s - 16)) >> 1;
        }
        s = (128 * (rcossin_tbl[(item->pos.z_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
        if (ABS(s) > 32) {
            dont_exit_jeep = 1;
            if (s < 0) {
                item->pos.x_pos += ((abs(s) - 24) * (rcossin_tbl[((item->pos.y_rot - 0x4000) >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
                item->pos.z_pos += ((abs(s) - 24) * (rcossin_tbl[(((item->pos.y_rot - 0x4000) >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
            } else {
                item->pos.x_pos += ((abs(s) - 24) * (rcossin_tbl[((item->pos.y_rot + 0x4000) >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
                item->pos.z_pos += ((abs(s) - 24) * (rcossin_tbl[(((item->pos.y_rot + 0x4000) >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
            }
        }
    }
    if (jeep->velocity > 0x8000)
        jeep->velocity = 0x8000;
    else if (jeep->velocity < -0x4000)
        jeep->velocity = -0x4000;
    newPos.x = item->pos.x_pos;
    newPos.z = item->pos.z_pos;
    if (!(sint16)(item->flags & 0x100)) {
        JeepBaddieCollision(item);
        JeepCollideStaticObjects(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 512);
    }
    shift2 = 0;
    shift = 0;
    if (TestHeight(item, 550, -256, &flPos2) < flPos.y - 256)
        shift = abs(DoShift(item, &flPos2, &flPos) << 2);
    if (TestHeight(item, -600, -256, &blPos2) < blPos.y - 256) {
        if (shift)
            shift += abs(DoShift(item, &blPos2, &blPos) << 2);
        else
            shift = -abs(DoShift(item, &blPos2, &blPos) << 2);
    }
    if (TestHeight(item, 550, 256, &frPos2) < frPos.y - 256)
        shift2 = -abs(DoShift(item, &frPos2, &frPos) << 2);
    if (TestHeight(item, -600, 0, &fmPos2) < fmPos.y - 256)
        DoShift(item, &fmPos2, &fmPos);
    if (TestHeight(item, -600, 256, &brPos2) < brPos.y - 256) {
        if (shift2)
            shift2 -= abs(DoShift(item, &brPos2, &brPos) << 2);
        else
            shift2 = abs(DoShift(item, &brPos2, &brPos) << 2);
    }
    if (!shift)
        shift = shift2;
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (h < item->pos.y_pos - 256)
        DoShift(item, (PHD_VECTOR *)&item->pos, &pos);
    if (!jeep->velocity)
        shift = 0;
    jeep->rot_thing = (sint16)((jeep->rot_thing + shift) >> 1);
    if (abs(jeep->rot_thing) < 2)
        jeep->rot_thing = 0;
    if (abs(jeep->rot_thing - jeep->extra_rotation) < 4)
        jeep->extra_rotation = jeep->rot_thing;
    else
        jeep->extra_rotation += (jeep->rot_thing - jeep->extra_rotation) >> 2;
    anim = GetCollisionAnim(item, &newPos);
    if (anim) {
        speed = ((((item->pos.z_pos - pos.z) * (rcossin_tbl[((jeep->move_angle >> 3) & 0x1FFE) + 1] << 2)) + ((item->pos.x_pos - pos.x) * (rcossin_tbl[(jeep->move_angle >> 3) & 0x1FFE] << 2))) >> W2V_SHIFT) << 8;
        if (&items[lara.vehicle] == item && jeep->velocity == 0x8000 && speed < 0x7FF6) {
            lara_item->hit_points -= (sint16)((0x8000 - speed) >> 7);
            lara_item->hit_status = 1;
        }
        if (jeep->velocity > 0 && speed < jeep->velocity)
            jeep->velocity = speed < 0 ? 0 : speed;
        else if (jeep->velocity < 0 && speed > jeep->velocity)
            jeep->velocity = speed > 0 ? 0 : speed;
        if (jeep->velocity < -0x4000)
            jeep->velocity = -0x4000;
    }
    return anim;
}

void JeepCollideStaticObjects(sint32 x, sint32 y, sint32 z, sint16 room_number, sint32 height)
{
    MESH_INFO *mesh;
    STATIC_INFO *sinfo;
    ROOM_INFO *r;
    PHD_VECTOR pos;
    sint16 *doors;
    sint32 i, j;
    sint16 room_count;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    JeepBounds1 = x - 256;
    JeepBounds0 = x + 256;
    JeepBounds3 = y - height;
    JeepBounds2 = y;
    JeepBounds5 = z - 256;
    JeepBounds4 = z + 256;
    room_count = 1;
    jroomies[0] = room_number;
    doors = room[room_number].door;
    if (doors) {
        for (i = *doors++; i > 0; i--, doors += 16) {
            for (j = 0; j < room_count; j++) {
                if (jroomies[j] == *doors)
                    break;
            }
            if (j == room_count) {
                jroomies[room_count] = *doors;
                room_count++;
            }
        }
    }
    for (i = 0; i < room_count; i++) {
        r = &room[jroomies[i]];
        mesh = r->mesh;
        for (j = r->num_meshes; j > 0; j--, mesh++) {
            if (mesh->Flags & 1) {
                if (mesh->static_number >= SHATTER0 && mesh->static_number <= SHATTER9) {
                    sinfo = &static_objects[mesh->static_number];
                    JeepStaticBounds3 = mesh->y + sinfo->y_minc;
                    JeepStaticBounds2 = mesh->y + sinfo->y_maxc;
                    switch (mesh->y_rot) {
                    case -0x8000:
                        JeepStaticBounds1 = mesh->x - sinfo->x_maxc;
                        JeepStaticBounds0 = mesh->x - sinfo->x_minc;
                        JeepStaticBounds5 = mesh->z - sinfo->z_maxc;
                        JeepStaticBounds4 = mesh->z - sinfo->z_minc;
                        break;
                    case -0x4000:
                        JeepStaticBounds1 = mesh->x - sinfo->z_maxc;
                        JeepStaticBounds0 = mesh->x - sinfo->z_minc;
                        JeepStaticBounds5 = mesh->z + sinfo->x_minc;
                        JeepStaticBounds4 = mesh->z + sinfo->x_maxc;
                        break;
                    case 0x4000:
                        JeepStaticBounds1 = mesh->x + sinfo->z_minc;
                        JeepStaticBounds0 = mesh->x + sinfo->z_maxc;
                        JeepStaticBounds5 = mesh->z - sinfo->x_maxc;
                        JeepStaticBounds4 = mesh->z - sinfo->x_minc;
                        break;
                    default:
                        JeepStaticBounds1 = mesh->x + sinfo->x_minc;
                        JeepStaticBounds0 = mesh->x + sinfo->x_maxc;
                        JeepStaticBounds5 = mesh->z + sinfo->z_minc;
                        JeepStaticBounds4 = mesh->z + sinfo->z_maxc;
                        break;
                    }
                    if (JeepBounds0 > JeepStaticBounds1 && JeepBounds1 < JeepStaticBounds0 && JeepBounds2 > JeepStaticBounds3 && JeepBounds3 < JeepStaticBounds2 && JeepBounds4 > JeepStaticBounds5 && JeepBounds5 < JeepStaticBounds4) {
                        ShatterObject(0, mesh, -128, jroomies[i], 0);
                        SoundEffect(SFX_HIT_ROCK, (PHD_3DPOS *)&pos, SFX_DEFAULT);
                        SmashedMeshRoom[SmashedMeshCount] = jroomies[i];
                        SmashedMesh[SmashedMeshCount] = mesh;
                        SmashedMeshCount++;
                        mesh->Flags &= ~1;
                    }
                }
            }
        }
    }
}

void JeepBaddieCollision(ITEM_INFO *item)
{
    JEEPINFO *jeep;
    ITEM_INFO *collided;
    OBJECT_INFO *obj;
    sint16 *doors;
    sint32 i, j, dx, dy, dz;
    sint16 room_count, item_number;

    jeep = (JEEPINFO *)item->data;
    room_count = 1;
    jroomies[0] = item->room_number;
    doors = room[item->room_number].door;
    if (doors) {
        for (i = *doors++; i > 0; i--, doors += 16) {
            for (j = 0; j < room_count; j++) {
                if (jroomies[j] == *doors)
                    break;
            }
            if (j == room_count) {
                jroomies[room_count] = *doors;
                room_count++;
            }
        }
    }
    for (i = 0; i < room_count; i++) {
        for (item_number = room[jroomies[i]].item_number; item_number != NO_ITEM; item_number = collided->next_item) {
            collided = &items[item_number];
            if (collided->collidable && collided->status != ITEM_INVISIBLE && collided != lara_item && collided != item) {
                if (collided->object_number == ENEMY_JEEP) {
                    mycoll.coll_type = 0;
                    mycoll.radius = 400;
                    mycoll.enable_baddie_push = 1;
                    mycoll.enable_spaz = 0;
                    ObjectCollision(item_number, item, &mycoll);
                } else {
                    obj = &objects[collided->object_number];
                    if ((obj->collision && obj->intelligent) || collided->object_number == ROLLINGBALL || collided->object_number == TEETH_SPIKES) {
                        dx = item->pos.x_pos - collided->pos.x_pos;
                        dy = item->pos.y_pos - collided->pos.y_pos;
                        dz = item->pos.z_pos - collided->pos.z_pos;
                        if (dx > -2048 && dx < 2048 && dz > -2048 && dz < 2048 && dy > -2048 && dy < 2048) {
                            if (collided->object_number == ROLLINGBALL) {
                                if (TestBoundsCollide(collided, lara_item, 100) && lara_item->hit_points > 0) {
                                    DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos, (GetRandomControl() & 3) + 8, lara_item->pos.y_rot, lara_item->room_number, 5);
                                    lara_item->hit_points -= 8;
                                    lara_item->hit_status = 1;
                                }
                            } else if (collided->object_number == TEETH_SPIKES) {
                                if (TestBoundsCollideTeethSpikes(collided) && collided->object_number == TEETH_SPIKES)
                                    jeep->flags |= 0x40;
                            } else if (TestBoundsCollide(collided, item, 550)) {
                                DoLotsOfBlood(collided->pos.x_pos, item->pos.y_pos - 256, collided->pos.z_pos, (GetRandomControl() & 3) + 8, item->pos.y_rot, collided->room_number, 3);
                                collided->hit_points = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

sint32 GetCollisionAnim(ITEM_INFO *item, PHD_VECTOR *moved, JEEPINFO *jeep)
{
    sint32 c, s, front, side;

    moved->x = item->pos.x_pos - moved->x;
    moved->z = item->pos.z_pos - moved->z;
    if (!moved->x && !moved->z)
        return 0;
    c = phd_cos(item->pos.y_rot);
    s = phd_sin(item->pos.y_rot);
    front = (moved->z * c + moved->x * s) >> W2V_SHIFT;
    side = (moved->x * c - moved->z * s) >> W2V_SHIFT;
    if (ABS(front) > ABS(side)) {
        if (front > 0)
            return 14;
        else
            return 13;
    } else {
        if (side > 0)
            return 11;
        else
            return 12;
    }
}

sint32 DoShift(ITEM_INFO *item, PHD_VECTOR *pos, PHD_VECTOR *old)
{
    sint32 x, z, x_old, z_old, shift_x, shift_z;
    sint16 room_number;
    FLOOR_INFO *floor;
    sint32 height;

    x = pos->x >> WALL_SHIFT;
    z = pos->z >> WALL_SHIFT;
    x_old = old->x >> WALL_SHIFT;
    z_old = old->z >> WALL_SHIFT;
    shift_x = pos->x & (WALL_L - 1);
    shift_z = pos->z & (WALL_L - 1);
    if (x == x_old) {
        if (z == z_old) {
            item->pos.z_pos += (old->z - pos->z);
            item->pos.x_pos += (old->x - pos->x);
        } else if (z > z_old) {
            item->pos.z_pos -= shift_z + 1;
            return (pos->x - item->pos.x_pos);
        } else {
            item->pos.z_pos += WALL_L + 1 - shift_z;
            return (item->pos.x_pos - pos->x);
        }
    } else if (z == z_old) {
        if (x > x_old) {
            item->pos.x_pos -= shift_x + 1;
            return (item->pos.z_pos - pos->z);
        } else {
            item->pos.x_pos += WALL_L + 1 - shift_x;
            return (pos->z - item->pos.z_pos);
        }
    } else {
        x = z = 0;
        room_number = item->room_number;
        floor = GetFloor(old->x, pos->y, pos->z, &room_number);
        height = GetHeight(floor, old->x, pos->y, pos->z);
        if (height < old->y - STEP_L) {
            if (pos->z > old->z)
                z = -shift_z - 1;
            else
                z = WALL_L + 1 - shift_z;
        }
        room_number = item->room_number;
        floor = GetFloor(pos->x, pos->y, old->z, &room_number);
        height = GetHeight(floor, pos->x, pos->y, old->z);
        if (height < old->y - STEP_L) {
            if (pos->x > old->x)
                x = -shift_x - 1;
            else
                x = WALL_L + 1 - shift_x;
        }
        if (x && z) {
            item->pos.z_pos += z;
            item->pos.x_pos += x;
        } else if (z) {
            item->pos.z_pos += z;
            if (z > 0)
                return (item->pos.x_pos - pos->x);
            else
                return (pos->x - item->pos.x_pos);
        } else if (x) {
            item->pos.x_pos += x;
            if (x > 0)
                return (pos->z - item->pos.z_pos);
            else
                return (item->pos.z_pos - pos->z);
        } else {
            item->pos.z_pos += (old->z - pos->z);
            item->pos.x_pos += (old->x - pos->x);
        }
    }
    return 0;
}

void AnimateJeep(ITEM_INFO *item, sint32 hitWall, sint32 killed)
{
    JEEPINFO *jeep;
    ITEM_INFO *l;

    jeep = (JEEPINFO *)item->data;
    l = lara_item;
    if (item->pos.y_pos != item->floor && l->current_anim_state != 11 && l->current_anim_state != 12 && !killed) {
        if (jeep->gear == 1)
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 20;
        else
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 6;
        l->frame_number = anims[l->anim_number].frame_base;
        l->goal_anim_state = 11;
        l->current_anim_state = 11;
    } else if (hitWall && l->current_anim_state != 4 && l->current_anim_state != 5 && l->current_anim_state != 2 && l->current_anim_state != 3 && l->current_anim_state != 11 && jeep->velocity > 10922 && !killed) {
        if (hitWall == 13) {
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 11;
            l->goal_anim_state = 4;
            l->current_anim_state = 4;
        } else if (hitWall == 14) {
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 10;
            l->goal_anim_state = 5;
            l->current_anim_state = 5;
        } else if (hitWall == 11) {
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 12;
            l->goal_anim_state = 2;
            l->current_anim_state = 2;
        } else {
            l->anim_number = objects[VEHICLE_EXTRA].anim_index + 13;
            l->goal_anim_state = 3;
            l->current_anim_state = 3;
        }
        l->frame_number = anims[l->anim_number].frame_base;
    } else {
        switch (l->current_anim_state) {
        case 0:
            if (killed)
                l->goal_anim_state = 16;
            else if ((input & (IN_JUMP | IN_LEFT)) == (IN_JUMP | IN_LEFT) && !jeep->velocity && !dont_exit_jeep) {
                if (CanGetOff_jeep(0))
                    l->goal_anim_state = 10;
            } else if (dbinput & IN_WALK) {
                if (jeep->gear)
                    jeep->gear--;
            } else if (dbinput & IN_SPRINT) {
                if (jeep->gear < 1) {
                    jeep->gear++;
                    if (jeep->gear == 1)
                        l->goal_anim_state = 17;
                }
            } else if (input & IN_ACTION && !(input & IN_JUMP))
                l->goal_anim_state = 1;
            else if (input & (IN_LSTEP | IN_LEFT))
                l->goal_anim_state = 7;
            else if (input & (IN_RSTEP | IN_RIGHT))
                l->goal_anim_state = 8;
            break;
        case 1:
            if (killed)
                l->goal_anim_state = 0;
            else if (!(jeep->velocity & 0xFFFFFF00) && !(input & (IN_ACTION | IN_JUMP)))
                l->goal_anim_state = 0;
            else if (input & IN_JUMP) {
                if (jeep->velocity > 21844)
                    l->goal_anim_state = 6;
                else
                    l->goal_anim_state = 0;
            } else if (input & (IN_LSTEP | IN_LEFT))
                l->goal_anim_state = 7;
            else if (input & (IN_RSTEP | IN_RIGHT))
                l->goal_anim_state = 8;
            break;
        case 6:
            if (killed)
                l->goal_anim_state = 0;
            else if (!(jeep->velocity & 0xFFFFFF00))
                l->goal_anim_state = 0;
            else if (input & (IN_LSTEP | IN_LEFT))
                l->goal_anim_state = 7;
            else if (input & (IN_RSTEP | IN_RIGHT))
                l->goal_anim_state = 8;
            break;
        case 7:
            if (killed)
                l->goal_anim_state = 0;
            else if (dbinput & IN_WALK) {
                if (jeep->gear)
                    jeep->gear--;
            } else if (dbinput & IN_SPRINT) {
                if (jeep->gear < 1) {
                    jeep->gear++;
                    if (jeep->gear == 1) {
                        l->goal_anim_state = 15;
                        l->current_anim_state = 15;
                        l->anim_number = objects[VEHICLE_EXTRA].anim_index + 40;
                        l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 40].frame_base;
                        break;
                    }
                }
            } else if (input & (IN_RSTEP | IN_RIGHT))
                l->goal_anim_state = 1;
            else if (!(input & (IN_LSTEP | IN_LEFT))) {
                if (jeep->velocity)
                    l->goal_anim_state = 1;
                else
                    l->goal_anim_state = 0;
            } else
                l->goal_anim_state = 7;
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 4 && !jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 32;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 32].frame_base + 14;
            }
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 32 && jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 4;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 4].frame_base;
            }
            break;
        case 8:
            if (killed)
                l->goal_anim_state = 0;
            else if (dbinput & IN_WALK) {
                if (jeep->gear)
                    jeep->gear--;
            } else if (dbinput & IN_SPRINT) {
                if (jeep->gear < 1) {
                    jeep->gear++;
                    if (jeep->gear == 1) {
                        l->goal_anim_state = 14;
                        l->current_anim_state = 14;
                        l->anim_number = objects[VEHICLE_EXTRA].anim_index + 41;
                        l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 41].frame_base;
                        break;
                    }
                }
            } else if (input & (IN_LSTEP | IN_LEFT))
                l->goal_anim_state = 1;
            else if (!(input & (IN_RSTEP | IN_RIGHT))) {
                if (jeep->velocity)
                    l->goal_anim_state = 1;
                else
                    l->goal_anim_state = 0;
            } else
                l->goal_anim_state = 8;
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 16 && !jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 33;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 33].frame_base + 14;
            }
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 33 && jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 16;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 16].frame_base;
            }
            break;
        case 13:
            if (killed)
                l->goal_anim_state = 17;
            else if (!(abs(jeep->velocity) & 0xFFFFFF00))
                l->goal_anim_state = 17;
            else if (input & (IN_LSTEP | IN_LEFT))
                l->goal_anim_state = 15;
            else if (input & (IN_RSTEP | IN_RIGHT))
                l->goal_anim_state = 14;
            break;
        case 17:
            if (killed)
                l->goal_anim_state = 0;
            if ((input & (IN_JUMP | IN_LEFT)) == (IN_JUMP | IN_LEFT) && !jeep->velocity && !dont_exit_jeep) {
                if (CanGetOff_jeep(0))
                    l->goal_anim_state = 10;
            } else if (dbinput & IN_WALK) {
                if (jeep->gear) {
                    jeep->gear--;
                    if (!jeep->gear)
                        l->goal_anim_state = 0;
                }
            } else if (dbinput & IN_SPRINT) {
                if (jeep->gear < 1)
                    jeep->gear++;
            } else if (input & IN_ACTION && !(input & IN_JUMP))
                l->goal_anim_state = 13;
            else if (input & (IN_LSTEP | IN_LEFT))
                l->goal_anim_state = 15;
            else if (input & (IN_RSTEP | IN_RIGHT))
                l->goal_anim_state = 14;
            break;
        case 14:
            if (killed)
                l->goal_anim_state = 17;
            else if (dbinput & IN_WALK) {
                if (jeep->gear) {
                    jeep->gear--;
                    if (!jeep->gear) {
                        l->goal_anim_state = 8;
                        l->current_anim_state = 8;
                        l->anim_number = objects[VEHICLE_EXTRA].anim_index + 44;
                        l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 44].frame_base;
                        break;
                    }
                }
            } else if (dbinput & IN_SPRINT) {
                if (jeep->gear < 1)
                    jeep->gear++;
            } else if (!(input & (IN_RSTEP | IN_RIGHT)))
                l->goal_anim_state = 13;
            else
                l->goal_anim_state = 14;
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 30 && !jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 37;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 37].frame_base + 14;
            }
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 37 && jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 30;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 30].frame_base;
            }
            break;
        case 15:
            if (killed)
                l->goal_anim_state = 17;
            else if (dbinput & IN_WALK) {
                if (jeep->gear) {
                    jeep->gear--;
                    if (!jeep->gear) {
                        l->goal_anim_state = 7;
                        l->current_anim_state = 7;
                        l->anim_number = objects[VEHICLE_EXTRA].anim_index + 44;
                        l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 44].frame_base;
                        break;
                    }
                }
            } else if (dbinput & IN_SPRINT) {
                if (jeep->gear < 1)
                    jeep->gear++;
            } else if (!(input & (IN_LSTEP | IN_LEFT)))
                l->goal_anim_state = 13;
            else
                l->goal_anim_state = 15;
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 27 && !jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 36;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 36].frame_base + 14;
            }
            if (l->anim_number == objects[VEHICLE_EXTRA].anim_index + 36 && jeep->velocity) {
                l->anim_number = objects[VEHICLE_EXTRA].anim_index + 27;
                l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 27].frame_base;
            }
            break;
        case 11:
            if (item->pos.y_pos == item->floor)
                l->goal_anim_state = 12;
            else if (item->fallspeed > 300)
                jeep->flags |= 0x40;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            if (killed)
                l->goal_anim_state = 0;
            else if (input & (IN_ACTION | IN_JUMP))
                l->goal_anim_state = 1;
            break;
        }
    }
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        l->goal_anim_state = 11;
        l->hit_points = 0;
        JeepExplode(item);
    }
}

sint32 CanGetOff_jeep(sint16 num)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 x, y, z, h, c;
    sint16 yrot, room_number;

    item = &items[lara.vehicle];
    yrot = item->pos.y_rot + 0x4000;
    x = item->pos.x_pos - (512 * phd_sin(yrot) >> W2V_SHIFT);
    y = item->pos.y_pos;
    z = item->pos.z_pos - (512 * phd_cos(yrot) >> W2V_SHIFT);
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (height_type != BIG_SLOPE && height_type != DIAGONAL && h != NO_HEIGHT) {
        if (ABS(h - item->pos.y_pos) <= 512) {
            c = GetCeiling(floor, x, y, z);
            if (c - item->pos.y_pos <= -762 && h - c >= 762)
                return 1;
        }
    }
    return 0;
}

sint32 UserControl_jeep(ITEM_INFO *item, sint32 height, sint32 *pitch)
{
    JEEPINFO *jeep;
    PHD_VECTOR pos;
    sint32 turn, maxTurn, vel;

    if (lara_item->current_anim_state == 10 || lara_item->goal_anim_state == 10)
        input = 0;
    jeep = (JEEPINFO *)item->data;
    if (jeep->unused1 > 16) {
        jeep->velocity += jeep->unused1 >> 4;
        jeep->unused1 -= jeep->unused1 >> 3;
    } else {
        jeep->unused1 = 0;
    }
    if (item->pos.y_pos >= height - 256) {
        if (!jeep->velocity && input & IN_LOOK)
            LookUpDown();
        vel = abs(jeep->velocity);
        if (vel > 0x4000) {
            maxTurn = 910;
            turn = 242;
        } else {
            maxTurn = (910 * vel) >> 14;
            turn = ((60 * vel) >> 14) + 182;
        }
        if (jeep->velocity > 0) {
            if (input & (IN_LSTEP | IN_LEFT)) {
                jeep->turn_rate -= turn;
                if (jeep->turn_rate < -maxTurn)
                    jeep->turn_rate = -maxTurn;
            } else if (input & (IN_RSTEP | IN_RIGHT)) {
                jeep->turn_rate += turn;
                if (jeep->turn_rate > maxTurn)
                    jeep->turn_rate = maxTurn;
            }
        } else if (jeep->velocity < 0) {
            if (input & (IN_RSTEP | IN_RIGHT)) {
                jeep->turn_rate -= turn;
                if (jeep->turn_rate < -maxTurn)
                    jeep->turn_rate = -maxTurn;
            } else if (input & (IN_LSTEP | IN_LEFT)) {
                jeep->turn_rate += turn;
                if (jeep->turn_rate > maxTurn)
                    jeep->turn_rate = maxTurn;
            }
        }
        if (input & IN_JUMP) {
            if (jeep->velocity > 0) {
                jeep->velocity -= 768;
                if (jeep->velocity < 0)
                    jeep->velocity = 0;
            } else if (jeep->velocity < 0) {
                jeep->velocity += 768;
                if (jeep->velocity > 0)
                    jeep->velocity = 0;
            }
        } else if (input & IN_ACTION) {
            if (!jeep->gear) {
                if (jeep->velocity < 0x8000) {
                    if (jeep->velocity < 0x4000)
                        jeep->velocity += ((0x4800 - jeep->velocity) >> 3) + 8;
                    else if (jeep->velocity < 0x7000)
                        jeep->velocity += ((0x7800 - jeep->velocity) >> 4) + 4;
                    else
                        jeep->velocity += ((0x8000 - jeep->velocity) >> 3) + 2;
                } else {
                    jeep->velocity = 0x8000;
                }
            } else if (jeep->gear == 1) {
                if (jeep->velocity > -0x4000)
                    jeep->velocity -= (abs(-0x4000 - jeep->velocity) >> 3) + 2;
                else
                    jeep->velocity = -0x4000;
            }
            jeep->velocity -= ABS(item->pos.y_rot - jeep->move_angle) >> 6;
        }
        if (!(input & IN_ACTION)) {
            if (jeep->velocity > 256)
                jeep->velocity -= 256;
            else if (jeep->velocity < -256)
                jeep->velocity += 256;
            else
                jeep->velocity = 0;
        }
        item->speed = (sint16)(jeep->velocity >> 8);
        if (jeep->pitch1 > 0xC000)
            jeep->pitch1 = (GetRandomControl() & 0x1FF) + 0xBF00;
        vel = jeep->velocity;
        if (vel < 0)
            vel >>= 1;
        jeep->pitch1 += (abs(vel) - jeep->pitch1) >> 3;
    } else if (jeep->pitch1 < 0xFFFF) {
        jeep->pitch1 += (0xFFFF - jeep->pitch1) >> 3;
    }
    if (input & IN_JUMP) {
        pos.x = 0;
        pos.y = -144;
        pos.z = -1024;
        GetJointAbsPosition(item, &pos, 11);
        TriggerDynamic(pos.x, pos.y, pos.z, 10, 64, 0, 0);
        item->mesh_bits = 0x27FFF;
    } else {
        item->mesh_bits = 0x1BFFF;
    }
    *pitch = jeep->pitch1;
    return 0;
}

void InitialiseEnemyJeep(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[item->object_number].anim_index + 14;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 0;
    item->mesh_bits = ~0x24000;
    item->status += ITEM_ACTIVE;
}

void EnemyJeepControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *enemy;
    CREATURE_INFO *jeep;
    FLOOR_INFO *floor;
    AIOBJECT *aiobj;
    AI_INFO info, larainfo;
    PHD_VECTOR pos;
    sint32 Xoffset, Zoffset, x, y, z, h1, h2, front, back, i;
    sint16 j;
    sint16 room_number, xrot, zrot;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    jeep = (CREATURE_INFO *)item->data;
    room_number = item->room_number;
    Xoffset = (682 * (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
    Zoffset = (682 * (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
    x = item->pos.x_pos - Zoffset;
    y = item->pos.y_pos;
    z = item->pos.z_pos + Xoffset;
    floor = GetFloor(x, y, z, &room_number);
    h1 = GetHeight(floor, x, y, z);
    if (abs(y - h1) > 768) {
        item->pos.y_rot += 364;
        h1 = y;
        item->pos.x_pos += Zoffset >> 6;
        item->pos.z_pos -= Xoffset >> 6;
    }
    x = Zoffset + item->pos.x_pos;
    z = item->pos.z_pos - Xoffset;
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h2 = GetHeight(floor, x, y, z);
    if (abs(y - h2) > 768) {
        item->pos.y_rot -= 364;
        h2 = y;
        item->pos.x_pos -= Zoffset >> 6;
        item->pos.z_pos += Xoffset >> 6;
    }
    zrot = (sint16)phd_atan(1364, h2 - h1);
    x = Xoffset + item->pos.x_pos;
    z = Zoffset + item->pos.z_pos;
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h1 = GetHeight(floor, x, y, z);
    front = h1;
    if (abs(y - h1) > 768)
        h1 = y;
    x = item->pos.x_pos - Xoffset;
    z = item->pos.z_pos - Zoffset;
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h2 = GetHeight(floor, x, y, z);
    back = h2;
    if (abs(y - h2) > 768)
        h2 = y;
    xrot = (sint16)phd_atan(1364, h2 - h1);
    CreatureAIInfo(item, &info);
    enemy = &jeep->ai_target;
    jeep->enemy = enemy;
    if (enemy == lara_item) {
        larainfo.angle = info.angle;
        larainfo.distance = info.distance;
    } else {
        z = lara_item->pos.z_pos - item->pos.z_pos;
        x = lara_item->pos.x_pos - item->pos.x_pos;
        larainfo.angle = phd_atan(z, x) - item->pos.y_rot;
        if (z > 32000 || z < -32000 || x > 32000 || x < -32000)
            larainfo.distance = 0x7FFFFFFF;
        else
            larainfo.distance = SQUARE(z) + SQUARE(x);
    }
    switch (item->current_anim_state) {
    case 1:
        jeep->maximum_turn = item->item_flags[0] >> 4;
        item->item_flags[0] += 37;
        item->mesh_bits = ~0x24000;
        if (item->item_flags[0] > 0x2200)
            item->item_flags[0] = 0x2200;
        if (info.angle > 256)
            item->goal_anim_state = 4;
        else if (info.angle < -256)
            item->goal_anim_state = 3;
        break;
    case 0:
    case 2:
        item->item_flags[0] -= 128;
        item->mesh_bits = ~0x18000;
        if (item->item_flags[0] < 0)
            item->item_flags[0] = 0;
        pos.x = 0;
        pos.y = -144;
        pos.z = -1024;
        GetJointAbsPosition(item, &pos, 11);
        TriggerDynamic(pos.x, pos.y, pos.z, 10, 64, 0, 0);
        if (item->required_anim_state)
            item->goal_anim_state = item->required_anim_state;
        else if (info.distance > 0x100000 || lara.location >= item->item_flags[3])
            item->goal_anim_state = 1;
        break;
    case 3:
    case 4:
        item->item_flags[0] += 18;
        if (item->item_flags[0] > 0x2200)
            item->item_flags[0] = 0x2200;
        if (abs(0) < 256)
            item->goal_anim_state = 1;
        else
            item->goal_anim_state = item->current_anim_state;
        break;
    case 5:
        if (item->item_flags[0] < 0x4A0)
            item->item_flags[0] = 0x4A0;
        break;
    }
    if (front > item->floor + 512) {
        jeep->LOT.is_jumping = 1;
        if (item->item_flags[1] <= 0) {
            item->item_flags[1] = xrot << 1;
            jeep->LOT.is_jumping = 1;
        } else {
            xrot = item->item_flags[1];
            item->item_flags[1] -= 8;
            if (item->item_flags[1] < 0)
                jeep->LOT.is_jumping = 0;
            item->pos.y_pos += item->item_flags[1] >> 6;
        }
        if (jeep->LOT.is_jumping) {
            jeep->maximum_turn = 0;
            item->goal_anim_state = 1;
        }
    } else if (back > item->floor + 512 && item->current_anim_state != 5) {
        item->item_flags[1] = 0;
        item->anim_number = objects[item->object_number].anim_index + 8;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = 5;
        item->goal_anim_state = 1;
    }
    if (info.distance < 0x240000 || item->item_flags[3] == -2)
        jeep->reached_goal = 1;
    if (jeep->reached_goal) {
        TestTriggersAtXYZ(enemy->pos.x_pos, enemy->pos.y_pos, enemy->pos.z_pos, enemy->room_number, 1, 0);
        if (lara.location < item->item_flags[3] && item->current_anim_state != 2 && item->goal_anim_state != 2) {
            item->anim_number = objects[item->object_number].anim_index + 1;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 2;
            item->goal_anim_state = 2;
            if (enemy->flags & 4) {
                item->pos = enemy->pos;
                if (item->room_number != enemy->room_number)
                    ItemNewRoom(item_number, enemy->room_number);
            }
        }
        if (larainfo.distance > 0x400000 && larainfo.distance < 0x6400000 && !item->item_flags[2] && (larainfo.angle < -20480 || larainfo.angle > 20480)) {
            JeepFireGrenade(item);
            item->item_flags[2] = 150;
        }
        if (enemy->flags == 62) {
            item->status = ITEM_INVISIBLE;
            RemoveActiveItem(item_number);
            DisableBaddieAI(item_number);
        }
        if (lara.location >= item->item_flags[3] || !(enemy->flags & 4)) {
            jeep->reached_goal = 0;
            item->item_flags[3]++;
            jeep->enemy = 0;
            aiobj = AIObjects;
            for (j = 0; j < nAIObjects; j++, aiobj++) {
                if (aiobj->trigger_flags == item->item_flags[3] && aiobj->room_number != 255) {
                    jeep->enemy = enemy;
                    enemy->object_number = aiobj->object_number;
                    enemy->room_number = aiobj->room_number;
                    enemy->pos.x_pos = aiobj->x;
                    enemy->pos.y_pos = aiobj->y;
                    enemy->pos.z_pos = aiobj->z;
                    enemy->pos.y_rot = aiobj->y_rot;
                    enemy->flags = aiobj->flags;
                    enemy->trigger_flags = aiobj->trigger_flags;
                    enemy->box_number = aiobj->box_number;
                    if (!(enemy->flags & 0x20)) {
                        enemy->pos.x_pos += (256 * (rcossin_tbl[(enemy->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
                        enemy->pos.z_pos += (256 * (rcossin_tbl[((enemy->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
                    }
                    break;
                }
            }
        }
    }
    item->item_flags[2]--;
    if (item->item_flags[2] < 0)
        item->item_flags[2] = 0;
    if (abs(xrot - item->pos.x_rot) < 256)
        item->pos.x_rot = xrot;
    else if (xrot > item->pos.x_rot)
        item->pos.x_rot += 256;
    else if (xrot < item->pos.x_rot)
        item->pos.x_rot -= 256;
    if (abs(zrot - item->pos.z_rot) < 256)
        item->pos.z_rot = zrot;
    else if (zrot > item->pos.z_rot)
        item->pos.z_rot += 256;
    else if (zrot < item->pos.z_rot)
        item->pos.z_rot -= 256;
    item->item_flags[0] += -2 - (xrot >> 9);
    if (item->item_flags[0] < 0)
        item->item_flags[0] = 0;
    x = ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * item->item_flags[0]) >> W2V_SHIFT;
    z = ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * item->item_flags[0]) >> W2V_SHIFT;
    for (i = 0; i < 4; i++)
        jeep->joint_rotation[i] -= item->item_flags[0];
    item->pos.x_pos += x >> 6;
    item->pos.z_pos += z >> 6;
    if (!jeep->reached_goal)
        CreatureYRot(&item->pos, info.angle, item->item_flags[0] >> 4);
    jeep->maximum_turn = 0;
    AnimateItem(item);
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (item->room_number != room_number)
        ItemNewRoom(item_number, room_number);
    if (item->pos.y_pos >= item->floor) {
        item->fallspeed = 0;
        item->pos.y_pos = item->floor;
        item->gravity_status = 0;
    } else
        item->gravity_status = 1;
    SoundEffect(SFX_JEEP_MOVE, &item->pos, (item->item_flags[0] << 10) + 0x1000004);
}

void JeepFireGrenade(ITEM_INFO *item)
{
    sint16 item_number;
    sint32 i;
    sint16 ang;
    ITEM_INFO *grenade;

    item_number = CreateItem();
    if (item_number != NO_ITEM) {
        grenade = &items[item_number];
        grenade->shade = -0x3DF0;
        grenade->object_number = GRENADE;
        grenade->room_number = item->room_number;
        InitialiseItem(item_number);
        grenade->pos.x_rot = item->pos.x_rot;
        ang = item->pos.y_rot + 0x8000;
        grenade->pos.z_rot = 0;
        grenade->pos.x_pos = item->pos.x_pos + ((1024 * phd_sin(ang)) >> W2V_SHIFT);
        grenade->pos.y_rot = ang;
        grenade->pos.y_pos = item->pos.y_pos - 768;
        grenade->pos.z_pos = item->pos.z_pos + ((1024 * phd_cos(ang)) >> W2V_SHIFT);
        SmokeCountL = 32;
        SmokeWeapon = 5;
        for (i = 0; i < 5; i++)
            TriggerGunSmoke(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 0, 0, 0, 1, 5, 32);
        if (GetRandomControl() & 3)
            grenade->item_flags[0] = 1;
        else
            grenade->item_flags[0] = 2;
        grenade->speed = 32;
        grenade->fallspeed = (-32 * phd_sin(grenade->pos.x_rot)) >> W2V_SHIFT;
        grenade->current_anim_state = grenade->pos.x_rot;
        grenade->goal_anim_state = grenade->pos.y_rot;
        grenade->required_anim_state = 0;
        grenade->hit_points = 120;
        AddActiveItem(item_number);
    }
}

void JeepStart(ITEM_INFO *item, ITEM_INFO *l)
{
    JEEPINFO *jeep;

    jeep = (JEEPINFO *)item->data;
    lara.gun_status = LG_HANDS_BUSY;
    lara.hit_direction = -1;
    l->current_anim_state = l->goal_anim_state = 0;
    l->anim_number = objects[VEHICLE_EXTRA].anim_index + 14;
    l->frame_number = anims[objects[VEHICLE_EXTRA].anim_index + 14].frame_base;
    item->anim_number = objects[JEEP].anim_index - objects[VEHICLE_EXTRA].anim_index + l->anim_number;
    item->frame_number = anims[item->anim_number].frame_base - anims[l->anim_number].frame_base + l->frame_number;
    item->flags |= IFL_TRIGGERED;
    item->hit_points = 1;
    jeep->unused1 = 0;
    jeep->gear = 0;
}
