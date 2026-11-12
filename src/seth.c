#include "seth.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "lara_states.h"
#include "objnums.h"
#include "people.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void TriggerSethMissile(PHD_3DPOS *pos, sint16 room_number, sint16 type)
{
    FX_INFO *fx;
    sint16 fx_number;

    fx_number = CreateEffect(room_number);
    if (fx_number != NO_ITEM) {
        fx = &effects[fx_number];
        fx->pos.x_pos = pos->x_pos;
        fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
        fx->pos.z_pos = pos->z_pos;
        fx->pos.x_rot = pos->x_rot;
        fx->pos.y_rot = pos->y_rot;
        fx->pos.z_rot = 0;
        fx->room_number = room_number;
        fx->counter = (sint16)(2 * GetRandomControl() + 0x8000);
        fx->speed = (GetRandomControl() & 0x1F) - (type == 1 ? 64 : 0) + 96;
        fx->flag1 = type;
        fx->object_number = BUBBLES;
        fx->frame_number = objects[BUBBLES].mesh_index + 2 * type;
    }
}

void TriggerSethSparks(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv)
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
    sptr->dR = 64;
    sptr->dG = (GetRandomControl() & 0x7F) + 64;
    sptr->dB = sptr->dG + 32;
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

void TriggerSethFlame(sint16 item_number, uint8 NodeNumber, sint32 size)
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
    sptr->dR = 0;
    sptr->dG = (GetRandomControl() & 0x7F) + 32;
    sptr->dB = sptr->dG + 64;
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

void TriggerSethMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv)
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
    sptr->dR = 0;
    sptr->dB = (GetRandomControl() & 0x7F) + 32;
    sptr->dG = sptr->dB + 64;
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
    if (effects[fx_number].flag1 == 1)
        sptr->Scalar = 3;
    else
        sptr->Scalar = 2;
    size = (GetRandomControl() & 7) + 64;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 5;
}

void InitialiseSeth(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[SETHA].anim_index + 4;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 12;
}

void SethControl(sint16 item_number)
{
    ITEM_INFO *item;
    CREATURE_INFO *seth;
    FLOOR_INFO *floor;
    AI_INFO info;
    sint32 x, y, z, Xoffset, Zoffset, c, h, nearheight, midheight, farheight;
    sint16 angle, room_number, hp, can_jump;

    if (!CreatureActive(item_number))
        return;
    angle = 0;
    item = &items[item_number];
    seth = (CREATURE_INFO *)item->data;
    room_number = item->room_number;
    Xoffset = (870 * (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
    Zoffset = (870 * (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
    x = item->pos.x_pos;
    y = item->pos.y_pos;
    z = item->pos.z_pos;
    floor = GetFloor(x, y, z, &room_number);
    c = GetCeiling(floor, x, y, z);
    x += Xoffset;
    z += Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    nearheight = GetHeight(floor, x, y, z);
    room_number = item->room_number;
    x += Xoffset;
    z += Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    midheight = GetHeight(floor, x, y, z);
    room_number = item->room_number;
    x += Xoffset;
    z += Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    farheight = GetHeight(floor, x, y, z);
    can_jump = (y < nearheight - 384 || y < midheight - 384) && ((y < farheight + 256 && y > farheight - 256) || farheight == NO_HEIGHT);
    x = item->pos.x_pos - Xoffset;
    z = item->pos.z_pos - Zoffset;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (item->hit_points <= 0)
        item->hit_points = 0;
    else {
        if (item->ai_bits == AMBUSH)
            GetAITarget(seth);
        else
            seth->enemy = lara_item;
        CreatureAIInfo(item, &info);
        GetCreatureMood(item, &info, 1);
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, seth->maximum_turn);
        switch (item->current_anim_state) {
        case 1:
            seth->LOT.is_jumping = 0;
            seth->flags = 0;
            if (item->required_anim_state)
                item->goal_anim_state = item->required_anim_state;
            else if (info.distance < 0x100000 && info.bite)
                item->goal_anim_state = 8;
            else if (lara_item->pos.y_pos < item->pos.y_pos - 1024) {
                if (seth->reached_goal)
                    item->goal_anim_state = 14;
                else {
                    item->ai_bits = AMBUSH;
                    seth->hurt_by_lara = 1;
                    item->goal_anim_state = 2;
                }
            } else if (info.distance < 0x640000 && info.ahead && (GetRandomControl() & 1) && Targetable(item, &info)) {
                item->item_flags[0] = 0;
                item->goal_anim_state = 11;
            } else if (c != NO_HEIGHT && c < y - 1792 && h != NO_HEIGHT && h > y - 1024 && (GetRandomControl() & 1)) {
                item->pos.y_pos -= 1536;
                if (Targetable(item, &info)) {
                    item->item_flags[0] = 0;
                    item->goal_anim_state = 12;
                } else
                    item->goal_anim_state = 2;
                item->pos.y_pos += 1536;
            } else if (info.distance < 0x900000 && info.angle < 0x1800 && info.angle > -0x1800 && info.bite && Targetable(item, &info))
                item->goal_anim_state = 4;
            else if (info.distance < 0x1000000 && info.angle < 0x2000 && info.angle > -0x2000 && h != NO_HEIGHT && h >= y - 256 && Targetable(item, &info)) {
                item->item_flags[0] = 0;
                item->goal_anim_state = 13;
            } else if (can_jump)
                item->goal_anim_state = 5;
            else
                item->goal_anim_state = 2;
            break;
        case 2:
            seth->maximum_turn = 1274;
            if ((info.ahead && info.distance < 0x1000000) || can_jump || seth->reached_goal)
                item->goal_anim_state = 1;
            else if (info.distance > 0x900000)
                item->goal_anim_state = 3;
            break;
        case 3:
            seth->maximum_turn = 2002;
            if ((info.ahead && info.distance < 0x1000000) || can_jump || seth->reached_goal)
                item->goal_anim_state = 1;
            else if (info.distance < 0x900000)
                item->goal_anim_state = 2;
            break;
        case 5:
            seth->LOT.is_jumping = 1;
            seth->maximum_turn = 0;
            break;
        case 4:
            if (can_jump) {
                if (item->anim_number == objects[SETHA].anim_index + 15 && item->frame_number == anims[item->anim_number].frame_base) {
                    seth->LOT.is_jumping = 1;
                    seth->maximum_turn = 0;
                }
            }
            if (!seth->flags && item->touch_bits && item->anim_number == objects[SETHA].anim_index + 16) {
                if (item->touch_bits & 0xE000) {
                    lara_item->hit_points -= 200;
                    lara_item->hit_status = 1;
                    seth->flags = 1;
                    CreatureEffectT(item, &seth_right_hand, 25, -1, DoBloodSplat);
                }
                if (item->touch_bits & 0xE0000) {
                    lara_item->hit_points -= 200;
                    lara_item->hit_status = 1;
                    seth->flags = 1;
                    CreatureEffectT(item, &seth_left_hand, 25, -1, DoBloodSplat);
                }
            }
            break;
        case 7:
            if (item->anim_number == objects[SETHA].anim_index + 17 && item->frame_number == anims[item->anim_number].frame_end && (GetRandomControl() & 1))
                item->required_anim_state = 10;
            break;
        case 15:
            seth->target.y = lara_item->pos.y_pos;
        case 11:
        case 12:
        case 13:
            seth->maximum_turn = 0;
            if (abs(info.angle) < 546)
                item->pos.y_rot += info.angle;
            else if (info.angle < 0)
                item->pos.y_rot -= 546;
            else
                item->pos.y_rot += 546;
            DoSethEffects(item_number);
            break;
        case 8:
            hp = lara_item->hit_points;
            seth->maximum_turn = 0;
            if (abs(info.angle) < 546)
                item->pos.y_rot += info.angle;
            else if (info.angle < 0)
                item->pos.y_rot -= 546;
            else
                item->pos.y_rot += 546;
            if (!seth->flags && item->touch_bits && item->frame_number > anims[item->anim_number].frame_base + 15 && item->frame_number < anims[item->anim_number].frame_base + 26) {
                lara_item->hit_points -= 250;
                lara_item->hit_status = 1;
                seth->flags = 1;
                CreatureEffectT(item, &seth_right_hand, 25, -1, DoBloodSplat);
            }
            if (hp && lara_item->hit_points <= 0) {
                CreatureKill(item, 14, 9, ANIM_SETHDEATH);
                seth->maximum_turn = 0;
                return;
            }
            break;
        case 14:
            if (item->anim_number != objects[SETHA].anim_index + 26) {
                seth->LOT.fly = 16;
                item->gravity_status = 0;
                seth->maximum_turn = 0;
                seth->target.y = lara_item->pos.y_pos;
                if (abs(info.angle) < 546)
                    item->pos.y_rot += info.angle;
                else if (info.angle < 0)
                    item->pos.y_rot -= 546;
                else
                    item->pos.y_rot += 546;
            }
            if (lara_item->pos.y_pos > item->floor - 512) {
                seth->LOT.fly = 0;
                item->gravity_status = 1;
                if (item->pos.y_pos >= item->floor)
                    item->goal_anim_state = 1;
            } else if (Targetable(item, &info)) {
                item->item_flags[0] = 0;
                item->goal_anim_state = 15;
            }
            break;
        }
    }
    if (item->hit_status && (lara.gun_type == WEAPON_SHOTGUN || lara.gun_type == WEAPON_REVOLVER) && info.distance < 0x400000 && !seth->LOT.is_jumping) {
        if (item->current_anim_state != 12) {
            if (item->current_anim_state > 13) {
                item->anim_number = objects[SETHA].anim_index + 25;
                item->current_anim_state = item->goal_anim_state = 16;
            } else if (abs(h - y) < 512) {
                item->anim_number = objects[SETHA].anim_index + 17;
                item->current_anim_state = item->goal_anim_state = 7;
            } else {
                item->anim_number = objects[SETHA].anim_index + 11;
                item->current_anim_state = item->goal_anim_state = 6;
            }
            item->frame_number = anims[item->anim_number].frame_base;
        }
    }
    CreatureAnimation(item_number, angle, 0);
}
