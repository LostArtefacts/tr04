#ifndef LARAMISC_H
#define LARAMISC_H

#include "lara.h"
#include "collide.h"

void LaraInitialiseMeshes(void);
void InitialiseLaraAnims(ITEM_INFO *item);
void LaraCheat(ITEM_INFO *item, COLL_INFO *coll);
void LaraCheatGetStuff(void);
void LaraCheatGetStuff(void);
void LaraCheatyBits(void);
void InitialiseLaraLoad(sint16 item_number);

extern sint16 cheat_hit_points;
extern COLL_INFO mycoll;

#endif
