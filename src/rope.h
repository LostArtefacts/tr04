#ifndef ROPE_H
#define ROPE_H

#include "3dglodef.h"
#include "items.h"
#include "collide.h"

typedef struct rope_struct {
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

PHD_VECTOR *Normalise(PHD_VECTOR *v);
void CalculateRope(ROPE_STRUCT *Rope);
void AlignLaraToRope(ITEM_INFO *l);
void RopeControl(sint16 item_num);
void vMul(PHD_VECTOR *v, sint32 scale, PHD_VECTOR *d);
sint32 mDotProduct(PHD_VECTOR *a, PHD_VECTOR *b);
void GetRopePos(ROPE_STRUCT *rope, sint32 pos, sint32 *x, sint32 *y, sint32 *z);
void phd_GetMatrixAngles(sint32 *m, sint16 *dest);
void DrawRopeList(void);
void ProjectRopePoints(ROPE_STRUCT *rope);
void SetPendulumPoint(ROPE_STRUCT *Rope, sint32 node);
sint32 RopeNodeCollision(ROPE_STRUCT *rope, sint32 x, sint32 y, sint32 z, sint32 rad);
void ModelRigid(PHD_VECTOR *pa, PHD_VECTOR *pb, PHD_VECTOR *va, PHD_VECTOR *vb, sint32 rlength);
void ModelRigidRope(PHD_VECTOR *pa, PHD_VECTOR *pb, PHD_VECTOR *va, PHD_VECTOR *vb, sint32 rlength);
void SetPendulumVelocity(sint32 x, sint32 y, sint32 z);
void RopeCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void init_all_ropes(void);
void mCrossProduct(PHD_VECTOR *a, PHD_VECTOR *b, PHD_VECTOR *n);

extern ROPE_STRUCT RopeList[5];
extern sint32 nRope;
extern PENDULUM CurrentPendulum;
extern PENDULUM NullPendulum;

#endif
