#ifndef HORSE_H
#define HORSE_H

#include "items.h"
#include "box.h"

void InitialiseHorseman(sint16 item_number);
void InitialiseHorse(sint16 item_number);
void TriggerHorsemanRicochets(PHD_VECTOR *pos, sint32 yrot, sint32 num);
void HorsemanControl(sint16 item_number);

extern BITE_INFO horseman_axe;
extern BITE_INFO horseman_lfoot;
extern BITE_INFO horseman_shield;
extern BITE_INFO horse_lfoot;
extern BITE_INFO horse_rfoot;
extern BITE_INFO horse_head;

#endif
