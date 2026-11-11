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
extern uint32 cutseq_meshbits[10];
extern char old_status_flags[16];
extern sint32 numnailed;
extern sint16 lara_chat_ranges6[4];
extern sint16 voncroy_chat_ranges6[4];
extern sint16 lara_chat_ranges7[4];
extern sint16 voncroy_chat_ranges7[4];
extern sint16 lara_chat_ranges8[4];
extern sint16 voncroy_chat_ranges8[6];
extern sint16 jeanyves_chat_ranges17[24];
extern sint16 jeanyves_chat_ranges18[14];
extern sint16 jeanyves_chat_ranges19[20];
extern sint16 jeanyves_chat_ranges20[28];
extern sint16 lara_chat_ranges25[14];
extern sint16 troop_chat_ranges25[26];
extern sint16 troop_chat_ranges26[4];

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
void eighteen_end(void);
void nineteen_init(void);
void nineteen_end(void);
void twenty_end(void);
void twentyone_init(void);
void twentyone_end(void);
void twentythree_init(void);
void twentythree_end(void);
void twentyfour_end(void);
void twentyseven_end(void);
void twentytwo_init(void);
void twentytwo_end(void);
void twentyfour_init(void);
void sixth_cutseq_control(void);
void seventh_control(void);
void eighth_control(void);
void ninth_cutseq_init(void);
void tenth_cutseq_init(void);
void seventeen_control(void);
void eighteen_control(void);
void nineteen_control(void);
void twenty_control(void);
void twentyfive_control(void);
void twentyfive_end(void);
void twentysix_control(void);
void twentyseven_init(void);
void twentyseven_control(void);
void twentyfive_init(void);
void force_sky_lightning(void);
void do_backpack_meshswap(void);
void third_cutseq_control(void);
void fifth_cutseq_control(void);
void do_spade_meshswap(void);
void special3_control(void);

#endif
