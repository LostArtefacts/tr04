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
#include "switch.h"
#include "newinv.h"

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

void PoleCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint16 roty;

    item = &items[item_num];
    if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH ||
        lara.IsMoving && lara.GeneralPtr == (void *)item_num) {
        roty = item->pos.y_rot;
        item->pos.y_rot = l->pos.y_rot;
        if (TestLaraPosition(PoleBounds, item, l)) {
            if (MoveLaraPosition(&PolePos, item, l)) {
                l->anim_number = ANIM_STAT2POLE;
                l->frame_number = anims[ANIM_STAT2POLE].frame_base;
                l->current_anim_state = AS_POLESTAT;
                lara.IsMoving = 0;
                lara.gun_status = LG_HANDS_BUSY;
            } else
                lara.GeneralPtr = (void *)item_num;
        } else {
            if (lara.IsMoving && lara.GeneralPtr == (void *)item_num) {
                lara.IsMoving = 0;
                lara.gun_status = LG_NO_ARMS;
            }
        }
        item->pos.y_rot = roty;
    } else if (input & IN_ACTION && lara.gun_status == LG_NO_ARMS && l->gravity_status && l->fallspeed > 0 && (l->current_anim_state == AS_REACH || l->current_anim_state == AS_UPJUMP)) {
        if (TestBoundsCollide(item, l, 100) && TestCollision(item, l)) {
            roty = item->pos.y_rot;
            item->pos.y_rot = l->pos.y_rot;
            if (l->current_anim_state == AS_REACH) {
                PolePosR.y = l->pos.y_pos - item->pos.y_pos + 10;
                AlignLaraPosition(&PolePosR, item, l);
                l->anim_number = ANIM_REACH2POLE;
                l->frame_number = anims[ANIM_REACH2POLE].frame_base;
            } else {
                PolePosR.y = l->pos.y_pos - item->pos.y_pos + 66;
                AlignLaraPosition(&PolePosR, item, l);
                l->anim_number = ANIM_JUMP2POLE;
                l->frame_number = anims[ANIM_JUMP2POLE].frame_base;
            }
            l->gravity_status = 0;
            l->fallspeed = 0;
            l->current_anim_state = AS_POLESTAT;
            lara.gun_status = LG_HANDS_BUSY;
            item->pos.y_rot = roty;
        }
    } else {
        if ((l->current_anim_state < AS_POLESTAT || l->current_anim_state > AS_POLERIGHT) && l->current_anim_state != AS_BACKJUMP)
            ObjectCollision(item_num, l, coll);
    }
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

void TriggerRopeFlame(PHD_VECTOR *pos)
{
    SPARKS *sptr;
    sint32 size;

    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    sptr->sR = 255;
    sptr->sG = (GetRandomControl() & 0x1F) + 48;
    sptr->sB = 48;
    sptr->dR = (GetRandomControl() & 0x3F) - 64;
    sptr->dG = (GetRandomControl() & 0x3F) + 0x80;
    sptr->dB = 32;
    sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
    sptr->FadeToBlack = 4;
    sptr->sLife = sptr->Life = (GetRandomControl() & 3) + 24;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0xF) + pos->x - 8;
    sptr->y = pos->y;
    sptr->z = (GetRandomControl() & 0xF) + pos->z - 8;
    sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
    sptr->Friction = 5;
    sptr->Flags = 538;
    if (!(GetRandomControl() & 3))
        sptr->Flags |= 0x20;
    sptr->RotAng = GetRandomControl() & 0xFFF;
    if (GetRandomControl() & 1)
        sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
    else
        sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
    if (GetRandomControl() & 0xF) {
        sptr->Yvel = -24 - (GetRandomControl() & 0xF);
        sptr->Gravity = -24 - (GetRandomControl() & 0x1F);
        sptr->MaxYvel = -16 - (GetRandomControl() & 7);
    } else {
        sptr->Yvel = (GetRandomControl() & 0xF) + 24;
        sptr->Gravity = (GetRandomControl() & 0x1F) + 24;
        sptr->MaxYvel = 0;
    }
    sptr->Scalar = 2;
    size = (GetRandomControl() & 0xF) + 96;
    sptr->Size = sptr->sSize = size;
    sptr->dSize = size >> 3;
}

void BurningRopeCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    SPHERE *sphere;
    PHD_VECTOR pos;
    sint32 nSpheres, dx, dy, dz, i;

    item = &items[item_number];
    if (item->trigger_flags || !lara.LitTorch)
        return;
    nSpheres = GetSpheres(item, Slist, 1);
    dx = item->pos.x_pos - lara_item->pos.x_pos;
    dy = item->pos.y_pos - lara_item->pos.y_pos;
    dz = item->pos.z_pos - lara_item->pos.z_pos;
    if (dx > 0x1000 || dx < -0x1000 || dy > 0x800 || dy < -0x800 || dz > 0x1000 || dz < -0x1000)
        return;
    pos.x = -32;
    pos.y = 64;
    pos.z = 256;
    GetLaraJointPos(&pos, 14);
    for (i = 0; i < nSpheres; i++) {
        sphere = &Slist[i];
        if (abs(sphere->x - pos.x) < sphere->r && abs(sphere->y - pos.y) < sphere->r && abs(sphere->z - pos.z) < sphere->r) {
            if (gfCurrentLevel == 27) {
                SoundEffect(SFX_BOULDER_FALL, 0, SFX_DEFAULT);
                TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
            }
            item->trigger_flags = 1;
            item->item_flags[0] = item->item_flags[1] = i << 1;
            item->item_flags[2] = i;
            item->item_flags[3] = 150;
            AddActiveItem(item_number);
            item->status = ITEM_ACTIVE;
            item->flags |= IFL_CODEBITS;
            return;
        }
    }
}

void ControlBurningRope(sint16 item_number)
{
    ITEM_INFO *item;
    SPHERE *sphere;
    PHD_VECTOR pos;
    sint32 passes, i, nmeshes;

    item = &items[item_number];
    if (!TriggerActive(item))
        return;
    nmeshes = objects[item->object_number].nmeshes - 1;
    if (!(GlobalCounter & 3)) {
        GetSpheres(item, Slist, 1);
        sphere = &Slist[item->item_flags[0] >> 1];
        for (i = item->item_flags[0]; i < item->item_flags[1]; i++) {
            if (item->mesh_bits & (1 << (i >> 1))) {
                if (i & 1 && GlobalCounter & 4) {
                    pos.x = (sphere->x + sphere[1].x) >> 1;
                    pos.y = (sphere->y + sphere[1].y) >> 1;
                    pos.z = (sphere->z + sphere[1].z) >> 1;
                    TriggerRopeFlame(&pos);
                } else if (!(GlobalCounter & 4)) {
                    TriggerRopeFlame((PHD_VECTOR *)sphere);
                }
            }
            if (i & 1)
                sphere++;
        }
        if (item->item_flags[3]) {
            if (item->item_flags[0] > 1)
                item->item_flags[0]--;
            if (item->item_flags[1] < nmeshes << 1)
                item->item_flags[1]++;
        }
    }
    DebrisFlags = 1;
    if (item->item_flags[3]) {
        item->item_flags[3]--;
        if (!item->item_flags[3]) {
            item->item_flags[1] = item->item_flags[2];
            item->item_flags[0] = item->item_flags[2];
            ExplodeItemNode(item, item->item_flags[2], 0, 256);
        }
    } else if (GlobalCounter & 1) {
        passes = 0;
        if (item->item_flags[0] > 0) {
            item->item_flags[0]--;
            ExplodeItemNode(item, item->item_flags[0], 0, 256);
        } else
            passes++;
        if (item->item_flags[1] < nmeshes) {
            item->item_flags[1]++;
            ExplodeItemNode(item, item->item_flags[1], 0, 256);
        } else
            passes++;
        if (passes == 2) {
            if (gfCurrentLevel != 27)
                TestTriggersAtXYZ(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 1, 0);
            KillItem(item_number);
        }
    }
    DebrisFlags = 0;
}

void StatuePlinthCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    FLOOR_INFO *floor;
    sint16 *bounds;
    sint16 room_number, y_rot;

    item = &items[item_number];
    if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && !l->gravity_status && lara.gun_status == LG_NO_ARMS && !item->trigger_flags && !item->item_flags[0] && !item->item_flags[1]) {
        bounds = GetBoundsAccurate(item);
        StatuePlinthBounds[0] = bounds[0];
        StatuePlinthBounds[1] = bounds[1];
        StatuePlinthBounds[4] = bounds[4] - 200;
        StatuePlinthBounds[5] = bounds[4] + 200;
        y_rot = item->pos.y_rot;
        item->pos.y_rot = l->pos.y_rot;
        if (TestLaraPosition(StatuePlinthBounds, item, l) && have_i_got_object(PUZZLE_ITEM5)) {
            GLOBAL_enterinventory = PUZZLE_ITEM5;
            item->item_flags[1] = 1;
        }
        item->pos.y_rot = y_rot;
    } else if (item->item_flags[1] && GLOBAL_inventoryitemchosen == PUZZLE_ITEM5) {
        l->anim_number = ANIM_PLINTHHI;
        l->frame_number = anims[ANIM_PLINTHHI].frame_base;
        l->current_anim_state = AS_CONTROLLED;
    } else if (l->anim_number == ANIM_PLINTHHI && l->frame_number == anims[ANIM_PLINTHHI].frame_base + 45) {
        room_number = item->room_number;
        floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
        GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
        TestTriggers(trigger_index, 1, item->flags & 0x3E00);
        item->mesh_bits = 255;
        item->item_flags[0] = 1;
        lara.puzzleitems[4]--;
    } else {
        item->item_flags[1] = 0;
        ObjectCollision(item_number, l, coll);
    }
}
