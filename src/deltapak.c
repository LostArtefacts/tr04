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
#include "gameflow.h"
#include "spotcam.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "lara_states.h"
#include "camera.h"
#include "specific/3dmath.h"
#include "specific/lighting.h"

void frigup_lara(void)
{
    sint32 *bone;

    lara_item->pos.x_pos = GLOBAL_cutme->orgx;
    lara_item->pos.y_pos = GLOBAL_cutme->orgy;
    lara_item->pos.z_pos = GLOBAL_cutme->orgz;
    bone = &bones[objects[lara_item->object_number].bone_index];
    updateAnimFrame(actor_pnodes[0], 16, temp_rotation_buffer);
    Rich_CalcLaraMatrices_Normal(temp_rotation_buffer, bone, 0);
    phd_PushUnitMatrix();
    Rich_CalcLaraMatrices_Normal(temp_rotation_buffer, bone, 1);
    phd_PopMatrix();
    HairControl(0, 0, temp_rotation_buffer);
    if (gfLevelFlags & GF_YOUNGLARA)
        HairControl(0, 1, temp_rotation_buffer);
    GLaraShadowframe = frig_shadow_bbox;
    if (cutseq_num == 12)
        GLaraShadowframe = frig_jeep_shadow_bbox;
}

void updateAnimFrame(PACKNODE *node, sint32 flags, sint16 *frame)
{
    sint16 *nex;
    sint32 i;
    uint32 t;

    nex = frame + 9;
    frame[6] = 3 * node->xrot_run;
    frame[7] = 3 * node->yrot_run;
    frame[8] = 3 * node->zrot_run;
    for (i = 1; i < flags; i++) {
        t = ((node[i].xrot_run << 10 | node[i].yrot_run) << 10) | node[i].zrot_run;
        *nex++ = (sint16)(t >> 16);
        *nex++ = (sint16)t;
    }
}

void *cutseq_malloc(sint32 size)
{
    return game_malloc(size, 0);
}

void finish_cutseq(void)
{
    GLOBAL_playing_cutseq = 0;
    InitialiseHair();
}

void DrawCutSeqActors(void)
{
    OBJECT_INFO *obj;
    sint16 **mesh;
    sint32 *bone, i, j;
    sint16 *rot;
    sint32 n, bits, swap, flags;

    phd_PushMatrix();
    for (i = 1; i < GLOBAL_cutme->numactors; i++) {
        bits = cutseq_meshbits[i];
        swap = cutseq_meshswapbits[i];
        n = 1;
        phd_PushMatrix();
        if (!(bits & 0x80000000)) {
            phd_PopMatrix();
            continue;
        }
        {
            updateAnimFrame(actor_pnodes[i], GLOBAL_cutme->actor_data[i].nodes + 1, temp_rotation_buffer);
            obj = &objects[GLOBAL_cutme->actor_data[i].objslot];
            phd_TranslateAbs(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz);
            CalcActorLighting(&duff_item[i - 1], obj, temp_rotation_buffer);
            mesh = &meshes[obj->mesh_index];
            bone = &bones[obj->bone_index];
            phd_TranslateRel(temp_rotation_buffer[6], temp_rotation_buffer[7], temp_rotation_buffer[8]);
            rot = &temp_rotation_buffer[9];
            gar_RotYXZsuperpack(&rot, 0);
            if (bits & 1) {
                if (swap & 1)
                    phd_PutPolygons(mesh[1], -1);
                else
                    phd_PutPolygons(mesh[0], -1);
            }
            mesh += 2;
            for (j = 0; j < obj->nmeshes - 1; j++, bone += 4, mesh += 2) {
                flags = *bone;
                if (flags & 1)
                    phd_PopMatrix();
                if (flags & 2)
                    phd_PushMatrix();
                phd_TranslateRel(bone[1], bone[2], bone[3]);
                gar_RotYXZsuperpack(&rot, 0);
                n <<= 1;
                if (n & bits) {
                    if (n & swap)
                        phd_PutPolygons(mesh[1], -1);
                    else
                        phd_PutPolygons(mesh[0], -1);
                }
            }
        }
        phd_PopMatrix();
    }
    phd_PopMatrix();
}

void CalcActorLighting(ITEM_INFO *item, OBJECT_INFO *obj, sint16 *rot)
{
    PHD_VECTOR pos;
    sint16 room_number;

    GetJointAbsPositionCutSeq(item, obj, rot, &pos);
    room_number = item->room_number;
    IsRoomOutsideNo = -1;
    IsRoomOutside(pos.x, pos.y, pos.z);
    if (IsRoomOutsideNo != -1)
        room_number = IsRoomOutsideNo;
    item->room_number = room_number;
    current_item = item;
    item->il.fcnt = -1;
    item->il.item_pos.x = pos.x;
    item->il.item_pos.y = pos.y;
    item->il.item_pos.z = pos.z;
    CalcAmbientLight(item);
    CreateLightList(item);
}

void GetJointAbsPositionCutSeq(ITEM_INFO *item, OBJECT_INFO *obj, sint16 *rot, PHD_VECTOR *pos)
{
    sint16 *rot2;

    phd_PushUnitMatrix();
    phd_TranslateRel(rot[6], rot[7], rot[8]);
    rot2 = rot + 9;
    gar_RotYXZsuperpack(&rot2, 0);
    pos->x = phd_mxptr[M03] >> W2V_SHIFT;
    pos->y = phd_mxptr[M13] >> W2V_SHIFT;
    pos->z = phd_mxptr[M23] >> W2V_SHIFT;
    pos->x += item->pos.x_pos;
    pos->y += item->pos.y_pos;
    pos->z += item->pos.z_pos;
    phd_PopMatrix();
}

void CalculateObjectLightingLaraCutSeq(void)
{
    PHD_VECTOR pos;
    sint16 room_num, room_num2;

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    GetLaraJointPos(&pos, 7);
    room_num = lara_item->room_number;
    IsRoomOutsideNo = -1;
    IsRoomOutside(pos.x, pos.y, pos.z);
    if (IsRoomOutsideNo != -1)
        room_num = IsRoomOutsideNo;
    room_num2 = lara_item->room_number;
    current_item = lara_item;
    lara_item->il.item_pos.x = pos.x;
    lara_item->il.item_pos.y = pos.y;
    lara_item->il.item_pos.z = pos.z;
    lara_item->room_number = room_num;
    CalcAmbientLight(lara_item);
    CreateLightList(lara_item);
    lara_item->room_number = room_num2;
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

void fourth_cutseq_control(void)
{
    sint32 frame;

    frame = GLOBAL_cutseq_frame;
    if (frame == 309) {
        undraw_pistol_mesh_left(1);
        undraw_pistol_mesh_right(1);
        lara.holster = old_lara_holster;
    } else {
        if (frame == 51 || frame == 68)
            cutseq_shoot_pistols(11);
        if (frame == 51 || frame == 64 || frame == 73)
            cutseq_shoot_pistols(14);
        if (frame == 108 || frame == 114 || frame == 120 || frame == 128 || frame == 134 || frame == 141 || frame == 147 || frame == 158) {
            cutseq_shoot_pistols(11);
            cutseq_shoot_pistols(14);
        }
    }
    deal_with_pistols();
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 8, 1, TROOPS, troop_chat_ranges4);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
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

void ninth_control(void)
{
    if (GLOBAL_cutseq_frame == 3000) {
        cutseq_meshbits[3] |= 0x80000000;
        cutseq_meshbits[4] |= 0x80000000;
        cutseq_meshbits[5] |= 0x80000000;
        cutseq_meshbits[6] |= 0x80000000;
        cutseq_meshbits[7] |= 0x80000000;
        cutseq_meshbits[1] &= ~0x80000000;
    } else if (GLOBAL_cutseq_frame == 1300) {
        cutseq_meshbits[1] &= ~0x80000000;
    } else if (GLOBAL_cutseq_frame == 1677) {
        cutseq_meshbits[1] |= 0x80000000;
    }
    handle_lara_chatting(lara_chat_ranges9);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, VON_CROY, voncroy_chat_ranges9);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void tenth_cutseq_init(void)
{
    cutseq_kill_item(DOOR_TYPE5);
    cutseq_meshbits[1] &= ~0x80000000;
    cutseq_meshbits[2] &= ~0x80000000;
    cutseq_meshbits[3] &= ~0x80000000;
    cutseq_meshbits[4] &= ~0x80000000;
}

void tenth_cutseq_control(void)
{
    sint32 frame, z;

    frame = GLOBAL_cutseq_frame;
    if (frame >= 2360 && frame <= 2450 && (GetRandomControl() & 7) >= 4) {
        z = (GetRandomControl() & 0x3FF) + 59714;
        TriggerDynamic(68646, -2304, z, 15, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
    }
    if (frame >= 1203 && frame <= 1205)
        InitialiseHair();
    if (frame == 1205 || frame == 1747 || frame == 2241) {
        if (frame != 2241)
            cutseq_meshbits[1] &= ~0x80000000;
        cutseq_meshbits[2] &= ~0x80000000;
        cutseq_meshbits[3] &= ~0x80000000;
        cutseq_meshbits[4] &= ~0x80000000;
    }
    if (frame == 906 || frame == 1316 || frame == 2007) {
        cutseq_meshbits[1] |= 0x80000000;
        cutseq_meshbits[2] |= 0x80000000;
        cutseq_meshbits[3] |= 0x80000000;
        cutseq_meshbits[4] |= 0x80000000;
    }
    handle_lara_chatting(lara_chat_ranges10);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, ANIMATING1, voncroy_chat_ranges10);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void tenth_cutseq_end(void)
{
    cutseq_restore_item(DOOR_TYPE5);
    DelsHandyTeleportLara(68756, 512, 56493, 983);
}

void eleventh_cutseq_init(void)
{
    ITEM_INFO *item;

    item = find_a_fucking_item(ANIMATING5);
    item->flags &= ~IFL_CODEBITS;
    old_status_flags[numnailed] = item->status;
    item->status = ITEM_INVISIBLE;
    numnailed++;
    KillActiveBaddies(0);
    cutseq_kill_item(MOTORBIKE);
    cutseq_kill_item(ANIMATING4);
    cutseq_kill_item(ANIMATING1);
    cutseq_meshbits[3] &= ~0x80000000;
    cutseq_meshbits[4] &= ~0x80000000;
    cutseq_meshbits[5] &= ~0x80000000;
    cutseq_meshbits[6] &= ~0x80000000;
}

void eleventh_cutseq_control(void)
{
    sint32 frame;

    frame = GLOBAL_cutseq_frame;
    if (frame == 581 || frame == 1423 || frame == 1950)
        cutseq_meshbits[3] |= 0x80000000;
    if (frame == 694 || frame == 1460 || frame == 2251)
        cutseq_meshbits[3] &= ~0x80000000;
    if (frame == 671)
        cutseq_meshbits[5] |= 0x80000000;
    else {
        if (frame == 763 || frame == 1201 || frame == 1668 || frame == 2383)
            cutseq_meshbits[1] |= 0x80000000;
        if (frame == 581 || frame == 1030 || frame == 1394 || frame == 1708)
            cutseq_meshbits[1] &= ~0x80000000;
        if (frame == 2280)
            cutseq_meshbits[2] &= ~0x80000000;
        else if (frame == 2790) {
            cutseq_meshbits[4] |= 0x80000000;
            cutseq_meshbits[6] |= 0x80000000;
        } else {
            if (frame >= 2536 && frame <= 2710) {
                if (GetRandomControl() & 1) {
                    TriggerDynamic(58672 - (GetRandomControl() & 0xFFF), -2816, 57865, 24, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
                }
                TriggerDynamic(60672 - (GetRandomControl() & 0xFFF), -2816, 56865, 31, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, GetRandomControl() & 0x3F);
            }
            if (frame == 2772)
                SetScreenFadeOut(16, 1);
        }
    }
    handle_lara_chatting(lara_chat_ranges11);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 2, ANIMATING8, captain_chat_ranges11);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
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

void fifteen_init(void)
{
    cutseq_meshbits[6] &= 0xFFFFFFFE;
    trigger_item_in_room(184, FLAME_EMITTER2);
    trigger_item_in_room(177, AMBER_LIGHT);
    cutseq_meshbits[1] &= ~0x80000000;
    cutseq_meshbits[2] &= ~0x80000000;
    cutseq_meshbits[3] &= ~0x80000000;
    cutseq_meshbits[4] &= ~0x80000000;
    cutseq_meshbits[5] &= ~0x80000000;
    cutseq_meshbits[6] &= ~0x80000000;
    cutseq_meshbits[7] &= ~0x80000000;
    cutseq_meshbits[8] &= ~0x80000000;
}

void fifteen_control(void)
{
    sint32 frame;

    frame = GLOBAL_cutseq_frame;
    if (frame == 201 || frame == 781 || frame == 1691)
        cutseq_meshbits[1] |= 0x80000000;
    if (frame == 681 || frame == 1306 || frame == 1750)
        cutseq_meshbits[1] &= ~0x80000000;
    if (frame == 360 || frame == 781 || frame == 1402 || frame == 1691) {
        cutseq_meshbits[2] |= 0x80000000;
        cutseq_meshbits[3] |= 0x80000000;
        cutseq_meshbits[4] |= 0x80000000;
        cutseq_meshbits[5] |= 0x80000000;
    }
    if (frame == 681 || frame == 1306 || frame == 1552 || frame == 1750) {
        cutseq_meshbits[2] &= ~0x80000000;
        cutseq_meshbits[3] &= ~0x80000000;
        cutseq_meshbits[4] &= ~0x80000000;
        cutseq_meshbits[5] &= ~0x80000000;
    }
    if (frame == 1601) {
        cutseq_meshbits[6] |= 0x80000000;
        cutseq_meshbits[7] |= 0x80000000;
    } else if (frame == 1691) {
        cutseq_meshbits[8] |= 0x80000000;
        cutseq_meshbits[6] &= ~0x80000000;
    }
    handle_lara_chatting(lara_chat_ranges15);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, ANIMATING9_MIP, voncroy_chat_ranges15);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
}

void fifteen_end(void)
{
    lara.questitems |= 1;
}

void sixteen_init(void)
{
    cutseq_meshbits[6] &= 0xFFFFFFFE;
    trigger_item_in_room(169, FLAME_EMITTER2);
    trigger_item_in_room(184, FLAME_EMITTER2);
    cutseq_meshbits[1] &= ~0x80000000;
    cutseq_meshbits[2] &= ~0x80000000;
    cutseq_meshbits[4] &= ~0x80000000;
    cutseq_meshbits[5] &= ~0x80000000;
    cutseq_meshbits[6] &= ~0x80000000;
    cutseq_meshbits[7] &= ~0x80000000;
    cutseq_meshbits[8] &= ~0x80000000;
    cutseq_meshbits[9] &= ~0x80000000;
}

void sixteen_control(void)
{
    sint32 frame;

    frame = GLOBAL_cutseq_frame;
    if (frame == 1194)
        cutseq_meshbits[3] &= ~0x80000000;
    else {
        if (frame == 1257 || frame == 1740)
            cutseq_meshbits[2] |= 0x80000000;
        if (frame == 1374 || frame == 2130)
            cutseq_meshbits[2] &= ~0x80000000;
        if (frame == 1300 || frame == 2820)
            cutseq_meshbits[1] |= 0x80000000;
        if (frame == 2401)
            cutseq_meshbits[1] &= ~0x80000000;
        else if (frame == 1300)
            cutseq_meshbits[9] |= 0x80000000;
        else if (frame == 2130)
            cutseq_meshbits[9] &= ~0x80000000;
        else if (frame == 2160)
            cutseq_meshbits[6] |= 0x80000000;
        else if (frame == 2340)
            cutseq_meshbits[7] |= 0x80000000;
        else if (frame == 2501) {
            cutseq_meshbits[6] &= ~0x80000000;
            cutseq_meshbits[7] &= ~0x80000000;
            cutseq_meshbits[4] |= 0x80000000;
            cutseq_meshbits[5] |= 0x80000000;
            cutseq_meshbits[8] |= 0x80000000;
        } else if (frame == 2980)
            trigger_item_in_room(177, AMBER_LIGHT);
    }
    handle_lara_chatting(lara_chat_ranges16);
    handle_actor_chatting(ACTOR2_SPEECH_HEAD1, 18, 3, ANIMATING14_MIP, jeanyves_chat_ranges16);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 21, 1, ANIMATING9_MIP, voncroy_chat_ranges16);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
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

void twentythree_control(void)
{
    sint32 f;

    f = GLOBAL_cutseq_frame;
    horus_item_thing->flags |= IFL_TRIGGERED;
    if (f == 194) {
        horus_item_thing->mesh_bits &= ~0x200;
        horus_item_thing->mesh_bits |= 0x20;
    } else if (f == 251) {
        horus_item_thing->mesh_bits &= ~0x400;
        horus_item_thing->mesh_bits |= 0x10;
    } else if (f == 350) {
        horus_item_thing->mesh_bits &= ~0x40;
        horus_item_thing->mesh_bits |= 8;
    } else if (f == 440) {
        horus_item_thing->mesh_bits &= ~4;
        horus_item_thing->mesh_bits |= 0x80;
    } else if (f == 514) {
        horus_item_thing->mesh_bits &= ~2;
        horus_item_thing->mesh_bits |= 0x100;
    } else if (f == 556) {
        horus_item_thing->mesh_bits |= 0x800;
    }
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

void twentyfour_control(void)
{
    if (GLOBAL_cutseq_frame == 552) {
        undraw_pistol_mesh_left(1);
        undraw_pistol_mesh_right(1);
        lara.holster = old_lara_holster;
    }
    handle_lara_chatting(lara_chat_ranges24);
    handle_actor_chatting(ACTOR1_SPEECH_HEAD1, 14, 1, ANIMATING13, troop_chat_ranges24);
    actor_chat_cnt = (actor_chat_cnt - 1) & 1;
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

void cutseq_shoot_pistols(sint32 left_or_right)
{
    if (left_or_right == 14) {
        lara.left_arm.flash_gun = 4;
        SmokeCountL = 16;
    } else {
        lara.right_arm.flash_gun = 4;
        SmokeCountR = 16;
    }
}

void trigger_weapon_dynamics(sint32 left_or_right)
{
    PHD_VECTOR pos;

    pos.x = (GetRandomControl() & 0xFF) - 128;
    pos.y = (GetRandomControl() & 0x7F) - 63;
    pos.z = (GetRandomControl() & 0xFF) - 128;
    GetLaraJointPos(&pos, left_or_right);
    TriggerDynamic(pos.x, pos.y, pos.z, 10, (GetRandomControl() & 0x3F) + 192, (GetRandomControl() & 0x1F) + 128, (GetRandomControl() & 0x3F));
}

void deal_with_pistols(void)
{
    PHD_VECTOR pos;

    if (SmokeCountL) {
        pos.x = 4;
        pos.y = 128;
        pos.z = 40;
        GetLaraJointPos(&pos, 14);
        TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
    }
    if (SmokeCountR) {
        pos.x = -16;
        pos.y = 128;
        pos.z = 40;
        GetLaraJointPos(&pos, 11);
        TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountR);
    }
    if (lara.left_arm.flash_gun) {
        lara.left_arm.flash_gun--;
        trigger_weapon_dynamics(14);
    }
    if (lara.right_arm.flash_gun) {
        lara.right_arm.flash_gun--;
        trigger_weapon_dynamics(11);
    }
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

sint32 Load_and_Init_Cutseq(sint32 num)
{
    sint32 *headerbuf;
    char *packed;
    sint32 Offset, Length, numactors, i;

    Log(5, "Initialising Cut Scene");
    CutSceneTriggered |= 1 << num;
    headerbuf = (sint32 *)cutseqpakPtr + 2 * num;
    Offset = headerbuf[0];
    Length = headerbuf[1];
    Log(5, "Offset=%d, Length=%d\n", Offset, Length);
    packed = &cutseqpakPtr[Offset];
    GLOBAL_cutme = (NEW_CUTSCENE *)packed;
    Log(5, "Loaded cutscene data...\n");
    numactors = GLOBAL_cutme->numactors;
    Log(5, "num actors = %d\n", numactors);
    Log(5, "num frames = %d\n", GLOBAL_cutme->numframes);
    Log(5, "OrgX=%d,OrgY=%d,OrgZ=%d\n", GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz);
    Log(5, "CameraOffset=%d\n", GLOBAL_cutme->camera_offset);
    for (i = 0; i < numactors; i++)
        Log(5, "Actor %d --- offset=%d,slot=%d,nodes=%d\n", i, GLOBAL_cutme->actor_data[i].offset, GLOBAL_cutme->actor_data[i].objslot, GLOBAL_cutme->actor_data[i].nodes);
    init_cutseq_actors(packed, 0);
    return 0;
}

void init_cutseq_actors(char *data, sint32 resident)
{
    char *packed;
    char *resident_addr;
    sint32 pda_nodes, offset;
    sint32 i;

    resident_addr = GLOBAL_resident_depack_buffers;
    lastcamnum = -1;
    GLOBAL_playing_cutseq = 0;
    GLOBAL_numcutseq_frames = GLOBAL_cutme->numframes;
    for (i = 0; i < GLOBAL_cutme->numactors; i++) {
        offset = GLOBAL_cutme->actor_data[i].offset;
        packed = &data[offset];
        pda_nodes = GLOBAL_cutme->actor_data[i].nodes;
        if (resident) {
            actor_pnodes[i] = (PACKNODE *)resident_addr;
            resident_addr += (pda_nodes + 1) * sizeof(PACKNODE);
        } else
            actor_pnodes[i] = (PACKNODE *)cutseq_malloc((pda_nodes + 1) * sizeof(PACKNODE));
        InitPackNodes((NODELOADHEADER *)packed, actor_pnodes[i], packed, pda_nodes + 1);
        memset(&duff_item[i], 0, sizeof(ITEM_INFO));
        duff_item[i].il.ambient = lara_item->il.ambient;
        duff_item[i].il.fcnt = -1;
        duff_item[i].il.room_number = -1;
        duff_item[i].il.RoomChange = 0;
        duff_item[i].il.nCurrentLights = 0;
        duff_item[i].il.nPrevLights = 0;
        duff_item[i].il.pCurrentLights = &duff_item[i].il.CurrentLights;
        duff_item[i].il.pPrevLights = &duff_item[i].il.PrevLights;
        duff_item[i].pos.x_pos = GLOBAL_cutme->orgx;
        duff_item[i].pos.y_pos = GLOBAL_cutme->orgy;
        duff_item[i].pos.z_pos = GLOBAL_cutme->orgz;
        duff_item[i].pos.x_rot = 0;
        duff_item[i].pos.y_rot = 0;
        duff_item[i].pos.z_rot = 0;
        duff_item[i].room_number = 0;
    }
    offset = GLOBAL_cutme->camera_offset;
    packed = &data[offset];
    if (resident)
        camera_pnodes = (PACKNODE *)resident_addr;
    else
        camera_pnodes = (PACKNODE *)cutseq_malloc(2 * sizeof(PACKNODE));
    InitPackNodes((NODELOADHEADER *)packed, camera_pnodes, packed, 2);
    GLOBAL_playing_cutseq = 1;
    GLOBAL_cutseq_frame = 0;
    DelsHandyTeleportLara(GLOBAL_cutme->orgx, GLOBAL_cutme->orgy, GLOBAL_cutme->orgz, 0);
    camera.pos.x = lara_item->pos.x_pos;
    camera.pos.y = lara_item->pos.y_pos;
    camera.pos.z = lara_item->pos.z_pos;
    camera.pos.room_number = lara_item->room_number;
    InitialiseHair();
}

void init_voncroy_meshbits(sint32 num)
{
    cutseq_meshswapbits[num] = 0x240080;
}

void DelsHandyTeleportLara(sint32 x, sint32 y, sint32 z, sint32 yrot)
{
    sint16 room_number;

    room_number = lara_item->room_number;
    lara_item->pos.x_pos = x;
    lara_item->pos.y_pos = y;
    lara_item->pos.z_pos = z;
    lara.head_x_rot = 0;
    lara.head_y_rot = 0;
    lara.torso_x_rot = 0;
    lara.torso_y_rot = 0;
    lara_item->pos.x_rot = 0;
    lara_item->pos.y_rot = (sint16)yrot;
    lara_item->pos.z_rot = 0;
    IsRoomOutside(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
    if (IsRoomOutsideNo != room_number)
        ItemNewRoom(lara.item_number, IsRoomOutsideNo);
    lara_item->current_anim_state = lara_item->goal_anim_state = AS_STOP;
    lara_item->frame_number = anims[ANIM_STOP].frame_base;
    lara_item->anim_number = ANIM_STOP;
    lara_item->speed = 0;
    lara_item->fallspeed = 0;
    lara_item->gravity_status = 0;
    lara.gun_status = LG_NO_ARMS;
    camera.fixed_camera = 1;
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

void handle_actor_chatting(sint32 speechslot, sint32 node, sint32 slot, sint32 objslot, sint16 *_ranges)
{
    sint32 r1, r2, f, rnd;

    rnd = GetRandomControl() & 1;
    f = GLOBAL_cutseq_frame;
    while (1) {
        r1 = _ranges[0];
        r2 = _ranges[1];
        if (r1 == -1)
            break;
        if (f >= r1 && f <= r2) {
            if (!actor_chat_cnt) {
                cutseq_meshswapbits[slot] |= (1 << node);
                meshes[objects[objslot].mesh_index + (2 * node) + 1] = meshes[objects[speechslot + rnd].mesh_index + 2 * node];
                if ((GetRandomControl() & 7) >= 6)
                    cutseq_meshswapbits[slot] &= ~(1 << node);
            }
            return;
        }
        _ranges += 2;
    }
    cutseq_meshswapbits[slot] &= ~(1 << node);
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

void untrigger_item_in_room(sint32 room_number, sint32 object_number)
{
    ITEM_INFO *item;
    sint16 item_number;
    sint16 next;

    item_number = room[room_number].item_number;
    while (item_number != NO_ITEM) {
        item = &items[item_number];
        next = item->next_item;
        if (item->object_number == object_number) {
            RemoveActiveItem(item_number);
            item->status = ITEM_INACTIVE;
            item->flags &= ~IFL_CODEBITS;
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

void special2_end(void)
{
    if (!bDoCredits) {
        InitialiseSpotCam(3);
        lara_item->mesh_bits = 0;
    }
    Chris_Menu = 0;
    title_controls_locked_out = 0;
}

void special2_init(void)
{
    lara_item->mesh_bits = -1;
    Chris_Menu = 0;
}

void special3_end(void)
{
    if (!bDoCredits) {
        InitialiseSpotCam(1);
        lara_item->mesh_bits = 0;
    }
    CutSceneTriggered = 0;
    Chris_Menu = 0;
    title_controls_locked_out = 0;
}

void special3_control(void)
{
    if (GLOBAL_cutseq_frame >= 348 && GLOBAL_cutseq_frame <= 358 && GLOBAL_cutseq_frame & 1)
        DoBloodSplat(6799 - (GetRandomDraw() & 0xFF), (GetRandomDraw() & 0x1FF) - 768, 76209, 7, -1, 24);
}

void special1_init(void)
{
    ITEM_INFO *item;
    sint32 i;

    for (i = 0; i < level_items; i++) {
        item = &items[i];
        if (item->object_number == ANIMATING6) {
            item->anim_number = objects[ANIMATING6].anim_index;
            item->frame_number = anims[item->anim_number].frame_base;
            item->flags &= ~IFL_CODEBITS;
        }
    }
    lara_item->mesh_bits = -1;
    Chris_Menu = 0;
}

void special1_end(void)
{
    if (!bDoCredits) {
        lara_item->mesh_bits = 0;
        InitialiseSpotCam(2);
    }
    Chris_Menu = 0;
    title_controls_locked_out = 0;
}
