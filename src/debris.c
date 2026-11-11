#include "debris.h"
#include "control.h"
#include "room.h"
#include "tomb4fx.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void UpdateDebris(void)
{
    DEBRIS_STRUCT *dptr;
    FLOOR_INFO *floor;
    sint32 height, ceiling;
    int i;

    dptr = debris;
    for (i = 256; i > 0; i--, dptr++) {
        if (dptr->On) {
            dptr->Yvel += dptr->Gravity;
            if (dptr->Yvel > 4096)
                dptr->Yvel = 4096;
            dptr->Speed -= dptr->Speed >> 4;
            dptr->x += phd_sin(dptr->Dir) * dptr->Speed >> W2V_SHIFT;
            dptr->y += dptr->Yvel >> 4;
            dptr->z += phd_cos(dptr->Dir) * dptr->Speed >> W2V_SHIFT;
            floor = GetFloor(dptr->x, dptr->y, dptr->z, &dptr->RoomNumber);
            height = GetHeight(floor, dptr->x, dptr->y, dptr->z);
            ceiling = GetCeiling(floor, dptr->x, dptr->y, dptr->z);
            if (dptr->y >= height || dptr->y < ceiling) {
                dptr->On = 0;
            } else {
                dptr->XRot += dptr->Yvel >> 6;
                if (dptr->Yvel)
                    dptr->YRot += dptr->Speed >> 5;
                if (room[dptr->RoomNumber].flags & ROOM_UNDERWATER)
                    dptr->Yvel -= dptr->Yvel >> 2;
            }
        }
    }
}

sint32 GetFreeDebris(void)
{
    DEBRIS_STRUCT *dptr;
    sint32 eldestage, eldestfree, free;
    int i;

    free = next_debris;
    eldestfree = 0;
    eldestage = -0x4000;
    dptr = &debris[next_debris];
    for (i = 0; i < 256; i++) {
        if (!dptr->On) {
            next_debris = (free + 1) & 0xFF;
            return free;
        }
        if (dptr->Yvel > eldestage) {
            eldestfree = free;
            eldestage = dptr->Yvel;
        }
        if (free == 255) {
            dptr = debris;
            free = 0;
        } else {
            free++;
            dptr++;
        }
    }
    next_debris = (eldestfree + 1) & 0xFF;
    return eldestfree;
}

void TriggerDebris(GAME_VECTOR *pos, void *TextInfo, sint16 *Offsets, sint32 *Vels, sint16 rgb)
{
    DEBRIS_STRUCT *dptr;

    if (!(GetRandomControl() & 3) && rgb >= 0)
        TriggerShatterSmoke(pos->x, pos->y, pos->z);
    else if (rgb < 0)
        rgb = -rgb;
    dptr = &debris[GetFreeDebris()];
    dptr->On = 1;
    dptr->x = pos->x;
    dptr->y = pos->y;
    dptr->z = pos->z;
    if (DebrisFlags & 1) {
        dptr->Dir = GetRandomControl() << 1;
        dptr->Speed = (GetRandomControl() & 0xF) + 16;
    } else {
        dptr->Dir = phd_atan(Vels[2], Vels[0]);
        dptr->Speed = (ABS(Vels[0]) + ABS(Vels[2])) >> 2;
    }
    if (!Vels[1]) {
        dptr->Yvel = 0;
        dptr->Gravity = (GetRandomControl() & 0x1F) + 32;
    } else {
        dptr->Yvel = -512 - (GetRandomControl() & 0x1FF);
        dptr->Gravity = (GetRandomControl() & 0x3F) + 64;
        if (Vels[1] == -1)
            dptr->Yvel <<= 1;
        else if (Vels[1] == -2)
            dptr->Yvel >>= 1;
    }
    dptr->RoomNumber = pos->room_number;
    dptr->XYZOffsets1[0] = Offsets[0];
    dptr->XYZOffsets1[1] = Offsets[1];
    dptr->XYZOffsets1[2] = Offsets[2];
    dptr->XYZOffsets2[0] = Offsets[3];
    dptr->XYZOffsets2[1] = Offsets[4];
    dptr->XYZOffsets2[2] = Offsets[5];
    dptr->XYZOffsets3[0] = Offsets[6];
    dptr->XYZOffsets3[1] = Offsets[7];
    dptr->XYZOffsets3[2] = Offsets[8];
    if (DebrisFlags & 1) {
        dptr->YRot = GetRandomControl() << 1;
        dptr->XRot = dptr->YRot;
    } else {
        dptr->YRot = 0;
        dptr->XRot = 0;
    }
    dptr->TextInfo = TextInfo;
    dptr->r = (rgb & 0x1F) << 3;
    dptr->g = ((rgb >> 5) & 0x1F) << 3;
    dptr->b = ((rgb >> 10) & 0x1F) << 3;
    if (DebrisMesh->prelight) {
        dptr->color1 = DebrisMesh->prelight[DebrisMeshC1];
        dptr->color2 = DebrisMesh->prelight[DebrisMeshC2];
        dptr->color3 = DebrisMesh->prelight[DebrisMeshC3];
        dptr->ambient = 0;
    } else {
        dptr->color1 = 0;
        dptr->color2 = 0;
        dptr->color3 = 0;
        dptr->ambient = DebrisMeshAmbient;
    }
    dptr->flags = DebrisMeshFlags;
}
