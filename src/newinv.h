#ifndef NEWINV_H
#define NEWINV_H

#include "types.h"

void DEL_picked_up_object(sint16 objnum);
sint32 have_i_got_object(sint16 object_number);

extern sint32 GLOBAL_enterinventory;
extern sint32 GLOBAL_inventoryitemchosen;

#endif
