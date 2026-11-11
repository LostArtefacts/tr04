#include <stdio.h>

#include "registry.h"
#include "winmain.h"
#include "input.h"
#include "audio.h"
#include "dxsound.h"
#include "cmdline.h"
#include "loadsave.h"

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

void CloseRegistry(void)
{
    REG_CloseKey();
}

uint8 LoadSettings(void)
{
    uint32 key;
    uint8 val;

    if (OpenRegistry("System")) {
        REG_ReadBool("Setup", &REG_Setup, 0);
        if (REG_Setup) {
            App.TextureSize = 256;
            App.BumpMapSize = 256;
            App.StartFlags = 32;
            REG_ReadLong("DD", (uint32 *)&App.DXInfo.nDD, 0);
            REG_ReadLong("D3D", (uint32 *)&App.DXInfo.nD3D, 0);
            REG_ReadLong("VMode", (uint32 *)&App.DXInfo.nDisplayMode, 0);
            REG_ReadLong("TFormat", (uint32 *)&App.DXInfo.nTexture, 0);
            REG_ReadLong("DS", (uint32 *)&App.DXInfo.nDS, 0);
            REG_ReadBool("BumpMap", &App.BumpMapping, 1);
            REG_ReadBool("Filter", &App.Filtering, 1);
            REG_ReadBool("DisableSound", &App.SoundDisabled, 0);
            REG_ReadBool("Volumetric", &App.Volumetric, 1);
            REG_ReadBool("NoFMV", &fmvs_disabled, 0);
            REG_ReadBool("TextLow", &val, 0);
            if (val)
                App.TextureSize = 128;
            REG_ReadBool("BumpLow", &val, 0);
            if (val)
                App.BumpMapSize = 128;
            REG_ReadBool("HardWare", &val, 1);
            if (val)
                App.StartFlags |= 0x90;
            REG_ReadBool("Window", &val, 0);
            if (val)
                App.StartFlags |= 2;
            else
                App.StartFlags |= 1;
        }
        CloseRegistry();
        OpenRegistry("Game");
        REG_ReadLong("Key0", &key, layout[0][0]);
        layout[1][0] = (sint16)key;
        REG_ReadLong("Key1", &key, layout[0][1]);
        layout[1][1] = (sint16)key;
        REG_ReadLong("Key2", &key, layout[0][2]);
        layout[1][2] = (sint16)key;
        REG_ReadLong("Key3", &key, layout[0][3]);
        layout[1][3] = (sint16)key;
        REG_ReadLong("Key4", &key, layout[0][4]);
        layout[1][4] = (sint16)key;
        REG_ReadLong("Key5", &key, layout[0][5]);
        layout[1][5] = (sint16)key;
        REG_ReadLong("Key6", &key, layout[0][6]);
        layout[1][6] = (sint16)key;
        REG_ReadLong("Key7", &key, layout[0][7]);
        layout[1][7] = (sint16)key;
        REG_ReadLong("Key8", &key, layout[0][8]);
        layout[1][8] = (sint16)key;
        REG_ReadLong("Key9", &key, layout[0][9]);
        layout[1][9] = (sint16)key;
        REG_ReadLong("Key10", &key, layout[0][10]);
        layout[1][10] = (sint16)key;
        REG_ReadLong("Key11", &key, layout[0][11]);
        layout[1][11] = (sint16)key;
        REG_ReadLong("Key12", &key, layout[0][12]);
        layout[1][12] = (sint16)key;
        REG_ReadLong("Key13", &key, layout[0][13]);
        layout[1][13] = (sint16)key;
        REG_ReadLong("Key14", &key, layout[0][14]);
        layout[1][14] = (sint16)key;
        REG_ReadLong("Key15", &key, layout[0][15]);
        layout[1][15] = (sint16)key;
        REG_ReadLong("Key16", &key, layout[0][16]);
        layout[1][16] = (sint16)key;
        REG_ReadLong("Key17", &key, layout[0][17]);
        layout[1][17] = (sint16)key;
        REG_ReadLong("MusicVolume", (uint32 *)&MusicVolume, 80);
        REG_ReadLong("SFXVolume", (uint32 *)&SFXVolume, 90);
        REG_ReadLong("ControlMethod", (uint32 *)&ControlMethod, 0);
        REG_ReadLong("SoundQuality", (uint32 *)&SoundQuality, 1);
        REG_ReadLong("AutoTarget", (uint32 *)&App.AutoTarget, 1);
        REG_ReadLong("WindowX", (uint32 *)&App.dx.rScreen.left, 0);
        REG_ReadLong("WindowY", (uint32 *)&App.dx.rScreen.top, 0);
        CloseRegistry();
        CheckKeyConflicts();
        return REG_Setup;
    }
    return 0;
}
