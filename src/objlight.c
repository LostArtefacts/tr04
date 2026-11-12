#include <stdlib.h>
#include "objlight.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "effect2.h"
#include "sphere.h"
#include "specific/function_stubs.h"
#include "specific/3dmath.h"
#include "camera.h"
#include "gameflow.h"
#include "room.h"
#include "sound.h"
#include "tomb4fx.h"

void ControlPulseLight(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 sin, r, g, b;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (!flip_stats[4] && gfLevelFlags & GF_PULSE)
        return;
    if (item->trigger_flags == 1) {
        item->trigger_flags = 0;
        FlashFadeR = 128;
        FlashFadeG = 255;
        FlashFadeB = 255;
        FlashFader = 32;
        camera.bounce = -128;
        SoundEffect(SFX_BOULDER_FALL, 0, SFX_DEFAULT);
        SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
    } else if (item->trigger_flags == 2) {
        SoundEffect(SFX_MAPPER_PYRAMID_OPEN, &item->pos, SFX_DEFAULT);
        if (room[camera.pos.room_number].flags & ROOM_NOT_INSIDE) {
            FlashFadeR = 64;
            FlashFadeG = 128;
            FlashFadeB = 128;
            FlashFader = 32;
        }
    }
    item->item_flags[0] -= 2048;
    sin = abs(phd_sin(item->item_flags[0] + ((item->pos.y_pos & 0x3FFF) << 2)) >> 6);
    if (sin > 255)
        sin = 255;
    r = (sin * 64) >> 9;
    g = (sin * 255) >> 9;
    b = (sin * 255) >> 9;
    TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 18, r, g, b);
}

void ControlElectricalLight(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 shade, r, g, b;

    item = &items[item_number];
    if (TriggerActive(item)) {
        if (item->item_flags[0] < 16) {
            shade = (GetRandomControl() & 0x3F) << 2;
            item->item_flags[0]++;
        } else if (item->item_flags[0] < 96) {
            if ((wibble & 0x3F) == 0 || (GetRandomControl() & 7) == 0)
                shade = 192 - (GetRandomControl() & 0x3F);
            else
                shade = GetRandomControl() & 0x3F;
            item->item_flags[0]++;
        } else if (item->item_flags[0] < 160) {
            shade = 96 - (GetRandomControl() & 0x1F);
            if ((GetRandomControl() & 0x1F) == 0 && item->item_flags[0] > 128)
                item->item_flags[0] = 160;
            else
                item->item_flags[0]++;
        } else {
            shade = 255 - (GetRandomControl() & 0x1F);
        }
        r = (shade * ((item->trigger_flags & 0x1F) << 3)) >> 8;
        g = (shade * ((item->trigger_flags >> 2) & 0xF8)) >> 8;
        b = (shade * ((item->trigger_flags >> 7) & 0xF8)) >> 8;
        TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 16, r, g, b);
    } else {
        item->item_flags[0] = 0;
    }
}

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
