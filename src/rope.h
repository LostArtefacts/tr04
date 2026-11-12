#ifndef ROPE_H
#define ROPE_H

#include "3dglodef.h"

typedef struct {
    PHD_VECTOR Segment[24];
    PHD_VECTOR Velocity[24];
    PHD_VECTOR NormalisedSegment[24];
    PHD_VECTOR MeshSegment[24];
    PHD_VECTOR Position;
    sint32 Coords[24][3];
    sint32 SegmentLength;
    sint32 Active;
} ROPE_STRUCT;

typedef struct {
    PHD_VECTOR Position;
    PHD_VECTOR Velocity;
    sint32 node;
    ROPE_STRUCT *Rope;
} PENDULUM;

extern ROPE_STRUCT RopeList[5];
extern PENDULUM CurrentPendulum;

#endif
