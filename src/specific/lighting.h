#ifndef SPECIFIC_LIGHTING_H
#define SPECIFIC_LIGHTING_H

#include "../items.h"
#include "../3dglodef.h"

extern ITEM_INFO *current_item;

void FreeD3DLights(void);
void CalcAmbientLight(ITEM_INFO *item);
void CreateLightList(ITEM_INFO *item);
void ApplyMatrix(sint32 *matrix, PHD_VECTOR *start, PHD_VECTOR *dest);
void ApplyTransposeMatrix(sint32 *matrix, PHD_VECTOR *start, PHD_VECTOR *dest);

#endif
