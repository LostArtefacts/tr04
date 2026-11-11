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

void phd_TranslateRel_I(sint32 x, sint32 y, sint32 z)
{
    sint32 *oldm;

    phd_TranslateRel(x, y, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_TranslateRel(x, y, z);
    phd_mxptr = oldm;
}

void phd_TranslateRel_ID(sint32 x, sint32 y, sint32 z, sint32 x2, sint32 y2, sint32 z2)
{
    sint32 *oldm;

    phd_TranslateRel(x, y, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_TranslateRel(x2, y2, z2);
    phd_mxptr = oldm;
}

void phd_RotYXZ_I(sint16 y, sint16 x, sint16 z)
{
    sint32 *oldm;

    phd_RotYXZ(y, x, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotYXZ(y, x, z);
    phd_mxptr = oldm;
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
