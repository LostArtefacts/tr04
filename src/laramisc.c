#include "laramisc.h"
#include "laraswim.h"
#include "anim.h"
#include "lara_states.h"
#include "specific/input.h"
#include "gameflow.h"
#include "objnums.h"
#include "room.h"
#include "control.h"
#include "effects.h"
#include "sound.h"
#include "rope.h"
#include "specific/3dmath.h"
#include "larasurf.h"
#include "spotcam.h"
#include "specific/audio.h"
#include "savegame.h"
#include "camera.h"
#include "collide.h"

void LaraInitialiseMeshes(void)
{
    sint32 i;

    for (i = 0; i < 15; i++) {
        meshes[objects[LARA].mesh_index + i * 2] = meshes[objects[LARA_SKIN].mesh_index + i * 2];
        lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + i * 2];
    }
    if (lara.gun_type == WEAPON_GRENADE)
        lara.back_gun = GRENADE_GUN_ANIM;
    else if (lara.shotgun_type_carried)
        lara.back_gun = SHOTGUN_ANIM;
    else if (lara.grenade_type_carried)
        lara.back_gun = GRENADE_GUN_ANIM;
    lara.gun_status = LG_NO_ARMS;
    lara.left_arm.frame_number = 0;
    lara.right_arm.frame_number = 0;
    lara.target = 0;
    lara.right_arm.lock = 0;
    lara.left_arm.lock = 0;
}

void LaraCheatGetStuff(void)
{
    lara.num_flares = -1;
    lara.num_small_medipack = -1;
    lara.num_large_medipack = -1;
    if (objects[CROWBAR_ITEM].loaded)
        lara.crowbar = 1;
    lara.uzis_type_carried = 9;
    lara.lasersight = 1;
    lara.shotgun_type_carried = 9;
    lara.sixshooter_type_carried = 9;
    lara.num_uzi_ammo = -1;
    lara.num_revolver_ammo = -1;
    lara.num_shotgun_ammo1 = -1;
}

void LaraControl(sint16 item_number)
{
    ITEM_INFO *l;
    sint32 oldx, oldy, oldz;
    sint32 wh, wd, hfw, room_water_state;

    l = lara_item;
    if (lara.IsMoving && lara.MoveCount++ > 90) {
        lara.IsMoving = 0;
        lara.gun_status = LG_NO_ARMS;
    }
    LaraCheatyBits();
    if (!bDisableLaraControl)
        lara.locationPad = -128;
    oldx = l->pos.x_pos;
    oldy = l->pos.y_pos;
    oldz = l->pos.z_pos;
    if (lara.gun_status == LG_HANDS_BUSY && lara_item->current_anim_state == AS_STOP && lara_item->goal_anim_state == AS_STOP
        && lara_item->anim_number == ANIM_BREATH && !lara_item->gravity_status)
        lara.gun_status = LG_NO_ARMS;
    if (l->current_anim_state != AS_DASH && DashTimer < 120)
        DashTimer++;
    lara.IsDucked = 0;
    room_water_state = room[l->room_number].flags & ROOM_UNDERWATER;
    wd = GetWaterDepth(l->pos.x_pos, l->pos.y_pos, l->pos.z_pos, l->room_number);
    wh = GetWaterHeight(l->pos.x_pos, l->pos.y_pos, l->pos.z_pos, l->room_number);
    if (wh != NO_HEIGHT)
        hfw = l->pos.y_pos - wh;
    else
        hfw = NO_HEIGHT;
    lara.water_surface_dist = -hfw;
    if (lara.vehicle == NO_ITEM) {
        WadeSplash(l, wh, wd);
        if (lara.vehicle == NO_ITEM) {
            switch (lara.water_status) {
            case LW_ABOVE_WATER:
                if (hfw != NO_HEIGHT && hfw >= 256) {
                    if (wd > 474) {
                        if (room_water_state) {
                            lara.air = 1800;
                            lara.water_status = LW_UNDERWATER;
                            l->gravity_status = 0;
                            l->pos.y_pos += 100;
                            UpdateLaraRoom(lara_item, 0);
                            StopSoundEffect(SFX_LARA_FALL);
                            if (l->current_anim_state == AS_SWANDIVE) {
                                l->pos.x_rot = -8190;
                                l->goal_anim_state = AS_DIVE;
                                AnimateLara(l);
                                l->fallspeed *= 2;
                            } else if (l->current_anim_state == AS_FASTDIVE) {
                                l->pos.x_rot = -15470;
                                l->goal_anim_state = AS_DIVE;
                                AnimateLara(l);
                                l->fallspeed *= 2;
                            } else {
                                l->pos.x_rot = -8190;
                                l->frame_number = anims[112].frame_base;
                                l->anim_number = 112;
                                l->current_anim_state = AS_DIVE;
                                l->goal_anim_state = AS_SWIM;
                                l->fallspeed = 3 * l->fallspeed / 2;
                            }
                            lara.torso_x_rot = lara.torso_y_rot = 0;
                            lara.head_x_rot = lara.head_y_rot = 0;
                            Splash(lara_item);
                        }
                    } else if (hfw > 256) {
                        lara.water_status = LW_WADE;
                        if (!l->gravity_status)
                            l->goal_anim_state = AS_STOP;
                    }
                } else
                    break;
            case LW_WADE:
                camera.target_elevation = -4004;
                if (hfw < 256) {
                    lara.water_status = LW_ABOVE_WATER;
                    if (l->current_anim_state == AS_WADE)
                        l->goal_anim_state = AS_RUN;
                } else if (hfw > 730) {
                    lara.water_status = LW_SURFACE;
                    l->pos.y_pos += 1 - hfw;
                    if (l->current_anim_state == AS_BACK) {
                        l->frame_number = anims[140].frame_base;
                        l->anim_number = 140;
                        l->current_anim_state = l->goal_anim_state = AS_SURFBACK;
                    } else if (l->current_anim_state == AS_STEPRIGHT) {
                        l->frame_number = anims[144].frame_base;
                        l->anim_number = 144;
                        l->current_anim_state = l->goal_anim_state = AS_SURFRIGHT;
                    } else if (l->current_anim_state == AS_STEPLEFT) {
                        l->frame_number = anims[143].frame_base;
                        l->anim_number = 143;
                        l->current_anim_state = l->goal_anim_state = AS_SURFLEFT;
                    } else {
                        l->frame_number = anims[116].frame_base;
                        l->anim_number = 116;
                        l->current_anim_state = l->goal_anim_state = AS_SURFSWIM;
                    }
                    l->gravity_status = 0;
                    l->fallspeed = 0;
                    lara.dive_count = 0;
                    l->pos.x_rot = lara_item->pos.z_rot = 0;
                    lara.torso_x_rot = lara.torso_y_rot = 0;
                    lara.head_x_rot = lara.head_y_rot = 0;
                    UpdateLaraRoom(l, 0);
                }
                break;
            case LW_SURFACE:
                if (!room_water_state) {
                    if (hfw > 256) {
                        lara.water_status = LW_WADE;
                        l->anim_number = ANIM_BREATH;
                        l->frame_number = anims[ANIM_BREATH].frame_base;
                        l->current_anim_state = AS_STOP;
                        l->goal_anim_state = AS_WADE;
                        AnimateItem(l);
                    } else {
                        lara.water_status = LW_ABOVE_WATER;
                        l->frame_number = anims[ANIM_FALLDOWN].frame_base;
                        l->anim_number = ANIM_FALLDOWN;
                        l->goal_anim_state = AS_FORWARDJUMP;
                        l->current_anim_state = AS_FORWARDJUMP;
                        l->speed = l->fallspeed / 4;
                        l->gravity_status = 1;
                    }
                    l->fallspeed = 0;
                    l->pos.x_rot = lara_item->pos.z_rot = 0;
                    lara.torso_x_rot = lara.torso_y_rot = 0;
                    lara.head_x_rot = lara.head_y_rot = 0;
                }
                break;
            case LW_UNDERWATER:
                if (!room_water_state) {
                    if (wd != NO_HEIGHT && ABS(hfw) < 256) {
                        lara.water_status = LW_SURFACE;
                        l->pos.y_pos += 1 - hfw;
                        l->frame_number = anims[114].frame_base;
                        l->anim_number = 114;
                        l->goal_anim_state = AS_SURFTREAD;
                        l->current_anim_state = AS_SURFTREAD;
                        l->fallspeed = 0;
                        lara.dive_count = 11;
                        l->pos.x_rot = lara_item->pos.z_rot = 0;
                        lara.torso_x_rot = lara.torso_y_rot = 0;
                        lara.head_x_rot = lara.head_y_rot = 0;
                        UpdateLaraRoom(l, -381);
                        SoundEffect(SFX_LARA_BREATH, &lara_item->pos, SFX_ALWAYS);
                    } else {
                        lara.water_status = LW_ABOVE_WATER;
                        l->frame_number = anims[ANIM_FALLDOWN].frame_base;
                        l->anim_number = ANIM_FALLDOWN;
                        l->goal_anim_state = AS_FORWARDJUMP;
                        l->current_anim_state = AS_FORWARDJUMP;
                        l->speed = l->fallspeed / 4;
                        l->gravity_status = 1;
                        l->fallspeed = 0;
                        l->pos.x_rot = lara_item->pos.z_rot = 0;
                        lara.torso_x_rot = lara.torso_y_rot = 0;
                        lara.head_x_rot = lara.head_y_rot = 0;
                    }
                }
                break;
            }
        }
    }
    nullsub_1(room[l->room_number].ReverbType);
    if (l->hit_points <= 0) {
        l->hit_points = -1;
        if (!lara.death_count)
            S_CDStop();
        lara.death_count++;
        if (lara_item->flags & IFL_INVISIBLE) {
            lara.death_count++;
            return;
        }
    }
    switch (lara.water_status) {
    case LW_ABOVE_WATER:
    case LW_WADE:
        if (lara.vehicle == NO_ITEM)
            lara.air = 1800;
        LaraAboveWater(l, lara_coll);
        break;
    case LW_UNDERWATER:
        if (l->hit_points >= 0) {
            lara.air--;
            if (lara.air < 0) {
                lara.air = -1;
                l->hit_points -= 5;
            }
        }
        LaraUnderWater(lara_item, lara_coll);
        break;
    case LW_SURFACE:
        if (l->hit_points >= 0) {
            lara.air += 10;
            if (lara.air > 1800)
                lara.air = 1800;
        }
        LaraSurface(l, lara_coll);
        break;
    case LW_FLYCHEAT:
        LaraCheat(l, lara_coll);
        break;
    }
    savegame.Game.Distance += phd_sqrt(SQUARE(l->pos.x_pos - oldx) + SQUARE(l->pos.y_pos - oldy) + SQUARE(l->pos.z_pos - oldz));
}

void LaraCheat(ITEM_INFO *item, COLL_INFO *coll)
{
    lara_item->hit_points = 1000;
    LaraUnderWater(item, coll);
    if ((input & IN_WALK) && !(input & IN_LOOK)) {
        lara.water_status = LW_ABOVE_WATER;
        item->frame_number = anims[ANIM_STOP].frame_base;
        item->anim_number = ANIM_STOP;
        item->pos.x_rot = item->pos.z_rot = 0;
        lara.torso_x_rot = lara.torso_y_rot = 0;
        lara.head_x_rot = lara.head_y_rot = 0;
        lara.gun_status = LG_NO_ARMS;
        LaraInitialiseMeshes();
        lara.mesh_effects = 0;
        lara_item->hit_points = cheat_hit_points;
    }
}

void LaraCheatyBits(void)
{
    if (!Gameflow->CheatEnabled)
        return;
    if (input & IN_D) {
        LaraCheatGetStuff();
        lara_item->hit_points = 1000;
    }
    if (input & IN_CHEAT) {
        lara_item->pos.y_pos -= 128;
        if (lara.water_status != LW_FLYCHEAT) {
            lara.water_status = LW_FLYCHEAT;
            lara_item->frame_number = anims[ANIM_SWIMCHEAT].frame_base;
            lara_item->anim_number = ANIM_SWIMCHEAT;
            lara_item->current_anim_state = AS_SWIM;
            lara_item->goal_anim_state = AS_SWIM;
            lara_item->gravity_status = 0;
            lara_item->pos.x_rot = 5460;
            lara_item->fallspeed = 30;
            lara.air = 1800;
            lara.death_count = 0;
            lara.torso_y_rot = 0;
            lara.torso_x_rot = 0;
            lara.head_y_rot = 0;
            lara.head_x_rot = 0;
            cheat_hit_points = lara_item->hit_points;
        }
    }
}

void AnimateLara(ITEM_INFO *item)
{
    sint32 i;
    ANIM_STRUCT *anim;
    sint16 *command;
    sint32 speed;
    uint16 type;

    item->frame_number++;
    anim = &anims[item->anim_number];
    if (anim->number_changes > 0) {
        if (GetChange(item, anim)) {
            anim = &anims[item->anim_number];
            item->current_anim_state = anim->current_anim_state;
        }
    }
    if (item->frame_number > anim->frame_end) {
        if (anim->number_commands > 0) {
            command = &commands[anim->command_index];
            for (i = anim->number_commands; i > 0; i--) {
                switch (*(command++)) {
                case 1:
                    TranslateItem(item, *command, *(command + 1), *(command + 2));
                    UpdateLaraRoom(item, -381);
                    command += 3;
                    break;
                case 2:
                    item->fallspeed = *(command++);
                    item->speed = *(command++);
                    item->gravity_status = 1;
                    if (lara.calc_fallspeed) {
                        item->fallspeed = lara.calc_fallspeed;
                        lara.calc_fallspeed = 0;
                    }
                    break;
                case 3:
                    if (lara.gun_status != LG_FLARE)
                        lara.gun_status = LG_NO_ARMS;
                    break;
                case 4:
                    break;
                case 5:
                case 6:
                    command += 2;
                    break;
                }
            }
        }
        item->anim_number = anim->jump_anim_num;
        item->frame_number = anim->jump_frame_num;
        anim = &anims[item->anim_number];
        item->current_anim_state = anim->current_anim_state;
    }
    if (anim->number_commands > 0) {
        command = &commands[anim->command_index];
        for (i = anim->number_commands; i > 0; i--) {
            switch (*(command++)) {
            case 1:
                command += 3;
                break;
            case 2:
                command += 2;
                break;
            case 3:
            case 4:
                break;
            case 5:
                if (item->frame_number == *command) {
                    type = *(command + 1) & 0xC000;
                    if (type == 0 || (type == 0x4000 && (lara.water_surface_dist >= 0 || lara.water_surface_dist == NO_HEIGHT)) ||
                        (type == 0x8000 && lara.water_surface_dist < 0 && lara.water_surface_dist != NO_HEIGHT))
                        SoundEffect(*(command + 1) & 0x3FFF, &item->pos, 2);
                }
                command += 2;
                break;
            case 6:
                if (item->frame_number == *command) {
                    FXType = *(command + 1) & 0xC000;
                    effect_routines[*(command + 1) & 0x3FFF](item);
                }
                command += 2;
                break;
            }
        }
    }
    if (!item->gravity_status) {
        speed = anim->velocity;
        if (anim->acceleration)
            speed += anim->acceleration * (item->frame_number - anim->frame_base);
        item->speed = (sint16)(speed >> 16);
    } else {
        speed = anim->velocity + anim->acceleration * (item->frame_number - anim->frame_base - 1);
        item->speed -= (sint16)(speed >> 16);
        speed += anim->acceleration;
        item->speed += (sint16)(speed >> 16);
        item->fallspeed += (item->fallspeed < FASTFALL_SPEED) ? 6 : 1;
        item->pos.y_pos += item->fallspeed;
    }
    if (lara.RopePtr != -1)
        AlignLaraToRope(item);
    if (!lara.IsMoving) {
        item->pos.x_pos += (rcossin_tbl[(lara.move_angle >> 3) & 0x1FFE] << 2) * item->speed >> W2V_SHIFT;
        item->pos.z_pos += (rcossin_tbl[((lara.move_angle >> 3) & 0x1FFE) + 1] << 2) * item->speed >> W2V_SHIFT;
    }
}

void InitialiseLaraLoad(sint16 item_number)
{
    lara.item_number = item_number;
    lara_item = &items[item_number];
}

void InitialiseLaraAnims(ITEM_INFO *item)
{
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        lara.water_status = LW_UNDERWATER;
        item->fallspeed = 0;
        item->current_anim_state = item->goal_anim_state = AS_TREAD;
        item->anim_number = ANIM_TREAD;
        item->frame_number = anims[ANIM_TREAD].frame_base;
    } else {
        lara.water_status = LW_ABOVE_WATER;
        item->current_anim_state = item->goal_anim_state = AS_STOP;
        item->anim_number = ANIM_STOP;
        item->frame_number = anims[ANIM_STOP].frame_base;
    }
}
