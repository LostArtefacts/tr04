#include "registry.h"

extern HKEY phkResult;
extern DWORD dwDisposition;

uint8 REG_OpenKey(LPCSTR lpSubKey)
{
    return RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &phkResult, &dwDisposition) == ERROR_SUCCESS;
}

void REG_CloseKey(void)
{
    RegCloseKey(phkResult);
}

void REG_WriteLong(char *SubKeyName, uint32 value)
{
    RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE *)&value, sizeof(uint32));
}
