#include "sas.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "items.h"
#include "lara.h"
#include "lara_states.h"
#include "objnums.h"
#include "sphere.h"
#include "switch.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/input.h"

void InitialiseSas(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[SAS].anim_index + 12;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}

void SasControl(sint16 item_number)
{
    ITEM_INFO *item;
    CREATURE_INFO *sas;
    AI_INFO info;
    PHD_VECTOR pos;
    sint32 iDistance, dx, dz;
    sint16 angle, tilt, head, torso_x, torso_y, iAngle, violent;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    sas = (CREATURE_INFO *)item->data;
    angle = 0;
    tilt = 0;
    head = 0;
    torso_x = 0;
    torso_y = 0;
    if (item->fired_weapon) {
        pos.x = sas_fire.x;
        pos.y = sas_fire.y;
        pos.z = sas_fire.z;
        GetJointAbsPosition(item, &pos, sas_fire.mesh_num);
        TriggerDynamic(pos.x, pos.y, pos.z, 2 * item->fired_weapon + 8, 24, 16, 4);
        item->fired_weapon--;
    }
    if (item->hit_points <= 0) {
        if (item->current_anim_state != 7) {
            item->anim_number = objects[item->object_number].anim_index + 19;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 7;
        }
    } else {
        if (item->ai_bits)
            GetAITarget(sas);
        else
            sas->enemy = lara_item;
        CreatureAIInfo(item, &info);
        if (sas->enemy == lara_item) {
            iDistance = info.distance;
            iAngle = info.angle;
        } else {
            dx = lara_item->pos.x_pos - item->pos.x_pos;
            dz = lara_item->pos.z_pos - item->pos.z_pos;
            iAngle = (sint16)(phd_atan(dz, dx) - item->pos.y_rot);
            iDistance = SQUARE(dx) + SQUARE(dz);
        }
        violent = sas->enemy != lara_item;
        GetCreatureMood(item, &info, violent);
        if (lara.vehicle != NO_ITEM && info.bite)
            sas->mood = ESCAPE_MOOD;
        CreatureMood(item, &info, violent);
        angle = CreatureTurn(item, sas->maximum_turn);
        if (item->hit_status)
            AlertAllGuards(item_number);
        switch (item->current_anim_state) {
        case 1:
            head = iAngle;
            sas->flags = 0;
            sas->maximum_turn = 0;
            if (item->anim_number == objects[item->object_number].anim_index + 17) {
                if (abs(info.angle) < 1820)
                    item->pos.y_rot += info.angle;
                else if (info.angle < 0)
                    item->pos.y_rot -= 1820;
                else
                    item->pos.y_rot += 1820;
            } else if (item->ai_bits == MODIFY || lara.vehicle) {
                if (abs(info.angle) < 364)
                    item->pos.y_rot += info.angle;
                else if (info.angle < 0)
                    item->pos.y_rot -= 364;
                else
                    item->pos.y_rot += 364;
            }
            if (item->ai_bits & GUARD) {
                head = AIGuard(sas);
                if (!(GetRandomControl() & 0xFF)) {
                    if (item->current_anim_state == 1)
                        item->goal_anim_state = 4;
                    else
                        item->goal_anim_state = 1;
                }
            } else if (item->ai_bits & PATROL1 && item->ai_bits != MODIFY && !lara.vehicle) {
                item->goal_anim_state = 2;
                head = 0;
            } else if (Targetable(item, &info)) {
                if (info.distance >= 0x900000 && info.zone_number == info.enemy_zone) {
                    if (item->ai_bits != MODIFY)
                        item->goal_anim_state = 2;
                } else if (GetRandomControl() & 1)
                    item->goal_anim_state = 8;
                else if (GetRandomControl() & 1)
                    item->goal_anim_state = 10;
                else
                    item->goal_anim_state = 12;
            } else if (item->ai_bits == MODIFY)
                item->goal_anim_state = 1;
            else if (sas->mood == ESCAPE_MOOD)
                item->goal_anim_state = 3;
            else if ((sas->alerted || sas->mood != BORED_MOOD) && (!(item->ai_bits & FOLLOW) || !sas->reached_goal && iDistance <= 0x400000)) {
                if (sas->mood != BORED_MOOD && info.distance > 0x400000)
                    item->goal_anim_state = 3;
                else
                    item->goal_anim_state = 2;
            } else
                item->goal_anim_state = 1;
            break;
        case 4:
            head = iAngle;
            sas->flags = 0;
            sas->maximum_turn = 0;
            if (item->ai_bits & GUARD) {
                head = AIGuard(sas);
                if (!(GetRandomControl() & 0xFF))
                    item->goal_anim_state = 1;
            } else if (Targetable(item, &info) || sas->mood || !info.ahead || item->ai_bits & MODIFY || lara.vehicle)
                item->goal_anim_state = 1;
            break;
        case 2:
            head = iAngle;
            sas->flags = 0;
            sas->maximum_turn = 910;
            if (item->ai_bits & PATROL1)
                item->goal_anim_state = 2;
            else if (lara.vehicle && (item->ai_bits == MODIFY || !item->ai_bits))
                item->goal_anim_state = 1;
            else if (sas->mood == ESCAPE_MOOD)
                item->goal_anim_state = 3;
            else if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (sas->reached_goal || iDistance > 0x400000))
                item->goal_anim_state = 1;
            else if (Targetable(item, &info)) {
                if (info.distance >= 0x900000 && info.zone_number == info.enemy_zone)
                    item->goal_anim_state = 9;
                else
                    item->goal_anim_state = 1;
            } else if (sas->mood == BORED_MOOD && info.ahead)
                item->goal_anim_state = 1;
            else if (sas->mood != BORED_MOOD && info.distance > 0x400000)
                item->goal_anim_state = 3;
            break;
        case 3:
            if (info.ahead)
                head = info.angle;
            sas->maximum_turn = 1820;
            tilt = angle / 2;
            if (lara.vehicle) {
                if (item->ai_bits == MODIFY || !item->ai_bits) {
                    item->goal_anim_state = 2;
                    break;
                }
            }
            if (item->ai_bits & GUARD || item->ai_bits & FOLLOW && (sas->reached_goal || iDistance > 0x400000))
                item->goal_anim_state = 2;
            else if (sas->mood != ESCAPE_MOOD) {
                if (Targetable(item, &info))
                    item->goal_anim_state = 2;
                else if (sas->mood == BORED_MOOD || sas->mood == STALK_MOOD && !(item->ai_bits & FOLLOW) && info.distance < 0x400000)
                    item->goal_anim_state = 2;
            }
            break;
        case 8:
        case 10:
        case 12:
            sas->flags = 0;
            if (info.ahead) {
                torso_y = info.angle;
                torso_x = info.x_angle;
                if (Targetable(item, &info)) {
                    if (item->current_anim_state == 8)
                        item->goal_anim_state = 5;
                    else if (item->current_anim_state == 12)
                        item->goal_anim_state = 13;
                    else if (GetRandomControl() & 1)
                        item->goal_anim_state = 11;
                    else
                        item->goal_anim_state = 15;
                } else
                    item->goal_anim_state = 1;
            }
            break;
        case 9:
            sas->flags = 0;
            if (info.ahead) {
                torso_y = info.angle;
                torso_x = info.x_angle;
                if (Targetable(item, &info))
                    item->goal_anim_state = 6;
                else
                    item->goal_anim_state = 2;
            }
            break;
        case 15:
            if (info.ahead) {
                torso_y = info.angle;
                torso_x = info.x_angle;
            }
            break;
        case 16:
            if (info.ahead) {
                torso_y = info.angle;
                torso_x = info.x_angle;
                if (info.distance > 0x900000)
                    torso_x = (sint16)(phd_sqrt(info.distance) + torso_x - 1024);
            }
            if (item->frame_number == anims[item->anim_number].frame_base + 20) {
                if (!sas->enemy->speed) {
                    torso_x += (GetRandomControl() & 0x1FF) - 256;
                    torso_y += (GetRandomControl() & 0x1FF) - 256;
                }
                SasFireGrenade(item, torso_x, torso_y);
                if (Targetable(item, &info))
                    item->goal_anim_state = 15;
            }
            break;
        case 11:
        case 13:
            if (item->goal_anim_state != 1 && item->goal_anim_state != 14 && (sas->mood == ESCAPE_MOOD || !Targetable(item, &info))) {
                if (item->current_anim_state == 11)
                    item->goal_anim_state = 1;
                else
                    item->goal_anim_state = 14;
            }
        case 5:
        case 6:
            if (info.ahead) {
                torso_y = info.angle;
                torso_x = info.x_angle;
            }
            if (!sas->flags) {
                ShotLara(item, &info, &sas_fire, torso_y, 15);
                sas->flags = 5;
                item->fired_weapon = 3;
            } else
                sas->flags--;
            break;
        case 17:
            if (!lara.blindTimer && !(GetRandomControl() & 0x7F))
                item->goal_anim_state = 4;
            break;
        }
        if (lara.blindTimer > 100 && item->current_anim_state != 17) {
            sas->maximum_turn = 0;
            item->anim_number = objects[SAS].anim_index + 28;
            item->frame_number = anims[item->anim_number].frame_base + (GetRandomControl() & 7);
            item->current_anim_state = 17;
        }
    }
    CreatureTilt(item, tilt);
    CreatureJoint(item, 0, torso_y);
    CreatureJoint(item, 1, torso_x);
    CreatureJoint(item, 2, head);
    CreatureAnimation(item_number, angle, 0);
}

void InitialiseInjuredSas(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->trigger_flags) {
        item->anim_number = objects[SAS_DYING].anim_index;
        item->current_anim_state = item->goal_anim_state = 1;
    } else {
        item->anim_number = objects[SAS_DYING].anim_index + 3;
        item->current_anim_state = item->goal_anim_state = 4;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}

void InjuredSasControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    switch (item->current_anim_state) {
    case 4:
        if (!(GetRandomControl() & 0x7F))
            item->goal_anim_state = 5;
        break;
    case 1:
        if (!(GetRandomControl() & 0x7F))
            item->goal_anim_state = 2;
        else if (!(GetRandomControl() & 0xFF))
            item->goal_anim_state = 3;
        break;
    }
    AnimateItem(item);
}

void SasFireGrenade(ITEM_INFO *sas, sint16 xrot, sint16 yrot)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    PHD_VECTOR pos;
    sint32 ox, oy, oz;
    sint32 h, i;
    sint16 item_number;

    item_number = CreateItem();
    if (item_number == NO_ITEM)
        return;
    item = &items[item_number];
    item->shade = -0x3DF0;
    item->object_number = GRENADE;
    item->room_number = sas->room_number;
    pos.x = sas_fire.x;
    pos.y = sas_fire.y;
    pos.z = sas_fire.z;
    GetJointAbsPosition(sas, &pos, sas_fire.mesh_num);
    ox = pos.x;
    item->pos.x_pos = ox;
    oy = pos.y;
    item->pos.y_pos = oy;
    oz = pos.z;
    item->pos.z_pos = oz;
    floor = GetFloor(pos.x, pos.y, pos.z, &item->room_number);
    h = GetHeight(floor, pos.x, pos.y, pos.z);
    if (h < pos.y) {
        item->pos.x_pos = sas->pos.x_pos;
        item->pos.y_pos = pos.y;
        item->pos.z_pos = sas->pos.z_pos;
        item->room_number = sas->room_number;
    }
    SmokeCountL = 32;
    SmokeWeapon = WEAPON_GRENADE;
    for (i = 0; i < 5; i++)
        TriggerGunSmoke(ox, oy, oz, pos.x - ox, pos.y - oy, pos.z - oz, 1, 5, 32);
    InitialiseItem(item_number);
    item->pos.x_rot = xrot + sas->pos.x_rot;
    item->pos.y_rot = yrot + sas->pos.y_rot;
    item->pos.z_rot = 0;
    if (GetRandomControl() & 3)
        item->item_flags[0] = 1;
    else
        item->item_flags[0] = 2;
    item->item_flags[2] = 1;
    item->speed = 128;
    item->fallspeed = (-128 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
    item->current_anim_state = item->pos.x_rot;
    item->goal_anim_state = item->pos.y_rot;
    item->required_anim_state = 0;
    item->hit_points = 120;
    AddActiveItem(item_number);
}

void DragSASCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 x, z;

    item = &items[item_number];
    if (((input & IN_ACTION) && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS && !l->gravity_status && !(item->flags & IFL_CODEBITS)) || (lara.IsMoving && lara.GeneralPtr == (void *)item_number)) {
        if (TestLaraPosition(DragSASBounds, item, l)) {
            if (MoveLaraPosition(&DragSASPos, item, l)) {
                l->anim_number = ANIM_DRAGSAS;
                l->frame_number = anims[ANIM_DRAGSAS].frame_base;
                l->current_anim_state = AS_CONTROLLED;
                l->pos.y_rot = item->pos.y_rot;
                lara.IsMoving = 0;
                lara.head_y_rot = 0;
                lara.head_x_rot = 0;
                lara.torso_y_rot = 0;
                lara.torso_x_rot = 0;
                lara.gun_status = LG_HANDS_BUSY;
                item->flags |= IFL_CODEBITS;
                item->status = ITEM_ACTIVE;
                AddActiveItem(item_number);
            } else
                lara.GeneralPtr = (void *)item_number;
        }
    } else {
        if (item->status == ITEM_ACTIVE) {
            if (item->frame_number == anims[item->anim_number].frame_end) {
                x = (2048 * phd_sin(l->pos.y_rot)) >> W2V_SHIFT;
                z = (2048 * phd_cos(l->pos.y_rot)) >> W2V_SHIFT;
                TestTriggersAtXYZ(l->pos.x_pos - x, l->pos.y_pos, l->pos.z_pos - z, l->room_number, 1, 0);
                RemoveActiveItem(item_number);
                item->status = ITEM_INACTIVE;
            }
        }
        ObjectCollision(item_number, l, coll);
    }
}
