#ifndef SENET_H
#define SENET_H

#include "items.h"

extern sint32 SenetTargetX;
extern sint32 SenetTargetZ;
extern sint16 senet_item[6];
extern sint8 piece_moving;
extern sint8 last_throw;
extern sint8 senet_board[17];
extern sint8 senet_piece[6];



void SenetControl(sint16 item_number);
void InitialiseGameStix(sint16 item_number);
void InitialiseSenet(sint16 item_number);
void ThrowSticks(ITEM_INFO *item);
void ShockwaveExplosion(ITEM_INFO *item, uint32 col, sint32 speed);
void ControlGodHead(sint16 item_number);
void DrawGodHead(ITEM_INFO *item);

#endif
