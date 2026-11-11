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
