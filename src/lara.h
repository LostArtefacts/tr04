#ifndef LARA_H
#define LARA_H

#include "items.h"

enum lara_gun_states {
    LG_NO_ARMS,
    LG_HANDS_BUSY,
    LG_DRAW_GUNS,
    LG_UNDRAW_GUNS,
    LG_READY,
    LG_FLARE
};

enum lara_water_states {
    LW_ABOVE_WATER,
    LW_UNDERWATER,
    LW_SURFACE,
    LW_FLYCHEAT,
    LW_WADE
};

typedef struct lara_arm {
    sint16 *frame_base;
    sint16 frame_number;
    sint16 anim_number;
    sint16 lock;
    sint16 y_rot;
    sint16 x_rot;
    sint16 z_rot;
    sint16 flash_gun;
} LARA_ARM;

typedef struct lara_info {
    sint16 item_number;
    sint16 gun_status;
    sint16 gun_type;
    sint16 request_gun_type;
    sint16 last_gun_type;
    sint16 calc_fallspeed;
    sint16 water_status;
    sint16 climb_status;
    sint16 pose_count;
    sint16 hit_frame;
    sint16 hit_direction;
    sint16 air;
    sint16 dive_count;
    sint16 death_count;
    sint16 current_active;
    sint16 current_xvel;
    sint16 current_yvel;
    sint16 current_zvel;
    sint16 spaz_effect_count;
    sint16 flare_age;
    sint16 vehicle;
    sint16 weapon_item;
    sint16 back_gun;
    sint16 flare_frame;
    sint16 poisoned;
    sint16 dpoisoned;
    uint8 electric;
    uint8 wet[15];
    uint16 flare_control_left:1;
    uint16 Unused1:1;
    uint16 look:1;
    uint16 burn:1;
    uint16 keep_ducked:1;
    uint16 IsMoving:1;
    uint16 CanMonkeySwing:1;
    uint16 Unused2:1;
    uint16 OnBeetleFloor:1;
    uint16 BurnGreen:1;
    uint16 IsDucked:1;
    uint16 has_fired:1;
    uint16 Busy:1;
    uint16 LitTorch:1;
    uint16 IsClimbing:1;
    uint16 Fired:1;
    sint32 water_surface_dist;
    PHD_VECTOR last_pos;
    struct fx_info *spaz_effect;
    sint32 mesh_effects;
    sint16 *mesh_ptrs[15];
    ITEM_INFO *target;
    sint16 target_angles[2];
    sint16 turn_rate;
    sint16 move_angle;
    sint16 head_y_rot;
    sint16 head_x_rot;
    sint16 head_z_rot;
    sint16 torso_y_rot;
    sint16 torso_x_rot;
    sint16 torso_z_rot;
    LARA_ARM left_arm;
    LARA_ARM right_arm;
    uint16 holster;
    struct creature_info *creature;
    sint32 CornerX;
    sint32 CornerZ;
    sint8 RopeSegment;
    sint8 RopeDirection;
    sint16 RopeArcFront;
    sint16 RopeArcBack;
    sint16 RopeLastX;
    sint16 RopeMaxXForward;
    sint16 RopeMaxXBackward;
    sint32 RopeDFrame;
    sint32 RopeFrame;
    uint16 RopeFrameRate;
    uint16 RopeY;
    sint32 RopePtr;
    void *GeneralPtr;
    sint32 RopeOffset;
    uint32 RopeDownVel;
    sint8 RopeFlag;
    sint8 MoveCount;
    sint32 RopeCount;
    sint8 pistols_type_carried;
    sint8 uzis_type_carried;
    sint8 shotgun_type_carried;
    sint8 crossbow_type_carried;
    sint8 grenade_type_carried;
    sint8 sixshooter_type_carried;
    sint8 lasersight;
    sint8 binoculars;
    sint8 crowbar;
    sint8 mechanical_scarab;
    uint8 small_water_skin;
    uint8 big_water_skin;
    sint8 examine1;
    sint8 examine2;
    sint8 examine3;
    sint8 puzzleitems[12];
    uint16 puzzleitemscombo;
    uint16 keyitems;
    uint16 keyitemscombo;
    uint16 pickupitems;
    uint16 pickupitemscombo;
    sint16 questitems;
    sint16 num_small_medipack;
    sint16 num_large_medipack;
    sint16 num_flares;
    sint16 num_pistols_ammo;
    sint16 num_uzi_ammo;
    sint16 num_revolver_ammo;
    sint16 num_shotgun_ammo1;
    sint16 num_shotgun_ammo2;
    sint16 num_grenade_ammo1;
    sint16 num_grenade_ammo2;
    sint16 num_grenade_ammo3;
    sint16 num_crossbow_ammo1;
    sint16 num_crossbow_ammo2;
    sint16 num_crossbow_ammo3;
    sint8 beetle_uses;
    sint8 blindTimer;
    sint8 location;
    sint8 highest_location;
    sint8 locationPad;
} LARA_INFO;

extern LARA_INFO lara;
extern ITEM_INFO *lara_item;
extern uint16 DashTimer;

#endif
