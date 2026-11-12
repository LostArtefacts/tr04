#ifndef SENET_H
#define SENET_H

#include "items.h"
#include "collide.h"

extern sint32 SenetTargetX;
extern sint32 SenetTargetZ;
extern sint16 senet_item[6];
extern sint8 piece_moving;
extern sint8 last_throw;
extern sint8 senet_board[17];
extern sint8 senet_piece[6];
extern sint16 GameStixBounds[12];
extern PHD_VECTOR GameStixPos;



void SenetControl(sint16 item_number);
void InitialiseGameStix(sint16 item_number);
void InitialiseSenet(sint16 item_number);
void ThrowSticks(ITEM_INFO *item);
void ShockwaveExplosion(ITEM_INFO *item, uint32 col, sint32 speed);
void ControlGodHead(sint16 item_number);
void DrawGodHead(ITEM_INFO *item);
void GameStixCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void MakeMove(sint32 piece, sint32 displacement);

#endif
