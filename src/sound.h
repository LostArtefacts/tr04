#ifndef SOUND_H
#define SOUND_H

#include "3dglodef.h"

enum sfx_options {
    SFX_WATER = 1,
    SFX_ALWAYS = 2,
    SFX_SETPITCH = 4,
    SFX_LANDONLY = 0x4000
};

#define SFX_DEFAULT 0

#define SFX_LARA_NO 2
#define SFX_LARA_NO_FRENCH 348
#define SFX_LARA_NO_JAPAN 349
#define SFX_FOOTSTEPS_MUD 288

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
