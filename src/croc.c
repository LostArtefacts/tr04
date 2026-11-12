#include "croc.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "draw.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "people.h"
#include "room.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/output.h"

void InitialiseCroc(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        item->anim_number = objects[CROCODILE].anim_index + 12;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 8;
    } else {
        item->anim_number = objects[CROCODILE].anim_index;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 1;
    }
}

void CrocControl(sint16 item_number)
{
    ITEM_INFO *item;
    CREATURE_INFO *croc;
    FLOOR_INFO *floor;
    AI_INFO info;
    sint32 s, c, x, y, z, h, h2;
    sint16 room_number, angle, rot, roll;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    croc = (CREATURE_INFO *)item->data;
    angle = 0;
    rot = 0;
    room_number = item->room_number;
    y = item->pos.y_pos;
    s = (1024 * (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
    x = item->pos.x_pos + s;
    c = (1024 * (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
    z = item->pos.z_pos + c;
    floor = GetFloor(x, y, z, &room_number);
    h = GetHeight(floor, x, y, z);
    if (abs(y - h) > 512)
        h = y;
    x = item->pos.x_pos - s;
    z = item->pos.z_pos - c;
    room_number = item->room_number;
    floor = GetFloor(x, y, z, &room_number);
    h2 = GetHeight(floor, x, y, z);
    if (abs(y - h2) > 512)
        h2 = y;
    roll = (sint16)phd_atan(2048, h2 - h);
    if (item->hit_points <= 0) {
        item->hit_points = 0;
        if (item->current_anim_state != 7 && item->current_anim_state != 10) {
            if (room[item->room_number].flags & ROOM_UNDERWATER) {
                item->anim_number = objects[CROCODILE].anim_index + 16;
                item->frame_number = anims[item->anim_number].frame_base;
                item->current_anim_state = item->goal_anim_state = 10;
                item->hit_points = -16384;
            } else {
                item->anim_number = objects[CROCODILE].anim_index + 11;
                item->frame_number = anims[item->anim_number].frame_base;
                item->current_anim_state = item->goal_anim_state = 7;
            }
        }
        if (room[item->room_number].flags & ROOM_UNDERWATER)
            CreatureFloat(item_number);
    } else {
        if (item->ai_bits)
            GetAITarget(croc);
        else if (croc->hurt_by_lara)
            croc->enemy = lara_item;
        CreatureAIInfo(item, &info);
        GetCreatureMood(item, &info, 1);
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, croc->maximum_turn);
        if (item->hit_status || info.distance < 0x240000 || (TargetVisible(item, &info) && info.distance < 0x1900000)) {
            if (!croc->alerted)
                croc->alerted = 1;
            AlertAllGuards(item_number);
        }
        rot = angle << 2;
        switch (item->current_anim_state) {
        case 8:
            croc->maximum_turn = 546;
            croc->LOT.step = 20480;
            croc->LOT.drop = -20480;
            if (item->required_anim_state)
                item->goal_anim_state = item->required_anim_state;
            else if (info.bite && item->touch_bits & 0x300)
                item->goal_anim_state = 9;
            break;
        case 9:
            if (item->frame_number == anims[item->anim_number].frame_base)
                item->required_anim_state = 0;
            if (info.bite && item->touch_bits & 0x300) {
                if (!item->required_anim_state) {
                    CreatureEffectT(item, &croc_bite, 10, -1, DoBloodSplat);
                    lara_item->hit_points -= 120;
                    lara_item->hit_status = 1;
                    item->required_anim_state = 8;
                }
            } else {
                item->goal_anim_state = 8;
            }
            break;
        case 1:
            croc->maximum_turn = 0;
            if (item->ai_bits & GUARD) {
                rot = item->item_flags[0];
                item->goal_anim_state = 1;
                item->item_flags[0] += item->item_flags[1];
                if (!(GetRandomControl() & 0x1F)) {
                    if (GetRandomControl() & 1)
                        item->item_flags[1] = 0;
                    else
                        item->item_flags[1] = (GetRandomControl() & 1) ? 12 : -12;
                }
                if (item->item_flags[0] > 1024)
                    item->item_flags[0] = 1024;
                else if (item->item_flags[0] < -1024)
                    item->item_flags[0] = -1024;
            } else if (info.bite && info.distance < 0x90000) {
                item->goal_anim_state = 5;
            } else if (info.ahead && info.distance < 0x100000) {
                item->goal_anim_state = 3;
            } else {
                item->goal_anim_state = 2;
            }
            break;
        case 3:
            croc->maximum_turn = 546;
            croc->LOT.step = 256;
            croc->LOT.drop = -256;
            if (item->required_anim_state)
                item->goal_anim_state = item->required_anim_state;
            else if (info.bite && info.distance < 0x90000)
                item->goal_anim_state = 1;
            else if (!info.ahead || info.distance > 0x240000)
                item->goal_anim_state = 2;
            break;
        case 2:
            croc->maximum_turn = 546;
            if (item->required_anim_state)
                item->goal_anim_state = item->required_anim_state;
            else if (info.bite && info.distance < 0x90000)
                item->goal_anim_state = 1;
            else if (info.ahead && info.distance < 0x100000)
                item->goal_anim_state = 3;
            break;
        case 5:
            if (item->frame_number == anims[item->anim_number].frame_base)
                item->required_anim_state = 0;
            if (info.bite && item->touch_bits & 0x300) {
                if (!item->required_anim_state) {
                    CreatureEffectT(item, &croc_bite, 10, -1, DoBloodSplat);
                    lara_item->hit_points -= 120;
                    lara_item->hit_status = 1;
                    item->required_anim_state = 1;
                }
            } else {
                item->goal_anim_state = 1;
            }
            break;
        }
    }
    CreatureTilt(item, 0);
    CreatureJoint(item, 0, rot);
    CreatureJoint(item, 1, rot);
    CreatureJoint(item, 2, -rot);
    CreatureJoint(item, 3, -rot);
    if (item->current_anim_state < 8) {
        if (abs(roll - item->pos.x_rot) < 256)
            item->pos.x_rot = roll;
        else if (roll > item->pos.x_rot)
            item->pos.x_rot += 256;
        else if (roll < item->pos.x_rot)
            item->pos.x_rot -= 256;
    }
    CreatureAnimation(item_number, angle, 0);
    room_number = item->room_number;
    if (item->current_anim_state == 8)
        GetFloor(item->pos.x_pos + ((1024 * (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT), item->pos.y_pos, item->pos.z_pos + ((1024 * (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT), &room_number);
    else
        GetFloor(item->pos.x_pos + ((512 * (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT), item->pos.y_pos, item->pos.z_pos + ((1024 * (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT), &room_number);
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        if (room[room_number].flags & ROOM_UNDERWATER) {
            if (item->current_anim_state == 2) {
                item->goal_anim_state = item->required_anim_state = 3;
            } else if (item->current_anim_state == 3) {
                item->goal_anim_state = item->required_anim_state = 8;
            } else if (item->anim_number != objects[CROCODILE].anim_index + 17) {
                croc->LOT.step = 20480;
                croc->LOT.drop = -20480;
                croc->LOT.fly = 16;
                CreatureUnderwater(item, 256);
            }
        } else {
            item->goal_anim_state = item->required_anim_state = 3;
            croc->LOT.step = 256;
            croc->LOT.drop = -256;
            croc->LOT.fly = 0;
            CreatureUnderwater(item, 0);
        }
    } else {
        croc->LOT.fly = 0;
    }
}

sint32 GetFreeLocust(void)
{
    LOCUST_STRUCT *fx;
    sint32 free, i;

    free = next_locust;
    fx = &Locusts[free];
    for (i = 0; i < 64; i++) {
        if (!fx->On) {
            next_locust = (free + 1) & 0x3F;
            return free;
        }
        if (free == 63) {
            fx = Locusts;
            free = 0;
        } else {
            free++;
            fx++;
        }
    }
    return NO_ITEM;
}

void Func403620(void)
{
    LOCUST_STRUCT *fx;
    sint32 i;

    fx = Locusts;
    for (i = 64; i > 0; i--, fx++)
        fx->On = 0;
}

void TriggerLocust(ITEM_INFO *item)
{
    LOCUST_STRUCT *fx;
    PHD_VECTOR vec;
    PHD_VECTOR vec2;
    sint16 fx_number;
    sint16 angles[2];

    fx_number = GetFreeLocust();
    if (fx_number == NO_ITEM)
        return;
    fx = &Locusts[fx_number];
    if (item->object_number == FISH) {
        vec.x = item->pos.x_pos;
        vec.y = item->pos.y_pos;
        vec.z = item->pos.z_pos;
        angles[0] = item->pos.y_rot + 0x8000;
        angles[1] = 0;
    } else {
        vec2.x = 0;
        vec2.y = -96;
        vec2.z = 144;
        GetJointAbsPosition(item, &vec2, 9);
        vec.x = 0;
        vec.y = -128;
        vec.z = 288;
        GetJointAbsPosition(item, &vec, 9);
        phd_GetVectorAngles(vec.x - vec2.x, vec.y - vec2.y, vec.z - vec2.z, angles);
    }
    fx->room_number = item->room_number;
    fx->pos.x_pos = vec.x;
    fx->pos.y_pos = vec.y;
    fx->pos.z_pos = vec.z;
    fx->pos.y_rot = (GetRandomControl() & 0x7FF) + angles[0] - 1024;
    fx->pos.x_rot = (GetRandomControl() & 0x3FF) + angles[1] - 512;
    fx->On = 1;
    fx->flags = 0;
    fx->speed = (GetRandomControl() & 0x1F) + 16;
    fx->LaraTarget = GetRandomControl() & 0x1FF;
    fx->Counter = 20 * ((GetRandomControl() & 7) + 15);
}

void InitialiseLocustEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    switch (item->pos.y_rot) {
    case 0:
        item->pos.z_pos += 512;
        break;
    case 16384:
        item->pos.x_pos += 512;
        break;
    case -32768:
        item->pos.z_pos -= 512;
        break;
    case -16384:
        item->pos.x_pos -= 512;
        break;
    }
}

void ControlLocustEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (!item->trigger_flags) {
        KillItem(item_number);
    } else {
        TriggerLocust(item);
        item->trigger_flags--;
    }
}

void DrawLocusts(void)
{
    LOCUST_STRUCT *fx;
    sint16 **meshpp;
    sint32 i;

    fx = Locusts;
    for (i = 64; i > 0; i--, fx++) {
        if (fx->On) {
            meshpp = &meshes[objects[AHMET_MIP].mesh_index + 2 * (-GlobalCounter & 3)];
            phd_PushMatrix();
            phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
            phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
            phd_PutPolygons_train(*meshpp, 0);
            phd_PopMatrix();
        }
    }
}

void TriggerCrocgodMissile(PHD_3DPOS *pos, sint16 room_number, sint16 num)
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
        fx->counter = 15 + (num << 4);
        fx->speed = (GetRandomControl() & 0x1F) + 96;
        fx->flag1 = 6;
        fx->object_number = 384;
        fx->frame_number = objects[BUBBLES].mesh_index + 10;
    }
}

void TriggerCrocgodMissileFlame(sint16 fx_number, sint32 xv, sint32 yv, sint32 zv)
{
    SPARKS *sptr;
    sint32 dx, dz, size;

    dx = lara_item->pos.x_pos - effects[fx_number].pos.x_pos;
    dz = lara_item->pos.z_pos - effects[fx_number].pos.z_pos;
    if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
        return;
    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = (GetRandomControl() & 0x3F) + 128;
    sptr->sG = sptr->sR >> 1;
    sptr->sB = 0;
    sptr->dR = (GetRandomControl() & 0x3F) + 128;
    sptr->dG = sptr->dR >> 1;
    sptr->dB = 0;
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
    sptr->FadeToBlack = 8;
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 32;
    sptr->TransType = 2;
    sptr->Dynamic = -1;
    sptr->x = (GetRandomControl() & 0xF) - 8;
    sptr->y = 0;
    sptr->z = (GetRandomControl() & 0xF) - 8;
    sptr->x += effects[fx_number].pos.x_pos;
    sptr->y += effects[fx_number].pos.y_pos;
    sptr->z += effects[fx_number].pos.z_pos;
    sptr->Xvel = (sint16)xv;
    sptr->Yvel = (sint16)yv;
    sptr->Zvel = (sint16)zv;
    sptr->Friction = 34;
    sptr->Flags = 538;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -32 - (GetRandomControl() & 0x1F);
    else
        sptr->RotAdd = (GetRandomControl() & 0x1F) + 32;
    sptr->Gravity = 0;
    sptr->MaxYvel = 0;
    sptr->FxObj = (uint8)fx_number;
    sptr->Scalar = 2;
    size = (GetRandomControl() & 0xF) + 128;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 2;
}

void InitialiseCrocgod(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[MUTANT].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
