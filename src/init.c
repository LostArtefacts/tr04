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
