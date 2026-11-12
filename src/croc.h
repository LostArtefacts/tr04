#ifndef CROC_H
#define CROC_H

#include "items.h"





void InitialiseCroc(sint16 item_number);
void InitialiseLocustEmitter(sint16 item_number);

void TriggerLocust(ITEM_INFO *item);
void ControlLocustEmitter(sint16 item_number);

#endif
