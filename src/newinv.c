#include "newinv.h"
#include "lara.h"
#include "objnums.h"
#include "lara1gun.h"
#include "lara2gun.h"
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

sint32 go_and_load_game(void)
{
    return LoadGame();
}

sint32 go_and_save_game(void)
{
    return SaveGame();
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
