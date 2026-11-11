#ifndef SPHERE_H
#define SPHERE_H

#include "items.h"

typedef struct sphere {
    sint32 x;
    sint32 y;
    sint32 z;
    sint32 r;
} SPHERE;

sint32 TestCollision(ITEM_INFO *item, ITEM_INFO *l);
sint32 GetSpheres(ITEM_INFO *item, SPHERE *ptr, sint32 WorldSpace);
void InitInterpolate2(sint32 frac, sint32 rate);
void GetJointAbsPosition(ITEM_INFO *item, PHD_VECTOR *pos, sint32 joint);

extern SPHERE Slist[34];
extern char GotLaraSpheres;
extern SPHERE LaraSpheres[15];
extern sint32 NumLaraSpheres;

#endif
