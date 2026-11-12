#include "croc.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "draw.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "room.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/output.h"

void InitialiseCroc(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (room[item->room_number].flags & ROOM_UNDERWATER) {
        item->anim_number = objects[CROCODILE].anim_index + 12;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 8;
    } else {
        item->anim_number = objects[CROCODILE].anim_index;
        item->frame_number = anims[item->anim_number].frame_base;
        item->current_anim_state = item->goal_anim_state = 1;
    }
}

sint32 GetFreeLocust(void)
{
    LOCUST_STRUCT *fx;
    sint32 free, i;

    free = next_locust;
    fx = &Locusts[free];
    for (i = 0; i < 64; i++) {
        if (!fx->On) {
            next_locust = (free + 1) & 0x3F;
            return free;
        }
        if (free == 63) {
            fx = Locusts;
            free = 0;
        } else {
            free++;
            fx++;
        }
    }
    return NO_ITEM;
}

void TriggerLocust(ITEM_INFO *item)
{
    LOCUST_STRUCT *fx;
    PHD_VECTOR vec;
    PHD_VECTOR vec2;
    sint16 fx_number;
    sint16 angles[2];

    fx_number = GetFreeLocust();
    if (fx_number == NO_ITEM)
        return;
    fx = &Locusts[fx_number];
    if (item->object_number == FISH) {
        vec.x = item->pos.x_pos;
        vec.y = item->pos.y_pos;
        vec.z = item->pos.z_pos;
        angles[0] = item->pos.y_rot + 0x8000;
        angles[1] = 0;
    } else {
        vec2.x = 0;
        vec2.y = -96;
        vec2.z = 144;
        GetJointAbsPosition(item, &vec2, 9);
        vec.x = 0;
        vec.y = -128;
        vec.z = 288;
        GetJointAbsPosition(item, &vec, 9);
        phd_GetVectorAngles(vec.x - vec2.x, vec.y - vec2.y, vec.z - vec2.z, angles);
    }
    fx->room_number = item->room_number;
    fx->pos.x_pos = vec.x;
    fx->pos.y_pos = vec.y;
    fx->pos.z_pos = vec.z;
    fx->pos.y_rot = (GetRandomControl() & 0x7FF) + angles[0] - 1024;
    fx->pos.x_rot = (GetRandomControl() & 0x3FF) + angles[1] - 512;
    fx->On = 1;
    fx->flags = 0;
    fx->speed = (GetRandomControl() & 0x1F) + 16;
    fx->LaraTarget = GetRandomControl() & 0x1FF;
    fx->Counter = 20 * ((GetRandomControl() & 7) + 15);
}

void InitialiseLocustEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    switch (item->pos.y_rot) {
    case 0:
        item->pos.z_pos += 512;
        break;
    case 16384:
        item->pos.x_pos += 512;
        break;
    case -32768:
        item->pos.z_pos -= 512;
        break;
    case -16384:
        item->pos.x_pos -= 512;
        break;
    }
}

void ControlLocustEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (!item->trigger_flags) {
        KillItem(item_number);
    } else {
        TriggerLocust(item);
        item->trigger_flags--;
    }
}

void DrawLocusts(void)
{
    LOCUST_STRUCT *fx;
    sint16 **meshpp;
    sint32 i;

    fx = Locusts;
    for (i = 64; i > 0; i--, fx++) {
        if (fx->On) {
            meshpp = &meshes[objects[AHMET_MIP].mesh_index + 2 * (-GlobalCounter & 3)];
            phd_PushMatrix();
            phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
            phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
            phd_PutPolygons_train(*meshpp, 0);
            phd_PopMatrix();
        }
    }
}

void TriggerCrocgodMissile(PHD_3DPOS *pos, sint16 room_number, sint16 num)
{
    FX_INFO *fx;
    sint16 fx_number;

    fx_number = CreateEffect(room_number);
    if (fx_number != NO_ITEM) {
        fx = &effects[fx_number];
        fx->pos.x_pos = pos->x_pos;
        fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
        fx->pos.z_pos = pos->z_pos;
        fx->pos.x_rot = pos->x_rot;
        fx->pos.y_rot = pos->y_rot;
        fx->pos.z_rot = 0;
        fx->room_number = room_number;
        fx->counter = 15 + (num << 4);
        fx->speed = (GetRandomControl() & 0x1F) + 96;
        fx->flag1 = 6;
        fx->object_number = 384;
        fx->frame_number = objects[BUBBLES].mesh_index + 10;
    }
}

void InitialiseCrocgod(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[MUTANT].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}
