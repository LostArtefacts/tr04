#ifndef SPECIFIC_3DMATH_H
#define SPECIFIC_3DMATH_H

#include "../3dglodef.h"

#define W2V_SHIFT 14

enum matrix_indices {
    M00, M01, M02, M03,
    M10, M11, M12, M13,
    M20, M21, M22, M23,
    indices_count
};

extern sint16 rcossin_tbl[8192];

__inline sint32 phd_sin(sint32 angle)
{
    angle >>= 3;
    return 4 * rcossin_tbl[angle & 0x1FFE];
}

__inline sint32 phd_cos(sint32 angle)
{
    angle >>= 3;
    return 4 * rcossin_tbl[(angle & 0x1FFE) + 1];
}

void AlterFOV(sint16 fov);
void SetupZRange(sint32 znear, sint32 zfar);
void ScaleCurrentMatrix(PHD_VECTOR *vec);

extern sint32 phdtan2[8];
extern sint16 phdtantab[2050];

extern float one;
extern float mone;
extern float FogStart;
extern float FogEnd;

extern sint32 *phd_mxptr;

extern float f_centerx;
extern float f_centery;
extern float f_top;
extern float f_left;
extern float f_bottom;
extern float f_right;
extern float f_znear;
extern float f_zfar;
extern float f_mznear;
extern float f_mzfar;
extern float f_persp;
extern float f_mpersp;
extern float f_oneopersp;
extern float f_moneopersp;
extern float f_perspoznear;
extern float f_mperspoznear;
extern float f_moneoznear;
extern float f_a;
extern float f_b;
extern float f_boo;

extern sint32 phd_winheight;
extern sint32 phd_winwidth;
extern sint32 phd_centerx;
extern sint32 phd_centery;
extern sint32 phd_top;
extern sint32 phd_left;
extern sint32 phd_bottom;
extern sint32 phd_right;
extern sint32 phd_znear;
extern sint32 phd_zfar;
extern sint32 phd_persp;
extern sint16 phd_winxmax;
extern sint16 phd_winxmin;
extern sint16 phd_winymax;
extern sint16 phd_winymin;

extern float LfAspectCorrection;

#endif
