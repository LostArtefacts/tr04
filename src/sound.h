#ifndef SOUND_H
#define SOUND_H

#include "3dglodef.h"

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

extern SAMPLE_INFO *sample_infos;
extern SoundSlot LaSlot[32];
extern sint16 *sample_lut;
extern sint32 sound_active;

#endif
