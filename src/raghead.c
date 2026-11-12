#include "raghead.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "specific/3dmath.h"

void InitialiseRaghead(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 obj_num, flag;

    item = &items[item_number];
    InitialiseCreature(item_number);
    if (objects[SUPER_RAGHEAD].loaded)
        obj_num = SUPER_RAGHEAD;
    else
        obj_num = RAGHEAD;
    if (item->object_number == RAGHEAD) {
        item->meshswap_meshbits = 0x7FC010;
        item->mesh_bits = ~0x7E0000;
        item->item_flags[2] = 24;
    } else {
        item->meshswap_meshbits = 0x880;
        item->mesh_bits = -1;
        item->item_flags[2] = 0;
    }
    item->item_flags[1] = -1;
    flag = item->trigger_flags % 1000;
    if (flag > 9 && flag < 20) {
        item->item_flags[2] += 24;
        item->trigger_flags -= 10;
        flag -= 10;
    }
    if (!flag || (flag > 4 && flag < 7)) {
        item->anim_number = objects[obj_num].anim_index + 18;
        item->current_anim_state = item->goal_anim_state = 0;
    } else if (flag == 1) {
        item->anim_number = objects[obj_num].anim_index + 47;
        item->current_anim_state = item->goal_anim_state = 24;
    } else if (flag == 2) {
        item->anim_number = objects[obj_num].anim_index + 24;
        item->current_anim_state = item->goal_anim_state = 23;
    } else if (flag == 3) {
        item->anim_number = objects[obj_num].anim_index + 29;
        item->current_anim_state = item->goal_anim_state = 26;
    } else if (flag == 4) {
        item->anim_number = objects[obj_num].anim_index + 62;
        item->current_anim_state = item->goal_anim_state = 39;
        item->pos.x_pos += (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 10) >> W2V_SHIFT;
        item->pos.z_pos += (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 10) >> W2V_SHIFT;
    } else if (flag > 100) {
        item->anim_number = objects[obj_num].anim_index + 29;
        item->current_anim_state = item->goal_anim_state = 26;
        item->pos.x_pos += (rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 10) >> W2V_SHIFT;
        item->pos.z_pos += (rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 10) >> W2V_SHIFT;
        item->item_flags[3] = flag;
    }
    item->frame_number = anims[item->anim_number].frame_base;
}
