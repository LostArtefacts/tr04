#ifndef NEWINV_H
#define NEWINV_H

#include "types.h"

enum ring_types {
    RING_INVENTORY,
    RING_AMMO
};

enum option_types {
    OPT_UNUSED = 0x1,
    OPT_EQUIP = 0x2,
    OPT_USE = 0x4,
    OPT_COMBINE = 0x8,
    OPT_SEPARATE = 0x10,
    OPT_EXAMINE = 0x20,
    OPT_SHOTGUN = 0x40,
    OPT_CROSSBOW = 0x80,
    OPT_GRENADE = 0x100,
    OPT_UZI = 0x200,
    OPT_PISTOLS = 0x400,
    OPT_REVOLVER = 0x800,
    OPT_LOAD = 0x1000,
    OPT_SAVE = 0x2000
};

enum invobj_types {
    INV_UZI_ITEM = 0,
    INV_PISTOLS_ITEM,
    INV_SHOTGUN_ITEM,
    INV_REVOLVER_ITEM,
    INV_REVOLVER_LASER_ITEM,
    INV_CROSSBOW_ITEM,
    INV_CROSSBOW_LASER_ITEM,
    INV_GRENADEGUN_ITEM,
    INV_SHOTGUN_AMMO1_ITEM,
    INV_SHOTGUN_AMMO2_ITEM,
    INV_GRENADEGUN_AMMO1_ITEM,
    INV_GRENADEGUN_AMMO2_ITEM,
    INV_GRENADEGUN_AMMO3_ITEM,
    INV_CROSSBOW_AMMO1_ITEM,
    INV_CROSSBOW_AMMO2_ITEM,
    INV_CROSSBOW_AMMO3_ITEM,
    INV_REVOLVER_AMMO_ITEM,
    INV_UZI_AMMO_ITEM,
    INV_PISTOLS_AMMO_ITEM,
    INV_LASERSIGHT_ITEM,
    INV_BIGMEDI_ITEM,
    INV_SMALLMEDI_ITEM,
    INV_BINOCULARS_ITEM,
    INV_FLARE_INV_ITEM,
    INV_COMPASS_ITEM,
    INV_MEMCARD_LOAD_ITEM,
    INV_MEMCARD_SAVE_ITEM,
    INV_WATERSKIN1_EMPTY_ITEM,
    INV_WATERSKIN1_1_ITEM,
    INV_WATERSKIN1_2_ITEM,
    INV_WATERSKIN1_3_ITEM,
    INV_WATERSKIN2_EMPTY_ITEM,
    INV_WATERSKIN2_1_ITEM,
    INV_WATERSKIN2_2_ITEM,
    INV_WATERSKIN2_3_ITEM,
    INV_WATERSKIN2_4_ITEM,
    INV_WATERSKIN2_5_ITEM,
    INV_PUZZLE1_ITEM,
    INV_PUZZLE2_ITEM,
    INV_PUZZLE3_ITEM,
    INV_PUZZLE4_ITEM,
    INV_PUZZLE5_ITEM,
    INV_PUZZLE6_ITEM,
    INV_PUZZLE7_ITEM,
    INV_PUZZLE8_ITEM,
    INV_PUZZLE9_ITEM,
    INV_PUZZLE10_ITEM,
    INV_PUZZLE11_ITEM,
    INV_PUZZLE12_ITEM,
    INV_PUZZLE1_COMBO1_ITEM,
    INV_PUZZLE1_COMBO2_ITEM,
    INV_PUZZLE2_COMBO1_ITEM,
    INV_PUZZLE2_COMBO2_ITEM,
    INV_PUZZLE3_COMBO1_ITEM,
    INV_PUZZLE3_COMBO2_ITEM,
    INV_PUZZLE4_COMBO1_ITEM,
    INV_PUZZLE4_COMBO2_ITEM,
    INV_PUZZLE5_COMBO1_ITEM,
    INV_PUZZLE5_COMBO2_ITEM,
    INV_PUZZLE6_COMBO1_ITEM,
    INV_PUZZLE6_COMBO2_ITEM,
    INV_PUZZLE7_COMBO1_ITEM,
    INV_PUZZLE7_COMBO2_ITEM,
    INV_PUZZLE8_COMBO1_ITEM,
    INV_PUZZLE8_COMBO2_ITEM,
    INV_KEY1_ITEM,
    INV_KEY2_ITEM,
    INV_KEY3_ITEM,
    INV_KEY4_ITEM,
    INV_KEY5_ITEM,
    INV_KEY6_ITEM,
    INV_KEY7_ITEM,
    INV_KEY8_ITEM,
    INV_KEY9_ITEM,
    INV_KEY10_ITEM,
    INV_KEY11_ITEM,
    INV_KEY12_ITEM,
    INV_KEY1_COMBO1_ITEM,
    INV_KEY1_COMBO2_ITEM,
    INV_KEY2_COMBO1_ITEM,
    INV_KEY2_COMBO2_ITEM,
    INV_KEY3_COMBO1_ITEM,
    INV_KEY3_COMBO2_ITEM,
    INV_KEY4_COMBO1_ITEM,
    INV_KEY4_COMBO2_ITEM,
    INV_KEY5_COMBO1_ITEM,
    INV_KEY5_COMBO2_ITEM,
    INV_KEY6_COMBO1_ITEM,
    INV_KEY6_COMBO2_ITEM,
    INV_KEY7_COMBO1_ITEM,
    INV_KEY7_COMBO2_ITEM,
    INV_KEY8_COMBO1_ITEM,
    INV_KEY8_COMBO2_ITEM,
    INV_PICKUP1_ITEM,
    INV_PICKUP2_ITEM,
    INV_PICKUP3_ITEM,
    INV_PICKUP4_ITEM,
    INV_PICKUP1_COMBO1_ITEM,
    INV_PICKUP1_COMBO2_ITEM,
    INV_PICKUP2_COMBO1_ITEM,
    INV_PICKUP2_COMBO2_ITEM,
    INV_PICKUP3_COMBO1_ITEM,
    INV_PICKUP3_COMBO2_ITEM,
    INV_PICKUP4_COMBO1_ITEM,
    INV_PICKUP4_COMBO2_ITEM,
    INV_QUEST1_ITEM,
    INV_QUEST2_ITEM,
    INV_QUEST3_ITEM,
    INV_QUEST4_ITEM,
    INV_QUEST5_ITEM,
    INV_QUEST6_ITEM,
    INV_BURNING_TORCH_ITEM,
    INV_CROWBAR_ITEM,
    INV_CLOCKWORK_BEETLE_ITEM,
    INV_MECHANICAL_SCARAB_ITEM,
    INV_WINDING_KEY_ITEM,
    INV_EXAMINE1_ITEM,
    INV_EXAMINE2_ITEM,
    INV_EXAMINE3_ITEM,

    NUM_INVOBJ
};

typedef struct {
    sint16 object_number;
    sint16 yoff;
    sint16 scale1;
    sint16 yrot;
    sint16 xrot;
    sint16 zrot;
    sint16 flags;
    sint16 objname;
    uint32 meshbits;
} INVOBJ;

typedef struct {
    sint16 invitem;
    uint16 yrot;
    uint16 bright;
} OBJLIST;

typedef struct {
    OBJLIST current_object_list[119];
    sint32 ringactive;
    sint32 objlistmovement;
    sint32 curobjinlist;
    sint32 numobjectsinlist;
} RINGME;

typedef struct {
    sint16 invitem;
    sint16 amount;
    uint16 yrot;
} AMMOLIST;

typedef struct {
    void (*combine_routine)(sint32 flag);
    sint16 item1;
    sint16 item2;
    sint16 combined_item;
} COMBINELIST;

void construct_object_list(void);
void Func43dbf0(void);
void DrawThreeDeeObject2D(sint32 x, sint32 y, sint32 num, sint32 shade, sint32 xrot, sint32 yrot, sint32 zrot, sint32 bright, sint32 overlay);
sint32 LoadGame(void);
sint32 SaveGame(void);
void DEL_picked_up_object(sint16 objnum);
sint32 have_i_got_object(sint16 object_number);
sint32 go_and_load_game(void);
sint32 go_and_save_game(void);
void combine_PuzzleItem1(sint32 flag);
void combine_PuzzleItem2(sint32 flag);
void combine_PuzzleItem3(sint32 flag);
void combine_PuzzleItem4(sint32 flag);
void combine_PuzzleItem5(sint32 flag);
void combine_PuzzleItem6(sint32 flag);
void combine_PuzzleItem7(sint32 flag);
void combine_PuzzleItem8(sint32 flag);
void combine_KeyItem1(sint32 flag);
void combine_KeyItem2(sint32 flag);
void combine_KeyItem3(sint32 flag);
void combine_KeyItem4(sint32 flag);
void combine_KeyItem5(sint32 flag);
void combine_KeyItem6(sint32 flag);
void combine_KeyItem7(sint32 flag);
void combine_KeyItem8(sint32 flag);
void combine_PickupItem1(sint32 flag);
void combine_PickupItem2(sint32 flag);
void combine_PickupItem3(sint32 flag);
void combine_PickupItem4(sint32 flag);
void combine_ClockWorkBeetle(sint32 flag);
void combine_revolver_lasersight(sint32 flag);
void combine_crossbow_lasersight(sint32 flag);
void remove_inventory_item(sint16 object_number);
sint32 have_i_got_item(sint16 obj);
void handle_object_changeover(sint32 ringnum);
void setup_objectlist_startposition(sint16 newobj);
void setup_objectlist_startposition2(sint16 newobj);
sint32 convert_obj_to_invobj(sint16 obj);
void insert_object_into_list(sint16 num);
void insert_object_into_list_v2(sint32 num);
sint32 do_these_objects_combine(sint32 obj1, sint32 obj2);
void combine_these_two_objects(sint16 obj1, sint16 obj2);
void seperate_object(sint16 obj);
sint32 is_item_currently_combinable(sint16 obj);
void spinback(uint16 *cock);
void fade_ammo_selector(void);
void update_laras_weapons_status(void);
void draw_compass(void);
sint32 do_special_waterskin_combine_bullshit(sint32 flag);
void dels_give_lara_items_cheat(void);
void construct_combine_object_list(void);
void do_debounced_joystick_poo(void);

extern sint32 InventoryActive;
extern INVOBJ inventry_objects_list[NUM_INVOBJ];
extern RINGME *rings[2];
extern sint16 options_table[NUM_INVOBJ];
extern COMBINELIST dels_handy_combine_table[23];
extern uint8 current_selected_option;
extern sint8 menu_active;
extern uint8 go_left;
extern uint8 go_right;
extern uint8 go_up;
extern uint8 go_down;
extern uint8 go_select;
extern uint8 go_deselect;
extern uint8 left_debounce;
extern uint8 right_debounce;
extern uint8 up_debounce;
extern uint8 down_debounce;
extern uint8 select_debounce;
extern uint8 deselect_debounce;
extern uint8 friggrimmer;
extern uint8 friggrimmer2;
extern uint8 left_repeat;
extern uint8 right_repeat;
extern sint16 ammo_selector_fade_val;
extern sint16 ammo_selector_fade_dir;
extern sint8 CurrentShotGunAmmoType;
extern sint8 CurrentGrenadeGunAmmoType;
extern sint8 CurrentCrossBowAmmoType;
extern sint16 inventry_xpos;
extern sint16 inventry_ypos;
extern sint32 combine_obj1;
extern sint32 combine_obj2;
extern sint32 GLOBAL_enterinventory;
extern sint32 GLOBAL_inventoryitemchosen;

#endif
