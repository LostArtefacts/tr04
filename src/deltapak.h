#ifndef DELTAPAK_H
#define DELTAPAK_H

#include "types.h"
#include "items.h"
#include "anim.h"

typedef struct actorme {
    sint32 offset;
    sint16 objslot;
    sint16 nodes;
} ACTORME;

typedef struct new_cutscene {
    sint16 numactors;
    sint16 numframes;
    sint32 orgx;
    sint32 orgy;
    sint32 orgz;
    sint32 audio_track;
    sint32 camera_offset;
    ACTORME actor_data[10];
} NEW_CUTSCENE;

typedef struct rtdecode {
    uint32 length;
    uint32 off;
    uint16 counter;
    uint16 data;
    uint8 decodetype;
    uint8 packmethod;
    uint16 padfuck;
} RTDECODE;

typedef struct packnode {
    sint16 xrot_run;
    sint16 yrot_run;
    sint16 zrot_run;
    sint16 xkey;
    sint16 ykey;
    sint16 zkey;
    RTDECODE decode_x;
    RTDECODE decode_y;
    RTDECODE decode_z;
    uint32 xlength;
    uint32 ylength;
    uint32 zlength;
    char *xpacked;
    char *ypacked;
    char *zpacked;
} PACKNODE;

typedef struct nodeloadheader {
    sint16 xkey;
    sint16 ykey;
    sint16 zkey;
    sint16 packmethod;
    sint16 xlength;
    sint16 ylength;
    sint16 zlength;
} NODELOADHEADER;

typedef struct
{
    void (*init_func)(void);
    void (*control_func)(void);
    void (*end_func)(void);
} CUTSEQ_ROUTINES;

extern sint32 GLOBAL_playing_cutseq;
extern NEW_CUTSCENE *GLOBAL_cutme;
extern char *cutseqpakPtr;
extern PACKNODE *actor_pnodes[10];
extern sint16 temp_rotation_buffer[160];
extern ITEM_INFO duff_item[10];
extern sint16 frig_shadow_bbox[6];
extern sint16 frig_jeep_shadow_bbox[6];
extern PACKNODE *camera_pnodes;
extern sint32 cutseq_num;
extern sint32 cutseq_trig;
extern uint8 DEL_playingamefmv;
extern sint32 GLOBAL_cutseq_frame;
extern sint32 lastcamnum;
extern char *GLOBAL_resident_depack_buffers;
extern sint32 GLOBAL_numcutseq_frames;
extern sint16 old_lara_holster;
extern ITEM_INFO *horus_item_thing;
extern char lara_chat_cnt;
extern char actor_chat_cnt;
extern uint32 cutseq_meshbits[10];
extern uint32 cutseq_meshswapbits[10];
extern char old_status_flags[16];
extern sint32 numnailed;
extern uint8 cutseq_busy_timeout;
extern sint32 GLOBAL_oldcamtype;
extern CUTSEQ_ROUTINES cutseq_control_routines[31];
extern sint16 troop_chat_ranges4[14];
extern sint16 lara_chat_ranges6[4];
extern sint16 voncroy_chat_ranges6[4];
extern sint16 lara_chat_ranges7[4];
extern sint16 voncroy_chat_ranges7[4];
extern sint16 lara_chat_ranges8[4];
extern sint16 voncroy_chat_ranges8[6];
extern sint16 lara_chat_ranges9[18];
extern sint16 voncroy_chat_ranges9[22];
extern sint16 lara_chat_ranges10[18];
extern sint16 voncroy_chat_ranges10[16];
extern sint16 lara_chat_ranges11[12];
extern sint16 captain_chat_ranges11[12];
extern sint16 lara_chat_ranges15[10];
extern sint16 voncroy_chat_ranges15[10];
extern sint16 lara_chat_ranges16[10];
extern sint16 jeanyves_chat_ranges16[18];
extern sint16 voncroy_chat_ranges16[10];
extern sint16 jeanyves_chat_ranges17[24];
extern sint16 jeanyves_chat_ranges18[14];
extern sint16 jeanyves_chat_ranges19[20];
extern sint16 jeanyves_chat_ranges20[28];
extern sint16 lara_chat_ranges24[16];
extern sint16 troop_chat_ranges24[16];
extern sint16 lara_chat_ranges25[14];
extern sint16 troop_chat_ranges25[26];
extern sint16 troop_chat_ranges26[4];

void frigup_lara(void);
void do_new_cutscene_camera(void);
void do_key_meshswap(void);
void cutseq_kill_item(sint32 num);
void cutseq_restore_item(sint32 num);
void init_voncroy_meshbits(sint32 num);
void DelsHandyTeleportLara(sint32 x, sint32 y, sint32 z, sint32 yrot);
void nail_intelligent_object(sint16 num);
void handle_lara_chatting(sint16 *_ranges);
void handle_actor_chatting(sint32 speechslot, sint32 node, sint32 slot, sint32 objslot, sint16 *_ranges);
void trigger_item_in_room(sint32 room_number, sint32 object_number);
void untrigger_item_in_room(sint32 room_number, sint32 object_number);
ITEM_INFO *find_a_fucking_item(sint32 object_number);
void *cutseq_malloc(sint32 size);
void InitPackNodes(NODELOADHEADER *lnode, PACKNODE *pnode, char *packed, sint32 numnodes);
void finish_cutseq();
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
void special2_init(void);
void special2_end(void);
void special3_end(void);
void special1_init(void);
void special1_end(void);
void twentyfour_control(void);
void trigger_weapon_dynamics(sint32 left_or_right);
void fifteen_init(void);
void sixteen_init(void);
void cutseq_shoot_pistols(sint32 left_or_right);
void deal_with_pistols(void);
void fourth_cutseq_control(void);
void eleventh_cutseq_init(void);
void twentythree_control(void);
void ninth_control(void);
void fifteen_control(void);
void sixteen_control(void);
void tenth_cutseq_control(void);
void eleventh_cutseq_control(void);
void GetJointAbsPositionCutSeq(ITEM_INFO *item, OBJECT_INFO *obj, sint16 *rot, PHD_VECTOR *pos);
void CalcActorLighting(ITEM_INFO *item, OBJECT_INFO *obj, sint16 *rot);
sint16 GetTrackWord(sint32 off, char *packed, sint32 packmethod);
void init_cutseq_actors(char *data, sint32 resident);
void updateAnimFrame(PACKNODE *node, sint32 flags, sint16 *frame);
void CalculateObjectLightingLaraCutSeq(void);
sint32 Load_and_Init_Cutseq(sint32 num);
void DrawCutSeqActors(void);
void handle_cutseq_triggering(sint32 name);
sint16 DecodeTrack(char *packed, RTDECODE *decode);

#endif
