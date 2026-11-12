#ifndef SOUND_H
#define SOUND_H

#include "3dglodef.h"

enum sfx_options {
    SFX_WATER = 1,
    SFX_ALWAYS = 2,
    SFX_SETPITCH = 4,
    SFX_LANDONLY = 0x4000,
    SFX_WATERONLY = 0x8000
};

#define SFX_DEFAULT 0
#define SFX_LANDANDWATER 0

#define SFX_LARA_NO 2
#define SFX_LARA_SLIPPING 3
#define SFX_ROCK_FALL_CRUMBLE 66
#define SFX_BIKE_HIT_OBJECTS 366
#define SFX_STARGATE_SWIRL 23
#define SFX_EARTHQUAKE_LOOP 107
#define SFX_LOOP_FOR_SMALL_FIRES 150
#define SFX_RUMBLE_NEXTDOOR 149
#define SFX_LARA_RICOCHET 10
#define SFX_MENU_MEDI 116
#define SFX_MENU_SELECT 109
#define SFX_MENU_CHOOSE 111
#define SFX_METAL_SCRAPE_LOOP 20
#define SFX_METAL_SCRAPE_LOOP1 12
#define SFX_LARA_NO_FRENCH 348
#define SFX_LARA_NO_JAPAN 349
#define SFX_FOOTSTEPS_MUD 288
#define SFX_MINE_EXP_OVERLAY 103
#define SFX_EXPLOSION1 105
#define SFX_EXPLOSION2 106
#define SFX_DART_SPITT 325
#define SFX_LARA_CROSSBOW 235
#define SFX_DOOR_GEN_THUD 255
#define SFX_TEETH_SPIKES 343
#define SFX_DEMI_SIREN_SWAVE 251
#define SFX_LARA_BUBBLES 37
#define SFX_HELICOPTER_LOOP 65
#define SFX_SAND_LOOP 344
#define SFX_WATERFALL_LOOP 79
#define SFX_MAPPER_LAZER 308
#define SFX_BOULDER_FALL 122
#define SFX_MAPPER_PYRAMID_OPEN 355
#define SFX_BAD_SWORD_RICO 262
#define SFX_LARA_POLE_LOOP 340
#define SFX_LARA_ROPE_CREAK 351
#define SFX_HIT_ROCK 347
#define SFX_BIKE_HIT_ENEMIES 367
#define SFX_OBJ_GEM_SMASH 285
#define SFX_JEEP_MOVE 155
#define SFX_LOOP_FOR_SMALL_FIRES 150
#define SFX_JOBY_WIND 362
#define SFX_POUR 97
#define SFX_SANDHAM_IN_THE_HOUSE 363
#define SFX_FLAME_EMITTER 368

typedef struct sample_info {
    sint16 number;
    uint8 volume;
    uint8 radius;
    uint8 randomness;
    sint8 pitch;
    sint16 flags;
} SAMPLE_INFO;

typedef struct sound_slot {
    sint32 OrigVolume;
    sint32 nVolume;
    sint32 nPan;
    sint32 nPitch;
    sint32 nSampleInfo;
    uint32 distance;
    PHD_VECTOR pos;
} SoundSlot;

void GetPanVolume(SoundSlot *slot);
void Func45f960(void);
void StopSoundEffect(sint32 sfx);
void SOUND_Stop(void);
void SOUND_Init(void);
void SayNo(void);
sint32 SoundEffect(sint32 sfx, PHD_3DPOS *pos, sint32 flags);
void nullsub_1();

extern SAMPLE_INFO *sample_infos;
extern SoundSlot LaSlot[32];
extern sint16 *sample_lut;
extern sint32 sound_active;
extern sint16 FXType;

#endif
