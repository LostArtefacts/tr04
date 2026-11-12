#include <stdlib.h>

#include "sphinx.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effects.h"
#include "lara.h"
#include "objnums.h"
#include "room.h"
#include "sound.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseSphinx(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    InitialiseCreature(item_number);
    item->anim_number = objects[SPHINX].anim_index + 1;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = 1;
}

void SphinxControl(sint16 item_number)
{
    ITEM_INFO *item;
    CREATURE_INFO *sphinx;
    FLOOR_INFO *floor;
    ROOM_INFO *r;
    MESH_INFO *mesh;
    AI_INFO info;
    sint32 s, c, x, y, z, h1, h2;
    sint16 i, room_number, angle;

    if (!CreatureActive(item_number))
        return;
    item = &items[item_number];
    sphinx = (CREATURE_INFO *)item->data;
    room_number = item->room_number;
    s = 614 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
    c = 614 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
    x = item->pos.x_pos + s;
    y = item->pos.y_pos;
    z = item->pos.z_pos + c;
    floor = GetFloor(x, y, z, &room_number);
    h1 = GetHeight(floor, x, y, z);
    if (item->current_anim_state == 5 && floor->stopper) {
        r = &room[item->room_number];
        mesh = r->mesh;
        for (i = r->num_meshes; i > 0; i--, mesh++) {
            if (!((mesh->z ^ z) & ~0x3FF) && !((mesh->x ^ x) & ~0x3FF) && mesh->static_number >= SHATTER0) {
                ShatterObject(0, mesh, -64, item->room_number, 0);
                SoundEffect(SFX_HIT_ROCK, &item->pos, 0);
                mesh->Flags &= ~1;
                floor->stopper = 0;
                TestTriggers(trigger_index, 1, 0);
            }
        }
    }
    x = item->pos.x_pos - s;
    z = item->pos.z_pos - c;
    floor = GetFloor(x, y, z, &room_number);
    h2 = GetHeight(floor, x, y, z);
    phd_atan(1228, h2 - h1);
    if (item->ai_bits)
        GetAITarget(sphinx);
    else
        sphinx->enemy = lara_item;
    CreatureAIInfo(item, &info);
    if (sphinx->enemy != lara_item)
        phd_atan(lara_item->pos.z_pos - item->pos.z_pos, lara_item->pos.x_pos - item->pos.x_pos);
    GetCreatureMood(item, &info, 1);
    CreatureMood(item, &info, 1);
    angle = CreatureTurn(item, sphinx->maximum_turn);
    x = abs(item->item_flags[2] - (sint16)item->pos.x_pos);
    z = abs(item->item_flags[3] - (sint16)item->pos.z_pos);
    switch (item->current_anim_state) {
    case 1:
        sphinx->maximum_turn = 0;
        if (info.distance < 0x100000 || item->trigger_flags)
            item->goal_anim_state = 3;
        else if (!GetRandomControl() & 0xFF)
            item->goal_anim_state = 2;
        break;
    case 2:
        sphinx->maximum_turn = 0;
        if (info.distance < 0x100000 || item->trigger_flags)
            item->goal_anim_state = 3;
        else if (!GetRandomControl() & 0x3F)
            item->goal_anim_state = 1;
        break;
    case 9:
        sphinx->flags = 0;
        if (item->required_anim_state == 6)
            item->goal_anim_state = 6;
        else
            item->goal_anim_state = 4;
        break;
    case 4:
        sphinx->maximum_turn = 546;
        if (info.distance > 0x400000 && abs(info.angle) <= 512 || item->required_anim_state == 5) {
            item->goal_anim_state = 5;
        } else if (info.distance < 0x400000 && item->goal_anim_state != 5) {
            if (h2 <= item->pos.y_pos + 256 && h2 >= item->pos.y_pos - 256) {
                item->goal_anim_state = 9;
                item->required_anim_state = 6;
            }
        }
        break;
    case 5:
        sphinx->maximum_turn = 60;
        if (!sphinx->flags && item->touch_bits & 0x40) {
            CreatureEffectT(item, &sphinx_bite, 20, -1, DoBloodSplat);
            lara_item->hit_points -= 200;
            sphinx->flags = 1;
        }
        if (x < 50 && z < 50 && item->anim_number - objects[SPHINX].anim_index == 0) {
            item->goal_anim_state = 7;
            item->required_anim_state = 6;
            sphinx->maximum_turn = 0;
        } else if (info.distance > 0x400000 && abs(info.angle) > 512) {
            item->goal_anim_state = 9;
        }
        break;
    case 6:
        sphinx->maximum_turn = 546;
        if (info.distance > 0x400000 || h2 > item->pos.y_pos + 256 || h2 < item->pos.y_pos - 256) {
            item->goal_anim_state = 9;
            item->required_anim_state = 5;
        }
        break;
    case 7:
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        if (item->frame_number == anims[item->anim_number].frame_base) {
            TestTriggers(trigger_index, 1, 0);
            if (item->touch_bits & 0x40) {
                CreatureEffectT(item, &sphinx_bite, 50, -1, DoBloodSplat);
                lara_item->hit_points = 0;
            }
        }
        break;
    }
    item->item_flags[2] = (sint16)item->pos.x_pos;
    item->item_flags[3] = (sint16)item->pos.z_pos;
    CreatureAnimation(item_number, angle, 0);
}
