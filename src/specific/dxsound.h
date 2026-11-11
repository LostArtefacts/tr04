#ifndef SPECIFIC_DXSOUND_H
#define SPECIFIC_DXSOUND_H

#include <windows.h>
#include <mmreg.h>
#include <dsound.h>

#include "../types.h"

typedef struct ds_sample {
    LPDIRECTSOUNDBUFFER buffer;
    sint32 frequency;
    sint32 playing;
} DS_SAMPLE;

uint8 DXChangeOutputFormat(sint32 nSamplesPerSec, uint8 force);
void DSChangeVolume(sint32 num, sint32 volume);
void DSAdjustPitch(sint32 num, sint32 pitch);
void DSAdjustPan(sint32 num, sint32 pan);
void S_SoundStopAllSamples(void);

extern LPDIRECTSOUNDBUFFER G_DSPrimary;
extern DS_SAMPLE DS_Samples[32];
extern DS_SAMPLE DS_Buffers[256];
extern sint32 lastSPC;

#endif
