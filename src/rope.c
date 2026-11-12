#include <stdlib.h>
#include "rope.h"
#include "items.h"
#include "collide.h"
#include "lara.h"
#include "lara_states.h"
#include "control.h"
#include "draw.h"
#include "specific/3dmath.h"
#include "specific/specificfx.h"
#include "specific/input.h"

void vMul(PHD_VECTOR *v, sint32 scale, PHD_VECTOR *d)
{
    d->x = scale * v->x >> W2V_SHIFT;
    d->y = scale * v->y >> W2V_SHIFT;
    d->z = scale * v->z >> W2V_SHIFT;
}

sint32 mDotProduct(PHD_VECTOR *a, PHD_VECTOR *b)
{
    return (a->x * b->x + a->y * b->y + a->z * b->z) >> W2V_SHIFT;
}

void GetRopePos(ROPE_STRUCT *rope, sint32 pos, sint32 *x, sint32 *y, sint32 *z)
{
    sint32 segment;

    segment = pos >> 7;
    pos &= 0x7F;
    *x = (rope->NormalisedSegment[segment].x * pos >> (W2V_SHIFT + 2)) + (rope->MeshSegment[segment].x >> (W2V_SHIFT + 2)) + rope->Position.x;
    *y = (rope->NormalisedSegment[segment].y * pos >> (W2V_SHIFT + 2)) + (rope->MeshSegment[segment].y >> (W2V_SHIFT + 2)) + rope->Position.y;
    *z = (rope->NormalisedSegment[segment].z * pos >> (W2V_SHIFT + 2)) + (rope->MeshSegment[segment].z >> (W2V_SHIFT + 2)) + rope->Position.z;
}
