#include "harpy.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

extern BITE_INFO harpy_left_hit;
extern BITE_INFO harpy_right_hit;
extern BITE_INFO harpy_tail_hit;

void TriggerHarpyMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv)
{
    SPARKS *sptr;
    sint32 dx, dz, size;

    dx = lara_item->pos.x_pos - effects[fx_number].pos.x_pos;
    dz = lara_item->pos.z_pos - effects[fx_number].pos.z_pos;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = (GetRandomControl() & 0x7F) + 32;
    sptr->sB = sptr->dG + 64;
    sptr->dR = sptr->dG = (GetRandomControl() & 0x7F) + 32;
    sptr->dB = 0;
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 16;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0xF) - 8;
    sptr->y = 0;
    sptr->z = (GetRandomControl() & 0xF) - 8;
    sptr->Xvel = (sint16)xv;
    sptr->Yvel = (sint16)yv;
    sptr->Zvel = (sint16)zv;
    sptr->Friction = 68;
    sptr->Flags = 602;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -32 - (GetRandomControl() & 0x1F);
    else
        sptr->RotAdd = (GetRandomControl() & 0x1F) + 32;
    sptr->Gravity = 0;
    sptr->MaxYvel = 0;
    sptr->FxObj = (uint8)fx_number;
    sptr->Scalar = 2;
    size = (GetRandomControl() & 7) + 64;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 5;
}

void DoHarpyEffects(ITEM_INFO *item, sint16 item_number)
{
    PHD_VECTOR pos;
    PHD_VECTOR rh;
    PHD_VECTOR lh;
    PHD_3DPOS mPos;
    sint32 i;
    sint16 xv, yv, zv, size;
    sint16 angles[2];

    item->item_flags[0]++;
    rh.x = harpy_right_hand.x;
    rh.y = harpy_right_hand.y;
    rh.z = harpy_right_hand.z;
    GetJointAbsPosition(item, &rh, harpy_right_hand.mesh_num);
    lh.x = harpy_left_hand.x;
    lh.y = harpy_left_hand.y;
    lh.z = harpy_left_hand.z;
    GetJointAbsPosition(item, &lh, harpy_left_hand.mesh_num);
    if (item->item_flags[0] >= 24 && item->item_flags[0] <= 47 && (GetRandomControl() & 0x1F) < item->item_flags[0]) {
        for (i = 0; i < 2; i++) {
            pos.x = (GetRandomControl() & 0x7FF) + rh.x - 1024;
            pos.y = (GetRandomControl() & 0x7FF) + rh.y - 1024;
            pos.z = (GetRandomControl() & 0x7FF) + rh.z - 1024;
            xv = (sint16)((rh.x - pos.x) << 3);
            yv = (sint16)((rh.y - pos.y) << 3);
            zv = (sint16)((rh.z - pos.z) << 3);
            TriggerHarpySparks(pos.x, pos.y, pos.z, xv, yv, zv);
            pos.x = (GetRandomControl() & 0x7FF) + lh.x - 1024;
            pos.y = (GetRandomControl() & 0x7FF) + lh.y - 1024;
            pos.z = (GetRandomControl() & 0x7FF) + lh.z - 1024;
            xv = (sint16)((lh.x - pos.x) << 3);
            yv = (sint16)((lh.y - pos.y) << 3);
            zv = (sint16)((lh.z - pos.z) << 3);
            TriggerHarpySparks(pos.x, pos.y, pos.z, xv, yv, zv);
        }
    }
    size = item->item_flags[0] * 2;
    if (size > 64)
        size = 64;
    if (item->item_flags[0] < 80) {
        if ((wibble & 0xF) == 8)
            TriggerHarpyFlame(item_number, 4, size);
        else if (!(wibble & 0xF))
            TriggerHarpyFlame(item_number, 5, size);
    }
    if (item->item_flags[0] >= 61 && item->item_flags[0] <= 65 && GlobalCounter & 1) {
        pos.x = harpy_right_hand.x;
        pos.y = harpy_right_hand.y << 1;
        pos.z = harpy_right_hand.z;
        GetJointAbsPosition(item, &pos, harpy_right_hand.mesh_num);
        mPos.x_pos = rh.x;
        mPos.y_pos = rh.y;
        mPos.z_pos = rh.z;
        phd_GetVectorAngles(pos.x - mPos.x_pos, pos.y - mPos.y_pos, pos.z - mPos.z_pos, angles);
        mPos.y_rot = angles[0];
        mPos.x_rot = angles[1];
        TriggerHarpyMissile(&mPos, item->room_number, 2);
    } else if (item->item_flags[0] >= 61 && item->item_flags[0] <= 65 && !(GlobalCounter & 1)) {
        pos.x = harpy_left_hand.x;
        pos.y = harpy_left_hand.y << 1;
        pos.z = harpy_left_hand.z;
        GetJointAbsPosition(item, &pos, harpy_left_hand.mesh_num);
        mPos.x_pos = lh.x;
        mPos.y_pos = lh.y;
        mPos.z_pos = lh.z;
        phd_GetVectorAngles(pos.x - mPos.x_pos, pos.y - mPos.y_pos, pos.z - mPos.z_pos, angles);
        mPos.y_rot = angles[0];
        mPos.x_rot = angles[1];
        TriggerHarpyMissile(&mPos, item->room_number, 2);
    }
}

void TriggerHarpyMissile(PHD_3DPOS *pos, sint16 room_number, sint16 mesh)
{
    FX_INFO *fx;
    sint16 fx_num;

    fx_num = CreateEffect(room_number);
    if (fx_num != NO_ITEM) {
        fx = &effects[fx_num];
        fx->pos.x_pos = pos->x_pos;
        fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
        fx->pos.z_pos = pos->z_pos;
        fx->pos.x_rot = pos->x_rot;
        fx->pos.y_rot = pos->y_rot;
        fx->pos.z_rot = 0;
        fx->room_number = room_number;
        fx->counter = (sint16)(2 * GetRandomControl() + 0x8000);
        fx->speed = (GetRandomControl() & 0x1F) + 96;
        fx->object_number = BUBBLES;
        fx->flag1 = mesh;
        fx->frame_number = objects[BUBBLES].mesh_index + mesh * 2;
    }
}

void TriggerHarpySparks(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv)
{
    SPARKS *sptr;
    sint32 dx, dz;

    dx = lara_item->pos.x_pos - x;
    dz = lara_item->pos.z_pos - z;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = 0;
    sptr->sB = 0;
    sptr->dG = sptr->dR = (GetRandomControl() & 0x7F) + 64;
    sptr->dB = 0;
    sptr->sLife = sptr->Life = 16;
    sptr->ColFadeSpeed = 4;
    sptr->TransType = 2;
    sptr->FadeToBlack = 4;
    sptr->x = x;
    sptr->y = y;
    sptr->z = z;
    sptr->Xvel = xv;
    sptr->Yvel = yv;
    sptr->Zvel = zv;
    sptr->Friction = 34;
    sptr->Scalar = 1;
    sptr->Size = sptr->sSize = (GetRandomControl() & 3) + 4;
    sptr->dSize = (GetRandomControl() & 1) + 1;
    sptr->MaxYvel = 0;
    sptr->Gravity = 0;
    sptr->Flags = 0;
}

void TriggerHarpyFlame(sint16 item_number, uint8 NodeNumber, sint32 size)
{
    SPARKS *sptr;
    sint32 dx, dz;

    dx = lara_item->pos.x_pos - items[item_number].pos.x_pos;
    dz = lara_item->pos.z_pos - items[item_number].pos.z_pos;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 0;
    sptr->sG = 0;
    sptr->sB = 0;
    sptr->dR = sptr->dG = (GetRandomControl() & 0x7F) + 32;
    sptr->dB = 0;
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 20;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0xF) - 8;
    sptr->y = 0;
    sptr->z = (GetRandomControl() & 0xF) - 8;
    sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Yvel = 0;
    sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Friction = 5;
    sptr->Flags = 4762;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -32 - (GetRandomControl() & 0x1F);
    else
        sptr->RotAdd = (GetRandomControl() & 0x1F) + 32;
    sptr->Gravity = (GetRandomControl() & 0x1F) + 16;
    sptr->MaxYvel = 0;
    sptr->FxObj = (uint8)item_number;
    sptr->NodeNumber = NodeNumber;
    sptr->Scalar = 2;
    size += GetRandomControl() & 0xF;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 4;
}

void InitialiseHarpy(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[HARPY].anim_index + 4;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}

void HarpyControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *enemy;
    CREATURE_INFO *harpy;
    CREATURE_INFO *baddie;
    AI_INFO info;
    sint32 dx, dy, dz, dist, max_dist, i;
    sint16 angle, head, torso_x, torso_y;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    harpy = (CREATURE_INFO *)item->data;
    angle = 0;
    head = 0;
    torso_x = 0;
    torso_y = 0;
    if (item->hit_points <= 0) {
        item->hit_points = 0;
        switch (item->current_anim_state) {
        case 9:
            item->goal_anim_state = 10;
        case 10:
            if (item->pos.y_pos >= item->floor) {
                item->pos.y_pos = item->floor;
                item->fallspeed = 0;
                item->gravity_status = 0;
                item->goal_anim_state = 11;
            }
            item->pos.x_rot = 0;
            break;
        case 11:
            item->pos.x_rot = 0;
            item->pos.y_pos = item->floor;
            break;
        default:
            item->anim_number = objects[HARPY].anim_index + 5;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 9;
            item->gravity_status = 1;
            item->speed = 0;
            item->pos.x_rot = 0;
            break;
        }
    } else {
        if (item->ai_bits)
            GetAITarget(harpy);
        harpy->enemy = 0;
        max_dist = 0x7FFFFFFF;
        baddie = baddie_slots;
        for (i = 5; i > 0; i--, baddie++) {
            if (baddie->item_num != NO_ITEM && baddie->item_num != item_number) {
                enemy = &items[baddie->item_num];
                if (enemy->object_number == LARA_DOUBLE) {
                    dx = enemy->pos.x_pos - item->pos.x_pos;
                    dz = enemy->pos.z_pos - item->pos.z_pos;
                    dist = SQUARE(dx) + SQUARE(dz);
                    if (dist < max_dist) {
                        harpy->enemy = enemy;
                        max_dist = dist;
                    }
                }
            }
        }
        CreatureAIInfo(item, &info);
        enemy = harpy->enemy;
        if (enemy != lara_item)
            phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);
        GetCreatureMood(item, &info, 1);
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, harpy->maximum_turn);
        if (info.ahead) {
            head = info.angle >> 1;
            torso_y = info.angle >> 1;
            torso_x = info.x_angle;
        }
        switch (item->current_anim_state) {
        case 1:
            harpy->flags = 0;
            harpy->maximum_turn = 1274;
            if (enemy) {
                dy = item->pos.y_pos + 2048;
                if (enemy->pos.y_pos > dy && item->floor > dy) {
                    item->goal_anim_state = 3;
                    break;
                }
            }
            if (info.ahead) {
                dy = abs(enemy->pos.y_pos - item->pos.y_pos);
                if (dy <= 1024 && info.distance < 0x1C639) {
                    item->goal_anim_state = 6;
                    break;
                }
                if (dy <= 1024 && info.distance < 0x400000) {
                    item->goal_anim_state = 4;
                    break;
                }
            }
            if (harpy->enemy == lara_item && Targetable(item, &info) && info.distance > 0xC40000 && GetRandomControl() & 1) {
                item->goal_anim_state = 8;
                item->item_flags[0] = 0;
            } else {
                item->goal_anim_state = 2;
            }
            break;
        case 2:
            harpy->maximum_turn = 1274;
            harpy->flags = 0;
            if (item->required_anim_state) {
                item->goal_anim_state = item->required_anim_state;
                if (item->goal_anim_state == 8)
                    item->item_flags[0] = 0;
            } else if (item->hit_status || !info.ahead && GetRandomControl() & 1) {
                item->goal_anim_state = 7;
            } else if (info.ahead && info.distance < 0x1C639) {
                item->goal_anim_state = 6;
            } else if (!info.ahead || info.distance < 0x400000) {
                item->goal_anim_state = 4;
            } else if (info.distance <= 0xC40000 || !(GetRandomControl() & 1)) {
                item->goal_anim_state = 4;
            } else {
                item->goal_anim_state = 8;
                item->item_flags[0] = 0;
            }
            break;
        case 4:
            harpy->maximum_turn = 364;
            if (info.ahead && info.distance < 0x400000)
                item->goal_anim_state = 5;
            else
                item->goal_anim_state = 13;
            break;
        case 5:
            harpy->maximum_turn = 364;
            item->goal_anim_state = 2;
            if (item->touch_bits & 0x14 || enemy && enemy != lara_item && abs(enemy->pos.y_pos - item->pos.y_pos) <= 1024 && info.distance < 0x40000) {
                lara_item->hit_points -= 10;
                lara_item->hit_status = 1;
                if (item->touch_bits & 0x10)
                    CreatureEffectT(item, &harpy_left_hit, 5, -1, DoBloodSplat);
                else
                    CreatureEffectT(item, &harpy_right_hit, 5, -1, DoBloodSplat);
            }
            break;
        case 6:
            harpy->maximum_turn = 364;
            if (!harpy->flags) {
                if (item->touch_bits & 0x300000 || enemy && enemy != lara_item && abs(enemy->pos.y_pos - item->pos.y_pos) <= 1024 && info.distance < 0x40000) {
                    lara_item->hit_points -= 100;
                    lara_item->hit_status = 1;
                    CreatureEffectT(item, &harpy_tail_hit, 10, -1, DoBloodSplat);
                    if (enemy == lara_item)
                        lara.dpoisoned += 2048;
                    harpy->flags = 1;
                }
            }
            break;
        case 12:
            if (info.ahead && info.distance > 0xC40000) {
                item->goal_anim_state = 2;
                item->required_anim_state = 8;
            } else if (GetRandomControl() & 1) {
                item->goal_anim_state = 1;
            }
            break;
        case 13:
            item->goal_anim_state = 2;
            break;
        case 3:
            dy = item->pos.y_pos + 2048;
            if (!enemy || enemy->pos.y_pos < dy || item->floor < dy)
                item->goal_anim_state = 1;
            break;
        case 8:
            DoHarpyEffects(item, item_number);
            break;
        }
    }
    CreatureTilt(item, 0);
    CreatureJoint(item, 0, head);
    CreatureJoint(item, 1, torso_y);
    CreatureJoint(item, 2, torso_x);
    CreatureAnimation(item_number, angle, 0);
}
