#include <stdio.h>

#include "health.h"
#include "control.h"
#include "deltapak.h"
#include "gameflow.h"
#include "lara.h"
#include "newinv.h"
#include "savegame.h"
#include "spotcam.h"
#include "text.h"
#include "specific/3dmath.h"
#include "specific/loadsave.h"

sint32 FlashIt(void)
{
    if (flash_count) {
        flash_count--;
    } else {
        flash_state ^= 1;
        flash_count = 5;
    }
    return flash_state;
}

void DrawGameInfo(sint32 timed)
{
    sint32 flash_state, seconds;
    char buf[80];

    if (!GLOBAL_playing_cutseq && !bDisableLaraControl && gfGameMode != 1) {
        flash_state = FlashIt();
        DrawHealthBar(flash_state);
        DrawAirBar(flash_state);
        DrawPickups();
        if (DashTimer < 120)
            S_DrawDashBar(100 * DashTimer / 120);
        if (gfLevelFlags & GF_TIMER && savegame.Level.Timer && savegame.Level.Timer < 108000) {
            seconds = savegame.Level.Timer / 30;
            sprintf(buf, "%.2d:%.2d:%.2d", seconds / 60, seconds % 60, (sint32)(334 * (savegame.Level.Timer % 30)) / 100);
            PrintString((uint16)(phd_winwidth >> 1), (uint16)font_height, 0, buf, FF_CENTER);
        }
    }
}

void DrawHealthBar(sint32 flash_state)
{
    sint32 hitpoints;

    hitpoints = lara_item->hit_points;
    if (hitpoints < 0)
        hitpoints = 0;
    else if (hitpoints > 1000)
        hitpoints = 1000;
    if (old_hitpoints != hitpoints) {
        old_hitpoints = hitpoints;
        health_bar_timer = 40;
    }
    if (health_bar_timer < 0)
        health_bar_timer = 0;
    if (hitpoints <= 250) {
        if (flash_state)
            S_DrawHealthBar(hitpoints / 10);
        else
            S_DrawHealthBar(0);
    } else if (health_bar_timer > 0 || hitpoints <= 0 || lara.gun_status == LG_READY && lara.gun_type != 8 || lara.poisoned >= 256) {
        S_DrawHealthBar(hitpoints / 10);
    }
    if (PoisonFlag)
        PoisonFlag--;
}

void DrawAirBar(sint32 flash_state)
{
    sint32 air;

    if (lara.vehicle == NO_ITEM && (lara.water_status == LW_UNDERWATER || lara.water_status == LW_SURFACE)) {
        air = lara.air;
        if (air < 0)
            air = 0;
        else if (air > 1800)
            air = 1800;
        if (air <= 450) {
            if (flash_state)
                S_DrawAirBar(100 * air / 1800);
            else
                S_DrawAirBar(0);
        } else {
            S_DrawAirBar(100 * air / 1800);
        }
    }
}

void InitialisePickUpDisplay(void)
{
    int i;

    for (i = 0; i < 8; i++)
        pickups[i].life = -1;
    PickupY = 128;
    PickupX = 128;
    PickupVel = 0;
    CurrentPickup = 0;
}

void DrawPickups(void)
{
    DISPLAYPU *pu;
    sint32 lp;

    pu = &pickups[CurrentPickup];
    if (pu->life > 0) {
        if (PickupX > 0)
            PickupX += -PickupX >> 3;
        else
            pu->life--;
    } else if (!pu->life) {
        if (PickupX < 128) {
            if (PickupVel < 16)
                PickupVel++;
            PickupX += PickupVel;
        } else {
            pu->life = -1;
            PickupVel = 0;
        }
    } else {
        for (lp = 0; lp < 8; lp++) {
            if (pickups[CurrentPickup].life > 0)
                break;
            CurrentPickup++;
            CurrentPickup &= 7;
        }
        if (lp == 8)
            CurrentPickup = 0;
    }
}

void AddDisplayPickup(sint16 object_number)
{
    DISPLAYPU *pu;
    int i;

    pu = pickups;
    for (i = 0; i < 8; pu++, i++) {
        if (pu->life < 0) {
            pu->life = 45;
            pu->object_number = object_number;
            break;
        }
    }
    DEL_picked_up_object(object_number);
}
