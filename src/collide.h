#ifndef COLLIDE_H
#define COLLIDE_H

#include "items.h"
#include "room.h"

enum direction {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

enum coll_type {
    COLL_NONE = 0,
    COLL_FRONT = 1,
    COLL_LEFT = 2,
    COLL_RIGHT = 4,
    COLL_TOP = 8,
    COLL_TOPFRONT = 16,
    COLL_CLAMP = 32
};

typedef struct coll_info {
    sint32 mid_floor;
    sint32 mid_ceiling;
    sint32 mid_type;
    sint32 front_floor;
    sint32 front_ceiling;
    sint32 front_type;
    sint32 left_floor;
    sint32 left_ceiling;
    sint32 left_type;
    sint32 right_floor;
    sint32 right_ceiling;
    sint32 right_type;
    sint32 left_floor2;
    sint32 left_ceiling2;
    sint32 left_type2;
    sint32 right_floor2;
    sint32 right_ceiling2;
    sint32 right_type2;
    sint32 radius;
    sint32 bad_pos;
    sint32 bad_neg;
    sint32 bad_ceiling;
    PHD_VECTOR shift;
    PHD_VECTOR old;
    sint16 old_anim_state;
    sint16 old_anim_number;
    sint16 old_frame_number;
    sint16 facing;
    sint16 quadrant;
    sint16 coll_type;
    sint16 *trigger;
    sint8 tilt_x;
    sint8 tilt_z;
    char hit_by_baddie;
    char hit_static;
    uint16 slopes_are_walls:2;
    uint16 slopes_are_pits:1;
    uint16 lava_is_pit:1;
    uint16 enable_baddie_push:1;
    uint16 enable_spaz:1;
    uint16 hit_ceiling:1;
} COLL_INFO;

void GetCollisionInfo(COLL_INFO *coll, sint32 x, sint32 y, sint32 z, sint16 room_number, sint32 hite);
sint32 FindGridShift(sint32 src, sint32 dst);
void ShiftItem(ITEM_INFO *item, COLL_INFO *coll);
void UpdateLaraRoom(ITEM_INFO *item, sint32 height);
sint32 TestBoundsCollide(ITEM_INFO *item, ITEM_INFO *l, sint32 rad);
sint32 ItemPushLara(ITEM_INFO *item, ITEM_INFO *l, COLL_INFO *coll, sint32 spaz, sint32 big_push);
sint32 TestLaraPosition(sint16 *bounds, ITEM_INFO *item, ITEM_INFO *laraitem);
sint32 MoveLaraPosition(PHD_VECTOR *v, ITEM_INFO *item, ITEM_INFO *l);
void AlignLaraPosition(PHD_VECTOR *vec, ITEM_INFO *item, ITEM_INFO *laraitem);
void ObjectCollision(sint16 item_num, ITEM_INFO *laraitem, COLL_INFO *coll);
void TrapCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void ObjectCollisionNoBigPush(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void AlignLaraPosition(PHD_VECTOR *pos, ITEM_INFO *item, ITEM_INFO *l);
sint32 TestBoundsCollideStatic(sint16 *bounds, PHD_3DPOS *pos, sint32 rad);
sint32 TestBoundsCollide2(ITEM_INFO *item, ITEM_INFO *l, sint32 rad);
void CogCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void CreatureCollision(sint16 item_num, ITEM_INFO *laraitem, COLL_INFO *coll);
sint32 GetCollidedObjects(ITEM_INFO *item, sint32 rad, sint32 noInvisible, ITEM_INFO **StoredItems, MESH_INFO **StoredStatics, sint32 StoreLara);
sint32 CollideStaticObjects(COLL_INFO *coll, sint32 x, sint32 y, sint32 z, sint16 room_number, sint32 hite);
sint32 ItemPushLaraStatic(ITEM_INFO *l, sint16 *bounds, PHD_3DPOS *pos, COLL_INFO *coll);
void LaraBaddieCollision(ITEM_INFO *l, COLL_INFO *coll);

extern sint16 GlobalCollisionBounds[6];

#endif
