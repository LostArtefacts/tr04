#ifndef SPECIFIC_REGISTRY_H
#define SPECIFIC_REGISTRY_H

#include <windows.h>

#include "../types.h"

uint8 REG_OpenKey(LPCSTR lpSubKey);
void REG_CloseKey(void);
void REG_WriteLong(char *SubKeyName, uint32 value);
void REG_WriteBool(char *SubKeyName, uint8 value);
uint8 REG_ReadLong(char *SubKeyName, uint32 *value, uint32 defaultValue);
uint8 REG_ReadBool(char *SubKeyName, uint8 *value, uint8 defaultValue);
uint8 OpenRegistry(LPCSTR SubKeyName);
void CloseRegistry(void);
uint8 LoadSettings(void);
uint8 SaveSetup(HWND hDlg);

extern uint8 REG_Setup;

#endif
