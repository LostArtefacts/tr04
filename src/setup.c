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
#include <stdlib.h>

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
