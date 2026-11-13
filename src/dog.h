#ifndef DOG_H
#define DOG_H

#include "box.h"

void InitialiseDog(sint16 item_number);
void DogControl(sint16 item_number);

extern BITE_INFO dog_bite;
extern uint8 DeathAnims[4];

#endif
