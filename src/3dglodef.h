#ifndef _3DGLODEF_H
#define _3DGLODEF_H

#include "types.h"

typedef struct phd_vector {
    sint32 x;
    sint32 y;
    sint32 z;
} PHD_VECTOR;

typedef sint16 PHD_ANGLE;

typedef struct cvector {
    uint8 b;
    uint8 g;
    uint8 r;
    uint8 a;
} CVECTOR;

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

typedef struct phd_3dpos {
    sint32 x_pos;
    sint32 y_pos;
    sint32 z_pos;
    PHD_ANGLE x_rot;
    PHD_ANGLE y_rot;
    PHD_ANGLE z_rot;
} PHD_3DPOS;

typedef struct game_vector {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 room_number;
    sint16 box_number;
} GAME_VECTOR;

typedef struct object_vector {
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 data;
    sint16 flags;
} OBJECT_VECTOR;

#endif
