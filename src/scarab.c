#include "scarab.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/output.h"
#include "lara.h"
#include "effects.h"

void InitialiseScarab(sint16 item_number)
{
    ITEM_INFO *item;

    InitialiseCreature(item_number);
    item = &items[item_number];
    item->anim_number = objects[BIG_BEETLE].anim_index + 3;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}

void ScarabControl(sint16 item_number)
{
    ITEM_INFO *item;
    CREATURE_INFO *beetle;
    AI_INFO info;
    sint16 angle;

    angle = 0;
    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    beetle = (CREATURE_INFO *)item->data;
    if (item->hit_points <= 0) {
        switch (item->current_anim_state) {
        case 6:
            break;
        case 7:
            if (item->pos.y_pos >= item->floor) {
                item->pos.y_pos = item->floor;
                item->gravity_status = 0;
                item->fallspeed = 0;
                item->goal_anim_state = 8;
            }
            break;
        case 8:
            item->pos.y_pos = item->floor;
            break;
        default:
            item->anim_number = objects[BIG_BEETLE].anim_index + 5;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 6;
            item->gravity_status = 1;
            item->speed = 0;
            break;
        }
        item->pos.x_rot = 0;
    } else {
        CreatureAIInfo(item, &info);
        GetCreatureMood(item, &info, 1);
        if (beetle->flags)
            beetle->mood = ESCAPE_MOOD;
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, beetle->maximum_turn);
        if (info.distance > 0x900000 || !(GetRandomControl() & 0x7F) || item->hit_status)
            beetle->flags = 0;
        switch (item->current_anim_state) {
        case 1:
            item->pos.y_pos = item->floor;
            beetle->maximum_turn = 182;
            if (item->hit_status || info.distance < 0x900000 || beetle->hurt_by_lara || item->ai_bits == MODIFY)
                item->goal_anim_state = 2;
            break;
        case 5:
            beetle->flags = 0;
            item->pos.y_pos += 51;
            if (item->pos.y_pos > item->floor)
                item->pos.y_pos = item->floor;
            break;
        case 9:
            beetle->maximum_turn = 1274;
            if (item->required_anim_state)
                item->goal_anim_state = item->required_anim_state;
            else if (item->hit_status || GetRandomControl() < 384 || item->ai_bits == MODIFY)
                item->goal_anim_state = 3;
            else if ((beetle->mood == BORED_MOOD || GetRandomControl() < 128) && !beetle->hurt_by_lara && item->ai_bits != MODIFY)
                item->goal_anim_state = 3;
            else if (info.ahead && info.distance < 0x10000 && !beetle->flags)
                item->goal_anim_state = 4;
            break;
        case 3:
            beetle->maximum_turn = 1274;
            if (item->required_anim_state)
                item->goal_anim_state = item->required_anim_state;
            else if (info.ahead) {
                if (info.distance < 0x10000)
                    item->goal_anim_state = 9;
            }
            break;
        case 4:
            beetle->maximum_turn = 1274;
            if (info.ahead) {
                if (info.distance < 0x10000)
                    item->goal_anim_state = 4;
                else {
                    item->goal_anim_state = 9;
                    item->required_anim_state = 3;
                }
            } else if (info.distance < 0x10000)
                item->goal_anim_state = 9;
            else {
                item->goal_anim_state = 9;
                item->required_anim_state = 3;
            }
            if (!beetle->flags && item->touch_bits & 0x60) {
                lara_item->hit_points -= 50;
                lara_item->hit_status = 1;
                CreatureEffectT(item, &beetle_bite, 5, -1, DoBloodSplat);
                beetle->flags = 1;
            }
            break;
        }
    }
    CreatureTilt(item, angle * 2);
    CreatureAnimation(item_number, angle, angle);
}

sint32 GetFreeScarab(void)
{
    SCARAB_STRUCT *fx;
    sint32 free, i;

    free = next_scarab;
    fx = &Scarabs[free];
    for (i = 0; i < 128; i++) {
        if (!fx->On) {
            next_scarab = (free + 1) & 0x7F;
            return free;
        }
        if (free == 127) {
            fx = Scarabs;
            free = 0;
        } else {
            free++;
            fx++;
        }
    }
    return -1;
}

void ClearScarabs(void)
{
    SCARAB_STRUCT *fx;
    sint32 i;

    fx = Scarabs;
    for (i = 128; i > 0; i--, fx++)
        fx->On = 0;
    next_scarab = 0;
    flipeffect = -1;
}

void TriggerScarab(sint16 item_number)
{
    SCARAB_STRUCT *fx;
    ITEM_INFO *item;
    sint16 fx_num;

    item = &items[item_number];
    if (item->trigger_flags && (!item->item_flags[2] || !(GetRandomControl() & 0xF))) {
        item->trigger_flags--;
        if (item->item_flags[2] && GetRandomControl() & 1)
            item->item_flags[2]--;
        fx_num = (sint16)GetFreeScarab();
        if (fx_num != -1) {
            fx = &Scarabs[fx_num];
            fx->pos.x_pos = item->pos.x_pos;
            fx->pos.y_pos = item->pos.y_pos;
            fx->pos.z_pos = item->pos.z_pos;
            fx->room_number = item->room_number;
            if (item->item_flags[0]) {
                fx->pos.y_rot = (sint16)(GetRandomControl() << 1);
                fx->fallspeed = -16 - (GetRandomControl() & 0x1F);
            } else {
                fx->pos.y_rot = (GetRandomControl() & 0x3FFF) + item->pos.y_rot - 8192;
                fx->fallspeed = 0;
            }
            fx->pos.x_rot = 0;
            fx->pos.z_rot = 0;
            fx->On = 1;
            fx->flags = 0;
            fx->speed = (GetRandomControl() & 0x1F) + 1;
        }
    }
}

void DrawScarabs(void)
{
    SCARAB_STRUCT *fx;
    sint16 **meshpp;
    sint32 i;

    meshpp = &meshes[objects[LITTLE_BEETLE].mesh_index + (wibble >> 2 & 2)];
    fx = Scarabs;
    for (i = 128; i > 0; i--, fx++) {
        if (fx->On) {
            phd_PushMatrix();
            phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
            phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
            phd_PutPolygons_train(*meshpp, 0);
            phd_PopMatrix();
        }
    }
}

void InitialiseScarabGenerator(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 flag, rot;

    item = &items[item_number];
    flag = item->trigger_flags / 1000;
    rot = item->pos.y_rot;
    item->pos.x_rot = 0x2000;
    item->item_flags[0] = flag & 1;
    item->item_flags[1] = flag & 2;
    item->item_flags[2] = flag & 4;
    item->trigger_flags %= 1000;
    if (!item->item_flags[0]) {
        if (rot > 4096 && rot < 28672)
            item->pos.x_pos -= 512;
        else if (rot < -4096 && rot > -28672)
            item->pos.x_pos += 512;
        if (rot > -8192 && rot < 8192)
            item->pos.z_pos -= 512;
        else if (rot < -20480 || rot > 20480)
            item->pos.z_pos += 512;
    }
}
