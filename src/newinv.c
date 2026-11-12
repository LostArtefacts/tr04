#include <stdio.h>
#include <dinput.h>

#include "newinv.h"
#include "items.h"
#include "lara.h"
#include "objnums.h"
#include "anim.h"
#include "gameflow.h"
#include "lara1gun.h"
#include "lara2gun.h"
#include "specific/3dmath.h"
#include "specific/input.h"
#include "specific/loadsave.h"
#include "text.h"
#include "camera.h"
#include "sound.h"
#include "savegame.h"
#include "larafire.h"
#include "lara_states.h"
#include "control.h"
#include "health.h"
#include "specific/output.h"
#include "specific/dxshell.h"
#include "draw.h"
#include "specific/gamemain.h"

sint32 LoadGame(void)
{
    if (S_LoadSave(IN_LOAD, 1) >= 0)
        return 1;
    else
        return -1;
}

sint32 SaveGame(void)
{
    sint32 Save;

    Save = S_LoadSave(IN_SAVE, 1);
    input = 0;
    dbinput = 0;
    if (Save >= 0)
        return 1;
    else
        return -1;
}

sint32 S_CallInventory2(void)
{
    sint32 return_value, flag;
    sint16 val;

    val = 0;
    oldLaraBusy = lara.Busy;
    friggrimmer = 0;
    if (input & IN_SELECT)
        friggrimmer = 1;
    rings[RING_INVENTORY] = &pcring1;
    rings[RING_AMMO] = &pcring2;
    CreateMonoScreen();
    InventoryActive = 1;
    init_new_inventry();
    camera.number_frames = 2;
    return_value = 0;
    while (!val && !reset_flag) {
        OBJLIST_SPACING = phd_centerx >> 1;
        if (compass_settle_thang != 1024)
            compass_settle_thang -= 32;
        S_InitialisePolyList();
        SetDebounce = 1;
        S_UpdateInput();
        input = inputBusy;
        UpdatePulseColour();
        GameTimer++;
        if (dbinput & IN_OPTION) {
            SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
            val = 1;
        }
        if (MainThread.ended)
            return MainThread.ended;
        S_DisplayMonoScreen();
        do_debounced_joystick_poo();
        if (!examine_mode) {
            draw_current_object_list(RING_INVENTORY);
            handle_inventry_menu();
            if (rings[RING_AMMO]->ringactive)
                draw_current_object_list(RING_AMMO);
            draw_ammo_selector();
            fade_ammo_selector();
            draw_compass();
        } else
            do_examine_mode();
        if (use_the_bitch && !input)
            val = 1;
        S_OutputPolyList();
        camera.number_frames = S_DumpScreen();
        if (loading_or_saving) {
            while (1) {
                S_InitialisePolyList();
                SetDebounce = 1;
                S_UpdateInput();
                input = inputBusy;
                UpdatePulseColour();
                if (loading_or_saving == 1) {
                    flag = go_and_load_game();
                    if (!flag)
                        continue;
                    if (flag != 1)
                        break;
                } else if (!go_and_save_game())
                    continue;
                if (loading_or_saving == 1) {
                    return_value = 1;
                    val = 1;
                }
                break;
            }
            friggrimmer2 = 1;
            friggrimmer = 1;
            deselect_debounce = 0;
            go_deselect = 0;
            loading_or_saving = 0;
        }
    }
    InitialisePickUpDisplay();
    GLOBAL_lastinvitem = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
    update_laras_weapons_status();
    if (use_the_bitch)
        use_current_item();
    FreeMonoScreen();
    InventoryActive = 0;
    lara.Busy = oldLaraBusy;
    return return_value;
}

void init_new_inventry(void)
{
    sint32 i;

    compass_settle_thang = 4096;
    examine_mode = 0;
    stats_mode = 0;
    AlterFOV(14560);
    lara.Busy = 0;
    GLOBAL_inventoryitemchosen = -1;
    left_debounce = 0;
    right_debounce = 0;
    up_debounce = 0;
    down_debounce = 0;
    go_left = 0;
    go_right = 0;
    go_up = 0;
    go_down = 0;
    select_debounce = 0;
    deselect_debounce = 0;
    go_select = 0;
    go_deselect = 0;
    left_repeat = 0;
    right_repeat = 0;
    loading_or_saving = 0;
    use_the_bitch = 0;
    if (lara.num_shotgun_ammo1 != -1)
        AmountShotGunAmmo1 = lara.num_shotgun_ammo1 / 6;
    else
        AmountShotGunAmmo1 = -1;
    if (lara.num_shotgun_ammo2 != -1)
        AmountShotGunAmmo2 = lara.num_shotgun_ammo2 / 6;
    else
        AmountShotGunAmmo2 = -1;
    AmountGrenadeAmmo1 = lara.num_grenade_ammo1;
    AmountGrenadeAmmo2 = lara.num_grenade_ammo2;
    AmountGrenadeAmmo3 = lara.num_grenade_ammo3;
    AmountCrossBowAmmo1 = lara.num_crossbow_ammo1;
    AmountCrossBowAmmo2 = lara.num_crossbow_ammo2;
    AmountCrossBowAmmo3 = lara.num_crossbow_ammo3;
    AmountUziAmmo = lara.num_uzi_ammo;
    AmountRevolverAmmo = lara.num_revolver_ammo;
    AmountPistolsAmmo = lara.num_pistols_ammo;
    construct_object_list();
    if (GLOBAL_enterinventory != NO_ITEM) {
        if (have_i_got_object((sint16)GLOBAL_enterinventory))
            setup_objectlist_startposition2((sint16)GLOBAL_enterinventory);
        GLOBAL_enterinventory = -1;
    } else if (GLOBAL_lastinvitem != NO_ITEM) {
        if (have_i_got_item((sint16)GLOBAL_lastinvitem))
            setup_objectlist_startposition((sint16)GLOBAL_lastinvitem);
        else if (GLOBAL_lastinvitem >= INV_WATERSKIN1_EMPTY_ITEM && GLOBAL_lastinvitem <= INV_WATERSKIN1_3_ITEM) {
            for (i = INV_WATERSKIN1_EMPTY_ITEM; i <= INV_WATERSKIN1_3_ITEM; i++) {
                if (have_i_got_item((sint16)i)) {
                    setup_objectlist_startposition((sint16)i);
                    break;
                }
            }
        } else if (GLOBAL_lastinvitem >= INV_WATERSKIN2_EMPTY_ITEM && GLOBAL_lastinvitem <= INV_WATERSKIN2_5_ITEM) {
            for (i = INV_WATERSKIN2_EMPTY_ITEM; i <= INV_WATERSKIN2_5_ITEM; i++) {
                if (have_i_got_item((sint16)i)) {
                    setup_objectlist_startposition((sint16)i);
                    break;
                }
            }
        }
        GLOBAL_lastinvitem = -1;
    }
    ammo_selector_fade_val = 0;
    ammo_selector_fade_dir = 0;
    combine_ring_fade_val = 0;
    combine_ring_fade_dir = 0;
    combine_type_flag = 0;
    seperate_type_flag = 0;
    combine_obj1 = 0;
    combine_obj2 = 0;
    normal_ring_fade_val = 128;
    normal_ring_fade_dir = 0;
    handle_object_changeover(RING_INVENTORY);
}

void do_debounced_joystick_poo(void)
{
    go_left = 0;
    go_right = 0;
    go_up = 0;
    go_down = 0;
    go_select = 0;
    go_deselect = 0;
    if (input & IN_LEFT) {
        if (left_repeat < 8)
            left_repeat++;
        else
            go_left = 1;
        if (!left_debounce)
            go_left = 1;
        left_debounce = 1;
    } else {
        left_debounce = 0;
        left_repeat = 0;
    }
    if (input & IN_RIGHT) {
        if (right_repeat < 8)
            right_repeat++;
        else
            go_right = 1;
        if (!right_debounce)
            go_right = 1;
        right_debounce = 1;
    } else {
        right_debounce = 0;
        right_repeat = 0;
    }
    if (input & IN_FORWARD) {
        if (!up_debounce)
            go_up = 1;
        up_debounce = 1;
    } else
        up_debounce = 0;
    if (input & IN_BACK) {
        if (!down_debounce)
            go_down = 1;
        down_debounce = 1;
    } else
        down_debounce = 0;
    if (input & IN_ACTION || input & IN_SELECT)
        select_debounce = 1;
    else {
        if (select_debounce == 1 && !friggrimmer)
            go_select = 1;
        select_debounce = 0;
        friggrimmer = 0;
    }
    if (input & IN_DESELECT)
        deselect_debounce = 1;
    else {
        if (deselect_debounce == 1 && !friggrimmer2)
            go_deselect = 1;
        deselect_debounce = 0;
        friggrimmer2 = 0;
    }
}

void DrawInventoryItemMe(ITEM_INFO *item, sint32 shade, sint32 overlay, sint32 shagflag)
{
    ANIM_STRUCT *anim;
    OBJECT_INFO *object;
    PHD_VECTOR vec;
    sint16 **meshpp;
    sint32 *bone, i;
    sint16 *rotation1;
    sint16 *frmptr;
    uint32 bit;
    sint32 poppush, alpha, compass;

    anim = &anims[item->anim_number];
    frmptr = anim->frame_ptr;
    object = &objects[item->object_number];
    phd_PushMatrix();
    phd_TranslateRel(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    if (item->object_number >= EXAMINE1 && item->object_number <= EXAMINE3 && examine_mode) {
        if (item->object_number == EXAMINE1) {
            vec.x = 7168;
            vec.y = 4608;
            vec.z = 4096;
            ScaleCurrentMatrix(&vec);
        } else if (item->object_number == EXAMINE2) {
            vec.x = 5760;
            vec.y = 5120;
            vec.z = 4096;
            ScaleCurrentMatrix(&vec);
        }
    }
    bit = 1;
    meshpp = &meshes[object->mesh_index];
    bone = &bones[object->bone_index];
    if (!shagflag)
        phd_TranslateRel(frmptr[6], frmptr[7], frmptr[8]);
    rotation1 = frmptr + 9;
    gar_RotYXZsuperpack(&rotation1, 0);
    if (item->mesh_bits & 1) {
        if (overlay)
            phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
        else {
            alpha = GlobalAlpha;
            if (shade > 127)
                shade = 0xFF;
            else
                shade <<= 1;
            GlobalAlpha = shade << 24;
            phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
            GlobalAlpha = alpha;
        }
    }
    meshpp += 2;
    for (i = 0; i < object->nmeshes - 1; i++, bone += 4, meshpp += 2) {
        poppush = *bone;
        if (poppush & 1)
            phd_PopMatrix();
        if (poppush & 2)
            phd_PushMatrix();
        phd_TranslateRel(bone[1], bone[2], bone[3]);
        gar_RotYXZsuperpack(&rotation1, 0);
        if (item->object_number == COMPASS_ITEM) {
            compass = ((rcossin_tbl[((GnFrameCounter & 0x3F) << 10) >> 3] << 2) * compass_settle_thang) >> W2V_SHIFT;
            compass += lara_item->pos.y_rot;
            phd_RotY((sint16)(compass - 0x8000));
            if (lara_item->pos.y_rot > -48 && lara_item->pos.y_rot <= 48) {
                shade = 96;
                if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_SMALLMEDI_ITEM) {
                    if (keymap[DIK_G] && keymap[DIK_U] && keymap[DIK_N] && keymap[DIK_S]) {
                        lara.num_large_medipack = -1;
                        lara.num_small_medipack = -1;
                        lara.num_revolver_ammo = -1;
                        lara.num_uzi_ammo = -1;
                        lara.num_crossbow_ammo3 = -1;
                        lara.num_crossbow_ammo2 = -1;
                        lara.num_crossbow_ammo1 = -1;
                        lara.num_grenade_ammo3 = -1;
                        lara.num_grenade_ammo2 = -1;
                        lara.num_grenade_ammo1 = -1;
                        lara.num_flares = -1;
                        lara.num_shotgun_ammo2 = -1;
                        lara.num_shotgun_ammo1 = -1;
                        if (!(gfLevelFlags & GF_YOUNGLARA)) {
                            lara.pistols_type_carried |= 1;
                            lara.uzis_type_carried |= 1;
                            lara.shotgun_type_carried |= 1;
                            lara.crossbow_type_carried |= 1;
                            lara.grenade_type_carried |= 1;
                            lara.sixshooter_type_carried |= 1;
                        }
                    }
                }
                if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_BIGMEDI_ITEM) {
                    if (keymap[DIK_B] && keymap[DIK_I] && keymap[DIK_T] && keymap[DIK_S])
                        dels_give_lara_items_cheat();
                }
                if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem == INV_MEMCARD_LOAD_ITEM) {
                    if (keymap[DIK_H] && keymap[DIK_E] && keymap[DIK_L] && keymap[DIK_P]) {
                        if (gfCurrentLevel == 2 || gfCurrentLevel == 6 || gfCurrentLevel == 13 || gfCurrentLevel == 21 || gfCurrentLevel == 27)
                            skipped_level = 1;
                        gfRequiredStartPos = 0;
                        gfLevelComplete = gfCurrentLevel + 1;
                    }
                }
            }
        }
        bit <<= 1;
        if (bit & item->mesh_bits) {
            if (overlay)
                phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
            else {
                alpha = GlobalAlpha;
                if (shade > 127)
                    shade = 0xFF;
                else
                    shade <<= 1;
                GlobalAlpha = shade << 25;
                phd_PutPolygonsPickup(*meshpp, (float)xoffset, (float)yoffset, pcbright);
                GlobalAlpha = alpha;
            }
        }
    }
    phd_PopMatrix();
}

sint32 go_and_load_game(void)
{
    return LoadGame();
}

sint32 go_and_save_game(void)
{
    return SaveGame();
}

void construct_combine_object_list(void)
{
    sint32 i;

    rings[RING_AMMO]->numobjectsinlist = 0;
    for (i = 0; i < NUM_INVOBJ; i++)
        rings[RING_AMMO]->current_object_list[i].invitem = NO_ITEM;
    if (!(gfLevelFlags & GF_YOUNGLARA)) {
        if (lara.sixshooter_type_carried & 1) {
            if (lara.sixshooter_type_carried & 4)
                insert_object_into_list_v2(INV_REVOLVER_LASER_ITEM);
            else
                insert_object_into_list_v2(INV_REVOLVER_ITEM);
        }
        if (lara.crossbow_type_carried & 1) {
            if (lara.crossbow_type_carried & 4)
                insert_object_into_list_v2(INV_CROSSBOW_LASER_ITEM);
            else
                insert_object_into_list_v2(INV_CROSSBOW_ITEM);
        }
        if (lara.lasersight)
            insert_object_into_list_v2(INV_LASERSIGHT_ITEM);
    }
    if (lara.small_water_skin)
        insert_object_into_list_v2(lara.small_water_skin + INV_WATERSKIN1_EMPTY_ITEM - 1);
    if (lara.big_water_skin)
        insert_object_into_list_v2(lara.big_water_skin + INV_WATERSKIN2_EMPTY_ITEM - 1);
    for (i = 0; i < 16; i++) {
        if ((1 << i) & lara.puzzleitemscombo)
            insert_object_into_list_v2(i + INV_PUZZLE1_COMBO1_ITEM);
    }
    for (i = 0; i < 16; i++) {
        if ((1 << i) & lara.keyitemscombo)
            insert_object_into_list_v2(i + INV_KEY1_COMBO1_ITEM);
    }
    for (i = 0; i < 8; i++) {
        if ((1 << i) & lara.pickupitemscombo)
            insert_object_into_list_v2(i + INV_PICKUP1_COMBO1_ITEM);
    }
    if (lara.mechanical_scarab & 2)
        insert_object_into_list_v2(INV_WINDING_KEY_ITEM);
    if (lara.mechanical_scarab & 4)
        insert_object_into_list_v2(INV_MECHANICAL_SCARAB_ITEM);
    rings[RING_AMMO]->objlistmovement = 0;
    rings[RING_AMMO]->curobjinlist = 0;
    rings[RING_AMMO]->ringactive = 0;
}

void insert_object_into_list_v2(sint32 num)
{
    if (options_table[num] & (OPT_UNUSED | OPT_COMBINE)) {
        if (rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem != num) {
            rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].invitem = num;
            rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].yrot = 0;
            rings[RING_AMMO]->current_object_list[rings[RING_AMMO]->numobjectsinlist].bright = 32;
            rings[RING_AMMO]->numobjectsinlist++;
        }
    }
}

void construct_object_list(void)
{
    sint32 i;

    rings[RING_INVENTORY]->numobjectsinlist = 0;
    for (i = 0; i < NUM_INVOBJ; i++)
        rings[RING_INVENTORY]->current_object_list[i].invitem = NO_ITEM;
    CurrentPistolsAmmoType = 0;
    CurrentUziAmmoType = 0;
    CurrentRevolverAmmoType = 0;
    CurrentShotGunAmmoType = 0;
    CurrentGrenadeGunAmmoType = 0;
    CurrentCrossBowAmmoType = 0;
    if (!(gfLevelFlags & GF_YOUNGLARA)) {
        if (lara.pistols_type_carried & 1)
            insert_object_into_list(INV_PISTOLS_ITEM);
        if (lara.uzis_type_carried & 1)
            insert_object_into_list(INV_UZI_ITEM);
        else if (AmountUziAmmo)
            insert_object_into_list(INV_UZI_AMMO_ITEM);
        if (lara.sixshooter_type_carried & 1) {
            if (lara.sixshooter_type_carried & 4)
                insert_object_into_list(INV_REVOLVER_LASER_ITEM);
            else
                insert_object_into_list(INV_REVOLVER_ITEM);
        } else if (AmountRevolverAmmo)
            insert_object_into_list(INV_REVOLVER_AMMO_ITEM);
        if (lara.shotgun_type_carried & 1) {
            insert_object_into_list(INV_SHOTGUN_ITEM);
            if (lara.shotgun_type_carried & 0x10)
                CurrentShotGunAmmoType = 1;
        } else {
            if (AmountShotGunAmmo1)
                insert_object_into_list(INV_SHOTGUN_AMMO1_ITEM);
            if (AmountShotGunAmmo2)
                insert_object_into_list(INV_SHOTGUN_AMMO2_ITEM);
        }
        if (lara.grenade_type_carried & 1) {
            insert_object_into_list(INV_GRENADEGUN_ITEM);
            if (lara.grenade_type_carried & 0x10)
                CurrentGrenadeGunAmmoType = 1;
            else if (lara.grenade_type_carried & 0x20)
                CurrentGrenadeGunAmmoType = 2;
        } else {
            if (AmountGrenadeAmmo1)
                insert_object_into_list(INV_GRENADEGUN_AMMO1_ITEM);
            if (AmountGrenadeAmmo2)
                insert_object_into_list(INV_GRENADEGUN_AMMO2_ITEM);
            if (AmountGrenadeAmmo3)
                insert_object_into_list(INV_GRENADEGUN_AMMO3_ITEM);
        }
        if (lara.crossbow_type_carried & 1) {
            if (lara.crossbow_type_carried & 4)
                insert_object_into_list(INV_CROSSBOW_LASER_ITEM);
            else
                insert_object_into_list(INV_CROSSBOW_ITEM);
            if (lara.crossbow_type_carried & 0x10)
                CurrentCrossBowAmmoType = 1;
            else if (lara.crossbow_type_carried & 0x20)
                CurrentCrossBowAmmoType = 2;
        } else {
            if (AmountCrossBowAmmo1)
                insert_object_into_list(INV_CROSSBOW_AMMO1_ITEM);
            if (AmountCrossBowAmmo2)
                insert_object_into_list(INV_CROSSBOW_AMMO2_ITEM);
            if (AmountCrossBowAmmo3)
                insert_object_into_list(INV_CROSSBOW_AMMO3_ITEM);
        }
        if (lara.lasersight)
            insert_object_into_list(INV_LASERSIGHT_ITEM);
        if (lara.binoculars)
            insert_object_into_list(INV_BINOCULARS_ITEM);
        if (lara.num_flares)
            insert_object_into_list(INV_FLARE_INV_ITEM);
    }
    if (lara.num_small_medipack)
        insert_object_into_list(INV_SMALLMEDI_ITEM);
    if (lara.num_large_medipack)
        insert_object_into_list(INV_BIGMEDI_ITEM);
    if (lara.small_water_skin)
        insert_object_into_list(lara.small_water_skin + INV_WATERSKIN1_EMPTY_ITEM - 1);
    if (lara.big_water_skin)
        insert_object_into_list(lara.big_water_skin + INV_WATERSKIN2_EMPTY_ITEM - 1);
    if (lara.crowbar)
        insert_object_into_list(INV_CROWBAR_ITEM);
    if (lara.mechanical_scarab & 1)
        insert_object_into_list(INV_CLOCKWORK_BEETLE_ITEM);
    if (lara.mechanical_scarab & 2)
        insert_object_into_list(INV_WINDING_KEY_ITEM);
    if (lara.mechanical_scarab & 4)
        insert_object_into_list(INV_MECHANICAL_SCARAB_ITEM);
    for (i = 0; i < 12; i++) {
        if (lara.puzzleitems[i])
            insert_object_into_list(i + INV_PUZZLE1_ITEM);
    }
    for (i = 0; i < 16; i++) {
        if ((1 << i) & lara.puzzleitemscombo)
            insert_object_into_list(i + INV_PUZZLE1_COMBO1_ITEM);
    }
    for (i = 0; i < 12; i++) {
        if ((1 << i) & lara.keyitems)
            insert_object_into_list(i + INV_KEY1_ITEM);
    }
    for (i = 0; i < 16; i++) {
        if ((1 << i) & lara.keyitemscombo)
            insert_object_into_list(i + INV_KEY1_COMBO1_ITEM);
    }
    for (i = 0; i < 4; i++) {
        if ((1 << i) & lara.pickupitems)
            insert_object_into_list(i + INV_PICKUP1_ITEM);
    }
    for (i = 0; i < 8; i++) {
        if ((1 << i) & lara.pickupitemscombo)
            insert_object_into_list(i + INV_PICKUP1_COMBO1_ITEM);
    }
    for (i = 0; i < 6; i++) {
        if ((1 << i) & lara.questitems)
            insert_object_into_list(i + INV_QUEST1_ITEM);
    }
    if (lara.examine1)
        insert_object_into_list(INV_EXAMINE1_ITEM);
    if (lara.examine2)
        insert_object_into_list(INV_EXAMINE2_ITEM);
    if (lara.examine3)
        insert_object_into_list(INV_EXAMINE3_ITEM);
    if (Gameflow->LoadSaveEnabled) {
        insert_object_into_list(INV_MEMCARD_LOAD_ITEM);
        insert_object_into_list(INV_MEMCARD_SAVE_ITEM);
    }
    rings[RING_INVENTORY]->objlistmovement = 0;
    rings[RING_INVENTORY]->curobjinlist = 0;
    rings[RING_AMMO]->objlistmovement = 0;
    rings[RING_AMMO]->curobjinlist = 0;
    handle_object_changeover(RING_INVENTORY);
    rings[RING_INVENTORY]->ringactive = 1;
    rings[RING_AMMO]->ringactive = 0;
    ammo_active = 0;
}

void insert_object_into_list(sint32 num)
{
    rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].invitem = num;
    rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].yrot = 0;
    rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->numobjectsinlist].bright = 32;
    rings[RING_INVENTORY]->numobjectsinlist++;
}

void handle_object_changeover(sint32 ringnum)
{
    current_selected_option = 0;
    menu_active = 1;
    Func43dbf0();
}

void fade_ammo_selector(void)
{
    if (rings[RING_INVENTORY]->ringactive && (right_repeat >= 8 || left_repeat >= 8))
        ammo_selector_fade_val = 0;
    else if (ammo_selector_fade_dir == 1) {
        if (ammo_selector_fade_val < 128)
            ammo_selector_fade_val += 32;
        if (ammo_selector_fade_val > 128) {
            ammo_selector_fade_val = 128;
            ammo_selector_fade_dir = 0;
        }
    } else if (ammo_selector_fade_dir == 2) {
        if (ammo_selector_fade_val > 0)
            ammo_selector_fade_val -= 32;
        if (ammo_selector_fade_val < 0) {
            ammo_selector_fade_val = 0;
            ammo_selector_fade_dir = 0;
        }
    }
}

void draw_ammo_selector(void)
{
    sint32 xpos;
    sint32 i;
    sint16 yrot;
    char cunter[256];

    if (!ammo_selector_flag)
        return;
    xpos = (2 * phd_centerx - OBJLIST_SPACING) >> 1;
    if (num_ammo_slots == 2)
        xpos -= OBJLIST_SPACING / 2;
    else if (num_ammo_slots == 3)
        xpos -= OBJLIST_SPACING;
    for (i = 0; i < num_ammo_slots; i++) {
        if (i == *current_ammo_type) {
            if (inventry_objects_list[ammo_object_list[i].invitem].flags & 2)
                ammo_object_list[i].yrot += 1022;
        } else
            spinback(&ammo_object_list[i].yrot);
        yrot = ammo_object_list[i].yrot;
        if (i == *current_ammo_type) {
            if (ammo_object_list[i].amount == -1)
                sprintf(cunter, SCRIPT_TEXT(TXT_Unlimited_s), SCRIPT_TEXT(inventry_objects_list[ammo_object_list[i].invitem].objname));
            else
                sprintf(cunter, "%d x %s", ammo_object_list[i].amount, SCRIPT_TEXT(inventry_objects_list[ammo_object_list[i].invitem].objname));
            if (ammo_selector_fade_val)
                PrintString((uint16)phd_centerx, (uint16)(font_height + phd_centery + 2 * font_height - 9), 8, cunter, FF_CENTER);
        }
        if (i == *current_ammo_type)
            DrawThreeDeeObject2D((sint32)(64.0F * ((float)phd_centerx * (1.0F / 256.0F)) + inventry_xpos + xpos), (sint32)(190.0F * ((float)phd_centery * (1.0F / 120.0F)) + inventry_ypos), ammo_object_list[i].invitem, ammo_selector_fade_val, 0, yrot, 0, 0, 0);
        else
            DrawThreeDeeObject2D((sint32)(64.0F * ((float)phd_centerx * (1.0F / 256.0F)) + inventry_xpos + xpos), (sint32)(190.0F * ((float)phd_centery * (1.0F / 120.0F)) + inventry_ypos), ammo_object_list[i].invitem, ammo_selector_fade_val, 0, yrot, 0, 1, 0);
        xpos += OBJLIST_SPACING;
    }
}

void spinback(uint16 *cock)
{
    uint16 val, val2;

    val = *cock;
    if (val) {
        if (val > 32768) {
            val2 = -val;
            if (val2 < 1022)
                val = 1022;
            if (val2 > 16384)
                val2 = 16384;
            val += val2 >> 3;
            if (val < 32768)
                val = 0;
        } else {
            val2 = val;
            if (val2 < 1022)
                val = 1022;
            if (val2 > 16384)
                val2 = 16384;
            val -= val2 >> 3;
            if (val > 32768)
                val = 0;
        }
        *cock = val;
    }
}

void update_laras_weapons_status(void)
{
    if (lara.shotgun_type_carried & 1) {
        lara.shotgun_type_carried &= 0xC7;
        if (!CurrentShotGunAmmoType)
            lara.shotgun_type_carried |= 0x8;
        else
            lara.shotgun_type_carried |= 0x10;
    }
    if (lara.grenade_type_carried & 1) {
        lara.grenade_type_carried &= 0xC7;
        if (!CurrentGrenadeGunAmmoType)
            lara.grenade_type_carried |= 0x8;
        else if (CurrentGrenadeGunAmmoType == 1)
            lara.grenade_type_carried |= 0x10;
        else if (CurrentGrenadeGunAmmoType == 2)
            lara.grenade_type_carried |= 0x20;
    }
    if (lara.crossbow_type_carried & 1) {
        lara.crossbow_type_carried &= 0xC7;
        if (!CurrentCrossBowAmmoType)
            lara.crossbow_type_carried |= 0x8;
        else if (CurrentCrossBowAmmoType == 1)
            lara.crossbow_type_carried |= 0x10;
        else if (CurrentCrossBowAmmoType == 2)
            lara.crossbow_type_carried |= 0x20;
    }
}

sint32 is_item_currently_combinable(sint16 obj)
{
    sint32 i;

    if (obj < INV_WATERSKIN1_EMPTY_ITEM || obj > INV_WATERSKIN2_5_ITEM) {
        for (i = 0; i < 23; i++) {
            if (dels_handy_combine_table[i].item1 == obj && have_i_got_item(dels_handy_combine_table[i].item2))
                return 1;
            if (dels_handy_combine_table[i].item2 == obj && have_i_got_item(dels_handy_combine_table[i].item1))
                return 1;
        }
        return 0;
    } else if (obj <= INV_WATERSKIN1_3_ITEM) {
        for (i = 0; i < 6; i++) {
            if (have_i_got_item(i + INV_WATERSKIN2_EMPTY_ITEM))
                return 1;
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (have_i_got_item(i + INV_WATERSKIN1_EMPTY_ITEM))
                return 1;
        }
    }
}

sint32 have_i_got_item(sint16 obj)
{
    sint32 i;

    for (i = 0; i < NUM_INVOBJ; i++) {
        if (obj == rings[RING_INVENTORY]->current_object_list[i].invitem)
            return 1;
    }
    return 0;
}

sint32 do_these_objects_combine(sint32 obj1, sint32 obj2)
{
    sint32 i;

    for (i = 0; i < 23; i++) {
        if (dels_handy_combine_table[i].item1 == obj1 && dels_handy_combine_table[i].item2 == obj2)
            return 1;
        if (dels_handy_combine_table[i].item1 == obj2 && dels_handy_combine_table[i].item2 == obj1)
            return 1;
    }
    return 0;
}

void combine_these_two_objects(sint16 obj1, sint16 obj2)
{
    sint32 i;
    sint16 obj;

    for (i = 0; i < 23; i++) {
        if (dels_handy_combine_table[i].item1 == obj1 && dels_handy_combine_table[i].item2 == obj2)
            break;
        if (dels_handy_combine_table[i].item1 == obj2 && dels_handy_combine_table[i].item2 == obj1)
            break;
    }
    obj = dels_handy_combine_table[i].combined_item;
    dels_handy_combine_table[i].combine_routine(0);
    construct_object_list();
    setup_objectlist_startposition(obj);
    handle_object_changeover(RING_INVENTORY);
}

void seperate_object(sint16 obj)
{
    sint32 i;
    sint16 item;

    for (i = 0; i < 23; i++) {
        if (dels_handy_combine_table[i].combined_item == obj)
            break;
    }
    item = dels_handy_combine_table[i].item1;
    dels_handy_combine_table[i].combine_routine(1);
    construct_object_list();
    setup_objectlist_startposition(item);
}

void combine_revolver_lasersight(sint32 flag)
{
    if (!flag) {
        lara.lasersight = 0;
        lara.sixshooter_type_carried |= 4;
    } else {
        lara.lasersight = 1;
        lara.sixshooter_type_carried &= ~4;
    }
    if (lara.gun_status && lara.gun_type == WEAPON_REVOLVER) {
        undraw_pistol_mesh_right(WEAPON_REVOLVER);
        draw_pistol_meshes(WEAPON_REVOLVER);
    }
}

void combine_crossbow_lasersight(sint32 flag)
{
    if (!flag) {
        lara.lasersight = 0;
        lara.crossbow_type_carried |= 4;
    } else {
        lara.lasersight = 1;
        lara.crossbow_type_carried &= ~4;
    }
    if (lara.gun_status && lara.gun_type == WEAPON_CROSSBOW) {
        undraw_shotgun_meshes(WEAPON_CROSSBOW);
        draw_shotgun_meshes(WEAPON_CROSSBOW);
    }
}

void combine_PuzzleItem1(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x3;
    lara.puzzleitems[0] = 1;
}

void combine_PuzzleItem2(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC;
    lara.puzzleitems[1] = 1;
}

void combine_PuzzleItem3(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x30;
    lara.puzzleitems[2] = 1;
}

void combine_PuzzleItem4(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC0;
    lara.puzzleitems[3] = 1;
}

void combine_PuzzleItem5(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x300;
    lara.puzzleitems[4] = 1;
}

void combine_PuzzleItem6(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC00;
    lara.puzzleitems[5] = 1;
}

void combine_PuzzleItem7(sint32 flag)
{
    lara.puzzleitemscombo &= ~0x3000;
    lara.puzzleitems[6] = 1;
}

void combine_PuzzleItem8(sint32 flag)
{
    lara.puzzleitemscombo &= ~0xC000;
    lara.puzzleitems[7] = 1;
}

void combine_KeyItem1(sint32 flag)
{
    lara.keyitems |= 0x1;
    lara.keyitemscombo &= ~0x3;
}

void combine_KeyItem2(sint32 flag)
{
    lara.keyitems |= 0x2;
    lara.keyitemscombo &= ~0xC;
}

void combine_KeyItem3(sint32 flag)
{
    lara.keyitems |= 0x4;
    lara.keyitemscombo &= ~0x30;
}

void combine_KeyItem4(sint32 flag)
{
    lara.keyitems |= 0x8;
    lara.keyitemscombo &= ~0xC0;
}

void combine_KeyItem5(sint32 flag)
{
    lara.keyitems |= 0x10;
    lara.keyitemscombo &= ~0x300;
}

void combine_KeyItem6(sint32 flag)
{
    lara.keyitems |= 0x20;
    lara.keyitemscombo &= ~0xC00;
}

void combine_KeyItem7(sint32 flag)
{
    lara.keyitems |= 0x40;
    lara.keyitemscombo &= ~0x3000;
}

void combine_KeyItem8(sint32 flag)
{
    lara.keyitems |= 0x80;
    lara.keyitemscombo &= ~0xC000;
}

void combine_PickupItem1(sint32 flag)
{
    lara.pickupitems |= 0x1;
    lara.pickupitemscombo &= ~0x3;
}

void combine_PickupItem2(sint32 flag)
{
    lara.pickupitems |= 0x2;
    lara.pickupitemscombo &= ~0xC;
}

void combine_PickupItem3(sint32 flag)
{
    lara.pickupitems |= 0x4;
    lara.pickupitemscombo &= ~0x30;
}

void combine_PickupItem4(sint32 flag)
{
    lara.pickupitems |= 0x8;
    lara.pickupitemscombo &= ~0xC0;
}

void combine_ClockWorkBeetle(sint32 flag)
{
    lara.mechanical_scarab = 1;
}

sint32 do_special_waterskin_combine_bullshit(sint32 flag)
{
    sint32 n;
    sint32 small_liters, big_liters, small_capacity, big_capacity;

    small_liters = lara.small_water_skin - 1;
    big_liters = lara.big_water_skin - 1;
    small_capacity = 3 - small_liters;
    big_capacity = 5 - big_liters;
    if (!flag) {
        if (!small_liters)
            return 0;
        if (!big_capacity)
            return 0;
        n = small_liters;
        do {
            if (big_capacity) {
                small_liters--;
                big_liters++;
                big_capacity--;
            }
            n--;
        } while (n);
        lara.small_water_skin = small_liters + 1;
        lara.big_water_skin = big_liters + 1;
        combine_obj1 = lara.big_water_skin + INV_WATERSKIN2_EMPTY_ITEM - 1;
        return 1;
    } else {
        if (!big_liters)
            return 0;
        if (!small_capacity)
            return 0;
        n = big_liters;
        do {
            if (small_capacity) {
                big_liters--;
                small_liters++;
                small_capacity--;
            }
            n--;
        } while (n);
        lara.small_water_skin = small_liters + 1;
        lara.big_water_skin = big_liters + 1;
        combine_obj1 = lara.small_water_skin + INV_WATERSKIN1_EMPTY_ITEM - 1;
        return 1;
    }
}

void setup_objectlist_startposition(sint16 newobj)
{
    sint32 i;

    for (i = 0; i < NUM_INVOBJ; i++) {
        if (newobj == rings[RING_INVENTORY]->current_object_list[i].invitem)
            rings[RING_INVENTORY]->curobjinlist = i;
    }
}

void setup_objectlist_startposition2(sint16 newobj)
{
    sint32 i;

    for (i = 0; i < NUM_INVOBJ; i++) {
        if (newobj == inventry_objects_list[rings[RING_INVENTORY]->current_object_list[i].invitem].object_number)
            rings[RING_INVENTORY]->curobjinlist = i;
    }
}

void use_current_item(void)
{
    sint32 OldBinocular;
    sint16 invobject, gmeobject;

    OldBinocular = BinocularRange;
    BinocularRange = 0;
    oldLaraBusy = 0;
    lara_item->mesh_bits = -1;
    invobject = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
    gmeobject = inventry_objects_list[invobject].object_number;
    if (lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE) {
        if (gmeobject == PISTOLS_ITEM) {
            lara.request_gun_type = WEAPON_PISTOLS;
            if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_PISTOLS)
                lara.gun_status = LG_DRAW_GUNS;
            return;
        }
        if (gmeobject == SHOTGUN_ITEM) {
            lara.request_gun_type = WEAPON_SHOTGUN;
            if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_SHOTGUN)
                lara.gun_status = LG_DRAW_GUNS;
            return;
        }
        if (gmeobject == UZI_ITEM) {
            lara.request_gun_type = WEAPON_UZI;
            if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_UZI)
                lara.gun_status = LG_DRAW_GUNS;
            return;
        }
        if (gmeobject == SIXSHOOTER_ITEM) {
            lara.request_gun_type = WEAPON_REVOLVER;
            if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_REVOLVER)
                lara.gun_status = LG_DRAW_GUNS;
            return;
        }
        if (gmeobject == GRENADE_GUN_ITEM) {
            lara.request_gun_type = WEAPON_GRENADE;
            if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_GRENADE)
                lara.gun_status = LG_DRAW_GUNS;
            return;
        }
        if (gmeobject == CROSSBOW_ITEM) {
            lara.request_gun_type = WEAPON_CROSSBOW;
            if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_CROSSBOW)
                lara.gun_status = LG_DRAW_GUNS;
            return;
        }
    }
    if (gmeobject == FLARE_INV_ITEM) {
        if (lara.gun_status == LG_NO_ARMS && lara_item->current_anim_state != AS_ALL4S && lara_item->current_anim_state != AS_CRAWL && lara_item->current_anim_state != AS_ALL4TURNL && lara_item->current_anim_state != AS_ALL4TURNR && lara_item->current_anim_state != AS_CRAWLBACK && lara_item->current_anim_state != AS_CRAWL2HANG) {
            if (lara.gun_type != WEAPON_FLARE) {
                input = IN_FLARE;
                LaraGun();
                input = 0;
            }
        } else
            SayNo();
    } else if (invobject == INV_BINOCULARS_ITEM) {
        if ((lara_item->current_anim_state == AS_STOP && lara_item->anim_number == ANIM_BREATH) || (lara.IsDucked && !(input & IN_DUCK))) {
            oldLaraBusy = 1;
            BinocularRange = 128;
            if (lara.gun_status != LG_NO_ARMS)
                lara.gun_status = LG_UNDRAW_GUNS;
        }
        if (OldBinocular)
            BinocularRange = OldBinocular;
        else
            BinocularOldCamera = camera.old_type;
    } else if (invobject == INV_SMALLMEDI_ITEM) {
        if ((lara_item->hit_points > 0 && lara_item->hit_points < 1000) || lara.poisoned) {
            if (lara.num_small_medipack != -1)
                lara.num_small_medipack--;
            lara.dpoisoned = 0;
            lara_item->hit_points += 500;
            if (lara_item->hit_points > 1000)
                lara_item->hit_points = 1000;
            SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
            savegame.Game.HealthUsed++;
        } else
            SayNo();
    } else if (invobject == INV_BIGMEDI_ITEM) {
        if ((lara_item->hit_points > 0 && lara_item->hit_points < 1000) || lara.poisoned) {
            if (lara.num_large_medipack != -1)
                lara.num_large_medipack--;
            lara.dpoisoned = 0;
            lara_item->hit_points += 1000;
            if (lara_item->hit_points > 1000)
                lara_item->hit_points = 1000;
            SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
            savegame.Game.HealthUsed++;
        } else
            SayNo();
    } else
        GLOBAL_inventoryitemchosen = gmeobject;
}

void DEL_picked_up_object(sint16 objnum)
{
    if (objnum == WATERSKIN1_EMPTY) {
        lara.small_water_skin = 1;
    } else if (objnum == WATERSKIN2_EMPTY) {
        lara.big_water_skin = 1;
    } else if (objnum == UZI_ITEM) {
        if (!(lara.uzis_type_carried & 1))
            lara.uzis_type_carried = 9;
        if (lara.num_uzi_ammo != -1)
            lara.num_uzi_ammo += 30;
    } else if (objnum == PISTOLS_ITEM) {
        if (!(lara.pistols_type_carried & 1))
            lara.pistols_type_carried = 9;
        lara.num_pistols_ammo = -1;
    } else if (objnum == SHOTGUN_ITEM) {
        if (!(lara.shotgun_type_carried & 1))
            lara.shotgun_type_carried = 9;
        if (lara.num_shotgun_ammo1 != -1)
            lara.num_shotgun_ammo1 += 36;
    } else if (objnum == SIXSHOOTER_ITEM) {
        if (!(lara.sixshooter_type_carried & 1))
            lara.sixshooter_type_carried = 9;
        if (lara.num_revolver_ammo != -1)
            lara.num_revolver_ammo += 6;
    } else if (objnum == CROSSBOW_ITEM) {
        if (!(lara.crossbow_type_carried & 1))
            lara.crossbow_type_carried = 9;
        if (lara.num_crossbow_ammo1 != -1)
            lara.num_crossbow_ammo1 += 10;
    } else if (objnum == GRENADE_GUN_ITEM) {
        if (!(lara.grenade_type_carried & 1))
            lara.grenade_type_carried = 9;
        if (lara.num_grenade_ammo1 != -1)
            lara.num_grenade_ammo1 += 10;
    } else if (objnum == SHOTGUN_AMMO1_ITEM) {
        if (lara.num_shotgun_ammo1 != -1)
            lara.num_shotgun_ammo1 += 36;
    } else if (objnum == SHOTGUN_AMMO2_ITEM) {
        if (lara.num_shotgun_ammo2 != -1)
            lara.num_shotgun_ammo2 += 36;
    } else if (objnum == GRENADE_GUN_AMMO1_ITEM) {
        if (lara.num_grenade_ammo1 != -1)
            lara.num_grenade_ammo1 += 4;
    } else if (objnum == GRENADE_GUN_AMMO2_ITEM) {
        if (lara.num_grenade_ammo2 != -1)
            lara.num_grenade_ammo2 += 4;
    } else if (objnum == GRENADE_GUN_AMMO3_ITEM) {
        if (lara.num_grenade_ammo3 != -1)
            lara.num_grenade_ammo3 += 4;
    } else if (objnum == CROSSBOW_AMMO1_ITEM) {
        if (lara.num_crossbow_ammo1 != -1)
            lara.num_crossbow_ammo1 += 10;
    } else if (objnum == CROSSBOW_AMMO2_ITEM) {
        if (lara.num_crossbow_ammo2 != -1)
            lara.num_crossbow_ammo2 += 10;
    } else if (objnum == CROSSBOW_AMMO3_ITEM) {
        if (lara.num_crossbow_ammo3 != -1)
            lara.num_crossbow_ammo3 += 10;
    } else if (objnum == SIXSHOOTER_AMMO_ITEM) {
        if (lara.num_revolver_ammo != -1)
            lara.num_revolver_ammo += 6;
    } else if (objnum == UZI_AMMO_ITEM) {
        if (lara.num_uzi_ammo != -1)
            lara.num_uzi_ammo += 30;
    } else if (objnum == FLARE_INV_ITEM) {
        if (lara.num_flares != -1)
            lara.num_flares += 12;
    } else if (objnum == LASERSIGHT_ITEM) {
        if (!((lara.grenade_type_carried | lara.crossbow_type_carried | lara.sixshooter_type_carried |
               lara.shotgun_type_carried | lara.pistols_type_carried | lara.uzis_type_carried) & 4))
            lara.lasersight = 1;
    } else if (objnum == BIGMEDI_ITEM) {
        if (lara.num_large_medipack != -1)
            lara.num_large_medipack++;
    } else if (objnum == SMALLMEDI_ITEM) {
        if (lara.num_small_medipack != -1)
            lara.num_small_medipack++;
    } else if (objnum == BINOCULARS_ITEM) {
        lara.binoculars = 1;
    } else if (objnum >= PUZZLE_ITEM1_COMBO1 && objnum <= PUZZLE_ITEM8_COMBO2) {
        lara.puzzleitemscombo |= 1 << (objnum - PUZZLE_ITEM1_COMBO1);
    } else if (objnum >= PUZZLE_ITEM1 && objnum <= PUZZLE_ITEM12) {
        lara.puzzleitems[objnum - PUZZLE_ITEM1]++;
    } else if (objnum >= KEY_ITEM1_COMBO1 && objnum <= KEY_ITEM8_COMBO2) {
        lara.keyitemscombo |= 1 << (objnum - KEY_ITEM1_COMBO1);
    } else if (objnum >= KEY_ITEM1 && objnum <= KEY_ITEM12) {
        lara.keyitems |= 1 << (objnum - KEY_ITEM1);
    } else if (objnum >= PICKUP_ITEM1_COMBO1 && objnum <= PICKUP_ITEM4_COMBO2) {
        lara.pickupitemscombo |= 1 << (objnum - PICKUP_ITEM1_COMBO1);
    } else if (objnum >= PICKUP_ITEM1 && objnum <= PICKUP_ITEM4) {
        lara.pickupitems |= 1 << (objnum - PICKUP_ITEM1);
    } else if (objnum >= QUEST_ITEM1 && objnum <= QUEST_ITEM6) {
        lara.questitems |= 1 << (objnum - QUEST_ITEM1);
    } else if (objnum == CROWBAR_ITEM) {
        lara.crowbar = 1;
    } else if (objnum == CLOCKWORK_BEETLE) {
        lara.mechanical_scarab |= 1;
    } else if (objnum == CLOCKWORK_BEETLE_COMBO1) {
        lara.mechanical_scarab |= 2;
    } else if (objnum == CLOCKWORK_BEETLE_COMBO2) {
        lara.mechanical_scarab |= 4;
    } else if (objnum == EXAMINE1) {
        lara.examine1 = 1;
    } else if (objnum == EXAMINE2) {
        lara.examine2 = 1;
    } else if (objnum == EXAMINE3) {
        lara.examine3 = 1;
    }
}

sint32 have_i_got_object(sint16 object_number)
{
    if (object_number >= PUZZLE_ITEM1_COMBO1 && object_number <= PUZZLE_ITEM8_COMBO2)
        return (lara.puzzleitemscombo >> (object_number - PUZZLE_ITEM1_COMBO1)) & 1;
    if (object_number >= PUZZLE_ITEM1 && object_number <= PUZZLE_ITEM12)
        return lara.puzzleitems[object_number - PUZZLE_ITEM1];
    if (object_number >= KEY_ITEM1_COMBO1 && object_number <= KEY_ITEM8_COMBO2)
        return (lara.keyitemscombo >> (object_number - KEY_ITEM1_COMBO1)) & 1;
    if (object_number >= KEY_ITEM1 && object_number <= KEY_ITEM12)
        return (lara.keyitems >> (object_number - KEY_ITEM1)) & 1;
    if (object_number >= PICKUP_ITEM1_COMBO1 && object_number <= PICKUP_ITEM4_COMBO2)
        return (lara.pickupitemscombo >> (object_number - PICKUP_ITEM1_COMBO1)) & 1;
    if (object_number >= PICKUP_ITEM1 && object_number <= PICKUP_ITEM4)
        return (lara.pickupitems >> (object_number - PICKUP_ITEM1)) & 1;
    if (object_number >= QUEST_ITEM1 && object_number <= QUEST_ITEM6)
        return (lara.questitems >> (object_number - QUEST_ITEM1)) & 1;
    if (object_number == CROWBAR_ITEM)
        return lara.crowbar;
    return 0;
}

void remove_inventory_item(sint16 object_number)
{
    if (object_number >= PUZZLE_ITEM1_COMBO1 && object_number <= PUZZLE_ITEM8_COMBO2)
        lara.puzzleitemscombo &= ~(1 << (object_number - PUZZLE_ITEM1_COMBO1));
    else if (object_number >= PUZZLE_ITEM1 && object_number <= PUZZLE_ITEM12) {
        if (lara.puzzleitems[object_number - PUZZLE_ITEM1])
            lara.puzzleitems[object_number - PUZZLE_ITEM1]--;
    } else if (object_number >= KEY_ITEM1_COMBO1 && object_number <= KEY_ITEM8_COMBO2)
        lara.keyitemscombo &= ~(1 << (object_number - KEY_ITEM1_COMBO1));
    else if (object_number >= KEY_ITEM1 && object_number <= KEY_ITEM11)
        lara.keyitems &= ~(1 << (object_number - KEY_ITEM1));
    else if (object_number >= PICKUP_ITEM1_COMBO1 && object_number <= PICKUP_ITEM4_COMBO2)
        lara.pickupitemscombo &= ~(1 << (object_number - PICKUP_ITEM1_COMBO1));
    else if (object_number >= PICKUP_ITEM1 && object_number <= PICKUP_ITEM4)
        lara.pickupitems &= ~(1 << (object_number - PICKUP_ITEM1));
    else if (object_number >= QUEST_ITEM1 && object_number <= QUEST_ITEM6)
        lara.questitems &= ~(1 << (object_number - QUEST_ITEM1));
}

sint32 convert_obj_to_invobj(sint16 obj)
{
    sint32 i;

    for (i = 0; i < NUM_INVOBJ; i++) {
        if (obj == inventry_objects_list[i].object_number)
            return i;
    }
    return INV_MEMCARD_LOAD_ITEM;
}

void draw_compass(void)
{
    DrawThreeDeeObject2D((sint32)(60.0F * ((float)phd_centerx * (1.0F / 256.0F)) + inventry_xpos), phd_centery + inventry_ypos, INV_COMPASS_ITEM, 128, 0, 0, 0, 0, 0);
}

void do_examine_mode(void)
{
    INVOBJ *objme;
    static sint32 WANK_RULES_YPOS;
    static sint32 WANK_SCROL_YPOS;
    sint32 saved_scale;
    sint16 invitem;

    WANK_RULES_YPOS = font_height;
    WANK_SCROL_YPOS = font_height * 5;
    examine_mode += 8;
    if (examine_mode > 128)
        examine_mode = 128;
    invitem = rings[RING_INVENTORY]->current_object_list[rings[RING_INVENTORY]->curobjinlist].invitem;
    objme = &inventry_objects_list[invitem];
    if (invitem == INV_EXAMINE1_ITEM) {
        saved_scale = objme->scale1;
        objme->scale1 = 300;
        DrawThreeDeeObject2D((sint32)(256.0F * ((float)phd_centerx * (1.0F / 256.0F)) + inventry_xpos), (sint32)(256.0F * ((float)phd_centery * (1.0F / 120.0F)) + inventry_ypos) / 2, INV_EXAMINE1_ITEM, examine_mode, 0x8000, 0x4000, 0x4000, 96, 0);
        objme->scale1 = saved_scale;
    } else if (invitem == INV_EXAMINE2_ITEM) {
        saved_scale = objme->scale1;
        objme->scale1 = 300;
        DrawThreeDeeObject2D((sint32)(256.0F * ((float)phd_centerx * (1.0F / 256.0F)) + inventry_xpos), (sint32)(256.0F * ((float)phd_centery * (1.0F / 120.0F)) + inventry_ypos) / 2, INV_EXAMINE2_ITEM, examine_mode, 0, 0, 0, 0, 0);
        objme->scale1 = saved_scale;
        PrintString((uint16)phd_centerx, (uint16)WANK_RULES_YPOS, 5, SCRIPT_TEXT(TXT_RULES1), FF_CENTER);
        PrintString((uint16)phd_centerx, (uint16)(WANK_RULES_YPOS + phd_winheight / 2), 5, SCRIPT_TEXT(TXT_RULES2), FF_CENTER);
    } else if (invitem == INV_EXAMINE3_ITEM) {
        saved_scale = objme->scale1;
        objme->scale1 = 400;
        DrawThreeDeeObject2D((sint32)(256.0F * ((float)phd_centerx * (1.0F / 256.0F)) + inventry_xpos), (sint32)(256.0F * ((float)phd_centery * (1.0F / 120.0F)) + inventry_ypos) / 2 - 8, INV_EXAMINE3_ITEM, examine_mode, 0x8000, 0x4000, 0x4000, 96, 0);
        objme->scale1 = saved_scale;
        PrintString((uint16)phd_centerx, (uint16)WANK_SCROL_YPOS, 8, SCRIPT_TEXT(TXT_PETEPOO), FF_CENTER);
    }
    if (go_deselect) {
        SoundEffect(SFX_MENU_SELECT, 0, SFX_ALWAYS);
        go_deselect = 0;
        examine_mode = 0;
    }
}

void dels_give_lara_items_cheat(void)
{
    sint32 piss;

    if (objects[CROWBAR_ITEM].loaded)
        lara.crowbar = 1;
    if (objects[CLOCKWORK_BEETLE].loaded)
        lara.mechanical_scarab = 1;
    for (piss = 0; piss < 12; piss++) {
        if (objects[PUZZLE_ITEM1 + piss].loaded)
            lara.puzzleitems[piss] = 1;
    }
    for (piss = 0; piss < 12; piss++) {
        if (objects[KEY_ITEM1 + piss].loaded)
            lara.keyitems |= 1 << piss;
    }
    for (piss = 0; piss < 4; piss++) {
        if (objects[PICKUP_ITEM1 + piss].loaded)
            lara.pickupitems |= 1 << piss;
    }
    for (piss = 0; piss < 6; piss++) {
        if (objects[QUEST_ITEM1 + piss].loaded)
            lara.questitems |= 1 << piss;
    }
    if (objects[WATERSKIN1_EMPTY].loaded)
        lara.small_water_skin = 1;
    if (objects[WATERSKIN2_EMPTY].loaded)
        lara.big_water_skin = 1;
    lara.pickupitemscombo = 0;
    lara.puzzleitemscombo = 0;
    lara.keyitemscombo = 0;
}
