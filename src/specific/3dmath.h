#ifndef SPECIFIC_3DMATH_H
#define SPECIFIC_3DMATH_H

#include "../3dglodef.h"

#define W2V_SHIFT 14

#define TRIGMULT2(a, b) (((a) * (b)) >> W2V_SHIFT)
#define TRIGMULT3(a, b, c) (TRIGMULT2((TRIGMULT2(a, b)), c))

enum matrix_indices {
    M00, M01, M02, M03,
    M10, M11, M12, M13,
    M20, M21, M22, M23,
    indices_count
};

extern sint16 rcossin_tbl[8192];
extern sint16 CurrentFov;

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
void InitWindow(sint32 x, sint32 y, sint32 w, sint32 h, sint32 znear, sint32 zfar, sint32 fov, sint32 a, sint32 b);
uint32 mGetAngle(sint32 x, sint32 z, sint32 x1, sint32 z1);
void phd_GenerateW2V(PHD_3DPOS *viewPos);
void phd_LookAt(sint32 sx, sint32 sy, sint32 sz, sint32 tx, sint32 ty, sint32 tz, sint16 roll);
sint32 phd_atan(sint32 x, sint32 y);
sint32 phd_sqrt(sint32 num);
#define phd_PopMatrix() phd_mxptr -= 12

void phd_PushMatrix(void);
void phd_PushUnitMatrix(void);
void Func490320(void);
sint32 phd_TranslateRel(sint32 x, sint32 y, sint32 z);
void phd_RotX(PHD_ANGLE rx);
void phd_RotY(PHD_ANGLE ry);
void phd_RotZ(PHD_ANGLE rz);
void phd_RotYXZpack(sint32 rots);
void phd_RotYXZ(PHD_ANGLE ry, PHD_ANGLE rx, PHD_ANGLE rz);
void phd_TranslateAbs(sint32 x, sint32 y, sint32 z);
void phd_GetVectorAngles(sint32 x, sint32 y, sint32 z, PHD_ANGLE *dest);
void nullsub_14(void);

extern sint32 phdtan2[8];
extern sint16 phdtantab[2050];

extern float one;
extern float mone;
extern float FogStart;
extern float FogEnd;

extern sint32 *phd_mxptr;
extern sint32 w2v_matrix[indices_count];
extern sint32 matrix_stack[20 * indices_count];

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

extern sint32 XSoff1;
extern sint32 XSoff2;
extern sint32 YSoff1;
extern sint32 YSoff2;
extern sint32 ZSoff1;
extern sint32 ZSoff2;

#endif
