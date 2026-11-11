#ifndef DELTAPAK_H
#define DELTAPAK_H

#include "types.h"
#include "items.h"

extern sint32 GLOBAL_playing_cutseq;
extern sint32 cutseq_num;
extern sint32 GLOBAL_cutseq_frame;
extern sint16 old_lara_holster;
extern ITEM_INFO *horus_item_thing;
extern char lara_chat_cnt;
extern char actor_chat_cnt;

void frigup_lara(void);
void do_key_meshswap(void);
void cutseq_kill_item(sint32 num);
ITEM_INFO *cutseq_restore_item(sint32 num);
void init_voncroy_meshbits(sint32 num);
void DelsHandyTeleportLara(sint32 x, sint32 y, sint32 z, sint32 yrot);
void nail_intelligent_object(sint16 num);
void handle_lara_chatting(sint16 *_ranges);
void handle_actor_chatting(sint32 speechslot, sint32 node, sint32 slot, sint32 objslot, sint16 *_ranges);
void trigger_item_in_room(sint32 room_number, sint32 object_number);
void untrigger_item_in_room(sint32 room_number, sint32 object_number);
ITEM_INFO *find_a_fucking_item(sint32 object_number);
void *cutseq_malloc(sint32 size);
void finish_cutseq(void);
void fourth_cutseq_init(void);
void fourth_cutseq_end(void);
void fifth_cutseq_end(void);
void sixth_cutseq_init(void);
void sixth_cutseq_end(void);
void seventh_cutseq_init(void);
void tenth_cutseq_end(void);
void twelth_cutseq_init(void);
void twelth_cutseq_end(void);
void thirteen_end(void);
void fourteen_end(void);
void fifteen_end(void);
void sixteen_end(void);
void fourteen_control(void);
void seventeen_end(void);
void eighteen_init(void);

#endif
