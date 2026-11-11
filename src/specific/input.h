#ifndef SPECIFIC_INPUT_H
#define SPECIFIC_INPUT_H

#include <windows.h>

#include "../types.h"

enum input_buttons {
    IN_NONE = 0x0,
    IN_FORWARD = 0x1,
    IN_BACK = 0x2,
    IN_LEFT = 0x4,
    IN_RIGHT = 0x8,
    IN_JUMP = 0x10,
    IN_DRAW = 0x20,
    IN_ACTION = 0x40,
    IN_WALK = 0x80,
    IN_OPTION = 0x100,
    IN_LOOK = 0x200,
    IN_LSTEP = 0x400,
    IN_RSTEP = 0x800,
    IN_ROLL = 0x1000,
    IN_PAUSE = 0x2000,
    IN_A = 0x4000,
    IN_B = 0x8000,
    IN_CHEAT = 0x10000,
    IN_D = 0x20000,
    IN_C = 0x30000,
    IN_E = 0x40000,
    IN_FLARE = 0x80000,
    IN_SELECT = 0x100000,
    IN_DESELECT = 0x200000,
    IN_SAVE = 0x400000,
    IN_LOAD = 0x800000,
    IN_STEPSHIFT = 0x1000000,
    IN_LOOKLEFT = 0x2000000,
    IN_LOOKRIGHT = 0x4000000,
    IN_LOOKFORWARD = 0x8000000,
    IN_LOOKBACK = 0x10000000,
    IN_DUCK = 0x20000000,
    IN_SPRINT = 0x40000000,
    IN_TARGET = 0x80000000,
    IN_ALL = 0xFFFFFFFF
};

sint32 Key(sint32 number);
sint32 ReadJoystick(sint32 *x, sint32 *y);

extern sint16 layout[2][18];
extern sint32 conflict[18];
extern sint32 input;
extern sint32 linput;
extern sint32 dbinput;
extern sint32 joy_x;
extern sint32 joy_y;
extern sint32 joy_fire;
extern JOYCAPS joy_caps;
extern sint32 joy_unavailable;

#endif
