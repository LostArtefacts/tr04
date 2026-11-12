#ifndef LARAFLAR_H
#define LARAFLAR_H

#include "items.h"

void ready_flare(void);
void set_flare_arm(sint32 frame);
void draw_flare_meshes(void);
void undraw_flare_meshes(void);
void DoFlareInHand(sint32 flare_age);
void CreateFlare(sint16 object, sint32 thrown);
void undraw_flare(void);
void DrawFlareInAir(ITEM_INFO *item);
void draw_flare(void);

#endif
