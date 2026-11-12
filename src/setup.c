#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "camera.h"
#include "control.h"
#include "specific/audio.h"
#include "footprnt.h"
#include "deltapak.h"
#include "tomb4fx.h"
#include "effect2.h"
#include "specific/function_stubs.h"
#include "specific/specificfx.h"
#include "specific/3dmath.h"
#include "anim.h"
#include "objnums.h"
#include "box.h"
#include "items.h"
#include "pickup.h"
#include "room.h"
#include "lara.h"
#include "laramisc.h"
#include "gameflow.h"
#include "draw.h"
#include "hair.h"
#include "init.h"
#include "switch.h"
#include "traps.h"

void InitialiseLara(void)
{
    ITEM_INFO *l;
    sint32 item_num;
    sint16 gun;

    item_num = lara.item_number;
    if (item_num == NO_ITEM)
        return;
    l = lara_item;
    l->data = &lara;
    l->collidable = 0;
    memset(&lara, 0, sizeof(LARA_INFO));
    lara.look = 1;
    lara.item_number = item_num;
    lara.hit_direction = -1;
    lara.air = 1800;
    lara.vehicle = NO_ITEM;
    lara.weapon_item = NO_ITEM;
    lara.water_surface_dist = 100;
    lara.holster = LARA_HOLSTERS_PISTOLS;
    lara.location = -1;
    lara.highest_location = -1;
    lara.RopePtr = -1;
    l->hit_points = 1000;
    lara.gun_status = LG_NO_ARMS;
    if (gfLevelFlags & GF_YOUNGLARA)
        gun = WEAPON_NONE;
    else
        gun = WEAPON_PISTOLS;
    lara.request_gun_type = lara.gun_type = lara.last_gun_type = gun;
    LaraInitialiseMeshes();
    lara.pistols_type_carried = 9;
    lara.binoculars = 1;
    lara.num_flares = 3;
    lara.num_small_medipack = 3;
    lara.num_large_medipack = 1;
    lara.num_pistols_ammo = -1;
    if (Gameflow->DemoDisc)
        lara.crowbar = 1;
    lara.beetle_uses = 3;
    InitialiseLaraAnims(l);
    DashTimer = 120;
}

void InitialiseObjects(void)
{
    OBJECT_INFO *obj;
    sint32 i, n;

    obj = objects;
    for (i = NUMBER_OBJECTS; i > 0; i--, obj++) {
        obj->initialise = 0;
        obj->collision = 0;
        obj->control = 0;
        obj->intelligent = 0;
        obj->save_position = 0;
        obj->save_hitpoints = 0;
        obj->save_flags = 0;
        obj->save_anim = 0;
        obj->water_creature = 0;
        obj->using_drawanimating_item = 1;
        obj->save_mesh = 0;
        obj->draw_routine = DrawAnimatingItem;
        obj->ceiling = 0;
        obj->floor = 0;
        obj->pivot_length = 0;
        obj->radius = 10;
        obj->shadow_size = 0;
        obj->hit_points = -16384;
        obj->explodable_meshbits = 0;
        obj->draw_routine_extra = 0;
        obj->frame_base = (sint16 *)((sint32)obj->frame_base + (char *)frames);
        obj->object_mip = 0;
    }
    BaddyObjects();
    ObjectObjects();
    TrapObjects();
    InitialiseHair();
    InitialiseEffects();
    NumRPickups = 0;
    CurrentSequence = 0;
    SequenceResults[0][1][2] = 0;
    SequenceResults[0][2][1] = 1;
    SequenceResults[1][0][2] = 2;
    SequenceResults[1][2][0] = 3;
    SequenceResults[2][0][1] = 4;
    SequenceResults[2][1][0] = 5;
    memset(SequenceUsed, 0, 6);
    memset(LibraryTab, 0, 8);
    n = gfNumMips;
    for (i = 0; i < n; i++) {
        obj = &objects[((gfMips[i] & 0xF) << 1) + ANIMATING1];
        obj->object_mip = (gfMips[i] & 0xF0) << 6;
    }
}

void GetCarriedItems(void)
{
    ITEM_INFO *baddy;
    ITEM_INFO *pickup;
    sint32 i;
    sint16 item_num;

    for (i = 0, baddy = items; i < level_items; i++, baddy++) {
        baddy->carried_item = NO_ITEM;
        if (objects[baddy->object_number].intelligent && baddy->object_number != SCORPION) {
            item_num = room[baddy->room_number].item_number;
            while (item_num != NO_ITEM) {
                pickup = &items[item_num];
                if (pickup->pos.x_pos == baddy->pos.x_pos && pickup->pos.z_pos == baddy->pos.z_pos && abs(pickup->pos.y_pos - baddy->pos.y_pos) < 256 && objects[pickup->object_number].collision == PickUpCollision) {
                    pickup->carried_item = baddy->carried_item;
                    baddy->carried_item = item_num;
                    RemoveDrawnItem(item_num);
                    pickup->room_number = 255;
                    break;
                }
                item_num = pickup->next_item;
            }
        }
    }
}

void GetAIPickups(void)
{
    ITEM_INFO *item;
    AIOBJECT *aiObj;
    sint32 i;
    sint16 aiObjNum;

    for (i = 0, item = items; i < level_items; i++, item++) {
        if (objects[item->object_number].intelligent) {
            item->ai_bits = 0;
            for (aiObjNum = 0; aiObjNum < nAIObjects; aiObjNum++) {
                aiObj = &AIObjects[aiObjNum];
                if (aiObj->x == item->pos.x_pos && aiObj->z == item->pos.z_pos && aiObj->room_number == item->room_number && aiObj->object_number < AI_PATROL2) {
                    item->ai_bits |= 1 << (aiObj->object_number - AI_GUARD);
                    item->item_flags[3] = aiObj->trigger_flags;
                    aiObj->room_number = 255;
                }
            }
        }
    }
}

void InitialiseGameFlags(void)
{
    memset(flipmap, 0, sizeof(flipmap));
    memset(flip_stats, 0, sizeof(flip_stats));
    flip_status = 0;
    memset(cd_flags, 0, sizeof(cd_flags));
    flipeffect = -1;
    IsAtmospherePlaying = 0;
    camera.underwater = 0;
}

void SetupGame(void)
{
    SeedRandomDraw(0xD371F947);
    SeedRandomControl(0xD371F947);
    wibble = 0;
    ClearFootPrints();
    InitBinoculars();
    InitTarget();
    trainmappos = 0;
    InitialiseGameFlags();
    InitialiseLara();
    GetCarriedItems();
    GetAIPickups();
    SeedRandomDraw(0xD371F947);
    SeedRandomControl(0xD371F947);
}

void InitTarget(void)
{
    sint16 **meshpp;
    sint16 *meshp;
    sint16 *vtx;
    sint32 nVtx;
    sint32 i;

    meshpp = &meshes[objects[TARGET_GRAPHICS].mesh_index];
    meshp = *meshpp;
    targetMeshP = (MESH_DATA *)meshp;
    nVtx = meshp[4] & 0xFF;
    vtx = meshp + 6;
    for (i = 0; i < nVtx; i++) {
        *vtx++ = (sint16)(phd_centerx + 80 * *vtx / 96);
        *vtx = (sint16)(phd_centery + 60 * *vtx / 224);
        vtx += 3;
    }
}

void ClearFootPrints(void)
{
    sint32 i;

    for (i = 0; i < 32; i++)
        FootPrint[i].Active = 0;
    FootPrintNum = 0;
}

void reset_cutseq_vars(void)
{
    cutseq_num = 0;
    cutseq_trig = 0;
    GLOBAL_playing_cutseq = 0;
    GLOBAL_cutseq_frame = 0;
    SetFadeClip(0, 1);
}
