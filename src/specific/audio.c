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

sint32 ACMSetupNotifications(void)
{
    DSBPOSITIONNOTIFY posNotif[5];
    DWORD ThreadId;
    sint32 result, size;
    int i;

    size = NotifySize;
    NotifyEventHandles[0] = CreateEvent(0, 0, 0, 0);
    NotifyEventHandles[1] = CreateEvent(0, 0, 0, 0);
    posNotif[0].dwOffset = size;
    posNotif[0].hEventNotify = NotifyEventHandles[0];
    Log(8, "Set notifies for position %lu", posNotif[0].dwOffset);
    for (i = 1; i < 4; i++) {
        posNotif[i].dwOffset = posNotif[i - 1].dwOffset + size;
        posNotif[i].hEventNotify = NotifyEventHandles[0];
        Log(8, "Set notifies for positions %lu", posNotif[i].dwOffset);
    }
    posNotif[3].dwOffset--;
    posNotif[4].dwOffset = -1;
    posNotif[4].hEventNotify = NotifyEventHandles[1];
    NotificationThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ACMHandleNotifications, 0, 0, &ThreadId);
    if (!NotificationThreadHandle)
        Log(1, "Create Notification Thread failed");
    result = IDirectSoundNotify_SetNotificationPositions(G_DSNotify, 5, posNotif);
    if (result != DS_OK) {
        CloseHandle(NotifyEventHandles[0]);
        CloseHandle(NotifyEventHandles[1]);
        NotifyEventHandles[1] = 0;
        NotifyEventHandles[0] = 0;
    } else {
        Log(8, "Setup Notifications OK");
    }
    return result;
}
