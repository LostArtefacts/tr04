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

void insert_object_into_list(sint16 num)
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
