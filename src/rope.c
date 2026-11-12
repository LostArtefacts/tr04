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
