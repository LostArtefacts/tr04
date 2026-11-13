#ifndef SPECIFIC_DXSOUND_H
#define SPECIFIC_DXSOUND_H

#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
#include <msacm.h>

#include "../types.h"

typedef struct ds_sample {
    LPDIRECTSOUNDBUFFER buffer;
    sint32 frequency;
    sint32 playing;
} DS_SAMPLE;

uint8 DXChangeOutputFormat(sint32 nSamplesPerSec, uint8 force);
void DSChangeVolume(sint32 num, sint32 volume);
void DS_AdjustPitch(sint32 num, sint32 pitch);
void DSAdjustPan(sint32 num, sint32 pan);
uint8 DXSetOutputFormat(void);
uint8 DXDSCreate(void);
uint8 InitSampleDecompress(void);
uint8 FreeSampleDecompress(void);
uint8 DXCreateSampleADPCM(char *data, sint32 comp_size, sint32 uncomp_size, sint32 num);
void DS_StopSample(sint32 num);
uint8 DS_IsChannelPlaying(sint32 num);
sint32 DS_GetFreeChannel(void);
sint32 CalcVolume(sint32 volume);
void S_SoundStopAllSamples(void);
void S_SoundStopSample(sint32 num);
sint32 S_SoundPlaySample();
sint32 S_SoundPlaySampleLooped();
void DXFreeSounds(void);
sint32 S_SoundSampleIsPlaying(sint32 num);
void S_SoundSetPanAndVolume();
void S_SoundSetPitch(sint32 num, sint32 pitch);
sint32 DS_StartSample(sint32 num, sint32 volume, sint32 pitch, sint32 pan, uint32 flags);

extern LPDIRECTSOUNDBUFFER G_DSPrimary;
extern DS_SAMPLE DS_Samples[32];
extern DS_SAMPLE DS_Buffers[256];
extern sint32 lastSPC;
extern char source_pcm_format[50];
extern char *samples_buffer;
extern char *decompressed_samples_buffer;
extern MMRESULT DS_mmresult;
extern WAVEFORMATEX pcm_format;
extern HACMSTREAM DS_hACMStream;
extern ACMSTREAMHEADER ACMStreamHeader;
extern HACMDRIVER hACMDriver;
extern sint32 SFXVolume;
extern sint32 sound_active;
extern sint32 sfx_frequencies[3];

#endif
