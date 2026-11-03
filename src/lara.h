#ifndef LARA_H
#define LARA_H

#include "types.h"

enum lara_gun_states {
    LG_NO_ARMS,
    LG_HANDS_BUSY,
    LG_DRAW_GUNS,
    LG_UNDRAW_GUNS,
    LG_READY,
    LG_FLARE
};

typedef struct lara_info {
    sint16 item_number;
    sint16 gun_status;
} LARA_INFO;

extern LARA_INFO lara;

#endif
