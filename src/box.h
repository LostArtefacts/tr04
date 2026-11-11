#ifndef BOX_H
#define BOX_H

#include "items.h"

#define NUM_SLOTS 5
#define NO_BOX 0x7FF
#define BOX_NUMBER 0x7FF
#define NO_FLYING 0
#define BLOCKABLE 0x8000
#define BLOCKED 0x4000

typedef enum {
    SKELLY_ZONE,
    BASIC_ZONE,
    CROC_ZONE,
    HUMAN_ZONE,
    FLYER_ZONE
} ZONE_TYPE;

typedef enum {
    BORED_MOOD,
    ATTACK_MOOD,
    ESCAPE_MOOD,
    STALK_MOOD
} MOOD_TYPE;

typedef struct box_node {
    sint16 exit_box;
    uint16 search_number;
    sint16 next_expansion;
    sint16 box_number;
} BOX_NODE;

typedef struct box_info {
    uint8 left;
    uint8 right;
    uint8 top;
    uint8 bottom;
    sint16 height;
    sint16 overlap_index;
} BOX_INFO;

typedef struct lot_info {
    BOX_NODE *node;
    sint16 head;
    sint16 tail;
    uint16 search_number;
    uint16 block_mask;
    sint16 step;
    sint16 drop;
    sint16 zone_count;
    sint16 target_box;
    sint16 required_box;
    sint16 fly;
    uint16 can_jump:1;
    uint16 can_monkey:1;
    uint16 is_amphibious:1;
    uint16 is_jumping:1;
    uint16 is_monkeying:1;
    PHD_VECTOR target;
    ZONE_TYPE zone;
} LOT_INFO;

typedef struct creature_info {
    sint16 joint_rotation[4];
    sint16 maximum_turn;
    sint16 flags;
    uint16 alerted:1;
    uint16 head_left:1;
    uint16 head_right:1;
    uint16 reached_goal:1;
    uint16 hurt_by_lara:1;
    uint16 patrol2:1;
    uint16 jump_ahead:1;
    uint16 monkey_ahead:1;
    MOOD_TYPE mood;
    ITEM_INFO *enemy;
    ITEM_INFO ai_target;
    sint16 pad;
    sint16 item_num;
    PHD_VECTOR target;
    LOT_INFO LOT;
} CREATURE_INFO;

enum ai_bits_types {
    GUARD = 1 << 0,
    AMBUSH = 1 << 1,
    PATROL1 = 1 << 2,
    MODIFY = 1 << 3,
    FOLLOW = 1 << 4
};

typedef struct ai_info {
    sint16 zone_number;
    sint16 enemy_zone;
    sint32 distance;
    sint32 ahead;
    sint32 bite;
    sint16 angle;
    sint16 x_angle;
    sint16 enemy_facing;
} AI_INFO;

typedef struct bite_info {
    sint32 x;
    sint32 y;
    sint32 z;
    sint32 mesh_num;
} BITE_INFO;

void InitialiseCreature(sint16 item_number);
sint32 CreatureActive(sint16 item_number);
void CreatureAIInfo(ITEM_INFO *item, AI_INFO *info);
void GetCreatureMood(ITEM_INFO *item, AI_INFO *info, sint32 violent);
void CreatureMood(ITEM_INFO *item, AI_INFO *info, sint32 violent);
sint32 CreatureAnimation(sint16 item_number, sint16 angle, sint16 tilt);
sint32 SearchLOT(LOT_INFO *LOT, sint32 expansion);
sint16 CreatureTurn(ITEM_INFO *item, sint16 maximum_turn);
sint16 CreatureEffect(ITEM_INFO *item, BITE_INFO *bite, sint16 (*generate)(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number));
void GetAITarget(CREATURE_INFO *creature);
void CreatureTilt(ITEM_INFO *item, sint16 angle);
void CreatureFloat(sint16 item_number);
void CreatureUnderwater(ITEM_INFO *item, sint32 depth);
void CreatureJoint(ITEM_INFO *item, sint16 joint, sint16 required);
sint16 CreatureEffectT(ITEM_INFO *item, BITE_INFO *bite, sint16 damage, sint16 angle, sint16 (*generate)(sint32 x, sint32 y, sint32 z, sint16 speed, sint16 yrot, sint16 room_number));
sint16 AIGuard(CREATURE_INFO *creature);
sint32 EscapeBox(ITEM_INFO *item, ITEM_INFO *enemy, sint16 box_number);
void TargetBox(LOT_INFO *LOT, sint16 box_number);
sint32 UpdateLOT(LOT_INFO *LOT, sint32 expansion);
sint32 ValidBox(ITEM_INFO *item, sint16 zone_number, sint16 box_number);
void AlertNearbyGuards(ITEM_INFO *item);
sint32 BadFloor(sint32 x, sint32 y, sint32 z, sint32 box_height, sint32 next_height, sint16 room_number, LOT_INFO *LOT);

extern CREATURE_INFO *baddie_slots;

extern BOX_INFO *boxes;
extern int num_boxes;
extern sint16 *ground_zone[5][2];

#endif
