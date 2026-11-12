#ifndef JEEP_H
#define JEEP_H

#include "items.h"
#include "collide.h"

typedef struct jeepinfo {
    sint16 right_front_wheelrot;
    sint16 left_front_wheelrot;
    sint16 left_back_wheelrot;
    sint16 right_back_wheelrot;
    sint32 velocity;
    sint32 unused1;
    sint32 pitch1;
    sint32 turn_rate;
    sint32 camera_angle;
    sint16 move_angle;
    sint16 extra_rotation;
    sint16 rot_thing;
    sint32 pitch2;
    sint16 flags;
    sint16 unused2;
    sint16 gear;
} JEEPINFO;

sint32 GetOnJeep(sint16 item_number, COLL_INFO *coll);
extern sint16 jroomies[22];
extern sint32 JeepBounds0;
extern sint32 JeepBounds1;
extern sint32 JeepBounds2;
extern sint32 JeepBounds3;
extern sint32 JeepBounds4;
extern sint32 JeepBounds5;
extern sint32 JeepStaticBounds0;
extern sint32 JeepStaticBounds1;
extern sint32 JeepStaticBounds2;
extern sint32 JeepStaticBounds3;
extern sint32 JeepStaticBounds4;
extern sint32 JeepStaticBounds5;

void JeepStart(ITEM_INFO *item, ITEM_INFO *l);
sint32 DoShift(ITEM_INFO *item, PHD_VECTOR *pos, PHD_VECTOR *old);
void DrawJeepExtras(ITEM_INFO *item);
sint32 JeepCheckGetOut(void);
void JeepExplode(ITEM_INFO *item);
sint32 DoDynamics_jeep(sint32 height, sint32 fallspeed, sint32 *y, sint32 zero);
sint32 GetCollisionAnim(ITEM_INFO *item, PHD_VECTOR *moved, JEEPINFO *jeep);
void InitialiseEnemyJeep(sint16 item_number);
sint32 CanGetOff_jeep(sint16 num);
void InitialiseJeep(sint16 item_number);
void TriggerExhaustSmoke_jeep(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 velocity, sint32 thing);
void JeepCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
void JeepBaddieCollision(ITEM_INFO *item);
void JeepCollideStaticObjects(sint32 x, sint32 y, sint32 z, sint16 room_number, sint32 height);
sint32 UserControl_jeep(ITEM_INFO *item, sint32 height, sint32 *pitch);

#endif
