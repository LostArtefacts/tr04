#ifndef HORSE_H
#define HORSE_H

#include "items.h"

void InitialiseHorseman(sint16 item_number);
void InitialiseHorse(sint16 item_number);
void TriggerHorsemanRicochets(PHD_VECTOR *pos, sint32 yrot, sint32 num);
void HorsemanControl(sint16 item_number);

#endif
