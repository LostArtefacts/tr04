#include "audio.h"

void S_CDPlay(sint32 track, sint32 mode)
{
    if (acm_ready) {
        __try {
            EnterCriticalSection(&audio_cs);
        } __finally {
            LeaveCriticalSection(&audio_cs);
        }
        IsAtmospherePlaying = track == CurrentAtmosphere;
        audio_counter = 0;
        S_CDStop();
        ACMEmulateCDPlay(track, mode);
    }
}

void S_CDStop(void)
{
    if (acm_ready && audio_stream_fp) {
        __try {
            EnterCriticalSection(&audio_cs);
        } __finally {
            LeaveCriticalSection(&audio_cs);
        }
        memset(wav_file_buffer, 0, 0x37000);
        IDirectSoundBuffer_Stop(G_DSBuffer);
        IDirectSoundBuffer_SetCurrentPosition(G_DSBuffer, 0);
        while (reading_audio_file) {
        }
        fclose(audio_stream_fp);
        audio_stream_fp = 0;
        audio_counter = 0;
        XAFlag = 7;
        XATrack = -1;
    }
}

void S_StartSyncedAudio(sint32 track)
{
    S_CDStop();
    S_CDPlay(track, 2);
}

void ACMSetVolume(void)
{
    sint32 volume;

    volume = 100 - MusicVolume;
    if (volume == 100)
        volume = -10000;
    else
        volume = -4000 * volume / 100;
    if (G_DSBuffer)
        IDirectSoundBuffer_SetVolume(G_DSBuffer, volume);
}

BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport)
{
    ACMDRIVERDETAILS driver;

    memset(&driver, 0, sizeof(driver));
    driver.cbStruct = sizeof(ACMDRIVERDETAILS);
    acmDriverDetails(hadid, &driver, 0);
    if (strcmp(driver.szShortName, "MS-ADPCM"))
        return 1;
    hACMDriverID = hadid;
    return 0;
}
