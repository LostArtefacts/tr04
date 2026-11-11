#include <windows.h>
#include <dinput.h>

#include "input.h"
#include "dxshell.h"
#include "winmain.h"
#include "loadsave.h"
#include "../lara.h"
#include "../lara_states.h"
#include "../items.h"
#include "../savegame.h"
#include "../gameflow.h"
#include "../sound.h"
#include "../control.h"
#include "../camera.h"

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

sint32 S_UpdateInput(void)
{
    sint16 state;
    uint8 debounce;

    debounce = SetDebounce != 0;
    DXReadKeyboard((char *)keymap);
    if (ControlMethod == 1)
        joy_fire = ReadJoystick(&joy_x, &joy_y);
    linput = 0;
    if (ControlMethod == 1) {
        if (joy_x < -8)
            linput = IN_LEFT;
        else if (joy_x > 8)
            linput = IN_RIGHT;
        if (joy_y > 8)
            linput |= IN_BACK;
        else if (joy_y < -8)
            linput |= IN_FORWARD;
    }
    if (Key(0))
        linput |= IN_FORWARD;
    if (Key(1))
        linput |= IN_BACK;
    if (Key(2))
        linput |= IN_LEFT;
    if (Key(3))
        linput |= IN_RIGHT;
    if (Key(4))
        linput |= IN_DUCK;
    if (Key(5))
        linput |= IN_SPRINT;
    if (Key(6))
        linput |= IN_WALK;
    if (Key(7))
        linput |= IN_JUMP;
    if (Key(8))
        linput |= IN_SELECT | IN_ACTION;
    if (Key(9))
        linput |= IN_DRAW;
    if (Key(10)) {
        if (!flare_no_db) {
            state = lara_item->current_anim_state;
            if (state == AS_ALL4S || state == AS_CRAWL || state == AS_ALL4TURNL || state == AS_ALL4TURNR || state == AS_CRAWLBACK || state == AS_CRAWL2HANG) {
                SoundEffect(SFX_LARA_NO, 0, SFX_ALWAYS);
                flare_no_db = 1;
            } else {
                linput |= IN_FLARE;
                flare_no_db = 0;
            }
        }
    } else {
        flare_no_db = 0;
    }
    if (Key(11))
        linput |= IN_LOOK;
    if (Key(12))
        linput |= IN_ROLL;
    if (Key(13))
        linput |= IN_OPTION;
    if (Key(14))
        linput |= IN_WALK | IN_LEFT;
    if (Key(15))
        linput |= IN_WALK | IN_RIGHT;
    if (Key(16))
        linput |= IN_PAUSE;
    if (Key(17))
        linput |= IN_SELECT;
    if (keymap[DIK_ESCAPE])
        linput |= IN_DESELECT | IN_OPTION;
    if (lara.gun_status == LG_READY) {
        savegame.AutoTarget = (uint8)App.AutoTarget;
        if (linput & IN_LOOK) {
            if (LookCnt < 6) {
                linput &= ~IN_LOOK;
                LookCnt++;
            } else {
                LookCnt = 100;
            }
        } else {
            if (LookCnt && LookCnt != 100)
                linput |= IN_TARGET;
            LookCnt = 0;
        }
    }
    if (!(gfLevelFlags & GF_YOUNGLARA)) {
        if (keymap[DIK_1]) {
            if (lara.pistols_type_carried & 1) {
                lara.request_gun_type = WEAPON_PISTOLS;
                if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_PISTOLS)
                    lara.gun_status = LG_DRAW_GUNS;
            }
        } else if (keymap[DIK_2]) {
            if (lara.shotgun_type_carried & 1) {
                lara.request_gun_type = WEAPON_SHOTGUN;
                if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_SHOTGUN)
                    lara.gun_status = LG_DRAW_GUNS;
            }
        } else if (keymap[DIK_3]) {
            if (lara.uzis_type_carried & 1) {
                lara.request_gun_type = WEAPON_UZI;
                if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_UZI)
                    lara.gun_status = LG_DRAW_GUNS;
            }
        } else if (keymap[DIK_4]) {
            if (lara.sixshooter_type_carried & 1) {
                lara.request_gun_type = WEAPON_REVOLVER;
                if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_REVOLVER)
                    lara.gun_status = LG_DRAW_GUNS;
            }
        } else if (keymap[DIK_5]) {
            if (lara.grenade_type_carried & 1) {
                lara.request_gun_type = WEAPON_GRENADE;
                if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_GRENADE)
                    lara.gun_status = LG_DRAW_GUNS;
            }
        } else if (keymap[DIK_6]) {
            if (lara.crossbow_type_carried & 1) {
                lara.request_gun_type = WEAPON_CROSSBOW;
                if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_CROSSBOW)
                    lara.gun_status = LG_DRAW_GUNS;
            }
        }
    }
    if (keymap[DIK_0]) {
        if (!med_hotkey_timer) {
            if (lara_item->hit_points > 0 && lara_item->hit_points < 1000 || lara.poisoned) {
                if (lara.num_small_medipack != -1 && lara.num_small_medipack)
                    lara.num_small_medipack--;
                if (lara.num_small_medipack) {
                    lara.dpoisoned = 0;
                    lara_item->hit_points += 500;
                    if (lara_item->hit_points > 1000) {
                        lara_item->hit_points = 1000;
                        SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
                        savegame.Game.HealthUsed++;
                    }
                }
                med_hotkey_timer = 15;
            }
        }
    } else if (keymap[DIK_9]) {
        if (!med_hotkey_timer) {
            if (lara_item->hit_points > 0 && lara_item->hit_points < 1000 || lara.poisoned) {
                if (lara.num_large_medipack != -1 && lara.num_large_medipack)
                    lara.num_large_medipack--;
                if (lara.num_large_medipack) {
                    lara.dpoisoned = 0;
                    lara_item->hit_points += 1000;
                    if (lara_item->hit_points > 1000) {
                        lara_item->hit_points = 1000;
                        SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
                        savegame.Game.HealthUsed++;
                    }
                }
                med_hotkey_timer = 15;
            }
        }
    } else if (med_hotkey_timer) {
        med_hotkey_timer--;
    }
    if (keymap[DIK_F10])
        linput |= IN_E;
    if (linput & IN_WALK && !(linput & (IN_FORWARD | IN_BACK))) {
        if (linput & IN_LEFT)
            linput = (linput & ~IN_LEFT) | IN_LSTEP;
        else if (linput & IN_RIGHT)
            linput = (linput & ~IN_RIGHT) | IN_RSTEP;
    }
    if (linput & IN_FORWARD && linput & IN_BACK)
        linput |= IN_ROLL;
    if (linput & IN_ROLL && BinocularRange)
        linput &= ~IN_ROLL;
    if ((linput & (IN_RIGHT | IN_LEFT)) == (IN_RIGHT | IN_LEFT))
        linput -= IN_RIGHT | IN_LEFT;
    if (debounce)
        dbinput = inputBusy;
    if (!gfGameMode && Gameflow->LoadSaveEnabled) {
        if (keymap[DIK_F5])
            linput |= IN_SAVE;
        if (keymap[DIK_F6])
            linput |= IN_LOAD;
    }
    if (Gameflow->CheatEnabled) {
        if (linput)
            cheat_code = 0;
        switch (cheat_code) {
        case 0:
            if (keymap[DIK_D])
                cheat_code = 1;
            break;
        case 1:
            if (keymap[DIK_O])
                cheat_code = 2;
            break;
        case 2:
            if (keymap[DIK_Z])
                cheat_code = 3;
            break;
        case 3:
            if (keymap[DIK_Y])
                linput = IN_C;
            break;
        }
    }
    if (keymap[DIK_APOSTROPHE])
        DXSaveScreen(App.dx.lpBackBuffer, "Tomb");
    inputBusy = linput;
    if (lara.Busy) {
        linput &= IN_PAUSE | IN_LOOK | IN_OPTION | IN_RIGHT | IN_LEFT | IN_BACK | IN_FORWARD;
        if (linput & IN_FORWARD && linput & IN_BACK)
            linput ^= IN_BACK;
    }
    if (debounce)
        dbinput = inputBusy & (dbinput ^ inputBusy);
    input = linput;
    return 1;
}
