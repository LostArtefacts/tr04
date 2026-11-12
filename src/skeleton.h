#ifndef SKELETON_H
#define SKELETON_H

#include "items.h"
#include "box.h"

void InitialiseSkeleton(sint16 item_number);
void TriggerRiseEffect(ITEM_INFO *item);
void SkeletonControl(sint16 item_number);

extern BITE_INFO skelly_hit;

#endif
