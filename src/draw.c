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

void phd_PushMatrix_I(void)
{
    phd_PushMatrix();
    S_LongMemCpy(IMptr + 12, IMptr, 12);
    IMptr += 12;
}

void phd_PutPolygons_I(sint16 *ptr, sint32 clip)
{
    phd_PushMatrix();
    InterpolateMatrix();
    phd_PutPolygons(ptr, clip);
    phd_PopMatrix();
}

sint16 *GetBestFrame(ITEM_INFO *item)
{
    sint32 frac;
    sint32 rate;
    sint16 *frmptr[2];

    frac = GetFrames(item, frmptr, &rate);
    if (frac <= (rate >> 1))
        return frmptr[0];
    else
        return frmptr[1];
}
