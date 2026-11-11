#ifndef SPECIFIC_REGISTRY_H
#define SPECIFIC_REGISTRY_H

#include <windows.h>

#include "../types.h"

uint8 REG_OpenKey(LPCSTR lpSubKey);
void REG_CloseKey(void);

#endif
