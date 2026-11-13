#include <windows.h>

LPVOID DBLog_MapView(HANDLE hMap)
{
    return MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0x100000);
}

HANDLE DBLog_OpenMapping(LPCSTR name)
{
    return OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, name);
}
