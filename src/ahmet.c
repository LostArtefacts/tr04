#include "ahmet.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "objnums.h"
#include "sound.h"
#include "sphere.h"
#include "specific/function_stubs.h"

void InitialiseAhmet(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[AHMET].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
    item->item_flags[0] = (sint16)(item->pos.x_pos >> 10);
    item->item_flags[1] = (sint16)(item->pos.y_pos >> 8);
    item->item_flags[2] = (sint16)(item->pos.z_pos >> 10);
}

void ExplodeAhmet(ITEM_INFO *item)
{
    SPHERE *sphere;
    sint32 spheres, i, r, g;

    if (!(wibble & 7)) {
        spheres = GetSpheres(item, Slist, 1);
        sphere = &Slist[((uint32)wibble >> 3) & 1];
        for (i = 0; i < spheres; i += 2, sphere += 2)
            TriggerFireFlame(sphere->x, sphere->y, sphere->z, -1, 1);
    }
    r = (GetRandomControl() & 0x3F) + 192;
    g = (GetRandomControl() & 0x1F) + 96;
    TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 13, r, g, 0);
    SoundEffect(SFX_LOOP_FOR_SMALL_FIRES, &item->pos, SFX_DEFAULT);
}
