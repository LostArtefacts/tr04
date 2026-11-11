#include "pickup.h"
#include "lara.h"
#include "lara_states.h"
#include "anim.h"
#include "room.h"
#include "control.h"
#include "draw.h"
#include "objects.h"
#include "newinv.h"
#include "sound.h"
#include "specific/input.h"

void AnimatingPickUp(sint16 item_number)
{
    if ((items[item_number].trigger_flags & 0x3F) == 2)
        AnimateItem(&items[item_number]);
}
