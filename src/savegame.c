#include "savegame.h"
#include "gameflow.h"
#include "control.h"
#include "anim.h"
#include "objnums.h"
#include "specific/function_stubs.h"
#include "box.h"
#include "laramisc.h"
#include "bike.h"
#include "jeep.h"
#include "items.h"
#include "room.h"
#include "lara.h"
#include "camera.h"
#include "newinv.h"
#include "traps.h"
#include "switch.h"
#include "spotcam.h"
#include "senet.h"
#include "scarab.h"
#include "voncroy.h"
#include "rope.h"
#include "specific/fmv.h"
#include "specific/audio.h"
#include "specific/file.h"
#include <string.h>

sint32 CheckSumValid(sint8 *buffer)
{
    sint8 checksum;
    sint32 i;

    checksum = 0;
    for (i = 0; i < sizeof(SAVEGAME_INFO); i++)
        checksum += *buffer++;
    return !checksum;
}

void sgInitialiseHub(sint32 dont_save_lara)
{
    sint32 i;

    for (i = 0; i < 10; i++) {
        savegame.HubOffsets[i] = 0;
        savegame.HubSizes[i] = 0;
        savegame.HubLevels[i] = 0;
    }
    if (dont_save_lara)
        savegame.HubSavedLara = 0;
    else {
        SaveLaraData();
        savegame.HubSavedLara = 1;
    }
    memset(savegame.Lara.puzzleitems, 0, 12);
    savegame.Lara.puzzleitemscombo = 0;
    savegame.Lara.keyitems = 0;
    savegame.Lara.keyitemscombo = 0;
    savegame.Lara.pickupitems = 0;
    savegame.Lara.pickupitemscombo = 0;
    savegame.Lara.examine1 = 0;
    savegame.Lara.examine2 = 0;
    savegame.Lara.examine3 = 0;
    savegame.Lara.mechanical_scarab = 0;
    savegame.Lara.location = -1;
    savegame.Lara.highest_location = -1;
    savegame.Lara.vehicle = NO_ITEM;
}

void SaveLaraData(void)
{
    ITEM_INFO *item;
    sint32 i;

    for (i = 0; i < 15; i++)
        lara.mesh_ptrs[i] = (sint16 *)((sint32)lara.mesh_ptrs[i] - (sint32)mesh_base);
    lara.left_arm.frame_base = (sint16 *)((sint32)lara.left_arm.frame_base - (sint32)objects[PISTOLS_ANIM].frame_base);
    lara.right_arm.frame_base = (sint16 *)((sint32)lara.right_arm.frame_base - (sint32)objects[PISTOLS_ANIM].frame_base);
    lara.GeneralPtr = (void *)((sint32)lara.GeneralPtr - (sint32)malloc_buffer);
    memcpy(&savegame.Lara, &lara, sizeof(savegame.Lara));
    for (i = 0; i < 15; i++)
        lara.mesh_ptrs[i] = (sint16 *)((sint32)lara.mesh_ptrs[i] + (sint32)mesh_base);
    lara.left_arm.frame_base = (sint16 *)((sint32)lara.left_arm.frame_base + (sint32)objects[PISTOLS_ANIM].frame_base);
    lara.right_arm.frame_base = (sint16 *)((sint32)lara.right_arm.frame_base + (sint32)objects[PISTOLS_ANIM].frame_base);
    lara.GeneralPtr = (void *)((sint32)lara.GeneralPtr + (sint32)malloc_buffer);
    if (lara.weapon_item != NO_ITEM) {
        item = &items[lara.weapon_item];
        savegame.WeaponObject = item->object_number;
        savegame.WeaponAnim = item->anim_number;
        savegame.WeaponFrame = item->frame_number;
        savegame.WeaponCurrent = item->current_anim_state;
        savegame.WeaponGoal = item->goal_anim_state;
    }
    savegame.cutscene_triggered = CutSceneTriggered;
}

void sgSaveGame(void)
{
    sint32 level_index;

    level_index = OpenSaveGame(gfCurrentLevel, 1);
    savegame.Game.Timer = GameTimer;
    savegame.CurrentLevel ^= (gfCurrentLevel ^ savegame.CurrentLevel) & 0x7F;
    SaveLevelData(1);
    SaveLaraData();
    SaveHubData(level_index);
    CreateCheckSum();
}

void CreateCheckSum(void)
{
    sint8 *ptr;
    sint8 checksum;
    sint32 i;

    ptr = (sint8 *)&savegame;
    savegame.Checksum = 0;
    checksum = 0;
    for (i = 0; i < sizeof(SAVEGAME_INFO); i++)
        checksum += *ptr++;
    savegame.Checksum = -checksum;
}

sint32 OpenSaveGame(uint8 current_level, sint32 saving)
{
    sint32 index, i, j;

    for (index = 0; index < 10; index++) {
        if (savegame.HubLevels[index] == current_level)
            break;
    }
    if (saving == 1) {
        if (index < 10) {
            i = index;
            for (j = index + 1; j < 10; j++) {
                if (!savegame.HubLevels[j])
                    break;
                memcpy(&savegame.buffer[savegame.HubOffsets[i]], &savegame.buffer[savegame.HubOffsets[j]], savegame.HubSizes[j]);
                savegame.HubSizes[i] = savegame.HubSizes[j];
                savegame.HubOffsets[j] = savegame.HubOffsets[i] + savegame.HubSizes[i];
                savegame.HubLevels[i] = savegame.HubLevels[j];
                i++;
            }
            savegame.HubLevels[i] = 0;
        }
        for (index = 0; index < 10; index++) {
            if (!savegame.HubLevels[index])
                break;
        }
        savegame.HubLevels[index] = current_level;
    } else if (index >= 10)
        return -1;
    SGcount = savegame.HubOffsets[index];
    SGpoint = &savegame.buffer[SGcount];
    return index;
}

void SaveHubData(sint32 index)
{
    savegame.HubSizes[index] = (uint16)(SGcount - savegame.HubOffsets[index]);
    if (index < 10)
        savegame.HubSizes[index - 9] = savegame.HubSizes[index] + savegame.HubOffsets[index];
}

void SaveLevelData(sint32 FullSave)
{
    ITEM_INFO *item;
    ROOM_INFO *r;
    OBJECT_INFO *obj;
    MESH_INFO *mesh;
    uint32 flags;
    sint32 word, packed, pos, i, j, k;
    uint8 byte;
    sint8 lflags;

    WriteSG(&FmvSceneTriggered, sizeof(sint32));
    WriteSG(&GLOBAL_lastinvitem, sizeof(sint32));
    word = 0;
    for (i = 0; i < 10; i++) {
        if (flip_stats[i])
            word |= 1 << i;
    }
    WriteSG(&word, sizeof(sint16));
    for (i = 0; i < 10; i++) {
        word = flipmap[i] >> 8;
        WriteSG(&word, sizeof(sint16));
    }
    WriteSG(&flipeffect, sizeof(sint32));
    WriteSG(&fliptimer, sizeof(sint32));
    WriteSG(&flip_status, sizeof(sint32));
    WriteSG(cd_flags, 128);
    WriteSG(&CurrentAtmosphere, sizeof(uint8));
    word = 0;
    k = 0;
    for (i = 0; i < number_rooms; i++) {
        r = &room[i];
        for (j = 0; j < r->num_meshes; j++) {
            mesh = &r->mesh[j];
            if (mesh->static_number >= SHATTER0) {
                word |= (mesh->Flags & 1) << k;
                k++;
                if (k == 16) {
                    WriteSG(&word, sizeof(sint16));
                    k = 0;
                    word = 0;
                }
            }
        }
    }
    if (k)
        WriteSG(&word, sizeof(sint16));
    byte = 0;
    for (i = 0; i < 8; i++)
        byte |= LibraryTab[i] << i;
    WriteSG(&byte, sizeof(uint8));
    WriteSG(&CurrentSequence, sizeof(uint8));
    byte = 0;
    for (i = 0; i < 6; i++)
        byte |= SequenceUsed[i] << i;
    WriteSG(&byte, sizeof(uint8));
    WriteSG(Sequences, 3);
    for (i = 0; i < number_cameras; i++)
        WriteSG(&camera.fixed[i].flags, sizeof(sint16));
    for (i = 0; i < number_spotcams; i++)
        WriteSG(&SpotCam[i].flags, sizeof(sint16));
    for (i = 0; i < level_items; i++) {
        item = &items[i];
        packed = 0;
        obj = &objects[item->object_number];
        if ((item->after_death && (item->object_number < GAME_PIECE1 || item->object_number > ENEMY_PIECE)) || (sint16)(item->flags & IFL_CLEARBODY)) {
            packed = 0x2000;
            WriteSG(&packed, sizeof(uint16));
        } else if (item->flags & (IFL_CODEBITS | IFL_INVISIBLE | IFL_TRIGGERED) || (item->object_number == LARA && FullSave)) {
            packed = 0x8000;
            if (item->pos.x_rot)
                packed |= 1;
            if (item->pos.z_rot)
                packed |= 2;
            if (item->pos.x_pos & 1)
                packed |= 4;
            if (item->pos.y_pos & 1)
                packed |= 8;
            if (item->pos.z_pos & 1)
                packed |= 0x10;
            if (item->speed)
                packed |= 0x20;
            if (item->fallspeed)
                packed |= 0x40;
            if (item->item_flags[0])
                packed |= 0x80;
            if (item->item_flags[1])
                packed |= 0x100;
            if (item->item_flags[2])
                packed |= 0x200;
            if (item->item_flags[3])
                packed |= 0x400;
            if (item->timer)
                packed |= 0x800;
            if (item->trigger_flags)
                packed |= 0x1000;
            if (obj->save_hitpoints && item->hit_points != obj->hit_points)
                packed |= 0x4000;
            WriteSG(&packed, sizeof(uint16));
            if (obj->save_position) {
                pos = item->pos.x_pos >> 1;
                WriteSG(&pos, sizeof(sint16));
                pos = item->pos.y_pos >> 1;
                WriteSG(&pos, sizeof(sint16));
                pos = item->pos.z_pos >> 1;
                WriteSG(&pos, sizeof(sint16));
                byte = (uint8)item->room_number;
                WriteSG(&byte, sizeof(uint8));
                WriteSG(&item->pos.y_rot, sizeof(sint16));
                if (packed & 1)
                    WriteSG(&item->pos.x_rot, sizeof(sint16));
                if (packed & 2)
                    WriteSG(&item->pos.z_rot, sizeof(sint16));
                if (packed & 0x20)
                    WriteSG(&item->speed, sizeof(sint16));
                if (packed & 0x40)
                    WriteSG(&item->fallspeed, sizeof(sint16));
            }
            if (obj->save_anim) {
                byte = (uint8)item->current_anim_state;
                WriteSG(&byte, sizeof(uint8));
                byte = (uint8)item->goal_anim_state;
                WriteSG(&byte, sizeof(uint8));
                byte = (uint8)item->required_anim_state;
                WriteSG(&byte, sizeof(uint8));
                if (item->object_number == LARA)
                    WriteSG(&item->anim_number, sizeof(sint16));
                else {
                    byte = (uint8)(item->anim_number - obj->anim_index);
                    WriteSG(&byte, sizeof(uint8));
                }
                WriteSG(&item->frame_number, sizeof(sint16));
            }
            if (packed & 0x4000)
                WriteSG(&item->hit_points, sizeof(sint16));
            if (obj->save_flags) {
                flags = ((((uint32 *)&item->meshswap_meshbits)[-1] & 0x7FFF) << 16) | (uint16)item->flags;
                if (obj->intelligent && item->data)
                    flags |= 0x80000000;
                WriteSG(&flags, sizeof(uint32));
                if (packed & 0x80)
                    WriteSG(&item->item_flags[0], sizeof(sint16));
                if (packed & 0x100)
                    WriteSG(&item->item_flags[1], sizeof(sint16));
                if (packed & 0x200)
                    WriteSG(&item->item_flags[2], sizeof(sint16));
                if (packed & 0x400)
                    WriteSG(&item->item_flags[3], sizeof(sint16));
                if (packed & 0x800)
                    WriteSG(&item->timer, sizeof(sint16));
                if (packed & 0x1000)
                    WriteSG(&item->trigger_flags, sizeof(sint16));
                if (obj->intelligent)
                    WriteSG(&item->carried_item, sizeof(sint16));
                if (flags & 0x80000000) {
                    lflags = 0;
                    ((CREATURE_INFO *)item->data)->enemy = (ITEM_INFO *)((sint32)((CREATURE_INFO *)item->data)->enemy - (sint32)malloc_buffer);
                    WriteSG(item->data, 22);
                    ((CREATURE_INFO *)item->data)->enemy = (ITEM_INFO *)((sint32)((CREATURE_INFO *)item->data)->enemy + (sint32)malloc_buffer);
                    WriteSG(&((CREATURE_INFO *)item->data)->ai_target.object_number, sizeof(sint16));
                    WriteSG(&((CREATURE_INFO *)item->data)->ai_target.room_number, sizeof(sint16));
                    WriteSG(&((CREATURE_INFO *)item->data)->ai_target.box_number, sizeof(uint16));
                    WriteSG(&((CREATURE_INFO *)item->data)->ai_target.flags, sizeof(sint16));
                    WriteSG(&((CREATURE_INFO *)item->data)->ai_target.trigger_flags, sizeof(sint16));
                    WriteSG(&((CREATURE_INFO *)item->data)->ai_target.pos, sizeof(PHD_3DPOS));
                    lflags |= ((CREATURE_INFO *)item->data)->LOT.can_jump;
                    lflags |= ((CREATURE_INFO *)item->data)->LOT.can_monkey << 1;
                    lflags |= ((CREATURE_INFO *)item->data)->LOT.is_amphibious << 2;
                    lflags |= ((CREATURE_INFO *)item->data)->LOT.is_jumping << 3;
                    lflags |= ((CREATURE_INFO *)item->data)->LOT.is_monkeying << 4;
                    WriteSG(&lflags, sizeof(sint8));
                }
            }
            if (obj->save_mesh) {
                WriteSG(&item->mesh_bits, sizeof(uint32));
                WriteSG(&item->meshswap_meshbits, sizeof(uint32));
            }
            if (item->object_number == MOTORBIKE)
                WriteSG(item->data, sizeof(BIKEINFO));
            if (item->object_number == JEEP)
                WriteSG(item->data, sizeof(JEEPINFO));
        } else
            WriteSG(&packed, sizeof(uint16));
    }
    if (objects[WHEEL_OF_FORTUNE].loaded) {
        WriteSG(senet_item, sizeof(sint16) * 6);
        WriteSG(senet_piece, sizeof(sint8) * 6);
        WriteSG(senet_board, sizeof(sint8) * 17);
        WriteSG(&last_throw, sizeof(sint8));
        WriteSG(&SenetTargetX, sizeof(sint32));
        WriteSG(&SenetTargetZ, sizeof(sint32));
        WriteSG(&piece_moving, sizeof(sint8));
    }
    if (FullSave) {
        byte = 0;
        item = &items[level_items];
        for (i = level_items; i < 256; i++) {
            if (item->active && (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM))
                byte++;
            item++;
        }
        WriteSG(&byte, sizeof(uint8));
        item = &items[level_items];
        for (i = level_items; i < 256; i++) {
            if (item->active && (item->object_number == FLARE_ITEM || item->object_number == BURNING_TORCH_ITEM)) {
                byte = item->object_number != FLARE_ITEM;
                WriteSG(&byte, sizeof(sint8));
                WriteSG(&item->pos, sizeof(PHD_3DPOS));
                WriteSG(&item->room_number, sizeof(sint16));
                WriteSG(&item->speed, sizeof(sint16));
                WriteSG(&item->fallspeed, sizeof(sint16));
                if (item->object_number == FLARE_ITEM) {
                    sint32 flare_age;

                    flare_age = (sint32)item->data;
                    WriteSG(&flare_age, sizeof(sint32));
                } else
                    WriteSG(&item->item_flags[3], sizeof(sint16));
            }
            item++;
        }
        if (objects[LITTLE_BEETLE].loaded) {
            byte = 0;
            for (j = 0; j < 128; j++) {
                if (Scarabs[j].On)
                    byte++;
            }
            WriteSG(&byte, sizeof(uint8));
            for (j = 0; j < 128; j++) {
                if (Scarabs[j].On) {
                    word = (uint8)Scarabs[j].room_number << 8;
                    if (Scarabs[j].pos.x_pos & 1)
                        word |= 1;
                    if (Scarabs[j].pos.y_pos & 1)
                        word |= 2;
                    if (Scarabs[j].pos.z_pos & 1)
                        word |= 4;
                    if (Scarabs[j].pos.x_rot)
                        word |= 8;
                    WriteSG(&word, sizeof(sint16));
                    pos = Scarabs[j].pos.x_pos >> 1;
                    WriteSG(&pos, sizeof(sint16));
                    pos = Scarabs[j].pos.y_pos >> 1;
                    WriteSG(&pos, sizeof(sint16));
                    pos = Scarabs[j].pos.z_pos >> 1;
                    WriteSG(&pos, sizeof(sint16));
                    WriteSG(&Scarabs[j].pos.y_rot, sizeof(sint16));
                    if (word & 8)
                        WriteSG(&Scarabs[j].pos.x_rot, sizeof(sint16));
                }
            }
        }
        byte = 0;
        item = &items[level_items];
        for (i = level_items; i < 256; i++) {
            if (item->active && item->object_number == CLOCKWORK_BEETLE) {
                byte = 1;
                break;
            }
            item++;
        }
        WriteSG(&byte, sizeof(uint8));
        if (byte) {
            WriteSG(&item->pos, sizeof(PHD_3DPOS));
            WriteSG(item->item_flags, sizeof(sint16) * 4);
        }
        if (gfCurrentLevel == 1) {
            word = 0;
            for (i = 0; i < 64; i++) {
                if (VonCroyCutFlags[i])
                    word |= 1 << (i & 0xF);
                if ((i & 0xF) == 0xF) {
                    WriteSG(&word, 2);
                    word = 0;
                }
            }
        }
        if (lara.RopePtr != -1) {
            WriteSG(&RopeList[lara.RopePtr], sizeof(ROPE_STRUCT));
            CurrentPendulum.Rope = (ROPE_STRUCT *)((sint8 *)CurrentPendulum.Rope - (sint8 *)RopeList);
            WriteSG(&CurrentPendulum, sizeof(PENDULUM));
            CurrentPendulum.Rope = (ROPE_STRUCT *)((sint8 *)CurrentPendulum.Rope + (sint32)RopeList);
        }
    }
}

void WriteSG(void *pointer, sint32 size)
{
    char *data;

    SGcount += size;
    for (data = pointer; size > 0; size--)
        *SGpoint++ = *data++;
}

void sgRestoreGame(void)
{
    OpenSaveGame(savegame.CurrentLevel & 0x7F, 0);
    GameTimer = savegame.Game.Timer;
    gfCurrentLevel = savegame.CurrentLevel & 0x7F;
    RestoreLevelData(1);
    RestoreLaraData(1);
}

void RestoreLaraData(sint32 FullSave)
{
    ITEM_INFO *item;
    sint32 i;

    if (!FullSave)
        savegame.Lara.item_number = lara.item_number;
    memcpy(&lara, &savegame.Lara, sizeof(lara));
    lara.target = 0;
    lara.spaz_effect = 0;
    lara.left_arm.frame_base = (sint16 *)((sint32)lara.left_arm.frame_base + (sint32)objects[PISTOLS_ANIM].frame_base);
    lara.right_arm.frame_base = (sint16 *)((sint32)lara.right_arm.frame_base + (sint32)objects[PISTOLS_ANIM].frame_base);
    lara.GeneralPtr = (void *)((sint32)lara.GeneralPtr + (sint32)malloc_buffer);
    if (lara.burn) {
        lara.burn = 0;
        LaraBurn();
    }
    if (lara.weapon_item != NO_ITEM) {
        lara.weapon_item = CreateItem();
        item = &items[lara.weapon_item];
        item->object_number = savegame.WeaponObject;
        item->anim_number = savegame.WeaponAnim;
        item->frame_number = savegame.WeaponFrame;
        item->current_anim_state = savegame.WeaponCurrent;
        item->goal_anim_state = savegame.WeaponGoal;
        item->status = ITEM_ACTIVE;
        item->room_number = 255;
    }
    if (savegame.HubSavedLara) {
        LaraInitialiseMeshes();
        savegame.HubSavedLara &= ~1;
        lara.last_gun_type = WEAPON_PISTOLS;
        lara.gun_type = WEAPON_PISTOLS;
        lara.request_gun_type = WEAPON_PISTOLS;
        if (lara.weapon_item != NO_ITEM) {
            KillItem(lara.weapon_item);
            lara.weapon_item = NO_ITEM;
        }
    } else {
        for (i = 0; i < 15; i++)
            lara.mesh_ptrs[i] = (sint16 *)((sint32)lara.mesh_ptrs[i] + (sint32)mesh_base);
    }
    CutSceneTriggered = savegame.cutscene_triggered;
}

void ReadSG(void *pointer, sint32 size)
{
    char *data;

    SGcount += size;
    data = pointer;
    for (; size > 0; size--)
        *(data++) = *(SGpoint++);
}

void sgSaveLevel(void)
{
    sint32 level_index;

    level_index = OpenSaveGame(gfCurrentLevel, 1);
    SaveLevelData(0);
    SaveLaraData();
    SaveHubData(level_index);
}

void sgRestoreLevel(void)
{
    AIOBJECT *lsp;
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 i;

    if (OpenSaveGame(gfCurrentLevel, 0) >= 0)
        RestoreLevelData(0);
    RestoreLaraData(0);
    if (gfRequiredStartPos) {
        lsp = &AIObjects[gfRequiredStartPos - 1];
        lara_item->pos.x_pos = lsp->x;
        lara_item->pos.y_pos = lsp->y;
        lara_item->pos.z_pos = lsp->z;
        lara_item->pos.y_rot = lsp->y_rot;
        if (lara_item->room_number != lsp->room_number)
            ItemNewRoom(lara.item_number, lsp->room_number);
    }
    InitialiseLaraAnims(lara_item);
    if (savegame.Lara.vehicle != NO_ITEM) {
        for (i = 0; i < level_items; i++) {
            if (items[i].object_number == MOTORBIKE || items[i].object_number == JEEP)
                break;
        }
        item = &items[i];
        item->pos.x_pos = lara_item->pos.x_pos;
        item->pos.y_pos = lara_item->pos.y_pos;
        item->pos.z_pos = lara_item->pos.z_pos;
        item->pos.y_rot = lara_item->pos.y_rot;
        if (item->room_number != lara_item->room_number)
            ItemNewRoom(i, lara_item->room_number);
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number);
        item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        lara.vehicle = i;
        if (items[lara.vehicle].object_number == MOTORBIKE)
            BikeStart(item, lara_item);
        else
            JeepStart(item, lara_item);
    }
}
