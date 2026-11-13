#include <stdlib.h>
#include "rope.h"
#include "items.h"
#include "collide.h"
#include "lara.h"
#include "lara_states.h"
#include "control.h"
#include "draw.h"
#include "anim.h"
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

void mCrossProduct(PHD_VECTOR *a, PHD_VECTOR *b, PHD_VECTOR *n)
{
    PHD_VECTOR t;

    t.x = a->y * b->z - a->z * b->y;
    t.y = a->z * b->x - a->x * b->z;
    t.z = a->x * b->y - a->y * b->x;
    n->x = t.x >> W2V_SHIFT;
    n->y = t.y >> W2V_SHIFT;
    n->z = t.z >> W2V_SHIFT;
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

void AlignLaraToRope(ITEM_INFO *l)
{
    ROPE_STRUCT *rope;
    PHD_VECTOR n, v, u, n2, up;
    PHD_VECTOR v1, v2;
    sint32 *mptr;
    sint16 *frame;
    sint32 temp[indices_count];
    static sint32 ropematrix[indices_count];
    sint32 i, x, y, z, x1, y1, z1;
    sint16 xyz[3];
    static sint16 ropeangle;

    up.x = 4096;
    up.y = 0;
    up.z = 0;
    frame = GetBestFrame(l);
    ropeangle = lara.RopeY - 16380;
    rope = &RopeList[lara.RopePtr];
    i = lara.RopeSegment - 1;
    GetRopePos(rope, (i << 7) + frame[7], &x, &y, &z);
    GetRopePos(rope, (i << 7) + frame[7] - 192, &x1, &y1, &z1);
    u.x = (x - x1) << (W2V_SHIFT + 2);
    u.y = (y - y1) << (W2V_SHIFT + 2);
    u.z = (z - z1) << (W2V_SHIFT + 2);
    Normalise(&u);
    u.x >>= 2;
    u.y >>= 2;
    u.z >>= 2;
    vMul(&u, mDotProduct(&up, &u), &v);
    v.x = up.x - v.x;
    v.y = up.y - v.y;
    v.z = up.z - v.z;
    v1 = v;
    v2 = v;
    n2 = u;
    vMul(&v1, phd_cos(ropeangle), &v1);
    vMul(&n2, mDotProduct(&n2, &v), &n2);
    vMul(&n2, 4096 - (rcossin_tbl[((ropeangle >> 3) & 0x1FFE) + 1] << 2), &n2);
    mCrossProduct(&u, &v, &v2);
    vMul(&v2, phd_sin(ropeangle), &v2);
    n2.x += v1.x;
    n2.y += v1.y;
    n2.z += v1.z;
    v.x = (n2.x + v2.x) << (W2V_SHIFT + 2);
    v.y = (n2.y + v2.y) << (W2V_SHIFT + 2);
    v.z = (n2.z + v2.z) << (W2V_SHIFT + 2);
    Normalise(&v);
    v.x >>= 2;
    v.y >>= 2;
    v.z >>= 2;
    mCrossProduct(&u, &v, &n);
    n.x <<= (W2V_SHIFT + 2);
    n.y <<= (W2V_SHIFT + 2);
    n.z <<= (W2V_SHIFT + 2);
    Normalise(&n);
    n.x >>= 2;
    n.y >>= 2;
    n.z >>= 2;
    temp[M00] = n.x;
    temp[M01] = u.x;
    temp[M02] = v.x;
    temp[M10] = n.y;
    temp[M11] = u.y;
    temp[M12] = v.y;
    temp[M20] = n.z;
    temp[M21] = u.z;
    temp[M22] = v.z;
    phd_GetMatrixAngles(temp, xyz);
    memcpy(ropematrix, temp, sizeof(ropematrix));
    l->pos.x_pos = rope->Position.x + (rope->MeshSegment[i + 1].x >> (W2V_SHIFT + 2));
    l->pos.y_pos = rope->Position.y + (rope->MeshSegment[i + 1].y >> (W2V_SHIFT + 2)) + lara.RopeOffset;
    l->pos.z_pos = rope->Position.z + (rope->MeshSegment[i + 1].z >> (W2V_SHIFT + 2));
    phd_PushUnitMatrix();
    phd_RotYXZ(xyz[1], xyz[0], xyz[2]);
    mptr = phd_mxptr;
    l->pos.x_pos += -112 * mptr[2] >> W2V_SHIFT;
    l->pos.y_pos += -112 * mptr[6] >> W2V_SHIFT;
    l->pos.z_pos += -112 * mptr[10] >> W2V_SHIFT;
    phd_PopMatrix();
    l->pos.x_rot = xyz[0];
    l->pos.y_rot = xyz[1];
    l->pos.z_rot = xyz[2];
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

void RopeCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ROPE_STRUCT *rope;
    sint16 *bounds;
    sint32 x, y, z, rad, i;

    rope = &RopeList[items[item_number].trigger_flags];
    if ((input & IN_ACTION) && lara.gun_status == LG_NO_ARMS && (l->current_anim_state == AS_REACH || l->current_anim_state == AS_UPJUMP) && l->gravity_status && l->fallspeed > 0 && rope->Active) {
        rad = l->current_anim_state == AS_REACH ? 128 : 64;
        bounds = GetBoundsAccurate(l);
        x = l->pos.x_pos;
        y = l->pos.y_pos + bounds[2] + 512;
        z = l->pos.z_pos + (bounds[5] * phd_cos(l->pos.y_rot) >> 14);
        i = RopeNodeCollision(rope, x, y, z, rad);
        if (i >= 0) {
            if (l->current_anim_state == AS_REACH) {
                l->anim_number = 379;
                l->current_anim_state = AS_ROPEFWD;
                lara.RopeFrame = (anims[ANIM_SWINGFWD].frame_base + 32) << 8;
                lara.RopeDFrame = (anims[ANIM_SWINGFWD].frame_base + 60) << 8;
            } else {
                l->anim_number = ANIM_UPJUMP2ROPE;
                l->current_anim_state = AS_ROPE;
            }
            l->frame_number = anims[l->anim_number].frame_base;
            l->gravity_status = 0;
            l->fallspeed = 0;
            lara.gun_status = LG_HANDS_BUSY;
            lara.RopePtr = items[item_number].trigger_flags;
            lara.RopeSegment = i;
            lara.RopeY = l->pos.y_rot;
            AlignLaraToRope(l);
            CurrentPendulum.Velocity.x = 0;
            CurrentPendulum.Velocity.y = 0;
            CurrentPendulum.Velocity.z = 0;
            ApplyVelocityToRope(i, l->pos.y_rot, lara_item->speed << 4);
        }
    }
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

void RopeControl(sint16 item_num)
{
    ROPE_STRUCT *currope;

    currope = &RopeList[items[item_num].trigger_flags];
    if (!TriggerActive(&items[item_num])) {
        currope->Active = 0;
    } else {
        currope->Active = 1;
        CalculateRope(currope);
    }
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

void ModelRigidRope(PHD_VECTOR *pa, PHD_VECTOR *pb, PHD_VECTOR *va, PHD_VECTOR *vb, sint32 rlength)
{
    PHD_VECTOR d, a, b;
    sint32 length, scale;

    a.x = pa->x;
    a.y = pa->y;
    a.z = pa->z;
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
    scale = (length << 16) - rlength;
    Normalise(&d);
    d.x = (__int64)scale * d.x >> 16;
    d.y = (__int64)scale * d.y >> 16;
    d.z = (__int64)scale * d.z >> 16;
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

void SetPendulumVelocity(sint32 x, sint32 y, sint32 z)
{
    sint32 scale;

    if ((CurrentPendulum.node & ~1) < 24) {
        scale = (4096 / (((12 - (CurrentPendulum.node >> 1)) << 9) >> 8)) << 8;
        x = (__int64)scale * x >> (W2V_SHIFT + 2);
        y = (__int64)scale * y >> (W2V_SHIFT + 2);
        z = (__int64)scale * z >> (W2V_SHIFT + 2);
    }
    CurrentPendulum.Velocity.x += x;
    CurrentPendulum.Velocity.y += y;
    CurrentPendulum.Velocity.z += z;
}

void CalculateRope(ROPE_STRUCT *Rope)
{
    PENDULUM *Pendulum;
    PHD_VECTOR dir;
    sint32 n, bSetFlag;

    bSetFlag = 0;
    if (Rope == &RopeList[lara.RopePtr]) {
        Pendulum = &CurrentPendulum;
        if (CurrentPendulum.node != lara.RopeSegment + 1) {
            SetPendulumPoint(Rope, lara.RopeSegment + 1);
            bSetFlag = 1;
        }
    } else {
        Pendulum = &NullPendulum;
        if (lara.RopePtr == -1 && CurrentPendulum.Rope) {
            for (n = 0; n < CurrentPendulum.node; n++) {
                CurrentPendulum.Rope->Velocity[n].x = CurrentPendulum.Rope->Velocity[CurrentPendulum.node].x;
                CurrentPendulum.Rope->Velocity[n].y = CurrentPendulum.Rope->Velocity[CurrentPendulum.node].y;
                CurrentPendulum.Rope->Velocity[n].z = CurrentPendulum.Rope->Velocity[CurrentPendulum.node].z;
            }
            CurrentPendulum.Rope = 0;
            CurrentPendulum.node = -1;
            CurrentPendulum.Position.x = 0;
            CurrentPendulum.Position.y = 0;
            CurrentPendulum.Position.z = 0;
            CurrentPendulum.Velocity.x = 0;
            CurrentPendulum.Velocity.y = 0;
            CurrentPendulum.Velocity.z = 0;
        }
    }
    if (lara.RopePtr != -1) {
        dir.x = Pendulum->Position.x - Rope->Segment[0].x;
        dir.y = Pendulum->Position.y - Rope->Segment[0].y;
        dir.z = Pendulum->Position.z - Rope->Segment[0].z;
        Normalise(&dir);
        for (n = Pendulum->node; n >= 0; n--) {
            Rope->Segment[n].x = Rope->MeshSegment[n - 1].x + ((__int64)dir.x * Rope->SegmentLength >> (W2V_SHIFT + 2));
            Rope->Segment[n].y = Rope->MeshSegment[n - 1].y + ((__int64)dir.y * Rope->SegmentLength >> (W2V_SHIFT + 2));
            Rope->Segment[n].z = Rope->MeshSegment[n - 1].z + ((__int64)dir.z * Rope->SegmentLength >> (W2V_SHIFT + 2));
            Rope->Velocity[n].x = 0;
            Rope->Velocity[n].y = 0;
            Rope->Velocity[n].z = 0;
        }
        if (bSetFlag) {
            sint32 x, y, z;

            x = Pendulum->Position.x - Rope->Segment[Pendulum->node].x;
            y = Pendulum->Position.y - Rope->Segment[Pendulum->node].y;
            z = Pendulum->Position.z - Rope->Segment[Pendulum->node].z;
            Rope->Segment[Pendulum->node].x = Pendulum->Position.x;
            Rope->Segment[Pendulum->node].y = Pendulum->Position.y;
            Rope->Segment[Pendulum->node].z = Pendulum->Position.z;
            for (n = Pendulum->node; n < 24; n++) {
                Rope->Segment[n].x -= x;
                Rope->Segment[n].y -= y;
                Rope->Segment[n].z -= z;
                Rope->Velocity[n].x = 0;
                Rope->Velocity[n].y = 0;
                Rope->Velocity[n].z = 0;
            }
        }
        ModelRigidRope(&Rope->Segment[0], &Pendulum->Position, &Rope->Velocity[0], &Pendulum->Velocity, Rope->SegmentLength * Pendulum->node);
        Pendulum->Velocity.y += 0x60000;
        Pendulum->Position.x += Pendulum->Velocity.x;
        Pendulum->Position.y += Pendulum->Velocity.y;
        Pendulum->Position.z += Pendulum->Velocity.z;
        Pendulum->Velocity.x -= Pendulum->Velocity.x >> 8;
        Pendulum->Velocity.z -= Pendulum->Velocity.z >> 8;
    }
    for (n = Pendulum->node; n < 23; n++)
        ModelRigid(&Rope->Segment[n], &Rope->Segment[n + 1], &Rope->Velocity[n], &Rope->Velocity[n + 1], Rope->SegmentLength);
    for (n = 0; n < 24; n++) {
        Rope->Segment[n].x += Rope->Velocity[n].x;
        Rope->Segment[n].y += Rope->Velocity[n].y;
        Rope->Segment[n].z += Rope->Velocity[n].z;
    }
    for (n = Pendulum->node; n < 24; n++) {
        Rope->Velocity[n].y += 0x30000;
        if (Pendulum->Rope) {
            Rope->Velocity[n].x -= Rope->Velocity[n].x >> 4;
            Rope->Velocity[n].z -= Rope->Velocity[n].z >> 4;
        } else {
            Rope->Velocity[n].x -= Rope->Velocity[n].x >> 7;
            Rope->Velocity[n].z -= Rope->Velocity[n].z >> 7;
        }
    }
    Rope->Segment[0].x = 0;
    Rope->Segment[0].y = 0;
    Rope->Segment[0].z = 0;
    Rope->Velocity[0].x = 0;
    Rope->Velocity[0].y = 0;
    Rope->Velocity[0].z = 0;
    for (n = 0; n < 23; n++) {
        Rope->NormalisedSegment[n].x = Rope->Segment[n + 1].x - Rope->Segment[n].x;
        Rope->NormalisedSegment[n].y = Rope->Segment[n + 1].y - Rope->Segment[n].y;
        Rope->NormalisedSegment[n].z = Rope->Segment[n + 1].z - Rope->Segment[n].z;
        Normalise(&Rope->NormalisedSegment[n]);
    }
    if (Rope != &RopeList[lara.RopePtr]) {
        dir = Rope->NormalisedSegment[0];
        Rope->MeshSegment[0].x = Rope->Segment[0].x;
        Rope->MeshSegment[0].y = Rope->Segment[0].y;
        Rope->MeshSegment[0].z = Rope->Segment[0].z;
        Rope->MeshSegment[1].x = Rope->Segment[0].x + ((__int64)Rope->SegmentLength * dir.x >> 16);
        Rope->MeshSegment[1].y = Rope->Segment[0].y + ((__int64)Rope->SegmentLength * dir.y >> 16);
        Rope->MeshSegment[1].z = Rope->Segment[0].z + ((__int64)Rope->SegmentLength * dir.z >> 16);
        for (n = 2; n < 24; n++) {
            dir = Rope->NormalisedSegment[n - 1];
            Rope->MeshSegment[n].x = Rope->MeshSegment[n - 1].x + ((__int64)dir.x * Rope->SegmentLength >> (W2V_SHIFT + 2));
            Rope->MeshSegment[n].y = Rope->MeshSegment[n - 1].y + ((__int64)dir.y * Rope->SegmentLength >> (W2V_SHIFT + 2));
            Rope->MeshSegment[n].z = Rope->MeshSegment[n - 1].z + ((__int64)dir.z * Rope->SegmentLength >> (W2V_SHIFT + 2));
        }
    } else {
        dir = Rope->NormalisedSegment[Pendulum->node];
        Rope->MeshSegment[Pendulum->node].x = Rope->Segment[Pendulum->node].x;
        Rope->MeshSegment[Pendulum->node].y = Rope->Segment[Pendulum->node].y;
        Rope->MeshSegment[Pendulum->node].z = Rope->Segment[Pendulum->node].z;
        Rope->MeshSegment[Pendulum->node + 1].x = Rope->Segment[Pendulum->node].x + ((__int64)dir.x * Rope->SegmentLength >> (W2V_SHIFT + 2));
        Rope->MeshSegment[Pendulum->node + 1].y = Rope->Segment[Pendulum->node].y + ((__int64)dir.y * Rope->SegmentLength >> (W2V_SHIFT + 2));
        Rope->MeshSegment[Pendulum->node + 1].z = Rope->Segment[Pendulum->node].z + ((__int64)dir.z * Rope->SegmentLength >> (W2V_SHIFT + 2));
        for (n = Pendulum->node + 1; n < 23; n++) {
            dir = Rope->NormalisedSegment[n];
            Rope->MeshSegment[n + 1].x = Rope->MeshSegment[n].x + ((__int64)dir.x * Rope->SegmentLength >> (W2V_SHIFT + 2));
            Rope->MeshSegment[n + 1].y = Rope->MeshSegment[n].y + ((__int64)dir.y * Rope->SegmentLength >> (W2V_SHIFT + 2));
            Rope->MeshSegment[n + 1].z = Rope->MeshSegment[n].z + ((__int64)dir.z * Rope->SegmentLength >> (W2V_SHIFT + 2));
        }
        for (n = 0; n < Pendulum->node; n++) {
            Rope->MeshSegment[n].x = Rope->Segment[n].x;
            Rope->MeshSegment[n].y = Rope->Segment[n].y;
            Rope->MeshSegment[n].z = Rope->Segment[n].z;
        }
    }
}

void ProjectRopePoints(ROPE_STRUCT *Rope)
{
    struct {
        float x, y, z;
    } Output;
    PHD_VECTOR t;
    float zv;
    sint32 i, w, h;

    w = phd_winwidth >> 1;
    h = phd_winheight >> 1;
    phd_PushMatrix();
    phd_TranslateAbs(Rope->Position.x, Rope->Position.y, Rope->Position.z);
    for (i = 0; i < 24; i++) {
        t.x = Rope->MeshSegment[i].x >> (W2V_SHIFT + 2);
        t.y = Rope->MeshSegment[i].y >> (W2V_SHIFT + 2);
        t.z = Rope->MeshSegment[i].z >> (W2V_SHIFT + 2);
        Output.x = (float)(t.x * phd_mxptr[M00] + t.y * phd_mxptr[M01] + t.z * phd_mxptr[M02] + phd_mxptr[M03]);
        Output.y = (float)(t.x * phd_mxptr[M10] + t.y * phd_mxptr[M11] + t.z * phd_mxptr[M12] + phd_mxptr[M13]);
        Output.z = (float)(t.x * phd_mxptr[M20] + t.y * phd_mxptr[M21] + t.z * phd_mxptr[M22] + phd_mxptr[M23]);
        zv = phd_persp / Output.z;
        Rope->Coords[i][0] = (sint32)(Output.x * zv + w);
        Rope->Coords[i][1] = (sint32)(Output.y * zv + h);
        Rope->Coords[i][2] = (sint32)Output.z;
    }
    phd_PopMatrix();
}
