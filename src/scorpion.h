#ifndef SCORPION_H
#define SCORPION_H

#include "box.h"
#include "items.h"

void InitialiseScorpion(sint16 item_number);
void InitialiseSmlscorp(sint16 item_number);
void SmlscorpControl(sint16 item_number);
void ScorpionControl(sint16 item_number);

extern BITE_INFO stinger;
extern BITE_INFO pincer;
extern BITE_INFO s_stinger;
extern BITE_INFO s_pincer;

#endif
