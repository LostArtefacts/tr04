#ifndef LARAMISC_H
#define LARAMISC_H

#include "lara.h"
#include "collide.h"

void LaraInitialiseMeshes(void);
void LaraCheat(ITEM_INFO *item, COLL_INFO *coll);
void LaraCheatGetStuff(void);
void LaraCheatyBits(void);

extern sint16 cheat_hit_points;

#endif
