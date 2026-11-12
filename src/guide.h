#ifndef GUIDE_H
#define GUIDE_H

#include "box.h"
#include "items.h"

void InitialiseGuide(sint16 item_number);
void GuideControl(sint16 item_number);

extern BITE_INFO guide_hit;
extern BITE_INFO guide_lighter;

#endif
