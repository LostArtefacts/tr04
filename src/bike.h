#ifndef BIKE_H
#define BIKE_H

#include "items.h"
#include "collide.h"

typedef struct bikeinfo {
    sint16 right_front_wheelrot;
    sint16 right_back_wheelrot;
    sint32 left_wheelrot;
    sint32 velocity;
    sint32 unused1;
    sint32 pitch1;
    sint16 move_angle;
    sint16 extra_rotation;
    sint16 rot_thing;
    sint32 bike_turn;
    sint32 pitch2;
    uint8 flags;
    uint8 flags2;
    sint16 light_intensity;
} BIKEINFO;

sint32 GetOnBike(sint16 item_number, COLL_INFO *coll);
void DrawBikeBeam(ITEM_INFO *item);
void BikeStart(ITEM_INFO *item, ITEM_INFO *l);
void DrawBikeExtras(ITEM_INFO *item);
void BikeExplode(ITEM_INFO *item);
void TriggerBikeBeam(ITEM_INFO *item);
sint32 CanGetOff_bike(sint16 num);
void InitialiseBike(sint16 item_number);
sint32 BikeCheckGetOff(void);
sint32 TestHeight(ITEM_INFO *item, sint32 z, sint32 x, PHD_VECTOR *pos);
sint32 DoDynamics_bike(sint32 height, sint32 fallspeed, sint32 *y, sint32 zero);
void TriggerExhaustSmoke_bike(sint32 x, sint32 y, sint32 z, sint32 angle, sint32 velocity, sint32 thing);
void BikeCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll);
sint32 BikeBaddieCollision(ITEM_INFO *bike);
void BikeCollideStaticObjects(sint32 x, sint32 y, sint32 z, sint16 room_number, sint32 height);

extern ITEM_INFO *GlobalBikeItem;
extern sint16 broomies[22];
extern sint32 BikeBounds[6];
extern sint32 CollidedStaticBounds[6];

#endif
