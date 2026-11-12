#include <string.h>

#include "control.h"
#include "objnums.h"
#include "box.h"
#include "anim.h"
#include "camera.h"
#include "gameflow.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "sound.h"
#include "sphere.h"
#include "debris.h"
#include "effects.h"
#include "lara.h"
#include "switch.h"
#include "larafire.h"
#include "effect2.h"
#include "draw.h"
#include "pickup.h"
#include "traps.h"
#include "spotcam.h"
#include "deltapak.h"
#include "savegame.h"
#include "lot.h"
#include "specific/fmv.h"
#include "lara_states.h"
#include "specific/input.h"
#include "specific/loadsave.h"
#include "newinv.h"
#include "tomb4fx.h"
#include "laramisc.h"
#include "hair.h"
#include "voncroy.h"
#include "objects.h"
#include "text.h"
#include "health.h"
#include "scarab.h"
#include "croc.h"

extern uint8 HeavyTriggered;
extern sint32 framecount;
extern uint8 InItemControlLoop;
extern sint16 cdtrack;
extern volatile sint32 thread_ended;

enum trigger_types {
    TRIGGER,
    PAD,
    SWITCH,
    KEY,
    PICKUP,
    HEAVY,
    ANTIPAD,
    COMBAT,
    DUMMY,
    ANTITRIGGER,
    HEAVYSWITCH,
    HEAVYANTITRIGGER,
    MONKEY
};

void KillMoveItems(void)
{
    sint16 i;

    for (i = 0; i < ItemNewRoomNo; i++) {
        if (ItemNewRooms[i][0] & 0x8000)
            KillItem(ItemNewRooms[i][0] & 0x7FFF);
        else
            ItemNewRoom(ItemNewRooms[i][0], ItemNewRooms[i][1]);
    }
    ItemNewRoomNo = 0;
}

void KillMoveEffects(void)
{
    sint16 i;

    for (i = 0; i < ItemNewRoomNo; i++) {
        if (ItemNewRooms[i][0] & 0x8000)
            KillEffect(ItemNewRooms[i][0] & 0x7FFF);
        else
            EffectNewRoom(ItemNewRooms[i][0], ItemNewRooms[i][1]);
    }
    ItemNewRoomNo = 0;
}

void UpdateSky(void)
{
    if (gfLevelFlags & GF_LAYER1) {
        SkyPos += gfLayer1Vel;
        if (SkyPos > 0x2C00)
            SkyPos -= 0x2C00;
        else if (SkyPos < 0)
            SkyPos += 0x2C00;
    }
    if (gfLevelFlags & GF_LAYER2) {
        SkyPos2 += gfLayer2Vel;
        if (SkyPos2 > 0x2C00)
            SkyPos2 -= 0x2C00;
        else if (SkyPos2 < 0)
            SkyPos2 += 0x2C00;
    }
}

sint32 ControlPhase(sint32 nframes, sint32 demo_mode)
{
    ITEM_INFO *item;
    FX_INFO *fx;
    FLOOR_INFO *floor;

    RegeneratePickups();
    if (nframes > 10)
        nframes = 10;
    if (bTrackCamInit)
        bUseSpotCam = 0;
    SetDebounce = 1;
    for (framecount += nframes; framecount > 0; framecount -= 2) {
        sint16 item_num, nex;

        GlobalCounter++;
        UpdateSky();
        if (cdtrack > 0)
            nullsub_1();
        if (S_UpdateInput() == IN_ALL)
            return 0;
        if (bDisableLaraControl) {
            if (gfCurrentLevel)
                dbinput = 0;
            input &= IN_LOOK;
        }
        if (cutseq_trig) {
            input = 0;
            if (cutseq_num == 27)
                input = IN_ACTION;
        }
        SetDebounce = 0;
        if (gfCurrentLevel && (dbinput & IN_OPTION || GLOBAL_enterinventory != -1) && !cutseq_trig && lara_item->hit_points > 0) {
            if (S_CallInventory2())
                return 2;
        }
        if (gfLevelComplete)
            return 3;
        if (reset_flag || lara.death_count > 300 || lara.death_count > 60 && input) {
            if (Gameflow->DemoDisc && reset_flag) {
                reset_flag = 0;
                return 4;
            }
            reset_flag = 0;
            return 1;
        }
        if (demo_mode && input == IN_ALL)
            input = 0;
        if (!FadeScreenHeight) {
            if (input & IN_SAVE)
                S_LoadSave(IN_SAVE, 0);
            else if (input & IN_LOAD) {
                if (S_LoadSave(IN_LOAD, 0) >= 0)
                    return 2;
            }
            if (input & IN_PAUSE && gfGameMode == 0) {
                if (S_PauseMenu() == 8)
                    return 1;
            }
        }
        if (thread_ended)
            return 4;
        if (input & IN_LOOK && (lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH ||
            (lara.IsDucked && !(input & IN_DUCK) && lara_item->anim_number == ANIM_DUCKBREATHE && lara_item->goal_anim_state == AS_DUCK))) {
            if (!BinocularRange) {
                switch (lara.gun_type) {
                case WEAPON_REVOLVER:
                    if (lara.sixshooter_type_carried & 4 && lara.gun_status == LG_READY) {
                        BinocularRange = 128;
                        BinocularOldCamera = camera.old_type;
                        lara.Busy = 1;
                        LaserSight = 1;
                    }
                    break;
                case WEAPON_CROSSBOW:
                    if (lara.crossbow_type_carried & 4 && lara.gun_status == LG_READY) {
                        BinocularRange = 128;
                        BinocularOldCamera = camera.old_type;
                        lara.Busy = 1;
                        LaserSight = 1;
                    }
                    break;
                }
            }
        } else if (BinocularRange) {
            if (LaserSight) {
                BinocularRange = 0;
                LaserSight = 0;
                AlterFOV(14560);
                lara_item->mesh_bits = -1;
                lara.Busy = 0;
                camera.type = BinocularOldCamera;
                lara.head_y_rot = 0;
                lara.head_x_rot = 0;
                lara.torso_y_rot = 0;
                lara.torso_x_rot = 0;
                BinocularOn = -8;
            } else
                input |= IN_LOOK;
        }
        ClearDynamics();
        ClearFires();
        GotLaraSpheres = 0;
        InItemControlLoop = 1;
        item_num = next_item_active;
        while (item_num != -1) {
            item = &items[item_num];
            nex = item->next_active;
            if (item->after_death >= 128)
                KillItem(item_num);
            else if (objects[item->object_number].control)
                objects[item->object_number].control(item_num);
            item_num = nex;
        }
        InItemControlLoop = 0;
        KillMoveItems();
        InItemControlLoop = 1;
        item_num = next_fx_active;
        while (item_num != -1) {
            fx = &effects[item_num];
            nex = fx->next_active;
            if (objects[fx->object_number].control)
                objects[fx->object_number].control(item_num);
            item_num = nex;
        }
        InItemControlLoop = 0;
        KillMoveEffects();
        if (KillEverythingFlag)
            KillEverything();
        if (SmokeCountL)
            SmokeCountL--;
        if (SmokeCountR)
            SmokeCountR--;
        if (SplashCount)
            SplashCount--;
        if (WeaponDelay)
            WeaponDelay--;
        if (lara.blindTimer)
            lara.blindTimer--;
        if (lara.has_fired && !(wibble & 0x7F)) {
            AlertNearbyGuards(lara_item);
            lara.has_fired = 0;
        }
        *(sint16 *)&XSoff1 += 150;
        *(sint16 *)&YSoff1 += 230;
        *(sint16 *)&ZSoff1 += 660;
        *(sint16 *)&XSoff2 += 270;
        *(sint16 *)&YSoff2 += 440;
        *(sint16 *)&ZSoff2 += 160;
        if (lara.poisoned && !GLOBAL_playing_cutseq) {
            if (lara.poisoned > 4096)
                lara.poisoned = 4096;
            else if (lara.dpoisoned)
                lara.dpoisoned++;
            if (lara.poisoned >= 256 && !(wibble & 0xFF)) {
                lara_item->hit_points -= lara.poisoned >> 8;
                PoisonFlag = 16;
            }
        }
        InItemControlLoop = 1;
        if (!GLOBAL_playing_cutseq && !gfGameMode)
            LaraControl(0);
        InItemControlLoop = 0;
        while (SmashedMeshCount) {
            SmashedMeshCount--;
            floor = GetFloor(SmashedMesh[SmashedMeshCount]->x, SmashedMesh[SmashedMeshCount]->y, SmashedMesh[SmashedMeshCount]->z, &SmashedMeshRoom[SmashedMeshCount]);
            GetHeight(floor, SmashedMesh[SmashedMeshCount]->x, SmashedMesh[SmashedMeshCount]->y, SmashedMesh[SmashedMeshCount]->z);
            TestTriggers(trigger_index, 1, 0);
            floor->stopper = 0;
            SmashedMesh[SmashedMeshCount] = 0;
        }
        KillMoveItems();
        if (GLOBAL_inventoryitemchosen != -1) {
            SayNo();
            GLOBAL_inventoryitemchosen = -1;
        }
        if (!GLOBAL_playing_cutseq) {
            HairControl(0, 0, 0);
            if (gfLevelFlags & GF_YOUNGLARA)
                HairControl(0, 1, 0);
        }
        if (!GLOBAL_playing_cutseq) {
            if (bUseSpotCam)
                CalculateSpotCams();
            else if (!bVoncroyCutScene)
                CalculateCamera();
        } else {
            camera.type = CINEMATIC_CAMERA;
            CalculateCamera();
        }
        CamRot.y = (mGetAngle(camera.pos.z, camera.pos.x, camera.target.z, camera.target.x) >> 4) & 0xFFF;
        wibble = (wibble + 4) & 0xFC;
        TriggerLaraDrips();
        UpdateSparks();
        UpdateFireSparks();
        UpdateSmokeSparks();
        UpdateBubbles();
        UpdateSplashes();
        UpdateDebris();
        UpdateBlood();
        UpdateDrips();
        UpdateGunShells();
        UpdateScarabs();
        UpdateLocusts();
        UpdateShockwaves();
        UpdateLightning();
        AnimateWaterfalls();
        UpdatePulseColour();
        SoundEffects();
        health_bar_timer--;
        if (!gfGameMode) {
            GameTimer++;
            if (savegame.Level.Timer) {
                if (!GLOBAL_playing_cutseq)
                    savegame.Level.Timer++;
            }
        }
        UpdateFadeClip();
    }
    return 0;
}

void AnimateItem(ITEM_INFO *item)
{
    ANIM_STRUCT *anim;
    sint16 *cmd;
    sint32 speed, speed2, i;
    uint16 type, num;

    item->touch_bits = 0;
    item->hit_status = 0;
    anim = &anims[item->anim_number];
    item->frame_number++;
    if (anim->number_changes > 0) {
        if (GetChange(item, anim)) {
            anim = &anims[item->anim_number];
            item->current_anim_state = anim->current_anim_state;
            if (item->required_anim_state == item->current_anim_state)
                item->required_anim_state = 0;
        }
    }
    if (item->frame_number > anim->frame_end) {
        if (anim->number_commands > 0) {
            cmd = &commands[anim->command_index];
            for (i = anim->number_commands; i > 0; i--) {
                switch (*cmd++) {
                case ACMD_SETPOS:
                    TranslateItem(item, cmd[0], cmd[1], cmd[2]);
                    cmd += 3;
                    break;
                case ACMD_JUMPVEL:
                    item->fallspeed = *cmd++;
                    item->speed = *cmd++;
                    item->gravity_status = 1;
                    break;
                case ACMD_KILL:
                    if (objects[item->object_number].intelligent && !item->after_death)
                        item->after_death = 1;
                    item->status = ITEM_DEACTIVATED;
                    break;
                case ACMD_PLAYSFX:
                case ACMD_FLIPEFFECT:
                    cmd += 2;
                    break;
                }
            }
        }
        item->anim_number = anim->jump_anim_num;
        item->frame_number = anim->jump_frame_num;
        anim = &anims[item->anim_number];
        if (item->current_anim_state != anim->current_anim_state) {
            item->goal_anim_state = item->current_anim_state = anim->current_anim_state;
        }
        if (item->required_anim_state == item->current_anim_state)
            item->required_anim_state = 0;
    }
    if (anim->number_commands > 0) {
        cmd = &commands[anim->command_index];
        for (i = anim->number_commands; i > 0; i--) {
            switch (*cmd++) {
            case ACMD_SETPOS:
                cmd += 3;
                break;
            case ACMD_PLAYSFX:
                if (item->frame_number == *cmd) {
                    type = cmd[1] & 0xC000;
                    num = cmd[1] & 0x3FFF;
                    if (objects[item->object_number].water_creature) {
                        if (room[item->room_number].flags & ROOM_UNDERWATER)
                            SoundEffect(num, &item->pos, SFX_WATER);
                        else
                            SoundEffect(num, &item->pos, SFX_DEFAULT);
                    } else if (item->room_number == 255) {
                        item->pos.x_pos = lara_item->pos.x_pos;
                        item->pos.y_pos = lara_item->pos.y_pos - 762;
                        item->pos.z_pos = lara_item->pos.z_pos;
                        SoundEffect(num, &item->pos, SFX_DEFAULT);
                    } else if (room[item->room_number].flags & ROOM_UNDERWATER) {
                        if (type == SFX_LANDANDWATER || type == SFX_WATERONLY)
                            SoundEffect(num, &item->pos, SFX_DEFAULT);
                    } else if (type == SFX_LANDANDWATER || type == SFX_LANDONLY)
                        SoundEffect(num, &item->pos, SFX_DEFAULT);
                }
                cmd += 2;
                break;
            case ACMD_FLIPEFFECT:
                if (item->frame_number == *cmd) {
                    FXType = cmd[1] & 0xC000;
                    effect_routines[cmd[1] & 0x3FFF](item);
                }
                cmd += 2;
                break;
            case ACMD_JUMPVEL:
                cmd += 2;
                break;
            }
        }
    }
    speed2 = 0;
    if (!item->gravity_status) {
        speed = anim->velocity;
        if (anim->acceleration)
            speed += anim->acceleration * (item->frame_number - anim->frame_base);
        item->speed = speed >> 16;
        speed2 = anim->Xvelocity;
        if (anim->Xacceleration)
            speed2 += anim->Xacceleration * (item->frame_number - anim->frame_base);
        speed2 >>= 16;
    } else {
        item->fallspeed += item->fallspeed < 128 ? 6 : 1;
        item->pos.y_pos += item->fallspeed;
    }
    item->pos.x_pos += ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * item->speed) >> W2V_SHIFT;
    item->pos.z_pos += ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * item->speed) >> W2V_SHIFT;
    item->pos.x_pos += ((rcossin_tbl[((item->pos.y_rot + 0x4000) >> 3) & 0x1FFE] << 2) * speed2) >> W2V_SHIFT;
    item->pos.z_pos += ((rcossin_tbl[(((item->pos.y_rot + 0x4000) >> 3) & 0x1FFE) + 1] << 2) * speed2) >> W2V_SHIFT;
}

sint32 GetChange(ITEM_INFO *item, ANIM_STRUCT *anim)
{
    sint32 i, j;
    CHANGE_STRUCT *change;
    RANGE_STRUCT *range;

    if (item->current_anim_state == item->goal_anim_state)
        return 0;
    change = &changes[anim->change_index];
    for (i = 0; i < anim->number_changes; i++, change++) {
        if (change->goal_anim_state == item->goal_anim_state) {
            range = &ranges[change->range_index];
            for (j = 0; j < change->number_ranges; j++, range++) {
                if (item->frame_number >= range->start_frame && item->frame_number <= range->end_frame) {
                    item->anim_number = range->link_anim_num;
                    item->frame_number = range->link_frame_num;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void TranslateItem(ITEM_INFO *item, sint32 x, sint32 y, sint32 z)
{
    sint32 s, c;

    c = phd_cos(item->pos.y_rot);
    s = phd_sin(item->pos.y_rot);
    item->pos.x_pos += (z * s + x * c) >> W2V_SHIFT;
    item->pos.y_pos += y;
    item->pos.z_pos += (z * c - x * s) >> W2V_SHIFT;
}

void AlterFloorHeight(ITEM_INFO *item, sint32 height)
{
    FLOOR_INFO *floor, *ceiling;
    sint16 room_num;

    room_num = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
    ceiling = GetFloor(item->pos.x_pos, item->pos.y_pos + height - WALL_L, item->pos.z_pos, &room_num);
    if (floor->floor == NO_HEIGHT / 256) {
        floor->floor = ceiling->ceiling + height / 256;
    } else {
        floor->floor = height / 256 + floor->floor;
        if (floor->floor == ceiling->ceiling)
            floor->floor = NO_HEIGHT / 256;
    }
    if (boxes[floor->box].overlap_index & BLOCKABLE) {
        if (height < 0)
            boxes[floor->box].overlap_index |= BLOCKED;
        else
            boxes[floor->box].overlap_index &= ~BLOCKED;
    }
}

FLOOR_INFO *GetFloor(sint32 x, sint32 y, sint32 z, sint16 *room_number)
{
    sint16 data;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    sint32 x_floor, y_floor, retval;

    r = &room[*room_number];
    do {
        x_floor = (z - r->z) >> WALL_SHIFT;
        y_floor = (x - r->x) >> WALL_SHIFT;
        if (x_floor <= 0) {
            x_floor = 0;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (x_floor >= r->x_size - 1) {
            x_floor = r->x_size - 1;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (y_floor < 0)
            y_floor = 0;
        else if (y_floor >= r->y_size)
            y_floor = r->y_size - 1;
        floor = &r->floor[x_floor + y_floor * r->x_size];
        data = GetDoor(floor);
        if (data != NO_ROOM) {
            *room_number = data;
            r = &room[data];
        }
    } while (data != NO_ROOM);
    if (y >= floor->floor << 8) {
        do {
            if (floor->pit_room == NO_ROOM)
                return floor;
            if ((retval = CheckNoColFloorTriangle(floor, x, z)) == 1)
                break;
            if (retval == -1) {
                if (y < r->minfloor)
                    break;
            }
            *room_number = floor->pit_room;
            r = &room[floor->pit_room];
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        } while (y >= floor->floor << 8);
    } else if (y < floor->ceiling << 8) {
        do {
            if (floor->sky_room == NO_ROOM)
                return floor;
            if ((retval = CheckNoColCeilingTriangle(floor, x, z)) == 1)
                break;
            if (retval == -1) {
                if (y >= r->maxceiling)
                    break;
            }
            *room_number = floor->sky_room;
            r = &room[floor->sky_room];
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        } while (y < floor->ceiling << 8);
    }
    return floor;
}

sint32 GetWaterHeight(sint32 x, sint32 y, sint32 z, sint16 room_number)
{
    sint32 x_floor, y_floor;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    sint16 data;

    r = &room[room_number];
    do {
        x_floor = (z - r->z) >> WALL_SHIFT;
        y_floor = (x - r->x) >> WALL_SHIFT;
        if (x_floor <= 0) {
            x_floor = 0;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (x_floor >= r->x_size - 1) {
            x_floor = r->x_size - 1;
            if (y_floor < 1)
                y_floor = 1;
            else if (y_floor > r->y_size - 2)
                y_floor = r->y_size - 2;
        } else if (y_floor < 0)
            y_floor = 0;
        else if (y_floor >= r->y_size)
            y_floor = r->y_size - 1;
        floor = &r->floor[x_floor + y_floor * r->x_size];
        data = GetDoor(floor);
        if (data != NO_ROOM) {
            room_number = data;
            r = &room[data];
        }
    } while (data != NO_ROOM);
    if (r->flags & ROOM_UNDERWATER) {
        while (floor->sky_room != NO_ROOM) {
            if (CheckNoColCeilingTriangle(floor, x, z) == 1)
                break;
            r = &room[floor->sky_room];
            if (!(r->flags & ROOM_UNDERWATER))
                return r->minfloor;
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        }
        return r->maxceiling;
    } else {
        while (floor->pit_room != NO_ROOM) {
            if (CheckNoColFloorTriangle(floor, x, z) == 1)
                break;
            r = &room[floor->pit_room];
            if (r->flags & ROOM_UNDERWATER)
                return r->maxceiling;
            floor = &r->floor[((z - r->z) >> WALL_SHIFT) + ((x - r->x) >> WALL_SHIFT) * r->x_size];
        }
    }
    return NO_HEIGHT;
}

sint32 GetHeight(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z)
{
    ITEM_INFO *item;
    ROOM_INFO *r;
    sint16 *data;
    sint32 height;
    uint16 trigger;
    sint32 xoff, yoff;
    sint16 type, dx, dz, tilt, hadj;
    uint16 tilt0, tilt1, tilt2, tilt3;

    tiltxoff = tiltyoff = 0;
    OnObject = 0;
    height_type = WALL;
    while (floor->pit_room != 255) {
        if (CheckNoColFloorTriangle(floor, x, z) == 1)
            break;
        r = &room[floor->pit_room];
        floor = &r->floor[((z - r->z) >> 10) + ((x - r->x) >> 10) * r->x_size];
    }
    height = floor->floor << 8;
    if (height == NO_HEIGHT)
        return height;
    trigger_index = 0;
    if (!floor->index)
        return height;
    data = &floor_data[floor->index];
    do {
        type = *data++;
        switch (type & 0x1F) {
        case SPLIT1:
        case SPLIT2:
        case NOCOLF1T:
        case NOCOLF1B:
        case NOCOLF2T:
        case NOCOLF2B:
            tilt = *data;
            tilt0 = tilt & 0xF;
            tilt1 = (tilt >> 4) & 0xF;
            tilt2 = (tilt >> 8) & 0xF;
            tilt3 = (tilt >> 12) & 0xF;
            dx = x & 1023;
            dz = z & 1023;
            height_type = SPLIT_TRI;
            if ((type & 0x1F) != SPLIT1 && (type & 0x1F) != NOCOLF1T && (type & 0x1F) != NOCOLF1B) {
                if (dx <= dz) {
                    hadj = (type >> 10) & 0x1F;
                    if (hadj & 0x10)
                        hadj |= 0xFFF0;
                    height += hadj << 8;
                    xoff = tilt2 - tilt1;
                    yoff = tilt3 - tilt2;
                } else {
                    hadj = (type >> 5) & 0x1F;
                    if (hadj & 0x10)
                        hadj |= 0xFFF0;
                    height += hadj << 8;
                    xoff = tilt3 - tilt0;
                    yoff = tilt0 - tilt1;
                }
            } else {
                if (dx <= 1024 - dz) {
                    hadj = (type >> 10) & 0x1F;
                    if (hadj & 0x10)
                        hadj |= 0xFFF0;
                    height += hadj << 8;
                    xoff = tilt2 - tilt1;
                    yoff = tilt0 - tilt1;
                } else {
                    hadj = (type >> 5) & 0x1F;
                    if (hadj & 0x10)
                        hadj |= 0xFFF0;
                    height += hadj << 8;
                    xoff = tilt3 - tilt0;
                    yoff = tilt3 - tilt2;
                }
            }
            tiltxoff = xoff;
            tiltyoff = yoff;
            if (ABS(xoff) > 2 || ABS(yoff) > 2)
                height_type = DIAGONAL;
            if (xoff < 0)
                height -= (z & 1023) * xoff >> 2;
            else
                height += ((-1 - z) & 1023) * xoff >> 2;
            if (yoff < 0)
                height -= (x & 1023) * yoff >> 2;
            else
                height += ((-1 - x) & 1023) * yoff >> 2;
            data++;
            break;
        case TILT_TYPE:
            tiltxoff = xoff = *data >> 8;
            tiltyoff = yoff = *(sint8 *)data;
            if (ABS(xoff) > 2 || ABS(yoff) > 2)
                height_type = BIG_SLOPE;
            else
                height_type = SMALL_SLOPE;
            if (xoff < 0)
                height -= (z & 1023) * xoff >> 2;
            else
                height += ((-1 - z) & 1023) * xoff >> 2;
            if (yoff < 0)
                height -= (x & 1023) * yoff >> 2;
            else
                height += ((-1 - x) & 1023) * yoff >> 2;
            data++;
            break;
        case DOOR_TYPE:
        case ROOF_TYPE:
        case SPLIT3:
        case SPLIT4:
        case NOCOLC1T:
        case NOCOLC1B:
        case NOCOLC2T:
        case NOCOLC2B:
            data++;
            break;
        case LAVA_TYPE:
            trigger_index = data - 1;
            break;
        case CLIMB_TYPE:
        case MONKEY_TYPE:
        case TRIGTRIGGER_TYPE:
        case MINER_TYPE:
            if (!trigger_index)
                trigger_index = data - 1;
            break;
        case TRIGGER_TYPE:
            if (!trigger_index)
                trigger_index = data - 1;
            data++;
            do {
                trigger = *data++;
                if ((trigger & 0x3C00) != (TO_OBJECT << 10)) {
                    if ((trigger & 0x3C00) == (TO_CAMERA << 10) || (trigger & 0x3C00) == (TO_FLYBY << 10))
                        trigger = *data++;
                    continue;
                }
                item = &items[trigger & 0x3FF];
                if (objects[item->object_number].floor && !(sint16)(item->flags & 0x8000))
                    objects[item->object_number].floor(item, x, y, z, &height);
            } while (!(trigger & 0x8000));
            break;
        default:
            S_ExitSystem("GetHeight(): Unknown type");
            break;
        }
    } while (!(type & 0x8000));
    return height;
}

void RefreshCamera(sint16 type, sint16 *data)
{
    sint16 trigger, value, target_ok;

    target_ok = 2;
    do {
        trigger = *data++;
        value = trigger & 0x3FF;
        switch ((trigger >> 10) & 0xF) {
        case 1:
            data++;
            if (value == camera.last) {
                camera.number = value;
                if (camera.timer >= 0 && (camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA || camera.fixed[camera.number].flags & 1)) {
                    camera.type = FIXED_CAMERA;
                    target_ok = 1;
                    break;
                }
                camera.timer = -1;
            }
            target_ok = 0;
            break;
        case 6:
            if (camera.type != LOOK_CAMERA && camera.type != COMBAT_CAMERA || camera.number == NO_ITEM || camera.fixed[camera.number].flags & 1)
                camera.item = &items[value];
            break;
        }
    } while (!(trigger & 0x8000));
    if (camera.item) {
        if (!target_ok || target_ok == 2 && camera.item->looked_at && camera.item != camera.last_item)
            camera.item = 0;
    }
    if (camera.number == -1 && camera.timer > 0)
        camera.timer = -1;
}

void TestTriggers(sint16 *data, sint32 heavy, sint32 HeavyFlags)
{
    ITEM_INFO *item;
    ITEM_INFO *camera_item;
    sint32 switch_off, flip, flip_available, neweffect, key, quad;
    sint16 type, trigger, value, flags, state;
    sint16 camera_timer;
    char timer;

    key = 0;
    switch_off = 0;
    flip = -1;
    flip_available = 0;
    neweffect = -1;
    HeavyTriggered = 0;
    if (!heavy) {
        lara.CanMonkeySwing = 0;
        lara.Unused2 = 0;
        lara.OnBeetleFloor = 0;
        lara.climb_status = 0;
    }
    if (!data)
        return;
    if ((*data & DATA_TYPE) == LAVA_TYPE) {
        if (!heavy && (lara_item->pos.y_pos == lara_item->floor || lara.water_status != LW_ABOVE_WATER))
            LavaBurn(lara_item);
        if (*data & END_BIT)
            return;
        data++;
    }
    if ((*data & DATA_TYPE) == CLIMB_TYPE) {
        if (!heavy) {
            quad = (uint16)(lara_item->pos.y_rot + 0x2000) >> 14;
            if ((1 << (quad + 8)) & *data)
                lara.climb_status = 1;
        }
        if (*data & END_BIT)
            return;
        data++;
    }
    if ((*data & DATA_TYPE) == MONKEY_TYPE) {
        if (!heavy)
            lara.CanMonkeySwing = 1;
        if (*data & END_BIT)
            return;
        data++;
    }
    if ((*data & DATA_TYPE) == TRIGTRIGGER_TYPE) {
        if (!(*data & 0x20) || *data & END_BIT)
            return;
        data++;
    }
    if ((*data & DATA_TYPE) == MINER_TYPE) {
        if (!heavy)
            lara.OnBeetleFloor = 1;
        if (*data & END_BIT)
            return;
        data++;
    }
    type = (*data++ >> 8) & 0x3F;
    flags = *data++;
    timer = flags & 0xFF;
    if (camera.type != HEAVY_CAMERA)
        RefreshCamera(type, data);
    if (heavy) {
        switch (type) {
        case HEAVY:
        case HEAVYANTITRIGGER:
            break;
        case HEAVYSWITCH:
            if (!HeavyFlags)
                return;
            if (HeavyFlags < 0) {
                flags |= IFL_CODEBITS;
                flags += (sint16)HeavyFlags;
            } else if ((flags & IFL_CODEBITS) != HeavyFlags)
                return;
            break;
        default:
            return;
        }
    } else {
        switch (type) {
        case SWITCH:
            value = *data++ & 0x3FF;
            if (flags & IFL_INVISIBLE)
                items[value].item_flags[0] = 1;
            if (!SwitchTrigger(value, timer))
                return;
            switch_off = items[value].current_anim_state == 1;
            break;
        case MONKEY:
            state = lara_item->current_anim_state;
            if (state >= AS_HANG2 && (state <= AS_MONKEY180 || state == AS_HANGTURNL || state == AS_HANGTURNR))
                break;
            return;
        case PAD:
        case ANTIPAD:
            if (lara_item->pos.y_pos != lara_item->floor)
                return;
            break;
        case KEY:
            value = *data++ & 0x3FF;
            key = KeyTrigger(value);
            if (key == -1)
                return;
            break;
        case PICKUP:
            value = *data++ & 0x3FF;
            if (!PickupTrigger(value))
                return;
            break;
        case COMBAT:
            if (lara.gun_status != LG_READY)
                return;
            break;
        case HEAVY:
        case DUMMY:
        case HEAVYSWITCH:
        case HEAVYANTITRIGGER:
            return;
        }
    }
    camera_item = NULL;
    do {
        trigger = *data++;
        value = trigger & 0x3FF;
        switch ((trigger & 0x3FFF) >> 10) {
        case TO_OBJECT:
            item = &items[value];
            if (key >= 2 || ((type == ANTIPAD || type == ANTITRIGGER || type == HEAVYANTITRIGGER) && item->flags & IFL_ANTITRIGGER_ONESHOT) ||
                (type == SWITCH && item->flags & IFL_SWITCH_ONESHOT) ||
                (type != SWITCH && type != ANTIPAD && type != ANTITRIGGER && type != HEAVYANTITRIGGER && item->flags & IFL_INVISIBLE) ||
                ((type != ANTIPAD && type != ANTITRIGGER && type != HEAVYANTITRIGGER) && (item->object_number == DART_EMITTER && item->active)))
                break;
            item->timer = timer;
            if (timer != 1)
                item->timer *= 30;
            if (type == SWITCH || type == HEAVYSWITCH) {
                if (HeavyFlags < 0) {
                    if (((item->flags ^ flags) & IFL_CODEBITS) == IFL_CODEBITS) {
                        item->flags ^= flags & IFL_CODEBITS;
                        if (flags & IFL_INVISIBLE)
                            item->flags |= IFL_SWITCH_ONESHOT;
                    }
                } else {
                    item->flags ^= flags & IFL_CODEBITS;
                    if (flags & IFL_INVISIBLE)
                        item->flags |= IFL_SWITCH_ONESHOT;
                }
            } else if (type == ANTIPAD || type == ANTITRIGGER || type == HEAVYANTITRIGGER) {
                if (item->object_number == EARTHQUAKE) {
                    item->item_flags[0] = 0;
                    item->item_flags[1] = 100;
                }
                item->flags &= ~(IFL_CODEBITS | IFL_REVERSE);
                if (flags & IFL_INVISIBLE)
                    item->flags |= IFL_ANTITRIGGER_ONESHOT;
            } else if (flags & IFL_CODEBITS)
                item->flags |= flags & IFL_CODEBITS;
            if ((item->flags & IFL_CODEBITS) != IFL_CODEBITS)
                break;
            item->flags |= IFL_TRIGGERED;
            if (flags & IFL_INVISIBLE)
                item->flags |= IFL_INVISIBLE;
            if (!item->active) {
                if (objects[item->object_number].intelligent) {
                    if (item->status == ITEM_INACTIVE) {
                        item->touch_bits = 0;
                        item->status = ITEM_ACTIVE;
                        AddActiveItem(value);
                        EnableBaddieAI(value, 1);
                    } else if (item->status == ITEM_INVISIBLE) {
                        item->touch_bits = 0;
                        if (EnableBaddieAI(value, 0))
                            item->status = ITEM_ACTIVE;
                        else
                            item->status = ITEM_INVISIBLE;
                        AddActiveItem(value);
                    }
                } else {
                    item->touch_bits = 0;
                    AddActiveItem(value);
                    item->status = ITEM_ACTIVE;
                    HeavyTriggered = (uint8)heavy;
                }
            }
            break;
        case TO_CAMERA:
            trigger = *data++;
            camera_timer = trigger & 0xFF;
            if (key == 1 || camera.fixed[value].flags & IFL_INVISIBLE)
                break;
            camera.number = value;
            if ((camera.type == LOOK_CAMERA || camera.type == COMBAT_CAMERA) && !(camera.fixed[camera.number].flags & 1) ||
                type == COMBAT || type == SWITCH && timer && switch_off)
                break;
            if (camera.number != camera.last || type == SWITCH) {
                camera.timer = camera_timer * 30;
                if (trigger & IFL_INVISIBLE)
                    camera.fixed[camera.number].flags |= IFL_INVISIBLE;
                camera.speed = ((trigger & IFL_CODEBITS) >> 6) + 1;
                camera.type = heavy ? HEAVY_CAMERA : FIXED_CAMERA;
            }
            break;
        case TO_FLYBY:
            trigger = *data++;
            if (key == 1)
                break;
            if (type == ANTIPAD || type == ANTITRIGGER || type == HEAVYANTITRIGGER) {
                bUseSpotCam = 0;
                break;
            }
            if (SpotCam[value].flags & SP_FLYBYONESHOT)
                break;
            if (trigger & IFL_INVISIBLE)
                SpotCam[value].flags |= SP_FLYBYONESHOT;
            if (bUseSpotCam)
                break;
            bUseSpotCam = 1;
            if (LastSequence != value)
                bTrackCamInit = 0;
            InitialiseSpotCam(value);
            break;
        case TO_TARGET:
            camera_item = &items[value];
            break;
        case TO_SINK:
            lara.current_active = value + 1;
            break;
        case TO_FLIPMAP:
            flip_available = 1;
            if (flipmap[value] & IFL_INVISIBLE)
                break;
            if (type == SWITCH)
                flipmap[value] ^= flags & IFL_CODEBITS;
            else if (flags & IFL_CODEBITS)
                flipmap[value] |= flags & IFL_CODEBITS;
            if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS) {
                if (flags & IFL_INVISIBLE)
                    flipmap[value] |= IFL_INVISIBLE;
                if (!flip_stats[value])
                    flip = value;
            } else if (flip_stats[value])
                flip = value;
            break;
        case TO_FLIPON:
            flip_available = 1;
            if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS && !flip_stats[value])
                flip = value;
            break;
        case TO_FLIPOFF:
            flip_available = 1;
            if ((flipmap[value] & IFL_CODEBITS) == IFL_CODEBITS && flip_stats[value])
                flip = value;
            break;
        case TO_FLIPEFFECT:
            TriggerTimer = timer;
            neweffect = value;
            break;
        case TO_FINISH:
            gfLevelComplete = (uint8)value;
            gfRequiredStartPos = timer;
            break;
        case TO_CD:
            TriggerCDTrack(value, flags, type);
            break;
        case TO_SECRET:
            if (!(savegame.Level.Secrets & 1 << value)) {
                S_CDPlay(5, 0);
                savegame.Level.Secrets |= 1 << value;
                savegame.Game.Secrets++;
            }
            break;
        case TO_CUTSCENE:
            if (value == 32) {
                if (!(FmvSceneTriggered & 0x80))
                    DEL_playingamefmv = 1;
            } else if (!cutseq_trig && !(CutSceneTriggered & 1 << value)) {
                if (value == 8)
                    CutSceneTriggered |= 1 << 8;
                else if (value >= 17 && value <= 20) {
                    if (lara.highest_location == value - 17)
                        cutseq_num = value;
                } else if (value == 27) {
                    if (lara_item->room_number == 55)
                        cutseq_num = 27;
                } else if (value == 7) {
                    if (CutSceneTriggered & 1 << 8)
                        cutseq_num = 8;
                    else {
                        cutseq_num = 7;
                        CutSceneTriggered |= 1 << 8;
                    }
                } else
                    cutseq_num = value;
            }
            break;
        }
    } while (!(trigger & END_BIT));
    if (camera_item && (camera.type == FIXED_CAMERA || camera.type == HEAVY_CAMERA))
        camera.item = camera_item;
    if (flip != -1)
        FlipMap(flip);
    if (neweffect != -1 && (flip || !flip_available)) {
        flipeffect = neweffect;
        fliptimer = 0;
    }
}

sint32 TriggerActive(ITEM_INFO *item)
{
    sint32 ok;

    ok = (item->flags & IFL_REVERSE) ? 0 : 1;
    if ((item->flags & IFL_CODEBITS) != IFL_CODEBITS)
        return !ok;
    if (!item->timer)
        return ok;
    if (item->timer > 0) {
        item->timer--;
        if (!item->timer)
            item->timer = -1;
    } else if (item->timer < -1) {
        item->timer++;
        if (item->timer == -1)
            item->timer = 0;
    }
    if (item->timer <= -1)
        return !ok;
    return ok;
}

sint32 GetCeiling(FLOOR_INFO *floor, sint32 x, sint32 y, sint32 z)
{
    ITEM_INFO *item;
    ROOM_INFO *r;
    FLOOR_INFO *f;
    sint32 xoff, yoff, height;
    sint16 *data, type, trigger, tilts, dx, dz, t0, t1, t2, t3, hadj, ended;

    f = floor;
    while (f->sky_room != NO_ROOM) {
        if (CheckNoColCeilingTriangle(floor, x, z) == 1)
            break;
        r = &room[f->sky_room];
        f = &r->floor[((z - r->z) >> 10) + ((x - r->x) >> 10) * r->x_size];
    }
    height = f->ceiling << 8;
    if (height == NO_HEIGHT)
        return height;
    if (f->index) {
        data = &floor_data[f->index];
        hadj = *data++;
        type = hadj & DATA_TYPE;
        ended = 0;
        if (type == TILT_TYPE || type == SPLIT1 || type == SPLIT2 || type == NOCOLF1T || type == NOCOLF1B || type == NOCOLF2T || type == NOCOLF2B) {
            ended = hadj & END_BIT;
            data++;
            hadj = *data++;
            type = hadj & DATA_TYPE;
        }
        if (!ended && (type == ROOF_TYPE || type == SPLIT3 || type == SPLIT4 || type == NOCOLC1T || type == NOCOLC1B || type == NOCOLC2T || type == NOCOLC2B)) {
            if (type != ROOF_TYPE) {
                tilts = *data;
                t0 = -(tilts & 15);
                t1 = -((tilts >> 4) & 15);
                t2 = -((tilts >> 8) & 15);
                t3 = -((tilts >> 12) & 15);
                dx = x & 1023;
                dz = z & 1023;
                xoff = yoff = 0;
                if (type == SPLIT3 || type == NOCOLC1T || type == NOCOLC1B) {
                    if (dx <= 1024 - dz) {
                        hadj = (hadj >> 10) & 0x1F;
                        if (hadj & 0x10)
                            hadj |= 0xFFF0;
                        height += hadj << 8;
                        xoff = t2 - t1;
                        yoff = t3 - t2;
                    } else {
                        hadj = (hadj >> 5) & 0x1F;
                        if (hadj & 0x10)
                            hadj |= 0xFFF0;
                        height += hadj << 8;
                        xoff = t3 - t0;
                        yoff = t0 - t1;
                    }
                } else {
                    if (dx <= dz) {
                        hadj = (hadj >> 10) & 0x1F;
                        if (hadj & 0x10)
                            hadj |= 0xFFF0;
                        height += hadj << 8;
                        xoff = t2 - t1;
                        yoff = t0 - t1;
                    } else {
                        hadj = (hadj >> 5) & 0x1F;
                        if (hadj & 0x10)
                            hadj |= 0xFFF0;
                        height += hadj << 8;
                        xoff = t3 - t0;
                        yoff = t3 - t2;
                    }
                }
            } else {
                xoff = *data >> 8;
                yoff = *(sint8 *)data;
            }
            if (xoff < 0)
                height += (z & 1023) * xoff >> 2;
            else
                height -= ((1023 - z) & 1023) * xoff >> 2;
            if (yoff >= 0)
                height -= (x & 1023) * yoff >> 2;
            else
                height += ((1023 - x) & 1023) * yoff >> 2;
        }
    }
    while (floor->pit_room != NO_ROOM) {
        if (CheckNoColFloorTriangle(floor, x, z) == 1)
            break;
        r = &room[floor->pit_room];
        floor = &r->floor[((z - r->z) >> 10) + ((x - r->x) >> 10) * r->x_size];
    }
    if (!floor->index)
        return height;
    data = &floor_data[floor->index];
    do {
        type = *data++;
        switch (type & DATA_TYPE) {
        case DOOR_TYPE:
        case TILT_TYPE:
        case ROOF_TYPE:
        case SPLIT1:
        case SPLIT2:
        case SPLIT3:
        case SPLIT4:
        case NOCOLF1T:
        case NOCOLF1B:
        case NOCOLF2T:
        case NOCOLF2B:
        case NOCOLC1T:
        case NOCOLC1B:
        case NOCOLC2T:
        case NOCOLC2B:
            data++;
            break;
        case TRIGGER_TYPE:
            data++;
            do {
                trigger = *data++;
                if ((trigger & 0x3C00) != TO_OBJECT << 10) {
                    if ((trigger & 0x3C00) == TO_CAMERA << 10 || (trigger & 0x3C00) == TO_FLYBY << 10)
                        trigger = *data++;
                    continue;
                }
                item = &items[trigger & 0x3FF];
                if (objects[item->object_number].ceiling && !(sint16)(item->flags & 0x8000))
                    objects[item->object_number].ceiling(item, x, y, z, &height);
            } while (!(trigger & END_BIT));
            break;
        case LAVA_TYPE:
        case CLIMB_TYPE:
        case MONKEY_TYPE:
        case TRIGTRIGGER_TYPE:
        case MINER_TYPE:
            break;
        default:
            S_ExitSystem("GetCeiling(): Unknown type");
        }
    } while (!(type & END_BIT));
    return height;
}

sint16 GetDoor(FLOOR_INFO *floor)
{
    sint16 *data;
    sint16 type;

    if (!floor->index)
        return NO_ROOM;
    data = &floor_data[floor->index];
    type = *data++;
    if ((type & DATA_TYPE) == TILT_TYPE || (type & DATA_TYPE) == SPLIT1 || (type & DATA_TYPE) == SPLIT2 || (type & DATA_TYPE) == NOCOLF1B || (type & DATA_TYPE) == NOCOLF1T || (type & DATA_TYPE) == NOCOLF2B || (type & DATA_TYPE) == NOCOLF2T) {
        if (type & END_BIT)
            return NO_ROOM;
        data++;
        type = *data++;
    }
    if ((type & DATA_TYPE) == ROOF_TYPE || (type & DATA_TYPE) == SPLIT3 || (type & DATA_TYPE) == SPLIT4 || (type & DATA_TYPE) == NOCOLC1B || (type & DATA_TYPE) == NOCOLC1T || (type & DATA_TYPE) == NOCOLC2B || (type & DATA_TYPE) == NOCOLC2T) {
        if (type & END_BIT)
            return NO_ROOM;
        data++;
        type = *data++;
    }
    if ((type & DATA_TYPE) == DOOR_TYPE)
        return *data;
    return NO_ROOM;
}

sint32 LOS(GAME_VECTOR *start, GAME_VECTOR *target)
{
    sint32 los1, los2;

    target->room_number = start->room_number;
    if (ABS(target->z - start->z) > ABS(target->x - start->x)) {
        los1 = xLOS(start, target);
        los2 = zLOS(start, target);
    } else {
        los1 = zLOS(start, target);
        los2 = xLOS(start, target);
    }
    if (!los2)
        return 0;
    GetFloor(target->x, target->y, target->z, &target->room_number);
    if (!ClipTarget(start, target) || los1 != 1 || los2 != 1)
        return 0;
    return 1;
}

sint32 zLOS(GAME_VECTOR *start, GAME_VECTOR *target)
{
    sint32 x, y, z;
    sint32 dx, dy, dz;
    sint16 room_number, last_room;
    FLOOR_INFO *floor;

    dz = target->z - start->z;
    if (dz == 0)
        return 1;
    dx = ((target->x - start->x) << WALL_SHIFT) / dz;
    dy = ((target->y - start->y) << WALL_SHIFT) / dz;
    los_rooms[0] = room_number = last_room = start->room_number;
    number_los_rooms = 1;
    if (dz < 0) {
        z = start->z & ~(WALL_L - 1);
        x = start->x + (dx * (z - start->z) >> WALL_SHIFT);
        y = start->y + (dy * (z - start->z) >> WALL_SHIFT);
        while (z > target->z) {
            floor = GetFloor(x, y, z, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_number;
                return -1;
            }
            floor = GetFloor(x, y, z - 1, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x, y, z - 1) || y < GetCeiling(floor, x, y, z - 1)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }
            z -= WALL_L;
            x -= dx;
            y -= dy;
        }
    } else {
        z = start->z | (WALL_L - 1);
        x = start->x + (dx * (z - start->z) >> WALL_SHIFT);
        y = start->y + (dy * (z - start->z) >> WALL_SHIFT);
        while (z < target->z) {
            floor = GetFloor(x, y, z, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_number;
                return -1;
            }
            floor = GetFloor(x, y, z + 1, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x, y, z + 1) || y < GetCeiling(floor, x, y, z + 1)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }
            z += WALL_L;
            x += dx;
            y += dy;
        }
    }
    target->room_number = room_number;
    return 1;
}

sint32 xLOS(GAME_VECTOR *start, GAME_VECTOR *target)
{
    sint32 x, y, z;
    sint32 dx, dy, dz;
    sint16 room_number, last_room;
    FLOOR_INFO *floor;

    dx = target->x - start->x;
    if (dx == 0)
        return 1;
    dy = ((target->y - start->y) << WALL_SHIFT) / dx;
    dz = ((target->z - start->z) << WALL_SHIFT) / dx;
    los_rooms[0] = room_number = last_room = start->room_number;
    number_los_rooms = 1;
    if (dx < 0) {
        x = start->x & ~(WALL_L - 1);
        y = start->y + (dy * (x - start->x) >> WALL_SHIFT);
        z = start->z + (dz * (x - start->x) >> WALL_SHIFT);
        while (x > target->x) {
            floor = GetFloor(x, y, z, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_number;
                return -1;
            }
            floor = GetFloor(x - 1, y, z, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x - 1, y, z) || y < GetCeiling(floor, x - 1, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }
            x -= WALL_L;
            y -= dy;
            z -= dz;
        }
    } else {
        x = start->x | (WALL_L - 1);
        y = start->y + (dy * (x - start->x) >> WALL_SHIFT);
        z = start->z + (dz * (x - start->x) >> WALL_SHIFT);
        while (x < target->x) {
            floor = GetFloor(x, y, z, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x, y, z) || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_number;
                return -1;
            }
            floor = GetFloor(x + 1, y, z, &room_number);
            if (room_number != last_room) {
                last_room = room_number;
                los_rooms[number_los_rooms++] = room_number;
            }
            if (y > GetHeight(floor, x + 1, y, z) || y < GetCeiling(floor, x + 1, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }
            x += WALL_L;
            y += dy;
            z += dz;
        }
    }
    target->room_number = room_number;
    return 1;
}

sint32 ClipTarget(GAME_VECTOR *start, GAME_VECTOR *target)
{
    sint32 dx, dy, dz, height, lp;
    sint16 room_no;
    FLOOR_INFO *floor;
    GAME_VECTOR src;

    room_no = target->room_number;
    floor = GetFloor(target->x, target->y, target->z, &room_no);
    height = GetHeight(floor, target->x, target->y, target->z);
    if (target->y > height) {
        src.x = start->x + (((target->x - start->x) * 7) >> 3);
        src.y = start->y + (((target->y - start->y) * 7) >> 3);
        src.z = start->z + (((target->z - start->z) * 7) >> 3);
        for (lp = 3; lp > 0; lp--) {
            dx = src.x + (((target->x - src.x) * lp) >> 2);
            dy = src.y + (((target->y - src.y) * lp) >> 2);
            dz = src.z + (((target->z - src.z) * lp) >> 2);
            floor = GetFloor(dx, dy, dz, &room_no);
            height = GetHeight(floor, dx, dy, dz);
            if (dy < height) {
                target->x = dx;
                target->y = dy;
                target->z = dz;
                target->room_number = room_no;
                return 0;
            }
        }
        target->x = dx;
        target->y = dy;
        target->z = dz;
        target->room_number = room_no;
        return 0;
    }
    room_no = target->room_number;
    floor = GetFloor(target->x, target->y, target->z, &room_no);
    height = GetCeiling(floor, target->x, target->y, target->z);
    if (target->y < height) {
        src.x = start->x + (((target->x - start->x) * 7) >> 3);
        src.y = start->y + (((target->y - start->y) * 7) >> 3);
        src.z = start->z + (((target->z - start->z) * 7) >> 3);
        for (lp = 3; lp > 0; lp--) {
            dx = src.x + (((target->x - src.x) * lp) >> 2);
            dy = src.y + (((target->y - src.y) * lp) >> 2);
            dz = src.z + (((target->z - src.z) * lp) >> 2);
            floor = GetFloor(dx, dy, dz, &room_no);
            height = GetCeiling(floor, dx, dy, dz);
            if (dy > height) {
                target->x = dx;
                target->y = dy;
                target->z = dz;
                target->room_number = room_no;
                return 0;
            }
        }
        target->x = dx;
        target->y = dy;
        target->z = dz;
        target->room_number = room_no;
        return 0;
    }
    return 1;
}

void FlipMap(sint32 FlipNumber)
{
    ROOM_INFO *r, *flipped, temp;
    CREATURE_INFO *cinfo;
    sint32 i;

    for (i = 0, r = room; i < number_rooms; i++, r++) {
        if (r->flipped_room >= 0 && r->FlipNumber == FlipNumber) {
            RemoveRoomFlipItems(r);
            flipped = &room[r->flipped_room];
            memcpy(&temp, r, sizeof(ROOM_INFO));
            memcpy(r, flipped, sizeof(ROOM_INFO));
            memcpy(flipped, &temp, sizeof(ROOM_INFO));
            r->flipped_room = flipped->flipped_room;
            flipped->flipped_room = -1;
            r->item_number = flipped->item_number;
            r->fx_number = flipped->fx_number;
            AddRoomFlipItems(r);
        }
    }
    flip_stats[FlipNumber] = !flip_stats[FlipNumber];
    flip_status = flip_stats[FlipNumber];
    cinfo = baddie_slots;
    for (i = 0; i < NUM_SLOTS; i++, cinfo++)
        cinfo->LOT.target_box = NO_BOX;
}

void RemoveRoomFlipItems(ROOM_INFO *r)
{
    ITEM_INFO *item;
    sint16 item_num;

    for (item_num = r->item_number; item_num != NO_ITEM; item_num = items[item_num].next_item) {
        item = &items[item_num];
        if (item->flags & IFL_INVISIBLE && objects[item->object_number].intelligent) {
            if (item->hit_points <= 0 && item->hit_points != -16384)
                KillItem(item_num);
        }
    }
}

void AddRoomFlipItems(ROOM_INFO *r)
{
    ITEM_INFO *item;
    sint16 item_num;

    for (item_num = r->item_number; item_num != NO_ITEM; item_num = items[item_num].next_item) {
        item = &items[item_num];
        if (item->object_number == RAISING_BLOCK1 && item->item_flags[1])
            AlterFloorHeight(item, -1024);
        if (item->object_number == RAISING_BLOCK2 && item->item_flags[1])
            AlterFloorHeight(item, -2048);
    }
}

void TriggerCDTrack(sint16 value, sint16 flags, sint16 type)
{
    if (value < 128)
        TriggerNormalCDTrack(value, flags, type);
}

void TriggerNormalCDTrack(sint16 value, sint16 flags, sint16 type)
{
    sint32 code;

    if (value >= 105 && value <= 111 || value == 102 || value == 97) {
        if (CurrentAtmosphere != value) {
            CurrentAtmosphere = (uint8)value;
            if (IsAtmospherePlaying)
                S_CDPlay(value, 1);
        }
    } else {
        code = (flags >> 8) & 0x3F;
        if ((cd_flags[value] & code) != code) {
            cd_flags[value] |= code;
            S_CDPlay(value, 0);
            IsAtmospherePlaying = 0;
        }
    }
}

sint32 CheckNoColFloorTriangle(FLOOR_INFO *floor, sint32 x, sint32 z)
{
    sint16 *data, type, dx, dz;

    if (!floor->index)
        return 0;
    data = &floor_data[floor->index];
    type = *data & DATA_TYPE;
    if (type == NOCOLF1T || type == NOCOLF1B || type == NOCOLF2T || type == NOCOLF2B) {
        dx = x & 1023;
        dz = z & 1023;
        if (type == NOCOLF1T && dx <= 1024 - dz)
            return -1;
        else if (type == NOCOLF1B && dx > 1024 - dz)
            return -1;
        else if (type == NOCOLF2T && dx <= dz)
            return -1;
        else if (type == NOCOLF2B && dx > dz)
            return -1;
        return 1;
    }
    return 0;
}

sint32 CheckNoColCeilingTriangle(FLOOR_INFO *floor, sint32 x, sint32 z)
{
    sint16 *data, type, dx, dz;

    if (!floor->index)
        return 0;
    data = &floor_data[floor->index];
    type = *data & DATA_TYPE;
    if (type == TILT_TYPE || type == SPLIT1 || type == SPLIT2 || type == NOCOLF1T || type == NOCOLF1B
        || type == NOCOLF2T || type == NOCOLF2B) {
        if (*data & END_BIT)
            return 0;
        type = *(data + 2) & DATA_TYPE;
    }
    if (type == NOCOLC1T || type == NOCOLC1B || type == NOCOLC2T || type == NOCOLC2B) {
        dx = x & 1023;
        dz = z & 1023;
        if (type == NOCOLC1T && dx <= 1024 - dz)
            return -1;
        else if (type == NOCOLC1B && dx > 1024 - dz)
            return -1;
        else if (type == NOCOLC2T && dx <= dz)
            return -1;
        else if (type == NOCOLC2B && dx > dz)
            return -1;
        return 1;
    }
    return 0;
}

sint32 IsRoomOutside(sint32 x, sint32 y, sint32 z)
{
    sint16 room_no;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    uint8 *pTable;
    sint32 h;
    sint32 c;
    sint16 offset;

    if (x < 0 || z < 0)
        return -2;
    offset = OutsideRoomOffsets[27 * (x >> 12) + (z >> 12)];
    if (offset == -1)
        return -2;
    if (offset < 0) {
        r = &room[offset & 0x7FFF];
        if (y >= r->maxceiling && y <= r->minfloor && z >= r->z + 1024 && z <= (r->x_size << 10) + r->z - 1024
            && x >= r->x + 1024 && x <= (r->y_size << 10) + r->x - 1024) {
            room_no = offset & 0x7FFF;
            IsRoomOutsideNo = room_no;
            floor = GetFloor(x, y, z, &room_no);
            h = GetHeight(floor, x, y, z);
            if (h == NO_HEIGHT || y > h)
                return -2;
            c = GetCeiling(floor, x, y, z);
            if (y < c)
                return -2;
            if (r->flags & (ROOM_UNDERWATER | ROOM_NOT_INSIDE))
                return 1;
            return -3;
        }
    } else {
        pTable = &OutsideRoomTable[offset];
        while (*pTable != 255) {
            r = &room[*pTable];
            if (y >= r->maxceiling && y <= r->minfloor && z >= r->z + 1024 && z <= (r->x_size << 10) + r->z - 1024
                && x >= r->x + 1024 && x <= (r->y_size << 10) + r->x - 1024) {
                room_no = *pTable;
                IsRoomOutsideNo = room_no;
                floor = GetFloor(x, y, z, &room_no);
                h = GetHeight(floor, x, y, z);
                if (h == NO_HEIGHT || y > h)
                    return -2;
                c = GetCeiling(floor, x, y, z);
                if (y < c)
                    return -2;
                if (r->flags & (ROOM_UNDERWATER | ROOM_NOT_INSIDE))
                    return 1;
                return -3;
            }
            pTable++;
        }
    }
    return -2;
}

sint32 RayBoxIntersect(PHD_VECTOR *min, PHD_VECTOR *max, PHD_VECTOR *origin, PHD_VECTOR *dir, PHD_VECTOR *Coord)
{
    sint32 planes[3], i;
    sint32 dists[3];
    sint32 plane;
    sint8 quad[3];
    sint8 inside;

    inside = 1;
    if (origin->x < min->x) {
        quad[0] = 1;
        planes[0] = min->x;
        inside = 0;
    } else if (origin->x > max->x) {
        quad[0] = 0;
        planes[0] = max->x;
        inside = 0;
    } else
        quad[0] = 2;
    if (origin->y < min->y) {
        quad[1] = 1;
        planes[1] = min->y;
        inside = 0;
    } else if (origin->y > max->y) {
        quad[1] = 0;
        planes[1] = max->y;
        inside = 0;
    } else
        quad[1] = 2;
    if (origin->z < min->z) {
        quad[2] = 1;
        planes[2] = min->z;
        inside = 0;
    } else if (origin->z > max->z) {
        quad[2] = 0;
        planes[2] = max->z;
        inside = 0;
    } else
        quad[2] = 2;
    if (inside) {
        Coord->x = origin->x >> 16;
        Coord->y = origin->y >> 16;
        Coord->z = origin->z >> 16;
        return 1;
    }
    if (quad[0] == 2 || !dir->x)
        dists[0] = -65536;
    else
        dists[0] = ((planes[0] - origin->x) / (dir->x >> 8)) << 8;
    if (quad[1] == 2 || !dir->y)
        dists[1] = -65536;
    else
        dists[1] = ((planes[1] - origin->y) / (dir->y >> 8)) << 8;
    if (quad[2] == 2 || !dir->z)
        dists[2] = -65536;
    else
        dists[2] = ((planes[2] - origin->z) / (dir->z >> 8)) << 8;
    plane = 0;
    for (i = 1; i < 3; i++) {
        if (dists[plane] < dists[i])
            plane = i;
    }
    if (dists[plane] < 0)
        return 0;
    if (plane) {
        Coord->x = origin->x + (((__int64)dir->x * (__int64)dists[plane]) >> 16);
        if ((!quad[0] && Coord->x < min->x) || (quad[0] == 1 && Coord->x > max->x))
            return 0;
    } else
        Coord->x = planes[0];
    if (plane != 1) {
        Coord->y = origin->y + (((__int64)dir->y * (__int64)dists[plane]) >> 16);
        if ((!quad[1] && Coord->y < min->y) || (quad[1] == 1 && Coord->y > max->y))
            return 0;
    } else
        Coord->y = planes[1];
    if (plane != 2) {
        Coord->z = origin->z + (((__int64)dir->z * (__int64)dists[plane]) >> 16);
        if ((!quad[2] && Coord->z < min->z) || (quad[2] == 1 && Coord->z > max->z))
            return 0;
    } else
        Coord->z = planes[2];
    Coord->x >>= 16;
    Coord->y >>= 16;
    Coord->z >>= 16;
    return 1;
}

void FireCrossBowFromLaserSight(GAME_VECTOR *start, GAME_VECTOR *target)
{
    PHD_3DPOS pos;
    sint16 angles[2];

    phd_GetVectorAngles(target->x - start->x, target->y - start->y, target->z - start->z, angles);
    pos.x_pos = start->x;
    pos.y_pos = start->y;
    pos.z_pos = start->z;
    pos.x_rot = angles[1];
    pos.y_rot = angles[0];
    pos.z_rot = 0;
    FireCrossbow(&pos);
}

sint32 GetTargetOnLOS(GAME_VECTOR *src, GAME_VECTOR *dest, sint32 DrawTarget, sint32 firing)
{
    ITEM_INFO *shotitem;
    MESH_INFO *Mesh;
    FLOOR_INFO *floor;
    GAME_VECTOR target;
    PHD_VECTOR v;
    sint16 item_no, hit, ricochet, room_number, TriggerItems[8], NumTrigs;
    sint16 i;

    hit = 0;
    target.x = dest->x;
    target.y = dest->y;
    target.z = dest->z;
    ricochet = LOS(src, &target);
    GetFloor(target.x, target.y, target.z, &target.room_number);
    if (firing && LaserSight) {
        if (lara.gun_type == WEAPON_REVOLVER)
            SoundEffect(SFX_DESERT_EAGLE_FIRE, 0, SFX_DEFAULT);
    }
    item_no = ObjectOnLOS2(src, dest, &v, &Mesh);
    if (item_no != 999) {
        target.x = v.x - ((v.x - src->x) >> 5);
        target.y = v.y - ((v.y - src->y) >> 5);
        target.z = v.z - ((v.z - src->z) >> 5);
        if (firing) {
            if (lara.gun_type != WEAPON_CROSSBOW) {
                if (item_no < 0) {
                    if (Mesh->static_number >= SHATTER0 && Mesh->static_number < SHATTER8) {
                        ShatterObject(0, Mesh, 128, target.room_number, 0);
                        SmashedMeshRoom[SmashedMeshCount] = target.room_number;
                        SmashedMesh[SmashedMeshCount++] = Mesh;
                        Mesh->Flags &= ~1;
                        SoundEffect(SFX_HIT_ROCK, (PHD_3DPOS *)Mesh, SFX_DEFAULT);
                    }
                    TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
                    TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
                } else {
                    shotitem = &items[item_no];
                    if (shotitem->object_number != SWITCH_TYPE7 && shotitem->object_number != SWITCH_TYPE8) {
                        if (objects[items[item_no].object_number].explodable_meshbits & ShatterItem.Bit && LaserSight) {
                            ShatterObject(&ShatterItem, 0, 128, target.room_number, 0);
                            items[item_no].mesh_bits &= ~ShatterItem.Bit;
                            TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
                        } else if (DrawTarget && lara.gun_type == WEAPON_REVOLVER) {
                            if (objects[items[item_no].object_number].intelligent)
                                HitTarget(shotitem, &target, weapons[lara.gun_type].damage, 0);
                        } else {
                            if (objects[items[item_no].object_number].HitEffect == 1)
                                DoBloodSplat(target.x, target.y, target.z, (GetRandomControl() & 3) + 3, shotitem->pos.y_rot, shotitem->room_number);
                            else if (objects[items[item_no].object_number].HitEffect == 3)
                                TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
                            else if (objects[items[item_no].object_number].HitEffect == 2)
                                TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, -5);
                            items[item_no].hit_status = 1;
                            if (!objects[items[item_no].object_number].undead)
                                items[item_no].hit_points -= weapons[lara.gun_type].damage;
                        }
                    } else {
                        if (ShatterItem.Bit == 1 << (objects[shotitem->object_number].nmeshes - 1) && !(shotitem->flags & IFL_SWITCH_ONESHOT)) {
                            if (shotitem->object_number == SWITCH_TYPE7)
                                ExplodeItemNode(shotitem, objects[shotitem->object_number].nmeshes - 1, 0, 64);
                            if (shotitem->flags & IFL_CODEBITS && (shotitem->flags & IFL_CODEBITS) != IFL_CODEBITS) {
                                room_number = shotitem->room_number;
                                floor = GetFloor(shotitem->pos.x_pos, shotitem->pos.y_pos - 256, shotitem->pos.z_pos, &room_number);
                                GetHeight(floor, shotitem->pos.x_pos, shotitem->pos.y_pos - 256, shotitem->pos.z_pos);
                                TestTriggers(trigger_index, 1, shotitem->flags & IFL_CODEBITS);
                            } else {
                                NumTrigs = GetSwitchTrigger(shotitem, TriggerItems, 1);
                                for (i = NumTrigs; i > 0; i--) {
                                    AddActiveItem(TriggerItems[i - 1]);
                                    items[TriggerItems[i - 1]].status = ITEM_ACTIVE;
                                    items[TriggerItems[i - 1]].flags |= IFL_CODEBITS;
                                }
                            }
                            AddActiveItem(item_no);
                            shotitem->status = ITEM_ACTIVE;
                            shotitem->flags |= IFL_SWITCH_ONESHOT | IFL_CODEBITS;
                        }
                        TriggerRicochetSpark(&target, lara_item->pos.y_rot, 3, 0);
                    }
                }
            } else if (LaserSight)
                FireCrossBowFromLaserSight(src, &target);
        }
        hit = 1;
    } else if (lara.gun_type != WEAPON_CROSSBOW) {
        target.x -= (target.x - src->x) >> 5;
        target.y -= (target.y - src->y) >> 5;
        target.z -= (target.z - src->z) >> 5;
        if (firing && !ricochet)
            TriggerRicochetSpark(&target, lara_item->pos.y_rot, 8, 0);
    } else if (firing && LaserSight)
        FireCrossBowFromLaserSight(src, &target);
    if (DrawTarget && (hit || !ricochet)) {
        LaserSightActive = 1;
        LaserSightX = target.x;
        LaserSightY = target.y;
        LaserSightZ = target.z;
        TriggerDynamic(target.x, target.y, target.z, 16, 32, 0, 0);
    }
    return hit;
}

sint32 ExplodeItemNode(ITEM_INFO *item, sint32 Node, sint32 NoXZVel, sint32 bits)
{
    OBJECT_INFO *object;
    sint16 **meshpp;

    if (!(item->mesh_bits & (1 << Node)))
        return 0;
    if (bits != 256)
        SoundEffect(SFX_HIT_ROCK, &item->pos, SFX_DEFAULT);
    else
        bits = -64;
    GetSpheres(item, Slist, 3);
    object = &objects[item->object_number];
    meshpp = &meshes[object->mesh_index + Node * 2];
    ShatterItem.YRot = item->pos.y_rot;
    ShatterItem.meshp = *meshpp;
    ShatterItem.Sphere.x = Slist[Node].x;
    ShatterItem.Sphere.y = Slist[Node].y;
    ShatterItem.Sphere.z = Slist[Node].z;
    ShatterItem.Bit = 1 << Node;
    ShatterItem.il = &item->il;
    if (item->object_number == CROSSBOW_BOLT)
        ShatterItem.Flags = 1024;
    else
        ShatterItem.Flags = 0;
    ShatterObject(&ShatterItem, 0, (sint16)bits, item->room_number, NoXZVel);
    item->mesh_bits &= ~ShatterItem.Bit;
    return 1;
}
