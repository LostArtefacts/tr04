#include <windows.h>
#include <dinput.h>

#include "input.h"
#include "dxshell.h"

sint32 Key(sint32 number)
{
    sint16 key;

    key = layout[1][number];
    if (key >= 256) {
        if (joy_fire & (1 << key))
            return 1;
    } else {
        if (keymap[key])
            return 1;
        if (key == DIK_RCONTROL)
            return keymap[DIK_LCONTROL];
        if (key == DIK_LCONTROL)
            return keymap[DIK_RCONTROL];
        if (key == DIK_RSHIFT)
            return keymap[DIK_LSHIFT];
        if (key == DIK_LSHIFT)
            return keymap[DIK_RSHIFT];
        if (key == DIK_RMENU)
            return keymap[DIK_LMENU];
        if (key == DIK_LMENU)
            return keymap[DIK_RMENU];
    }
    if (conflict[number])
        return 0;
    key = layout[0][number];
    if (keymap[key])
        return 1;
    if (key == DIK_RCONTROL)
        return keymap[DIK_LCONTROL];
    if (key == DIK_LCONTROL)
        return keymap[DIK_RCONTROL];
    if (key == DIK_RSHIFT)
        return keymap[DIK_LSHIFT];
    if (key == DIK_LSHIFT)
        return keymap[DIK_RSHIFT];
    if (key == DIK_RMENU)
        return keymap[DIK_LMENU];
    if (key == DIK_LMENU)
        return keymap[DIK_RMENU];
    return 0;
}

sint32 ReadJoystick(sint32 *x, sint32 *y)
{
    JOYINFOEX joystick;

    joystick.dwSize = sizeof(JOYINFOEX);
    joystick.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
    if (joyGetPosEx(0, &joystick) != JOYERR_NOERROR) {
        joy_unavailable = 1;
        *x = 0;
        *y = 0;
        return 0;
    }
    if (joy_unavailable) {
        if (joyGetDevCaps(JOYSTICKID1, &joy_caps, sizeof(joy_caps)) == JOYERR_NOERROR) {
            joy_unavailable = 0;
        } else {
            *y = 0;
            *x = 0;
            return 0;
        }
    }
    *x = (joystick.dwXpos << 5) / (joy_caps.wXmax - joy_caps.wXmin) - 16;
    *y = (joystick.dwYpos << 5) / (joy_caps.wYmax - joy_caps.wYmin) - 16;
    return joystick.dwButtons;
}
