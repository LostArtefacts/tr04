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
