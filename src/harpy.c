#include "harpy.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void TriggerHarpyMissile(PHD_3DPOS *pos, sint16 room_number, sint16 mesh)
{
    FX_INFO *fx;
    sint16 fx_num;

    fx_num = CreateEffect(room_number);
    if (fx_num != NO_ITEM) {
        fx = &effects[fx_num];
        fx->pos.x_pos = pos->x_pos;
        fx->pos.y_pos = pos->y_pos - (GetRandomControl() & 0x3F) - 32;
        fx->pos.z_pos = pos->z_pos;
        fx->pos.x_rot = pos->x_rot;
        fx->pos.y_rot = pos->y_rot;
        fx->pos.z_rot = 0;
        fx->room_number = room_number;
        fx->counter = (sint16)(2 * GetRandomControl() + 0x8000);
        fx->speed = (GetRandomControl() & 0x1F) + 96;
        fx->object_number = BUBBLES;
        fx->flag1 = mesh;
        fx->frame_number = objects[BUBBLES].mesh_index + mesh * 2;
    }
}
