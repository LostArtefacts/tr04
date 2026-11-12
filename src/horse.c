#include "horse.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "sound.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseHorseman(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[HORSEMAN].anim_index + 8;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 9;
    item->item_flags[0] = -1;
}

void HorsemanControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    CREATURE_INFO *horseman;
    FLOOR_INFO *floor;
    AI_INFO info, larainfo;
    PHD_VECTOR v;
    sint32 x, y, z, c, s, h1, h2;
    sint16 room_number, angle, rot, i;

    room_number = 0;
    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    if (item->item_flags[0] == -1) {
        item2 = items;
        for (i = 0; i < level_items; i++, item2++) {
            if (item2->object_number == HORSE && item->trigger_flags == item2->trigger_flags) {
                item->item_flags[0] = i;
                items[i].flags |= IFL_TRIGGERED;
            }
        }
        if (item->item_flags[0] == -1)
            item->item_flags[0] = 0;
    }
    if (item->item_flags[0])
        item2 = &items[item->item_flags[0]];
    else
        item2 = 0;
    horseman = (CREATURE_INFO *)item->data;
    angle = 0;
    if (item2) {
        room_number = item->room_number;
        s = (341 * (rcossin_tbl[(item2->pos.y_rot >> 3) & 0x1FFE] << 2)) >> W2V_SHIFT;
        c = (341 * (rcossin_tbl[((item2->pos.y_rot >> 3) & 0x1FFE) + 1] << 2)) >> W2V_SHIFT;
        x = item2->pos.x_pos + s;
        y = item2->pos.y_pos;
        z = item2->pos.z_pos + c;
        floor = GetFloor(x, y, z, &room_number);
        h1 = GetHeight(floor, x, y, z);
        x = item2->pos.x_pos - s;
        y = item2->pos.y_pos;
        z = item2->pos.z_pos - c;
        floor = GetFloor(x, y, z, &room_number);
        h2 = GetHeight(floor, x, y, z);
        rot = (sint16)phd_atan(682, h2 - h1);
    }
    if (item->hit_points <= 0) {
        item->hit_points = 0;
        if (!item->item_flags[1]) {
            if (item->current_anim_state != 16) {
                item->anim_number = objects[HORSEMAN].anim_index + 21;
                item->frame_number = anims[item->anim_number].frame_base;
                item->current_anim_state = 16;
                if (item->item_flags[0])
                    items[item->item_flags[0]].after_death = 1;
            }
        } else {
            item->hit_points = 100;
            item->ai_bits = 0;
            item->item_flags[1] = 0;
            horseman->enemy = 0;
            item->anim_number = objects[HORSEMAN].anim_index + 3;
            item->frame_number = anims[item->anim_number].frame_base;
            item->current_anim_state = 8;
            item2->goal_anim_state = 1;
        }
    } else {
        if (item->ai_bits)
            GetAITarget(horseman);
        else if (horseman->hurt_by_lara)
            horseman->enemy = lara_item;
        CreatureAIInfo(item, &info);
        if (horseman->enemy == lara_item) {
            larainfo.angle = info.angle;
            larainfo.distance = info.distance;
        } else {
            z = horseman->enemy->pos.z_pos - item->pos.z_pos;
            x = horseman->enemy->pos.x_pos - item->pos.x_pos;
            larainfo.angle = (sint16)(phd_atan(z, x) - item->pos.y_rot);
            larainfo.distance = x * x + z * z;
        }
        if (item->hit_status) {
            item->hit_status = 0;
            if (larainfo.angle < 0x3000 && larainfo.angle > -0x3000 && larainfo.distance < 0x400000) {
                if (item->current_anim_state != 15 && (larainfo.angle > 0 || (!item->item_flags[1] && !(item->mesh_bits & 0x400)))) {
                    if (lara.gun_type == WEAPON_SHOTGUN) {
                        item->hit_points -= 10;
                        item->hit_status = 1;
                    } else if (lara.gun_type == WEAPON_REVOLVER) {
                        item->hit_points -= 20;
                        item->hit_status = 1;
                    } else {
                        item->hit_points--;
                    }
                    SoundEffect(SFX_HORSEMAN_TAKEHIT, &item->pos, SFX_DEFAULT);
                    SoundEffect(SFX_HORSE_RICOCHETS, &item->pos, SFX_DEFAULT);
                    v.x = 0;
                    v.y = -128;
                    v.z = 80;
                    GetJointAbsPosition(item, &v, 1);
                    TriggerHorsemanRicochets(&v, item->pos.y_rot, 7);
                } else if ((item->current_anim_state == 15 || larainfo.angle > 0 || !item->item_flags[1]) && !(GetRandomControl() & 7)) {
                    if (item->current_anim_state == 15)
                        item->goal_anim_state = 9;
                    ExplodeItemNode(item, 10, 1, -24);
                }
                if (!item->item_flags[1] && item->mesh_bits & 0x400)
                    item->required_anim_state = 15;
            }
        }
        horseman->hurt_by_lara = 0;
        GetCreatureMood(item, &info, 1);
        CreatureMood(item, &info, 1);
        angle = CreatureTurn(item, horseman->maximum_turn);
        switch (item->current_anim_state) {
        case 3:
            horseman->maximum_turn = 0;
            item2->goal_anim_state = 1;
            if (horseman->flags) {
                item->ai_bits = FOLLOW;
                if (item->item_flags[3] == 1)
                    item->item_flags[3] = 2;
                else
                    item->item_flags[3] = 1;
            } else {
                horseman->flags = 0;
            }
            if (item->required_anim_state) {
                item->goal_anim_state = 1;
                item2->goal_anim_state = 2;
                item2->flags = 0;
            } else if (!horseman->reached_goal && (item2->flags || info.distance >= 0x100000 || !info.bite || info.angle >= 1820 || info.angle <= -1820)) {
                item->goal_anim_state = 1;
                item2->goal_anim_state = 2;
                item2->flags = 0;
            } else {
                item->goal_anim_state = 4;
                if (horseman->reached_goal)
                    item->required_anim_state = 17;
                item2->flags = 0;
            }
            break;
        case 1:
            horseman->maximum_turn = 546;
            item2->goal_anim_state = 2;
            if (item->required_anim_state) {
                item->goal_anim_state = 17;
                item2->goal_anim_state = 5;
            } else if (horseman->flags || horseman->reached_goal || (item->hit_status && !GetRandomControl)) {
                if (larainfo.distance <= 0x1000000 && !horseman->reached_goal) {
                    item->ai_bits = FOLLOW;
                    if (item->item_flags[3] == 1)
                        item->item_flags[3] = 2;
                    else
                        item->item_flags[3] = 1;
                } else {
                    horseman->flags = 0;
                    horseman->enemy = lara_item;
                    if (larainfo.angle > -0x2000 && larainfo.angle < 0x2000) {
                        item->goal_anim_state = 3;
                        item2->goal_anim_state = 1;
                    }
                }
            }
            if (info.distance < 0x100000 && info.bite && info.angle < 1820 && info.angle > -1820) {
                item->goal_anim_state = 3;
                item2->goal_anim_state = 1;
            } else if (info.bite && info.angle < -1820 && (info.distance < 0x100000 || (info.distance < 0x1C6E39 && info.angle > -3640))) {
                horseman->maximum_turn = 0;
                item->goal_anim_state = 7;
            } else if (info.bite && info.angle > 1820 && (info.distance < 0x100000 || (info.distance < 0x1C6E39 && info.angle < 3640))) {
                horseman->maximum_turn = 0;
                item->goal_anim_state = 6;
            }
            break;
        case 2:
            horseman->maximum_turn = 273;
            if (larainfo.distance > 0x1000000 || horseman->reached_goal || horseman->enemy == lara_item) {
                horseman->reached_goal = 0;
                horseman->flags = 0;
                item->goal_anim_state = 1;
                item2->goal_anim_state = 2;
                horseman->enemy = lara_item;
            }
            break;
        case 4:
            horseman->maximum_turn = 0;
            if (item->frame_number == anims[item->anim_number].frame_base) {
                item2->anim_number = objects[HORSE].anim_index + 1;
                item2->frame_number = anims[item->anim_number].frame_base;
                item2->current_anim_state = 4;
            }
            if (!item2->flags && item2->touch_bits & 0x22000) {
                lara_item->hit_points -= 150;
                lara_item->hit_status = 1;
                if (item2->touch_bits & 0x2000) {
                    CreatureEffectT(item2, &horse_lfoot, 10, -1, DoBloodSplat);
                    item2->flags = 1;
                } else {
                    CreatureEffectT(item2, &horse_rfoot, 10, -1, DoBloodSplat);
                    item2->flags = 1;
                }
            }
            break;
        case 17:
            horseman->reached_goal = 0;
            horseman->maximum_turn = 546;
            if (!item2->flags && item2->touch_bits & 0xA2000) {
                lara_item->hit_points -= 150;
                lara_item->hit_status = 1;
                if (item2->touch_bits & 0x2000)
                    CreatureEffectT(item2, &horse_lfoot, 10, -1, DoBloodSplat);
                if (item2->touch_bits & 0x20000)
                    CreatureEffectT(item2, &horse_rfoot, 10, -1, DoBloodSplat);
                if (item2->touch_bits & 0x80000)
                    CreatureEffectT(item2, &horse_head, 10, -1, DoBloodSplat);
                item2->flags = 1;
            }
            if (!horseman->flags && item->touch_bits & 0x460) {
                lara_item->hit_status = 1;
                if (item->touch_bits & 0x60) {
                    CreatureEffectT(item2, &horseman_axe, 20, -1, DoBloodSplat);
                    lara_item->hit_points -= 250;
                } else if (item->touch_bits & 0x400) {
                    CreatureEffectT(item2, &horseman_shield, 10, -1, DoBloodSplat);
                    lara_item->hit_points -= 150;
                }
                horseman->flags = 1;
            }
            if (item->anim_number == objects[HORSEMAN].anim_index + 29 && item->frame_number == anims[item->anim_number].frame_base) {
                item2->anim_number = objects[HORSE].anim_index + 10;
                item2->frame_number = anims[item2->anim_number].frame_base;
            }
            if (larainfo.distance <= 0x1000000 && !horseman->reached_goal) {
                if (!info.ahead) {
                    item->goal_anim_state = 3;
                    item2->goal_anim_state = 1;
                }
            } else {
                horseman->reached_goal = 0;
                horseman->flags = 0;
                horseman->enemy = lara_item;
            }
            break;
        case 9:
            horseman->maximum_turn = 0;
            horseman->flags = 0;
            if (item->ai_bits && !item->item_flags[3])
                item->goal_anim_state = 10;
            else if (item->required_anim_state)
                item->goal_anim_state = item->required_anim_state;
            else if (info.bite && info.distance < 0x718E4)
                item->goal_anim_state = 14;
            else if (info.distance < 0x2400000 && info.distance > 0x718E4)
                item->goal_anim_state = 10;
            break;
        case 10:
            horseman->maximum_turn = 546;
            horseman->flags = 0;
            if (horseman->reached_goal) {
                room_number = item->room_number;
                item->item_flags[1] = 1;
                item->ai_bits = 0;
                item->pos = item2->pos;
                horseman->reached_goal = 0;
                horseman->enemy = 0;
                item->anim_number = objects[HORSEMAN].anim_index + 14;
                item->frame_number = anims[item->anim_number].frame_base;
                item->current_anim_state = 5;
                horseman->maximum_turn = 0;
            } else if (item->hit_status) {
                item->goal_anim_state = 9;
            } else if (info.bite && info.distance < 0x718E4) {
                if (GetRandomControl() & 1)
                    item->goal_anim_state = 12;
                else if (GetRandomControl() & 1)
                    item->goal_anim_state = 13;
                else
                    item->goal_anim_state = 9;
            } else if (info.distance < 0x1900000 && info.distance > 0x1C6E39) {
                item->goal_anim_state = 11;
            }
            break;
        case 11:
            if (info.distance < 0x1C6E39)
                item->goal_anim_state = 10;
            break;
        case 15:
            if (lara.target != item || (info.bite && info.distance < 0x718E4))
                item->goal_anim_state = 9;
            break;
        case 12:
        case 13:
        case 14:
            horseman->maximum_turn = 0;
            if (abs(info.angle) < 546)
                item->pos.y_rot += info.angle;
            else if (info.angle < 0)
                item->pos.y_rot -= 546;
            else
                item->pos.y_rot += 546;
        case 6:
            if (!horseman->flags && item->touch_bits & 0x60) {
                lara_item->hit_points -= 250;
                lara_item->hit_status = 1;
                CreatureEffectT(item, &horseman_axe, 10, item->pos.y_rot, DoBloodSplat);
                horseman->flags = 1;
            }
            if (item->hit_status)
                item->goal_anim_state = 9;
            break;
        case 7:
            if (!horseman->flags && item->touch_bits & 0x4000) {
                lara_item->hit_points -= 100;
                lara_item->hit_status = 1;
                CreatureEffectT(item, &horseman_lfoot, 3, item->pos.y_rot, DoBloodSplat);
                horseman->flags = 1;
            }
            break;
        }
    }
    if (item2 && item->item_flags[1]) {
        if (abs(rot - item->pos.x_rot) < 256)
            item->pos.x_rot = rot;
        else if (rot > item->pos.x_rot)
            item->pos.x_rot += 256;
        else if (rot < item->pos.x_rot)
            item->pos.x_rot -= 256;
        item2->pos = item->pos;
        if (item2->room_number != item->room_number)
            ItemNewRoom(item->item_flags[0], item->room_number);
        AnimateItem(item2);
    }
    if (item->item_flags[1])
        objects[HORSEMAN].radius = 409;
    else
        objects[HORSEMAN].radius = 170;
    CreatureAnimation(item_number, angle, 0);
}

void TriggerHorsemanRicochets(PHD_VECTOR *pos, sint32 yrot, sint32 num)
{
    SPARKS *sptr;
    sint32 random, rot, i;

    for (i = num; i > 0; i--) {
        sptr = &spark[GetFreeSpark()];
        random = GetRandomControl();
        sptr->On = 1;
        sptr->sG = 128;
        sptr->sB = (random & 0xF) + 16;
        sptr->sR = 0;
        sptr->dG = 96;
        sptr->dB = (random >> 4 & 0x1F) + 48;
        sptr->dR = 0;
        sptr->ColFadeSpeed = 2;
        sptr->FadeToBlack = 4;
        sptr->Life = 9;
        sptr->sLife = 9;
        sptr->TransType = 2;
        sptr->x = pos->x;
        sptr->y = pos->y;
        sptr->z = pos->z;
        sptr->Yvel = (random & 0xFFF) - 2048;
        sptr->Gravity = random >> 7 & 0x1F;
        rot = (random >> 3 & 0x7FF) + yrot - 1024 & 0xFFF;
        sptr->Zvel = rcossin_tbl[(rot << 1) + 1] >> 2;
        sptr->Xvel = -rcossin_tbl[rot << 1] >> 2;
        sptr->Friction = 34;
        sptr->Flags = 0;
        sptr->MaxYvel = 0;
    }
    for (i = num; i > 0; i--) {
        sptr = &spark[GetFreeSpark()];
        random = GetRandomControl();
        sptr->On = 1;
        sptr->sG = 128;
        sptr->sR = 0;
        sptr->dG = 96;
        sptr->sB = (random & 0xF) + 16;
        sptr->dR = 0;
        sptr->dB = (random >> 4 & 0x1F) + 48;
        sptr->ColFadeSpeed = 2;
        sptr->FadeToBlack = 4;
        sptr->Life = 9;
        sptr->sLife = 9;
        sptr->TransType = 2;
        sptr->x = pos->x;
        sptr->y = pos->y;
        sptr->z = pos->z;
        sptr->Yvel = (random & 0xFFF) - 2048;
        sptr->Gravity = random >> 7 & 0x1F;
        sptr->RotAng = (sint16)(random >> 3);
        if (random & 1)
            sptr->RotAdd = 240 - (random & 0xF);
        else
            sptr->RotAdd = (random & 0xF) + 16;
        sptr->Scalar = 3;
        sptr->sSize = (random >> 5 & 0x7) + 4;
        sptr->Size = sptr->sSize;
        sptr->dSize = sptr->sSize >> 1;
        rot = (random >> 3 & 0x7FF) + yrot - 1024 & 0xFFF;
        sptr->Zvel = rcossin_tbl[(rot << 1) + 1] >> 2;
        sptr->Xvel = -rcossin_tbl[rot << 1] >> 2;
        sptr->Friction = 34;
        sptr->Flags = 26;
        sptr->MaxYvel = 0;
    }
}

void InitialiseHorse(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->anim_number = objects[HORSE].anim_index + 2;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
