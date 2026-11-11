#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "lara.h"

typedef struct stats {
    uint32 Timer;
    uint32 Distance;
    uint32 AmmoUsed;
    uint32 AmmoHits;
    uint16 Kills;
    uint8 Secrets;
    uint8 HealthUsed;
} STATS;

typedef struct savegame_info {
    LARA_INFO Lara;
    sint32 cutscene_triggered;
    uint8 HubLevels[10];
    uint16 HubOffsets[10];
    uint16 HubSizes[10];
    char CurrentLevel;
    char Checksum;
    STATS Game;
    STATS Level;
    sint16 WeaponObject;
    sint16 WeaponAnim;
    sint16 WeaponFrame;
    sint16 WeaponCurrent;
    sint16 WeaponGoal;
    CVECTOR fog_colour;
    uint8 HubSavedLara : 1;
    uint8 AutoTarget : 1;
    uint8 HaveBikeBooster : 1;
    char buffer[15410];
} SAVEGAME_INFO;

extern SAVEGAME_INFO savegame;
extern char *SGpoint;
extern sint32 SGcount;

void ReadSG(void *pointer, sint32 size);

#endif
