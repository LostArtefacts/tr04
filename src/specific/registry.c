#include <stdio.h>

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

void REG_WriteBool(char *SubKeyName, uint8 value)
{
    uint32 Lvalue;

    Lvalue = (uint32)value;
    RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE *)&Lvalue, sizeof(uint32));
}

uint8 REG_ReadLong(char *SubKeyName, uint32 *value, uint32 defaultValue)
{
    uint32 type;
    uint32 cbData;

    cbData = 4;
    if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)value, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
        return 1;
    REG_WriteLong(SubKeyName, defaultValue);
    *value = defaultValue;
    return 0;
}

uint8 REG_ReadBool(char *SubKeyName, uint8 *value, uint8 defaultValue)
{
    uint32 type;
    uint32 cbData;
    uint32 data;

    cbData = 4;
    if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&data, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4) {
        *value = data != 0;
        return 1;
    }
    REG_WriteBool(SubKeyName, defaultValue);
    *value = defaultValue;
    return 0;
}

uint8 OpenRegistry(LPCSTR SubKeyName)
{
    char buffer[256];

    if (SubKeyName) {
        sprintf(buffer, "%s\\%s", "Software\\Core Design\\Tomb Raider IV", SubKeyName);
        return REG_OpenKey(buffer);
    }
    return REG_OpenKey("Software\\Core Design\\Tomb Raider IV");
}
