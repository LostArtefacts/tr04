#ifndef DEBRIS_H
#define DEBRIS_H

#include "3dglodef.h"

typedef struct debris_struct {
    void *TextInfo;
    sint32 x;
    sint32 y;
    sint32 z;
    sint16 XYZOffsets1[3];
    sint16 Dir;
    sint16 XYZOffsets2[3];
    sint16 Speed;
    sint16 XYZOffsets3[3];
    sint16 Yvel;
    sint16 Gravity;
    sint16 RoomNumber;
    uint8 On;
    uint8 XRot;
    uint8 YRot;
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 Pad[2];
    sint32 color1;
    sint32 color2;
    sint32 color3;
    sint32 ambient;
    sint32 flags;
} DEBRIS_STRUCT;

sint32 GetFreeDebris(void);

extern DEBRIS_STRUCT debris[256];
extern sint32 next_debris;
extern sint16 DebrisFlags;

#endif
