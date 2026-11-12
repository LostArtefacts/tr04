#include "objlight.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "effect2.h"
#include "sphere.h"
#include "specific/function_stubs.h"

void ControlBlinker(sint16 item_number)
{
    ITEM_INFO *item;
    PHD_VECTOR pos;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    item->trigger_flags--;
    if (item->trigger_flags < 3) {
        pos.z = 0;
        pos.y = 0;
        pos.x = 0;
        GetJointAbsPosition(item, &pos, 0);
        TriggerDynamic(pos.x, pos.y, pos.z, 16, 255, 192, 16);
        item->mesh_bits = 2;
        if (item->trigger_flags < 0)
            item->trigger_flags = 30;
    } else
        item->mesh_bits = 1;
}
