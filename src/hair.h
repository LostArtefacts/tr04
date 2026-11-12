#ifndef HAIR_H
#define HAIR_H

#include "3dglodef.h"

typedef struct hair_struct {
    PHD_3DPOS pos;
    PHD_VECTOR vel;
} HAIR_STRUCT;

void InitialiseHair(void);
void HairControl(sint32 in_cutscene, sint32 pigtail, sint16 *cutscenething);
void GetCorrectStashPoints(sint32 pigtail, sint32 hair_node, sint32 skin_node);
void DrawHair(void);

extern sint32 first_hair[2];
extern HAIR_STRUCT hairs[2][7];

#endif
