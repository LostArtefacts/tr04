#include "train.h"
#include "lara.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "camera.h"
#include "sound.h"
#include "specific/3dmath.h"
#include "specific/specificfx.h"

void DrawTrainFloor(void)
{
    sint32 x;

    x = lara_item->pos.x_pos;
    lara_item->pos.x_pos = camera.pos.x;
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos & -1024, 0, 52224);
    DrawTrainStrips();
    camera.bounce = -12;
    phd_PopMatrix();
    DrawTrainObjects();
    lara_item->pos.x_pos = x;
}

void InitialiseTrainJeep(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint16 i;

    item = &items[item_number];
    item->item_flags[0] = -80;
    item2 = items;
    for (i = 0; i < level_items; i++, item2++) {
        if (item != item2 && item2->trigger_flags == item->trigger_flags) {
            item->item_flags[1] = i;
            item2->pos.y_pos = item->pos.y_pos - 1024;
            item2->item_flags[0] = -80;
            return;
        }
    }
}

void TrainJeepControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint16 room_number;

    item = &items[item_number];
    if (item->item_flags[0] != -80) {
        if (item->item_flags[2] > 1024)
            item->item_flags[2] -= 512;
    } else if (item->item_flags[2] < 0x4000)
        item->item_flags[2] += 32;
    SoundEffect(SFX_JEEP_MOVE, &item->pos, (item->item_flags[2] << 9) + (0x1000000 | SFX_SETPITCH));
    room_number = item->room_number;
    item->pos.x_pos += item->item_flags[0];
    AnimateItem(item);
    GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (room_number != item->room_number)
        ItemNewRoom(item_number, room_number);
    item2 = &items[item->item_flags[1]];
    if (item2->item_flags[0] > -40 || item2->hit_points < 1)
        item->item_flags[0] += 3;
    if (item->item_flags[0] > 400)
        KillItem(item_number);
}
