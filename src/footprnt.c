#include "footprnt.h"
#include "control.h"
#include "delstuff.h"
#include "lara.h"
#include "sound.h"

void AddFootprint(ITEM_INFO *item)
{
    FOOTPRINT *print;
    FLOOR_INFO *floor;
    PHD_VECTOR pos;
    sint16 room_num;

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    if (FXType == SFX_LANDONLY)
        GetLaraJointPos(&pos, LM_LFOOT);
    else
        GetLaraJointPos(&pos, LM_RFOOT);
    room_num = item->room_number;
    floor = GetFloor(pos.x, pos.y, pos.z, &room_num);
    if (floor->fx != 6 && floor->fx != 5 && floor->fx != 11)
        SoundEffect(footsounds[floor->fx] + SFX_FOOTSTEPS_MUD, &lara_item->pos, SFX_DEFAULT);
    if (floor->fx < 3 && !OnObject) {
        print = &FootPrint[FootPrintNum];
        print->x = pos.x;
        print->y = GetHeight(floor, pos.x, pos.y, pos.z);
        print->z = pos.z;
        print->YRot = item->pos.y_rot;
        print->Active = 512;
        FootPrintNum = (FootPrintNum + 1) & 0x1F;
    }
}
