#include <windows.h>
#include <ddraw.h>

#include "loadsave.h"
#include "winmain.h"
#include "input.h"
#include "../text.h"
#include "../lara.h"
#include "../gameflow.h"

void CreateMonoScreen(void)
{
    MonoScreenOn = 1;
    ConvertSurfaceToTextures(App.dx.lpPrimaryBuffer);
}
