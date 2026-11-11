#include <string.h>

#include "dxsound.h"
#include "dxshell.h"
#include "function_stubs.h"

uint8 DXChangeOutputFormat(sint32 nSamplesPerSec, uint8 force)
{
    WAVEFORMATEX pcfxFormat;

    if (!force && lastSPC == nSamplesPerSec)
        return 1;
    memset(&pcfxFormat, 0, sizeof(pcfxFormat));
    lastSPC = nSamplesPerSec;
    pcfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    pcfxFormat.nChannels = 2;
    pcfxFormat.nSamplesPerSec = nSamplesPerSec;
    pcfxFormat.nAvgBytesPerSec = 4 * nSamplesPerSec;
    pcfxFormat.nBlockAlign = 4;
    pcfxFormat.wBitsPerSample = 16;
    pcfxFormat.cbSize = 0;
    S_SoundStopAllSamples();
    if (G_DSPrimary && DXAttempt(IDirectSoundBuffer_SetFormat(G_DSPrimary, &pcfxFormat)) != DS_OK) {
        Log(1, "Can't set sound output format to %d", pcfxFormat.nSamplesPerSec);
        return 0;
    }
    return 1;
}

void DSChangeVolume(sint32 num, sint32 volume)
{
    if (DS_Samples[num].buffer)
        IDirectSoundBuffer_SetVolume(DS_Samples[num].buffer, volume);
}

void DSAdjustPitch(sint32 num, sint32 pitch)
{
    uint32 frequency;

    if (DS_Samples[num].buffer) {
        frequency = (uint32)(22050.0F * ((float)pitch / 65536.0F));
        if (frequency < 100)
            frequency = 100;
        else if (frequency > 100000)
            frequency = 100000;
        IDirectSoundBuffer_SetFrequency(DS_Samples[num].buffer, frequency);
    }
}

void DSAdjustPan(sint32 num, sint32 pan)
{
    if (DS_Samples[num].buffer) {
        if (pan < 0) {
            if (pan < -0x4000)
                pan = -0x8000 - pan;
        } else if (pan > 0 && pan > 0x4000) {
            pan = 0x8000 - pan;
        }
        pan >>= 4;
        IDirectSoundBuffer_SetPan(DS_Samples[num].buffer, pan);
    }
}
