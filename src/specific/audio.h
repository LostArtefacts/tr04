#ifndef SPECIFIC_AUDIO_H
#define SPECIFIC_AUDIO_H

#include <windows.h>
#include <stdio.h>
#include <mmreg.h>
#include <dsound.h>
#include <msacm.h>

#include "../types.h"
#include "function_stubs.h"
#include "file.h"
#include "dxshell.h"

void S_CDPlay(sint32 track, sint32 mode);
void S_CDStop(void);
void S_StartSyncedAudio(sint32 track);
void ACMSetVolume(void);
BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD dwInstance, DWORD fdwSupport);
sint32 ACMSetupNotifications(void);
sint32 ACMHandleNotifications(void);
void ACMClose(void);
void OpenStreamFile(char *name);
void FillADPCMBuffer(char *p, sint32 track);
void GetADPCMData(void);
void ACMEmulateCDPlay(sint32 track, sint32 mode);

extern CRITICAL_SECTION audio_cs;
extern HACMDRIVERID hACMDriverID;
extern HACMSTREAM hACMStream;
extern HACMDRIVER hACMDriver;
extern ACMSTREAMHEADER StreamHeaders[4];
extern uint8 acm_ready;
extern FILE *audio_stream_fp;
extern sint32 audio_counter;
extern volatile uint8 reading_audio_file;
extern uint8 continue_reading_audio_file;
extern LPDIRECTSOUNDBUFFER G_DSBuffer;
extern LPDIRECTSOUNDNOTIFY G_DSNotify;
extern HANDLE NotifyEventHandles[2];
extern HANDLE NotificationThreadHandle;
extern sint32 NotifySize;
extern char *wav_file_buffer;
extern char *audio_fp_write_ptr;
extern sint32 auido_play_mode;
extern sint32 XATrack;
extern sint32 XAReqTrack;
extern char TrackFileNames[112][64];
extern char *ADPCMBuffer;
extern char *pAudioWrite;
extern DWORD AudioBytes;
extern sint32 audio_buffer_size;
extern sint32 CurrentNotify;
extern uint32 NextWriteOffset;
extern sint32 XAFlag;
extern uint8 CurrentAtmosphere;
extern sint32 MusicVolume;
extern uint8 IsAtmospherePlaying;

#endif
