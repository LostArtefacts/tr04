#ifndef SPECIFIC_LIGHTING_H
#define SPECIFIC_LIGHTING_H

#include "../items.h"

extern ITEM_INFO *current_item;

void FreeD3DLights(void);
void CalcAmbientLight(ITEM_INFO *item);
void CreateLightList(ITEM_INFO *item);

#endif
