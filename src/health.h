#ifndef HEALTH_H
#define HEALTH_H

#include "types.h"

typedef struct displaypu {
    sint16 life;
    sint16 object_number;
} DISPLAYPU;

sint32 FlashIt(void);
void DrawGameInfo(sint32 timed);
void DrawHealthBar(sint32 flash_state);
void DrawAirBar(sint32 flash_state);
void InitialisePickUpDisplay(void);
void DrawPickups(void);
void AddDisplayPickup(sint16 object_number);

extern DISPLAYPU pickups[8];
extern sint16 PickupX;
extern sint16 PickupY;
extern sint16 PickupVel;
extern sint16 CurrentPickup;
extern sint32 health_bar_timer;
extern sint32 flash_state;
extern sint32 flash_count;
extern sint32 old_hitpoints;

#endif
