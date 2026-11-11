#include "objects.h"
#include "room.h"
#include "control.h"
#include "lara.h"
#include "box.h"
#include "anim.h"
#include "sound.h"
#include "effects.h"
#include "tomb4fx.h"
#include "sphere.h"
#include "specific/3dmath.h"
#include "effect2.h"
#include "debris.h"
#include "gameflow.h"
#include "lara_states.h"
#include "specific/input.h"

void SmashObject(sint16 item_number)
{
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    ITEM_INFO *item;

    item = &items[item_number];
    r = &room[item->room_number];
    floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + ((item->pos.x_pos - r->x) >> WALL_SHIFT) * r->x_size];
    if (boxes[floor->box].overlap_index & 0x8000)
        boxes[floor->box].overlap_index &= ~0x4000;
    SoundEffect(SFX_EXPLOSION1, &item->pos, SFX_DEFAULT);
    SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
    item->mesh_bits = 0xFFFE;
    item->collidable = 0;
    ExplodingDeath2(item_number, -1, 256);
    item->flags |= IFL_INVISIBLE;
    if (item->status == ITEM_ACTIVE)
        RemoveActiveItem(item_number);
    item->status = ITEM_DEACTIVATED;
}

void SmashObjectControl(sint16 item_number)
{
    ITEM_INFO *item;
    sint32 speed;

    item = &items[item_number];
    if (item->flags & IFL_INVISIBLE)
        return;
    if (lara.vehicle == NO_ITEM) {
        if (!item->touch_bits)
            return;
        item->touch_bits = 0;
        speed = ((rcossin_tbl[(((sint16)(lara_item->pos.y_rot - item->pos.y_rot) >> 3) & 0x1FFE) + 1] << 2) * lara_item->speed) >> W2V_SHIFT;
        if (ABS(speed) < 50)
            return;
    } else if (!ItemNearLara(&item->pos, 512)) {
        return;
    }
    SmashObject(item_number);
}

void BridgeFlatFloor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    if (y <= item->pos.y_pos) {
        *height = item->pos.y_pos;
        height_type = WALL;
        OnObject = 1;
    }
}

void BridgeFlatCeiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    if (y > item->pos.y_pos)
        *height = item->pos.y_pos + STEP_L;
}

sint32 GetOffset(ITEM_INFO *item, sint32 x, sint32 z)
{
    if (item->pos.y_rot == 0)
        return (WALL_L - x) & (WALL_L - 1);
    else if (item->pos.y_rot == -0x8000)
        return x & (WALL_L - 1);
    else if (item->pos.y_rot == 0x4000)
        return z & (WALL_L - 1);
    return (WALL_L - z) & (WALL_L - 1);
}

void BridgeTilt1Floor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);
    if (y <= level) {
        *height = level;
        height_type = WALL;
        OnObject = 1;
    }
}

void BridgeTilt1Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);
    if (y > level)
        *height = level + STEP_L;
}

void BridgeTilt2Floor(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);
    if (y <= level) {
        *height = level;
        height_type = WALL;
        OnObject = 1;
    }
}

void BridgeTilt2Ceiling(ITEM_INFO *item, sint32 x, sint32 y, sint32 z, sint32 *height)
{
    sint32 level;

    level = item->pos.y_pos + (GetOffset(item, x, z) >> 1);
    if (y > level)
        *height = level + STEP_L;
}

void ControlAnimatingSlots(sint16 item_number)
{
    ITEM_INFO *item;
    PHD_VECTOR pos;

    item = &items[item_number];
    if (TriggerActive(item)) {
        item->status = ITEM_ACTIVE;
        AnimateItem(item);
        if (item->trigger_flags == 666) {
            pos.x = pos.y = pos.z = 0;
            GetJointAbsPosition(item, &pos, 0);
            SoundEffect(SFX_HELICOPTER_LOOP, (PHD_3DPOS *)&pos, SFX_DEFAULT);
            if (item->frame_number == anims[item->anim_number].frame_end)
                item->flags &= ~IFL_CODEBITS;
        }
    } else if (item->trigger_flags == 2)
        item->status = ITEM_INVISIBLE;
}

void ControlTriggerTriggerer(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint16 *data;

    item = &items[item_number];
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &item->room_number);
    GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    data = trigger_index;
    if (data) {
        if ((*data & 0x1F) == LAVA_TYPE) {
            if (*data & 0x8000)
                return;
            data++;
        }
        if ((*data & 0x1F) == CLIMB_TYPE) {
            if (*data & 0x8000)
                return;
            data++;
        }
        if ((*data & 0x1F) == MONKEY_TYPE) {
            if (*data & 0x8000)
                return;
            data++;
        }
        if ((*data & 0x1F) == TRIGTRIGGER_TYPE) {
            if (TriggerActive(item))
                *data |= IFL_TRIGGERED;
            else
                *data &= ~IFL_TRIGGERED;
        }
    }
}

void ControlWaterfall(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (!TriggerActive(item)) {
        if (item->trigger_flags == 2 || item->trigger_flags == 668)
            item->status = ITEM_INVISIBLE;
    } else {
        item->status = ITEM_ACTIVE;
        if (item->trigger_flags == 668)
            SoundEffect(SFX_SAND_LOOP, &item->pos, SFX_DEFAULT);
        else if (item->trigger_flags == 777)
            SoundEffect(SFX_WATERFALL_LOOP, &item->pos, SFX_DEFAULT);
    }
}

void ControlMapper(sint16 item_number)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    SPARKS *sptr;
    PHD_VECTOR pos;
    sint32 rg, h, i;
    sint16 room_number;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    if (item->frame_number - anims[item->anim_number].frame_base >= 200) {
        SoundEffect(SFX_MAPPER_LAZER, &item->pos, SFX_DEFAULT);
        item->mesh_bits |= 2;
        pos.x = pos.y = pos.z = 0;
        GetJointAbsPosition(item, &pos, 1);
        rg = (GetRandomControl() & 0x1F) + 192;
        TriggerDynamic(pos.x, pos.y, pos.z, (GetRandomControl() & 3) + 16, rg, rg, 0);
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        for (i = 0; i < 2; i++) {
            sptr = &spark[GetFreeSpark()];
            sptr->On = 1;
            sptr->sR = (GetRandomControl() & 0x7F) + 64;
            sptr->sG = sptr->sR - (GetRandomControl() & 0x1F);
            sptr->sB = 0;
            sptr->dR = (GetRandomControl() & 0x7F) + 64;
            sptr->dG = sptr->dR - (GetRandomControl() & 0x1F);
            sptr->dB = 0;
            sptr->ColFadeSpeed = 4;
            sptr->FadeToBlack = 4;
            sptr->TransType = 2;
            sptr->Life = 12;
            sptr->sLife = 12;
            sptr->x = pos.x;
            sptr->y = h;
            sptr->z = pos.z;
            sptr->Xvel = (GetRandomControl() & 0x3FF) - 512;
            sptr->Yvel = -256 - (GetRandomControl() & 0x7F);
            sptr->Zvel = (GetRandomControl() & 0x3FF) - 512;
            sptr->Friction = 4;
            sptr->Scalar = 2;
            sptr->Size = sptr->sSize = (GetRandomControl() & 0xF) + 16;
            sptr->dSize = (GetRandomControl() & 1) + 3;
            sptr->Gravity = (GetRandomControl() & 0x1F) + 32;
            sptr->MaxYvel = 0;
            sptr->Flags = 10;
        }
    }
    AnimateItem(item);
}
