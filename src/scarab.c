#include "scarab.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"
#include "specific/output.h"

void InitialiseScarab(sint16 item_number)
{
    ITEM_INFO *item;

    InitialiseCreature(item_number);
    item = &items[item_number];
    item->anim_number = objects[BIG_BEETLE].anim_index + 3;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}

sint32 GetFreeScarab(void)
{
    SCARAB_STRUCT *fx;
    sint32 free, i;

    free = next_scarab;
    fx = &Scarabs[free];
    for (i = 0; i < 128; i++) {
        if (!fx->On) {
            next_scarab = (free + 1) & 0x7F;
            return free;
        }
        if (free == 127) {
            fx = Scarabs;
            free = 0;
        } else {
            free++;
            fx++;
        }
    }
    return -1;
}

void ClearScarabs(void)
{
    SCARAB_STRUCT *fx;
    sint32 i;

    fx = Scarabs;
    for (i = 128; i > 0; i--, fx++)
        fx->On = 0;
    next_scarab = 0;
    flipeffect = -1;
}

void TriggerScarab(sint16 item_number)
{
    SCARAB_STRUCT *fx;
    ITEM_INFO *item;
    sint16 fx_num;

    item = &items[item_number];
    if (item->trigger_flags && (!item->item_flags[2] || !(GetRandomControl() & 0xF))) {
        item->trigger_flags--;
        if (item->item_flags[2] && GetRandomControl() & 1)
            item->item_flags[2]--;
        fx_num = (sint16)GetFreeScarab();
        if (fx_num != -1) {
            fx = &Scarabs[fx_num];
            fx->pos.x_pos = item->pos.x_pos;
            fx->pos.y_pos = item->pos.y_pos;
            fx->pos.z_pos = item->pos.z_pos;
            fx->room_number = item->room_number;
            if (item->item_flags[0]) {
                fx->pos.y_rot = (sint16)(GetRandomControl() << 1);
                fx->fallspeed = -16 - (GetRandomControl() & 0x1F);
            } else {
                fx->pos.y_rot = (GetRandomControl() & 0x3FFF) + item->pos.y_rot - 8192;
                fx->fallspeed = 0;
            }
            fx->pos.x_rot = 0;
            fx->pos.z_rot = 0;
            fx->On = 1;
            fx->flags = 0;
            fx->speed = (GetRandomControl() & 0x1F) + 1;
        }
    }
}

void DrawScarabs(void)
{
    SCARAB_STRUCT *fx;
    sint16 **meshpp;
    sint32 i;

    meshpp = &meshes[objects[LITTLE_BEETLE].mesh_index + (wibble >> 2 & 2)];
    fx = Scarabs;
    for (i = 128; i > 0; i--, fx++) {
        if (fx->On) {
            phd_PushMatrix();
            phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
            phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
            phd_PutPolygons_train(*meshpp, 0);
            phd_PopMatrix();
        }
    }
}

void InitialiseScarabGenerator(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 flag, rot;

    item = &items[item_number];
    flag = item->trigger_flags / 1000;
    rot = item->pos.y_rot;
    item->pos.x_rot = 0x2000;
    item->item_flags[0] = flag & 1;
    item->item_flags[1] = flag & 2;
    item->item_flags[2] = flag & 4;
    item->trigger_flags %= 1000;
    if (!item->item_flags[0]) {
        if (rot > 4096 && rot < 28672)
            item->pos.x_pos -= 512;
        else if (rot < -4096 && rot > -28672)
            item->pos.x_pos += 512;
        if (rot > -8192 && rot < 8192)
            item->pos.z_pos -= 512;
        else if (rot < -20480 || rot > 20480)
            item->pos.z_pos += 512;
    }
}
