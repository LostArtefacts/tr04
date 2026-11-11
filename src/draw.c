#include <string.h>

#include "draw.h"
#include "specific/3dmath.h"

#define S_LongMemCpy(d, s, n) memcpy(d, s, (n) * sizeof(sint32))

void InitInterpolate(sint32 frac, sint32 rate)
{
    IM_frac = frac;
    IM_rate = rate;
    IMptr = IMstack;
    S_LongMemCpy(IMstack, phd_mxptr, 12);
}

void phd_PopMatrix_I(void)
{
    phd_PopMatrix();
    IMptr -= 12;
}
