#include "deltapak.h"
#include "lara.h"
#include "objnums.h"
#include "anim.h"
#include "control.h"
#include "lara2gun.h"
#include "hair.h"
#include "specific/function_stubs.h"

void *cutseq_malloc(sint32 size)
{
    return game_malloc(size, 0);
}

void finish_cutseq(void)
{
    GLOBAL_playing_cutseq = 0;
    InitialiseHair();
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

void nineteen_end(void)
{
    cutseq_restore_item(ANIMATING4);
    cutseq_restore_item(JEAN_YVES);
    DelsHandyTeleportLara(53469, -8192, 48687, 23355);
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
