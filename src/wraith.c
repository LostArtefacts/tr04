#include "wraith.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "effect2.h"
#include "lara.h"
#include "objnums.h"
#include "room.h"
#include "senet.h"
#include "sound.h"
#include "traps.h"
#include "specific/3dmath.h"
#include "specific/function_stubs.h"

void InitialiseWraith(sint16 item_number)
{
    ITEM_INFO *item;
    WRAITH_STRUCT *data;
    sint32 i;

    item = &items[item_number];
    data = (WRAITH_STRUCT *)game_malloc(8 * sizeof(WRAITH_STRUCT));
    item->data = data;
    item->item_flags[0] = 0;
    item->hit_points = 0;
    item->speed = WraithSpeed;
    for (i = 0; i < 8; i++) {
        data->pos.x = item->pos.x_pos;
        data->pos.y = item->pos.y_pos;
        data->pos.z = item->pos.z_pos;
        data->zv = 0;
        data->yv = 0;
        data->xv = 0;
        data->b = 0;
        data->g = 0;
        data->r = 0;
        data++;
    }
}

void TriggerWraithFlame(sint32 x, sint32 y, sint32 z, sint16 xv, sint16 yv, sint16 zv, sint32 objnum)
{
    SPARKS *sptr;
    sint32 size;
    uint8 col;

    sptr = &spark[GetFreeSpark()];
    sptr->On = 1;
    if (objnum == WRAITH1) {
        sptr->sR = (GetRandomControl() & 0x1F) + 128;
        sptr->sG = (GetRandomControl() & 0x1F) + 48;
        sptr->sB = 24;
        sptr->dR = (GetRandomControl() & 0x1F) + 128;
        sptr->dG = (GetRandomControl() & 0x1F) + 64;
        sptr->dB = 24;
    } else if (objnum == WRAITH2) {
        sptr->sB = (GetRandomControl() & 0x1F) + 128;
        sptr->sG = (GetRandomControl() & 0x1F) + 48;
        sptr->sR = 24;
        sptr->dB = (GetRandomControl() & 0x1F) + 128;
        sptr->dG = (GetRandomControl() & 0x1F) + 64;
        sptr->dR = 24;
    } else {
        col = (GetRandomControl() & 0xF) + 64;
        sptr->sR = sptr->sG = sptr->sB = sptr->dR = sptr->dG = col;
        sptr->dB = (GetRandomControl() & 0xF) + sptr->sB;
    }
    sptr->ColFadeSpeed = 4;
    sptr->FadeToBlack = 7;
    sptr->sLife = sptr->Life = (GetRandomControl() & 7) + 12;
    sptr->TransType = 2;
    sptr->x = (GetRandomControl() & 0x1F) + x - 16;
    sptr->y = y;
    sptr->z = (GetRandomControl() & 0x1F) + z - 16;
    sptr->Friction = 85;
    sptr->Flags = 522;
    sptr->Xvel = xv;
    sptr->Yvel = yv;
    sptr->Zvel = zv;
    sptr->Gravity = 0;
    sptr->MaxYvel = 0;
    sptr->Scalar = 2;
    size = (GetRandomControl() & 0x1F) + 48;
    sptr->dSize = 2;
    sptr->Size = sptr->sSize = size;
}

void WraithControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    ITEM_INFO *target;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    WRAITH_STRUCT *wraith;
    sint32 x, y, z, dx, dy, dz, dist, h;
    sint16 rotY, rotX, xrot, speed, room_number, nr, oob, i;

    item = &items[item_number];
    SoundEffect(184, &item->pos, 0);
    if (item->hit_points)
        target = &items[item->hit_points];
    else
        target = lara_item;
    if (target != lara_item && target->object_number != ANIMATING10) {
        r = &room[lara_item->room_number];
        dx = r->x + (r->y_size << 9) - item->pos.x_pos;
        dz = r->z + (r->x_size << 9) - item->pos.z_pos;
        dist = SQUARE(dx) + SQUARE(dz);
        dy = abs((dist >> 13) - 768);
        y = r->y + ((r->minfloor - r->maxceiling) >> 1);
    } else {
        dx = target->pos.x_pos - item->pos.x_pos;
        y = target->pos.y_pos;
        dz = target->pos.z_pos - item->pos.z_pos;
        dist = SQUARE(dx) + SQUARE(dz);
        dy = abs((dist >> 13) - 512);
    }
    y = y - item->pos.y_pos - dy - 128;
    rotY = (sint16)phd_atan(dz, dx) - item->pos.y_rot;
    xrot = item->pos.x_rot;
    if (abs(dx) > abs(dz))
        rotX = (sint16)phd_atan(abs(dx) + (abs(dz) >> 1), y);
    else
        rotX = (sint16)phd_atan(abs(dz) + (abs(dx) >> 1), y);
    rotX -= xrot;
    speed = (WraithSpeed / item->speed) << 3;
    if (abs(rotY) < item->item_flags[2] && (rotY > 0) == (item->item_flags[2] > 0))
        item->pos.y_rot += rotY;
    else if (rotY < 0) {
        if (item->item_flags[2] < 0) {
            item->item_flags[2] -= speed;
            item->pos.y_rot += item->item_flags[2];
        } else
            item->item_flags[2] = -1;
    } else if (item->item_flags[2] > 0) {
        item->item_flags[2] += speed;
        item->pos.y_rot += item->item_flags[2];
    } else
        item->item_flags[2] = 1;
    if (abs(rotX) < item->item_flags[3] && (rotX > 0) == (item->item_flags[3] > 0))
        item->pos.x_rot = xrot + rotX;
    else if (rotX < 0) {
        if (item->item_flags[3] < 0) {
            item->item_flags[3] -= speed;
            item->pos.x_rot = xrot + item->item_flags[3];
        } else
            item->item_flags[3] = -1;
    } else if (item->item_flags[3] > 0) {
        item->item_flags[3] += speed;
        item->pos.x_rot = xrot + item->item_flags[3];
    } else
        item->item_flags[3] = 1;
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    oob = h < item->pos.y_pos || GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) > item->pos.y_pos;
    x = item->pos.x_pos;
    y = item->pos.y_pos;
    z = item->pos.z_pos;
    item->pos.x_pos += ((rcossin_tbl[(item->pos.y_rot >> 3) & 0x1FFE] << 2) * item->speed) >> W2V_SHIFT;
    item->pos.y_pos += ((rcossin_tbl[(item->pos.x_rot >> 3) & 0x1FFE] << 2) * item->speed) >> W2V_SHIFT;
    item->pos.z_pos += ((rcossin_tbl[((item->pos.y_rot >> 3) & 0x1FFE) + 1] << 2) * item->speed) >> W2V_SHIFT;
    IsRoomOutsideNo = 255;
    IsRoomOutside(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    nr = IsRoomOutsideNo;
    if (item->room_number != nr && nr != 255) {
        ItemNewRoom(item_number, nr);
        for (i = room[nr].item_number; i != NO_ITEM; i = item2->next_item) {
            item2 = &items[i];
            if (item2->active) {
                if ((item->object_number == WRAITH1 && item2->object_number == WRAITH2) ||
                    (item->object_number == WRAITH2 && item2->object_number == WRAITH1) ||
                    (item->object_number == WRAITH3 && item2->object_number == ANIMATING10)) {
                    item->hit_points = i;
                    item2->hit_points = item_number;
                    break;
                }
            }
        }
    }
    if (item->object_number != WRAITH3) {
        if (room[item->room_number].flags & ROOM_UNDERWATER) {
            TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -2, 1, item->room_number);
            if (item->item_flags[1]-- > -1)
                item->item_flags[1] = -1;
            else if (item->item_flags[1] < 30) {
                if (item->object_number == WRAITH2 && item->trigger_flags && !flip_stats[item->trigger_flags]) {
                    FlipMap(item->trigger_flags);
                    flip_stats[item->trigger_flags] = 1;
                }
                KillItem(item_number);
            }
        } else {
            if (item->item_flags[1]-- < 1)
                item->item_flags[1] = 0;
        }
    }
    if (dist < 28900 && abs(item->pos.y_pos - target->pos.y_pos + 384) < 256) {
        if (item->speed > 32)
            item->speed -= 12;
        if (target == lara_item) {
            target->hit_points -= (sint16)(dist >> 10);
            if (item->object_number == WRAITH1) {
                item->item_flags[1] += 400;
                if (item->item_flags[1] > 8000)
                    LaraBurn();
            }
        } else if (target->object_number == ANIMATING10) {
            item->ai_bits++;
            if (item->ai_bits > 25) {
                item->pos.x_pos = target->pos.x_pos;
                item->pos.y_pos = target->pos.y_pos - 384;
                item->pos.z_pos = target->pos.z_pos;
                ShockwaveExplosion(item, 0x606060, -32);
                ShockwaveExplosion(item, 0x303030, 48);
                target->hit_points = 0;
                target->trigger_flags--;
                if (target->trigger_flags > 0)
                    target->frame_number = anims[target->anim_number].frame_base;
                KillItem(item_number);
            }
        } else {
            target->ai_bits = 10;
            if (item->ai_bits > 0) {
                TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -2, 1, item->room_number);
                target->hit_points = 0;
                KillItem(item->hit_points);
                KillItem(item_number);
            }
        }
    } else if (wibble & 0x10) {
        if (item->speed < WraithSpeed)
            item->speed++;
        if (item->hit_points && target->ai_bits > 0)
            target->ai_bits--;
    }
    room_number = item->room_number;
    floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    if (h >= item->pos.y_pos && GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) <= item->pos.y_pos) {
        if (oob == 1)
            TriggerWraithEffect(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->pos.y_rot, item->object_number);
    } else if (!oob)
        TriggerWraithEffect(x, y, z, item->pos.y_rot + 0x8000, item->object_number);
    wraith = (WRAITH_STRUCT *)item->data + 7;
    for (i = 0; i < 7; i++, wraith--) {
        wraith[-1].pos.x += wraith[-1].xv >> 4;
        wraith[-1].pos.y += wraith[-1].yv >> 4;
        wraith[-1].pos.z += wraith[-1].zv >> 4;
        wraith[-1].xv -= wraith[-1].xv >> 4;
        wraith[-1].yv -= wraith[-1].yv >> 4;
        wraith[-1].zv -= wraith[-1].zv >> 4;
        wraith->pos.x = wraith[-1].pos.x;
        wraith->pos.y = wraith[-1].pos.y;
        wraith->pos.z = wraith[-1].pos.z;
        wraith->xv = wraith[-1].xv;
        wraith->yv = wraith[-1].yv;
        wraith->zv = wraith[-1].zv;
        if (item->object_number == WRAITH1) {
            wraith->r = (GetRandomControl() & 0x3F) - 64;
            wraith->g = (uint8)(16 * (i + 1) + (GetRandomControl() & 0x3F));
            wraith->b = GetRandomControl() & 0xF;
        } else if (item->object_number == WRAITH2) {
            wraith->b = (GetRandomControl() & 0x3F) - 64;
            wraith->g = (uint8)(16 * (i + 1) + (GetRandomControl() & 0x3F));
            wraith->r = GetRandomControl() & 0xF;
        } else {
            wraith->r = wraith->g = (uint8)(8 * (i + 2) + (GetRandomControl() & 0x3F));
            wraith->b = wraith->r + (GetRandomControl() & 0xF);
        }
    }
    wraith->pos.x = item->pos.x_pos;
    wraith->pos.y = item->pos.y_pos;
    wraith->pos.z = item->pos.z_pos;
    wraith->xv = dx = (sint16)((item->pos.x_pos - x) << 2);
    wraith->yv = dy = (sint16)((item->pos.y_pos - y) << 2);
    wraith->zv = dz = (sint16)((item->pos.z_pos - z) << 2);
    TriggerWraithFlame(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, dx, dy, dz, item->object_number);
    TriggerWraithFlame((item->pos.x_pos + x) >> 1, (item->pos.y_pos + y) >> 1, (item->pos.z_pos + z) >> 1, dx, dy, dz, item->object_number);
    if (item->object_number == WRAITH3)
        TriggerDynamic(wraith->pos.x, wraith->pos.y, wraith->pos.z, 16, wraith[5].r, wraith[5].g, wraith[5].b);
    else
        TriggerDynamic(wraith->pos.x, wraith->pos.y, wraith->pos.z, 16, wraith[1].r, wraith[1].g, wraith[1].b);
}
