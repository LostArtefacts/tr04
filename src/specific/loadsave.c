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

void CheckKeyConflicts(void)
{
    sint16 key;
    sint32 i;
    sint32 j;

    for (i = 0; i < 18; i++) {
        key = layout[0][i];
        conflict[i] = 0;
        for (j = 0; j < 18; j++) {
            if (key == layout[1][j]) {
                conflict[i] = 1;
                break;
            }
        }
    }
}
