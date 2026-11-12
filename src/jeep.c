#include "jeep.h"
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
#include "sphere.h"
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

void DrawJeepExtras(ITEM_INFO *item)
{
    JEEPINFO *jeep;

    if (lara.vehicle == NO_ITEM)
        return;
    jeep = (JEEPINFO *)item->data;
    DrawJeepSpeedo(phd_winwidth - 64, phd_winheight - 16, jeep->velocity, 0x6000, 0x8000, 32, jeep->gear);
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
