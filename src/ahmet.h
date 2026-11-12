#ifndef AHMET_H
#define AHMET_H

#include "items.h"

void InitialiseAhmet(sint16 item_number);
void ExplodeAhmet(ITEM_INFO *item);
sint32 ReTriggerAhmet(sint16 item_number);
void ScalesControl(sint16 item_number);

#endif
