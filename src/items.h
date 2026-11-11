#ifndef ITEMS_H
#define ITEMS_H

#include "3dglodef.h"

#define NO_ITEM (-1)

#define ITEM_INACTIVE 0
#define ITEM_ACTIVE 1
#define ITEM_DEACTIVATED 2
#define ITEM_INVISIBLE 3

#define IFL_TRIGGERED 0x20
#define IFL_INVISIBLE 0x100
#define IFL_CODEBITS 0x3E00
#define IFL_REVERSE 0x4000
#define IFL_CLEARBODY 0x8000

typedef struct pclight {
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    sint32 shadow;
    float Inner;
    float Outer;
    float InnerAngle;
    float OuterAngle;
    float Cutoff;
    float nx;
    float ny;
    float nz;
    sint32 ix;
    sint32 iy;
    sint32 iz;
    sint32 inx;
    sint32 iny;
    sint32 inz;
    float tr;
    float tg;
    float tb;
    float rs;
    float gs;
    float bs;
    sint32 fcnt;
    uint8 Type;
    uint8 Active;
    PHD_VECTOR rlp;
    sint32 Range;
} PCLIGHT;

typedef struct item_light {
    sint32 r;
    sint32 g;
    sint32 b;
    sint32 ambient;
    sint32 rs;
    sint32 gs;
    sint32 bs;
    sint32 fcnt;
    PCLIGHT CurrentLights[21];
    PCLIGHT PrevLights[21];
    sint32 nCurrentLights;
    sint32 nPrevLights;
    sint32 room_number;
    sint32 RoomChange;
    PHD_VECTOR item_pos;
    void *pCurrentLights;
    void *pPrevLights;
} ITEM_LIGHT;

typedef struct item_info {
    sint32 floor;
    uint32 touch_bits;
    uint32 mesh_bits;
    sint16 object_number;
    sint16 current_anim_state;
    sint16 goal_anim_state;
    sint16 required_anim_state;
    sint16 anim_number;
    sint16 frame_number;
    sint16 room_number;
    sint16 next_item;
    sint16 next_active;
    sint16 speed;
    sint16 fallspeed;
    sint16 hit_points;
    uint16 box_number;
    sint16 timer;
    sint16 flags;
    sint16 shade;
    sint16 trigger_flags;
    sint16 carried_item;
    sint16 after_death;
    uint16 fired_weapon;
    sint16 item_flags[4];
    void *data;
    PHD_3DPOS pos;
    ITEM_LIGHT il;
    uint32 active:1;
    uint32 status:2;
    uint32 gravity_status:1;
    uint32 hit_status:1;
    uint32 collidable:1;
    uint32 looked_at:1;
    uint32 dynamic_light:1;
    uint32 poisoned:1;
    uint32 ai_bits:5;
    uint32 really_active:1;
    uint32 meshswap_meshbits;
    sint16 draw_room;
    sint16 TOSSPAD;
} ITEM_INFO;

void InitialiseItemArray(int num);
void KillItem(sint16 item_num);
sint16 CreateItem(void);
void InitialiseItem(sint16 item_num);
void RemoveActiveItem(sint16 item_num);
void RemoveDrawnItem(sint16 item_num);
void AddActiveItem(sint16 item_num);

extern ITEM_INFO *items;
extern sint16 next_item_active;
extern int level_items;

#endif
