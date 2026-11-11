#include <stdlib.h>
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

uint8 InitSampleDecompress(void)
{
    pcm_format.wFormatTag = WAVE_FORMAT_PCM;
    pcm_format.cbSize = 0;
    pcm_format.nChannels = 1;
    pcm_format.nAvgBytesPerSec = 44100;
    pcm_format.nSamplesPerSec = 22050;
    pcm_format.nBlockAlign = 2;
    pcm_format.wBitsPerSample = 16;
    DS_mmresult = acmStreamOpen(&DS_hACMStream, hACMDriver, (LPWAVEFORMATEX)source_pcm_format, &pcm_format, 0, 0, 0, 0);
    if (DS_mmresult != DS_OK)
        Log(1, "Stream Open %d", DS_mmresult);
    decompressed_samples_buffer = (char *)malloc(0x40000);
    samples_buffer = (char *)malloc(0x4005A);
    memset(&ACMStreamHeader, 0, sizeof(ACMStreamHeader));
    ACMStreamHeader.pbSrc = (LPBYTE)(samples_buffer + 90);
    ACMStreamHeader.cbStruct = 84;
    ACMStreamHeader.cbSrcLength = 0x40000;
    ACMStreamHeader.cbDstLength = 0x40000;
    ACMStreamHeader.pbDst = (LPBYTE)decompressed_samples_buffer;
    DS_mmresult = acmStreamPrepareHeader(DS_hACMStream, &ACMStreamHeader, 0);
    if (DS_mmresult != DS_OK)
        Log(1, "Prepare Stream %d", DS_mmresult);
    return 1;
}

uint8 FreeSampleDecompress(void)
{
    ACMStreamHeader.cbSrcLength = 0x40000;
    DS_mmresult = acmStreamUnprepareHeader(DS_hACMStream, &ACMStreamHeader, 0);
    if (DS_mmresult != DS_OK)
        Log(1, "UnPrepare Stream %d", DS_mmresult);
    DS_mmresult = acmStreamClose(DS_hACMStream, 0);
    if (DS_mmresult != DS_OK)
        Log(1, "Stream Close %d", DS_mmresult);
    free(decompressed_samples_buffer);
    free(samples_buffer);
    return 1;
}

void DXStopSample(sint32 num)
{
    LPDIRECTSOUNDBUFFER buffer;

    if (num >= 0 && (buffer = DS_Samples[num].buffer) != 0) {
        DXAttempt(IDirectSoundBuffer_Stop(buffer));
        DXAttempt(IDirectSoundBuffer_Release(buffer));
        DS_Samples[num].playing = 0;
        DS_Samples[num].buffer = 0;
    }
}
