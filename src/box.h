#ifndef BOX_H
#define BOX_H

#include "items.h"

#define NUM_SLOTS 5

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

extern int num_boxes;

#endif
