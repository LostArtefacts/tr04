#ifndef SPECIFIC_SWRENDER_H
#define SPECIFIC_SWRENDER_H

#include <windows.h>
#include <d3d.h>

HRESULT SW_SetRenderStateFlags(D3DRENDERSTATETYPE type, DWORD state);
void SW_FreeBuffer(void);

#endif
