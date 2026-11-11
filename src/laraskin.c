#include "laraskin.h"

extern PHD_VECTOR *SkinXYZPtr;

void PushXYZ(void)
{
    PHD_VECTOR *ptr;

    ptr = SkinXYZPtr++;
    SkinXYZPtr->x = ptr->x;
    SkinXYZPtr->y = ptr->y;
    SkinXYZPtr->z = ptr->z;
}
