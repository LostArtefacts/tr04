#ifndef SPECIFIC_DXSHELL_H
#define SPECIFIC_DXSHELL_H

#include <windows.h>

#include "../types.h"

void DXReadKeyboard(char *KeyMap);
char *DXGetErrorString(HRESULT hr);
sint32 DXAttempt(HRESULT r);
void DXBitMask2ShiftCnt(uint32 mask, uint8 *shift, uint8 *count);
HRESULT DXShowFrame(void);
sint32 BPPToDDBD(sint32 BPP);
void *AddStruct(void *p, sint32 num, sint32 size);

extern uint8 keymap[256];

#endif
