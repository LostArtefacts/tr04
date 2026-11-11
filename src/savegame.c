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

void SaveHubData(sint32 index)
{
    savegame.HubSizes[index] = (uint16)(SGcount - savegame.HubOffsets[index]);
    if (index < 10)
        savegame.HubSizes[index - 9] = savegame.HubSizes[index] + savegame.HubOffsets[index];
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
