#ifndef GAMEFLOW_H
#define GAMEFLOW_H

#include "types.h"

enum languages {
    ENGLISH,
    FRENCH,
    GERMAN,
    ITALIAN,
    SPANISH,
    US,
    JAPAN,
    DUTCH
};

typedef struct gameflow {
    uint32 CheatEnabled:1;
    uint32 LoadSaveEnabled:1;
    uint32 TitleEnabled:1;
    uint32 PlayAnyLevel:1;
    uint32 Language:3;
    uint32 DemoDisc:1;
    uint32 Unused:24;
    uint32 InputTimeout;
    uint8 SecurityTag;
    uint8 nLevels;
    uint8 nFileNames;
    uint8 Pad;
    uint16 FileNameLen;
    uint16 ScriptLen;
} GAMEFLOW;

enum gf_level_options {
    GF_TIMER = 0x1000
};

extern uint16 gfLevelFlags;
extern char gfGameMode;
extern GAMEFLOW *Gameflow;

#endif
