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
