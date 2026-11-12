#include "init.h"
#include "items.h"
#include "objects.h"
#include "objnums.h"
#include "anim.h"
#include "room.h"
#include "control.h"
#include "door.h"
#include "traps.h"
#include "draw.h"
#include "rope.h"
#include "scarab.h"
#include "debris.h"
#include "box.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "pickup.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseTrapDoor(sint16 item_number)
{
    CloseTrapDoor(&items[item_number]);
}

void InitialiseFallingBlock2(sint16 item_number)
{
    items[item_number].mesh_bits = 1;
}

void InitialiseFlameEmitter(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (item->trigger_flags < 0) {
        item->item_flags[0] = (GetRandomControl() & 0x3F) + 90;
        item->item_flags[2] = 256;
        if ((-item->trigger_flags & 7) == 7) {
            switch (item->pos.y_rot) {
            case 0:
                item->pos.z_pos += 512;
                break;
            case 0x4000:
                item->pos.x_pos += 512;
                break;
            case -0x8000:
                item->pos.z_pos -= 512;
                break;
            case -0x4000:
                item->pos.x_pos -= 512;
                break;
            }
        }
    }
}

void InitialiseJobySpike(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint32 h, c;
    sint16 room_number;

    item = &items[item_number];
    item->pos.y_rot = (sint16)(GetRandomControl() << 10);
    item->item_flags[2] = GetRandomControl() & 1;
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    item->item_flags[3] = (sint16)((4096 * (h - c)) / 3328);
}

void InitialiseTwoBlockPlatform(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->item_flags[0] = (sint16)item->pos.y_pos;
    item->item_flags[1] = 1;
}

void InitialiseBurningFloor(sint16 item_number)
{
    items[item_number].required_anim_state = 127;
}
