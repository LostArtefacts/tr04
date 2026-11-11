#include "deltapak.h"
#include "lara.h"
#include "objnums.h"
#include "anim.h"
#include "control.h"
#include "lara2gun.h"
#include "hair.h"
#include "specific/function_stubs.h"
#include "draw.h"
#include "delstuff.h"

void *cutseq_malloc(sint32 size)
{
    return game_malloc(size, 0);
}

void finish_cutseq(void)
{
    GLOBAL_playing_cutseq = 0;
    InitialiseHair();
}

void third_cutseq_control(void)
{
    if (GLOBAL_cutseq_frame == 1890)
        force_sky_lightning();
}

void fourth_cutseq_init(void)
{
    old_lara_holster = lara.holster;
    lara.holster = LARA_HOLSTERS;
    draw_pistol_meshes(1);
}

void fourth_cutseq_end(void)
{
    trigger_item_in_room(101, ANIMATING9);
    lara.keyitems |= 0xC00;
}

void fifth_cutseq_control(void)
{
    if (GLOBAL_cutseq_frame == 1350)
        do_backpack_meshswap();
}

void fifth_cutseq_end(void)
{
    DelsHandyTeleportLara(100938, 768, 58040, -32552);
}

void sixth_cutseq_init(void)
{
    init_voncroy_meshbits(1);
}

void sixth_cutseq_control(void)
{
    handle_lara_chatting(lara_chat_ranges6);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges6);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void sixth_cutseq_end(void)
{
    DelsHandyTeleportLara(5632, 1280, 86528, 28987);
}

void seventh_cutseq_init(void)
{
    init_voncroy_meshbits(1);
    cutseq_kill_item(ANIMATING3);
    nail_intelligent_object(VON_CROY);
}

void seventh_control(void)
{
    handle_lara_chatting(lara_chat_ranges7);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges7);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void eighth_control(void)
{
    handle_lara_chatting(lara_chat_ranges8);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges8);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void ninth_cutseq_init(void)
{
    init_voncroy_meshbits(1);
    cutseq_meshbits[3] &= ~0x80000000;
    cutseq_meshbits[4] &= ~0x80000000;
    cutseq_meshbits[5] &= ~0x80000000;
    cutseq_meshbits[6] &= ~0x80000000;
    cutseq_meshbits[7] &= ~0x80000000;
}

void tenth_cutseq_init(void)
{
    cutseq_kill_item(DOOR_TYPE5);
    cutseq_meshbits[1] &= ~0x80000000;
    cutseq_meshbits[2] &= ~0x80000000;
    cutseq_meshbits[3] &= ~0x80000000;
    cutseq_meshbits[4] &= ~0x80000000;
}

void tenth_cutseq_end(void)
{
    cutseq_restore_item(DOOR_TYPE5);
    DelsHandyTeleportLara(68756, 512, 56493, 983);
}

void twelth_cutseq_init(void)
{
    cutseq_kill_item(ANIMATING6);
}

void twelth_cutseq_end(void)
{
    cutseq_restore_item(ANIMATING6);
}

void thirteen_end(void)
{
    DelsHandyTeleportLara(79194, 0, 40951, -20741);
}

void fourteen_end(void)
{
    DelsHandyTeleportLara(27282, 256, 65654, 0);
    FlipMap(1);
    lara.questitems |= 1;
}

void fourteen_control(void)
{
    ITEM_INFO *item;

    if (GLOBAL_cutseq_frame == 393) {
        item = find_a_fucking_item(SARCOPHAGUS_CUT);
        item->flags |= IFL_TRIGGERED;
        item->mesh_bits = 3;
    }
}

void fifteen_end(void)
{
    lara.questitems |= 1;
}

void sixteen_end(void)
{
    untrigger_item_in_room(169, FLAME_EMITTER2);
    untrigger_item_in_room(184, FLAME_EMITTER2);
    untrigger_item_in_room(177, AMBER_LIGHT);
    DelsHandyTeleportLara(61868, 0, 44994, -16060);
}

void seventeen_control(void)
{
    handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges17);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void seventeen_end(void)
{
    cutseq_restore_item(ANIMATING4);
    cutseq_restore_item(JEAN_YVES);
    DelsHandyTeleportLara(54706, -8192, 50470, 4514);
}

void eighteen_init(void)
{
    cutseq_kill_item(JEAN_YVES);
}

void eighteen_control(void)
{
    handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges18);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void eighteen_end(void)
{
    cutseq_restore_item(JEAN_YVES);
    DelsHandyTeleportLara(55311, -8192, 53160, 2304);
}

void nineteen_init(void)
{
    cutseq_kill_item(ANIMATING4);
    cutseq_kill_item(JEAN_YVES);
}

void nineteen_control(void)
{
    handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges19);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void nineteen_end(void)
{
    cutseq_restore_item(ANIMATING4);
    cutseq_restore_item(JEAN_YVES);
    DelsHandyTeleportLara(53469, -8192, 48687, 23355);
}

void twenty_control(void)
{
    handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 1, JEAN_YVES, jeanyves_chat_ranges20);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twenty_end(void)
{
    cutseq_restore_item(ANIMATING4);
    cutseq_restore_item(JEAN_YVES);
    DelsHandyTeleportLara(55348, -8192, 50436, -9731);
}

void twentyone_init(void)
{
    cutseq_kill_item(ANIMATING10);
    do_key_meshswap();
}

void twentyone_end(void)
{
    do_key_meshswap();
    DelsHandyTeleportLara(33279, -4352, 40697, 21847);
}

void twentythree_init(void)
{
    horus_item_thing = find_a_fucking_item(HORUS_STATUE);
}

void twentythree_end(void)
{
    DelsHandyTeleportLara(17476, 29952, 15872, -16135);
    lara.questitems &= 1;
}

void twentyfour_init(void)
{
    cutseq_kill_item(ANIMATING13);
    old_lara_holster = lara.holster;
    lara.holster = LARA_HOLSTERS;
    draw_pistol_meshes(1);
}

void twentyfour_end(void)
{
    cutseq_restore_item(ANIMATING13);
    trigger_item_in_room(107, PUZZLE_ITEM8_COMBO1);
}

void twentyfive_control(void)
{
    handle_lara_chatting(lara_chat_ranges25);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 1, ANIMATING8, troop_chat_ranges25);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twentyfive_end(void)
{
    ITEM_INFO *item;

    item = find_a_fucking_item(ANIMATING5);
    item->flags |= IFL_CODEBITS;
    item->status = old_status_flags[numnailed];
    numnailed++;
    DelsHandyTeleportLara(55396, -1792, 77122, -7105);
}

void twentyfive_init(void)
{
    ITEM_INFO *item;

    item = find_a_fucking_item(ANIMATING5);
    item->flags &= ~IFL_CODEBITS;
    old_status_flags[numnailed] = item->status;
    item->status = ITEM_INVISIBLE;
    numnailed++;
}

void twentysix_control(void)
{
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 1, ANIMATING8, troop_chat_ranges26);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void twentyseven_init(void)
{
    ITEM_INFO *item;

    item = find_a_fucking_item(ANIMATING4);
    item->flags &= ~IFL_CODEBITS;
    cutseq_meshbits[1] &= ~0x200;
    lara.questitems = 0;
}

void twentyseven_control(void)
{
    if (GLOBAL_cutseq_frame == 283)
        cutseq_meshbits[1] |= 0x200;
}

void twentyseven_end(void)
{
    ITEM_INFO *item;

    item = find_a_fucking_item(ANIMATING5);
    item->status = ITEM_ACTIVE;
    DelsHandyTeleportLara(18982, 18176, 15841, -16184);
    FlipMap(8);
    nail_intelligent_object(SETHA);
}

void twentytwo_init(void)
{
    cutseq_kill_item(DOOR_TYPE6);
}

void twentytwo_end(void)
{
    DelsHandyTeleportLara(72770, -11776, 54077, 0);
    cutseq_restore_item(DOOR_TYPE6);
    trigger_item_in_room(39, DOOR_TYPE6);
    trigger_item_in_room(39, DEMIGOD3);
}

void do_spade_meshswap(void)
{
    sint16 *temp;

    temp = lara.mesh_ptrs[LM_LHAND];
    lara.mesh_ptrs[LM_LHAND] = meshes[objects[MESHSWAP1].mesh_index + LM_LHAND * 2];
    meshes[objects[MESHSWAP1].mesh_index + LM_LHAND * 2] = temp;
}

void do_backpack_meshswap(void)
{
    sint16 *temp;

    temp = lara.mesh_ptrs[LM_TORSO];
    lara.mesh_ptrs[LM_TORSO] = meshes[objects[PISTOLS_ANIM].mesh_index + LM_TORSO * 2];
    meshes[objects[PISTOLS_ANIM].mesh_index + LM_TORSO * 2] = temp;
}

void do_key_meshswap(void)
{
    sint16 *temp;

    temp = lara.mesh_ptrs[LM_RHAND];
    lara.mesh_ptrs[LM_RHAND] = meshes[objects[MESHSWAP1].mesh_index + LM_RHAND * 2];
    meshes[objects[MESHSWAP1].mesh_index + LM_RHAND * 2] = temp;
}

void cutseq_kill_item(sint32 num)
{
    ITEM_INFO *item;
    sint32 i;

    for (i = 0; i < level_items; i++) {
        item = &items[i];
        if (item->object_number == num) {
            old_status_flags[numnailed] = item->status;
            item->status = ITEM_INVISIBLE;
            numnailed++;
        }
    }
}

void cutseq_restore_item(sint32 num)
{
    sint32 i;

    for (i = 0; i < level_items; i++) {
        if (items[i].object_number == num) {
            items[i].status = old_status_flags[numnailed];
            numnailed++;
        }
    }
}

void nail_intelligent_object(sint16 num)
{
    ITEM_INFO *item;
    sint32 i;

    item = items;
    for (i = 0; i < level_items; i++, item++) {
        if (item->object_number == num) {
            item->status = ITEM_INVISIBLE;
            RemoveActiveItem(i);
            DisableBaddieAI(i);
        }
    }
}

void force_sky_lightning(void)
{
    LightningCount = (GetRandomDraw() & 0x1F) + 64;
    dLightningRand = (GetRandomDraw() & 0xFF) + 256;
}

void handle_lara_chatting(sint16 *_ranges)
{
    sint32 r1, r2, f;

    lara_chat_cnt = (lara_chat_cnt - 1) & 1;
    f = GLOBAL_cutseq_frame;
    while (1) {
        r1 = _ranges[0];
        r2 = _ranges[1];
        if (r1 == -1)
            break;
        if (f >= r1 && f <= r2) {
            if (!lara_chat_cnt)
                lara.mesh_ptrs[LM_HEAD] = meshes[objects[(GetRandomControl() & 3) + LARA_SPEECH_HEAD1].mesh_index + 2 * LM_HEAD];
            return;
        }
        _ranges += 2;
    }
    lara.mesh_ptrs[LM_HEAD] = meshes[objects[LARA_SKIN].mesh_index + 2 * LM_HEAD];
}

void trigger_item_in_room(sint32 room_number, sint32 object_number)
{
    ITEM_INFO *item;
    sint16 item_number;
    sint16 next;

    item_number = room[room_number].item_number;
    while (item_number != NO_ITEM) {
        item = &items[item_number];
        next = item->next_item;
        if (item->object_number == object_number) {
            AddActiveItem(item_number);
            if (object_number != DEMIGOD3)
                item->status = ITEM_ACTIVE;
            item->flags |= IFL_CODEBITS;
        }
        item_number = next;
    }
}

ITEM_INFO *find_a_fucking_item(sint32 object_number)
{
    ITEM_INFO *item;
    sint32 i;

    for (i = 0; i < level_items; i++) {
        item = &items[i];
        if (item->object_number == object_number)
            return item;
    }
    return 0;
}

void special3_control(void)
{
    if (GLOBAL_cutseq_frame >= 348 && GLOBAL_cutseq_frame <= 358 && GLOBAL_cutseq_frame & 1)
        DoBloodSplat(6799 - (GetRandomDraw() & 0xFF), (GetRandomDraw() & 0x1FF) - 768, 76209, 7, -1, 24);
}
