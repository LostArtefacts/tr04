#ifndef ANIM_H
#define ANIM_H

#include "items.h"
#include "objnums.h"

struct coll_info;

typedef struct anim_struct {
    sint16 *frame_ptr;
    sint16 interpolation;
    sint16 current_anim_state;
    sint32 velocity;
    sint32 acceleration;
    sint32 Xvelocity;
    sint32 Xacceleration;
    sint16 frame_base;
    sint16 frame_end;
    sint16 jump_anim_num;
    sint16 jump_frame_num;
    sint16 number_changes;
    sint16 change_index;
    sint16 number_commands;
    sint16 command_index;
} ANIM_STRUCT;

typedef struct change_struct {
    sint16 goal_anim_state;
    sint16 number_ranges;
    sint16 range_index;
} CHANGE_STRUCT;

typedef struct range_struct {
    sint16 start_frame;
    sint16 end_frame;
    sint16 link_anim_num;
    sint16 link_frame_num;
} RANGE_STRUCT;

typedef struct object_info {
    sint16 nmeshes;
    sint16 mesh_index;
    sint32 bone_index;
    sint16 *frame_base;
    void (*initialise)(sint16 item_number);
    void (*control)(sint16 item_number);
    void (*floor)(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
    void (*ceiling)(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height);
    void (*draw_routine)(ITEM_INFO *item);
    void (*collision)(sint16 item_num, ITEM_INFO *laraitem, struct coll_info *coll);
    sint16 object_mip;
    sint16 anim_index;
    sint16 hit_points;
    sint16 pivot_length;
    sint16 radius;
    sint16 shadow_size;
    uint16 bite_offset;
    uint16 loaded:1;
    uint16 intelligent:1;
    uint16 non_lot:1;
    uint16 save_position:1;
    uint16 save_hitpoints:1;
    uint16 save_flags:1;
    uint16 save_anim:1;
    uint16 semi_transparent:1;
    uint16 water_creature:1;
    uint16 using_drawanimating_item:1;
    uint16 HitEffect:2;
    uint16 undead:1;
    uint16 save_mesh:1;
    void (*draw_routine_extra)(ITEM_INFO *item);
    uint32 explodable_meshbits;
    uint32 padfuck;
} OBJECT_INFO;

extern OBJECT_INFO objects[NUMBER_OBJECTS];
extern ANIM_STRUCT *anims;
extern CHANGE_STRUCT *changes;
extern RANGE_STRUCT *ranges;
extern sint16 **meshes;
extern sint32 *bones;

sint32 GetChange(ITEM_INFO *item, ANIM_STRUCT *anim);

#endif
