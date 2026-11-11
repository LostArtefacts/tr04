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
