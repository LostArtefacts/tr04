#ifndef SPECIFIC_INPUT_H
#define SPECIFIC_INPUT_H

#include <windows.h>

#include "../types.h"

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
