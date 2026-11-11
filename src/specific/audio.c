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

void ACMClose(void)
{
    if (!acm_ready)
        return;
    EnterCriticalSection(&audio_cs);
    S_CDStop();
    CloseHandle(NotifyEventHandles[0]);
    CloseHandle(NotifyEventHandles[1]);
    acmStreamUnprepareHeader(hACMStream, &StreamHeaders[0], 0);
    acmStreamUnprepareHeader(hACMStream, &StreamHeaders[1], 0);
    acmStreamUnprepareHeader(hACMStream, &StreamHeaders[2], 0);
    acmStreamUnprepareHeader(hACMStream, &StreamHeaders[3], 0);
    acmStreamClose(hACMStream, 0);
    acmDriverClose(hACMDriver, 0);
    if (G_DSNotify) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Notification", G_DSNotify, IDirectSoundNotify_Release(G_DSNotify));
        G_DSNotify = 0;
    } else {
        Log(1, "%s Attempt To Release NULL Ptr", "Notification");
    }
    if (G_DSBuffer) {
        Log(4, "Released %s @ %x - RefCnt = %d", "Stream Buffer", G_DSBuffer, IDirectSoundBuffer_Release(G_DSBuffer));
        G_DSBuffer = 0;
    } else {
        Log(1, "%s Attempt To Release NULL Ptr", "Stream Buffer");
    }
    LeaveCriticalSection(&audio_cs);
}

void OpenStreamFile(char *name)
{
    sint32 n;

    __try {
        EnterCriticalSection(&audio_cs);
    } __finally {
        LeaveCriticalSection(&audio_cs);
    }
    audio_stream_fp = FileOpen(name);
    if (!audio_stream_fp) {
        Log(1, "%s - Not Found", name);
        return;
    }
    fseek(audio_stream_fp, 90, SEEK_SET);
    audio_fp_write_ptr = wav_file_buffer;
    memset(wav_file_buffer, 0, 0x37000);
    n = fread(wav_file_buffer, 1, 0x37000, audio_stream_fp);
    if (n < 0x37000 && auido_play_mode == 1) {
        fseek(audio_stream_fp, 90, SEEK_SET);
        Log(0, "FileReset In OpenStreamFile");
    }
}

void FillADPCMBuffer(char *p, sint32 track)
{
    sint32 n;

    reading_audio_file = 1;
    __try {
        EnterCriticalSection(&audio_cs);
    } __finally {
        LeaveCriticalSection(&audio_cs);
    }
    if (!audio_stream_fp) {
        reading_audio_file = 0;
        continue_reading_audio_file = 0;
        return;
    }
    if (track != XATrack || track == -1) {
        Log(0, "Not Current Track %d", track);
        reading_audio_file = 0;
        continue_reading_audio_file = 0;
        return;
    }
    memset(p, 0, 0x5800);
    if (!audio_stream_fp) {
        reading_audio_file = 0;
        continue_reading_audio_file = 0;
        return;
    }
    n = fread(p, 1, 0x5800, audio_stream_fp);
    if (audio_stream_fp && feof(audio_stream_fp)) {
        if (auido_play_mode == 1) {
            fseek(audio_stream_fp, 90, SEEK_SET);
        } else {
            audio_counter++;
            if (audio_counter > 8) {
                audio_counter = 0;
                if (auido_play_mode == 2) {
                    reading_audio_file = 0;
                    continue_reading_audio_file = 0;
                    S_CDStop();
                    return;
                }
                if (CurrentAtmosphere && !IsAtmospherePlaying) {
                    reading_audio_file = 0;
                    continue_reading_audio_file = 0;
                    S_CDStop();
                    S_CDPlay(CurrentAtmosphere, 1);
                    return;
                }
            }
        }
    }
    reading_audio_file = 0;
    continue_reading_audio_file = 1;
}

void GetADPCMData(void)
{
    if (!audio_stream_fp)
        return;
    memset(audio_fp_write_ptr, 0, 0x5800);
    if ((sint32)fread(audio_fp_write_ptr, 1, 0x5800, audio_stream_fp) < 0x5800 && auido_play_mode == 1) {
        Log(0, "FileReset In GetADPCMData");
        fseek(audio_stream_fp, 90, SEEK_SET);
    }
    audio_fp_write_ptr += 0x5800;
    if ((sint32)audio_fp_write_ptr >= (sint32)(wav_file_buffer + 0x37000))
        audio_fp_write_ptr = wav_file_buffer;
}

void ACMEmulateCDPlay(sint32 track, sint32 mode)
{
    char name[256];

    __try {
        EnterCriticalSection(&audio_cs);
    } __finally {
        LeaveCriticalSection(&audio_cs);
    }
    wsprintf(name, "audio\\%s", TrackFileNames[track]);
    Log(8, "Playing %s %s %d", name, mode ? "Looped" : "", track);
    XATrack = track;
    XAReqTrack = track;
    XAFlag = 6;
    auido_play_mode = mode;
    OpenStreamFile(name);
    if (!audio_stream_fp)
        return;
    memcpy(ADPCMBuffer, audio_fp_write_ptr, 0x5800);
    GetADPCMData();
    DXAttempt(IDirectSoundBuffer_Lock(G_DSBuffer, 0, audio_buffer_size, (LPVOID *)&pAudioWrite, &AudioBytes, 0, 0, 0));
    acmStreamConvert(hACMStream, &StreamHeaders[0], ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);
    memcpy(ADPCMBuffer, audio_fp_write_ptr, 0x5800);
    GetADPCMData();
    acmStreamConvert(hACMStream, &StreamHeaders[1], ACM_STREAMCONVERTF_BLOCKALIGN);
    DXAttempt(IDirectSoundBuffer_Unlock(G_DSBuffer, pAudioWrite, audio_buffer_size, 0, 0));
    CurrentNotify = 2;
    NextWriteOffset = 2 * NotifySize;
    ACMSetVolume();
    IDirectSoundBuffer_Play(G_DSBuffer, 0, 0, DSBPLAY_LOOPING);
}

sint32 ACMHandleNotifications(void)
{
    char *write;
    DWORD wait, bytes;

    while ((wait = WaitForMultipleObjects(2, NotifyEventHandles, 0, INFINITE)) != WAIT_FAILED) {
        EnterCriticalSection(&audio_cs);
        if (!wait && G_DSBuffer) {
            memcpy(ADPCMBuffer, audio_fp_write_ptr, 0x5800);
            if (XATrack == -1)
                memset(ADPCMBuffer, 0, 0x5800);
            else
                FillADPCMBuffer(audio_fp_write_ptr, XATrack);
            if (continue_reading_audio_file) {
                audio_fp_write_ptr += 0x5800;
                if ((sint32)audio_fp_write_ptr >= (sint32)(wav_file_buffer + 0x37000))
                    audio_fp_write_ptr = wav_file_buffer;
                IDirectSoundBuffer_Lock(G_DSBuffer, NextWriteOffset, NotifySize, (LPVOID *)&write, &bytes, 0, 0, 0);
                acmStreamConvert(hACMStream, &StreamHeaders[CurrentNotify], ACM_STREAMCONVERTF_BLOCKALIGN);
                IDirectSoundBuffer_Unlock(G_DSBuffer, write, bytes, 0, 0);
                NextWriteOffset += bytes;
                if (NextWriteOffset >= audio_buffer_size)
                    NextWriteOffset -= audio_buffer_size;
                CurrentNotify = (CurrentNotify + 1) & 3;
            }
        }
        LeaveCriticalSection(&audio_cs);
        if (!G_DSBuffer)
            break;
    }
    return DS_OK;
}
