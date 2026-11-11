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
