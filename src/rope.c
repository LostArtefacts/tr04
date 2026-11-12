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

sint32 RopeNodeCollision(ROPE_STRUCT *rope, sint32 x, sint32 y, sint32 z, sint32 rad)
{
    sint32 rx, ry, rz, i;

    for (i = 0; i < 22; ++i) {
        if (y > rope->Position.y + (rope->MeshSegment[i].y >> (W2V_SHIFT + 2)) && y < rope->Position.y + (rope->MeshSegment[i + 1].y >> (W2V_SHIFT + 2))) {
            rx = x - ((rope->MeshSegment[i + 1].x + rope->MeshSegment[i].x) >> 17) - rope->Position.x;
            ry = y - ((rope->MeshSegment[i + 1].y + rope->MeshSegment[i].y) >> 17) - rope->Position.y;
            rz = z - ((rope->MeshSegment[i + 1].z + rope->MeshSegment[i].z) >> 17) - rope->Position.z;
            if (SQUARE(rx) + SQUARE(ry) + SQUARE(rz) < SQUARE(rad + 64))
                return i;
        }
    }
    return -1;
}

void DrawRopeList(void)
{
    sint32 i;

    for (i = 0; i < nRope; i++) {
        if (RopeList[i].Active) {
            DrawRope(&RopeList[i]);
        }
    }
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

void ModelRigid(PHD_VECTOR *pa, PHD_VECTOR *pb, PHD_VECTOR *va, PHD_VECTOR *vb, sint32 rlength)
{
    PHD_VECTOR d, a, b;
    sint32 length, scale;

    a.x = pa->x + va->x;
    a.y = pa->y + va->y;
    a.z = pa->z + va->z;
    b.x = pb->x + vb->x;
    b.y = pb->y + vb->y;
    b.z = pb->z + vb->z;
    d.x = b.x - a.x;
    d.y = b.y - a.y;
    d.z = b.z - a.z;
    a.x = d.x >> 16;
    a.y = d.y >> 16;
    a.z = d.z >> 16;
    length = SQUARE(a.z) + SQUARE(a.y) + SQUARE(a.x);
    if (length < 0) {
        length = -length;
    }
    length = phd_sqrt(length);
    scale = ((length << 16) - rlength) >> 1;
    Normalise(&d);
    d.x = (__int64)scale * d.x >> 16;
    d.y = (__int64)scale * d.y >> 16;
    d.z = (__int64)scale * d.z >> 16;
    va->x += d.x;
    va->y += d.y;
    va->z += d.z;
    vb->x -= d.x;
    vb->y -= d.y;
    vb->z -= d.z;
}

void SetPendulumPoint(ROPE_STRUCT *Rope, sint32 node)
{
    CurrentPendulum.Position.x = Rope->Segment[node].x;
    CurrentPendulum.Position.y = Rope->Segment[node].y;
    CurrentPendulum.Position.z = Rope->Segment[node].z;
    if (CurrentPendulum.node == -1) {
        CurrentPendulum.Velocity.x += Rope->Velocity[node].x;
        CurrentPendulum.Velocity.y += Rope->Velocity[node].y;
        CurrentPendulum.Velocity.z += Rope->Velocity[node].z;
    }
    CurrentPendulum.Rope = Rope;
    CurrentPendulum.node = node;
}
