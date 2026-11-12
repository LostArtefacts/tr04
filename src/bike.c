#include "bike.h"
#include "control.h"
#include "lara.h"
#include "lara1gun.h"
#include "lara_states.h"
#include "anim.h"
#include "objnums.h"
#include "box.h"
#include "sound.h"
#include "effect2.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/audio.h"
#include "specific/function_stubs.h"
#include "specific/specificfx.h"

void DrawBikeExtras(ITEM_INFO *item)
{
    if (lara.vehicle != NO_ITEM)
        DrawBikeSpeedo(phd_winwidth - 64, phd_winheight - 16, ((BIKEINFO *)item->data)->velocity, 0x8000, 0xC000, 32, 0);
    DrawBikeBeam(GlobalBikeItem);
}

void BikeExplode(ITEM_INFO *item)
{
    sint32 i;

    if (room[item->room_number].flags & ROOM_UNDERWATER)
        TriggerUnderwaterExplosion(item, 1);
    else {
        TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -2, 0, item->room_number);
        for (i = 0; i < 3; i++)
            TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 3, -1, 0, item->room_number);
    }
    ExplodingDeath2(lara.vehicle, -2, 256);
    KillItem(lara.vehicle);
    item->status = ITEM_DEACTIVATED;
    SoundEffect(SFX_EXPLOSION1, 0, SFX_DEFAULT);
    SoundEffect(SFX_EXPLOSION2, 0, SFX_DEFAULT);
    lara.vehicle = NO_ITEM;
}
