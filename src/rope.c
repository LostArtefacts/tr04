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

void phd_GetMatrixAngles(sint32 *m, sint16 *dest)
{
    sint32 sy, cy, yaw;
    sint16 roll, pitch;

    yaw = phd_atan(m[M22], m[M02]);
    pitch = (sint16)phd_atan(phd_sqrt(SQUARE(m[M22]) + SQUARE(m[M02])), m[M12]);
    sy = phd_sin(yaw);
    cy = phd_cos(yaw);
    roll = (sint16)phd_atan(m[M00] * cy - m[M20] * sy, m[M21] * sy - m[M01] * cy);
    if ((m[M12] >= 0 && pitch > 0) || (m[M12] < 0 && pitch < 0)) {
        pitch = -pitch;
    }
    dest[0] = pitch;
    dest[1] = (sint16)yaw;
    dest[2] = roll;
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

PHD_VECTOR *Normalise(PHD_VECTOR *v)
{
    sint32 mod, a, b, c, d, e;

    a = v->x >> 16;
    b = v->y >> 16;
    c = v->z >> 16;
    if (!a && !b && !c) {
        return v;
    }
    d = SQUARE(c) + SQUARE(b) + SQUARE(a);
    if (d < 0) {
        d = -d;
    }
    e = phd_sqrt(d);
    mod = (0x1000000 / ((e << 16) >> 8)) << 8 >> 8;
    v->x = (__int64)mod * v->x >> 16;
    v->y = (__int64)mod * v->y >> 16;
    v->z = (__int64)mod * v->z >> 16;
    return v;
}
