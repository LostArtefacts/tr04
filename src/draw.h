#ifndef DRAW_H
#define DRAW_H

#include "items.h"

void phd_PopMatrix_I(void);
void phd_PushMatrix_I(void);
void phd_RotY_I(sint16 ang);
void phd_RotX_I(sint16 ang);
void phd_RotZ_I(sint16 ang);
void phd_TranslateRel_I(sint32 x, sint32 y, sint32 z);
void phd_TranslateRel_ID(sint32 x, sint32 y, sint32 z, sint32 x2, sint32 y2, sint32 z2);
void gar_RotYXZsuperpack_I(sint16 **pprot1, sint16 **pprot2, sint32 skip);
void gar_RotYXZsuperpack(sint16 **pprot, sint32 skip);
void InterpolateMatrix(void);
sint32 GetFrames(ITEM_INFO *item, sint16 *frm[], sint32 *rate);
sint16 *GetBestFrame(ITEM_INFO *item);
void phd_PutPolygons(sint16 *objptr, sint32 clip);
void InitInterpolate(sint32 frac, sint32 rate);
void phd_PutPolygons_I(sint16 *ptr, sint32 clip);
void phd_RotYXZ_I(sint16 y, sint16 x, sint16 z);

extern sint32 *IMptr;
extern sint32 IM_rate;
extern sint32 IM_frac;
extern sint32 IMstack[768];
extern sint16 no_rotation[12];

#endif
