#include <string.h>

#include "senet.h"
#include "room.h"
#include "objects.h"
#include "items.h"
#include "deltapak.h"
#include "specific/function_stubs.h"
#include "sound.h"
#include "control.h"
#include "lara_states.h"
#include "collide.h"
#include "tomb4fx.h"
#include "draw.h"
#include "specific/3dmath.h"
#include "specific/output.h"
#include "specific/input.h"
#include "lara.h"
#include "objnums.h"
#include "anim.h"
#include "effect2.h"

void InitialiseSenet(sint16 item_number)
{
    ITEM_INFO *item;
    sint16 i;

    if (senet_item[0])
        return;
    memset(senet_piece, 0, 6);
    memset(&senet_board[1], 0, 16);
    senet_board[0] = 3;
    for (i = 0; i < level_items; i++) {
        item = &items[i];
        if (item->object_number == GAME_PIECE1) {
            senet_item[0] = i;
            SenetTargetX = item->pos.x_pos + 1024;
            SenetTargetZ = item->pos.z_pos;
        } else if (item->object_number == GAME_PIECE2) {
            senet_item[1] = i;
        } else if (item->object_number == GAME_PIECE3) {
            senet_item[2] = i;
        } else if (item->object_number == ENEMY_PIECE) {
            senet_item[item->trigger_flags + 3] = i;
        }
    }
}

void SenetControl(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (last_throw > 0 && item->trigger_flags != 1)
        MakeMove(item->object_number - GAME_PIECE1, last_throw);
    RemoveActiveItem(item_number);
}

void MakeMove(sint32 piece, sint32 displacement)
{
    sint32 num;
    sint8 last;
    sint16 pos, i, n;

    pos = senet_piece[piece] + displacement;
    num = (piece >= 3) + 1;
    if (senet_piece[piece] == NO_ITEM || !displacement || pos > 16 || (sint16)(senet_board[pos] & num))
        return;
    senet_board[senet_piece[piece]] &= ~num;
    if (!senet_piece[piece]) {
        n = 3 * num - 3;
        for (i = n; i < n + 3; i++) {
            if (i != piece && !senet_piece[i])
                senet_board[senet_piece[piece]] |= num;
        }
    }
    piece_moving = (sint8)piece;
    senet_piece[piece] = (sint8)pos;
    if (pos > 4) {
        senet_board[pos] = 0;
        n = 3 * (2 - num);
        for (i = n; i < n + 3; i++) {
            if (senet_piece[i] == pos) {
                senet_piece[i] = 0;
                senet_board[0] |= 3 - num;
            }
        }
    }
    if (pos < 16)
        senet_board[pos] |= num;
    else
        senet_piece[piece] = -1;
    if (!(pos & 3)) {
        last_throw = 0;
    } else {
        last = last_throw;
        last_throw = -1;
        if (last == 6)
            last_throw = 0;
    }
}

void InitialiseGameStix(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    item->trigger_flags = NO_ITEM;
    item->data = item->item_flags;
}

void ShockwaveExplosion(ITEM_INFO *item, uint32 col, sint32 speed)
{
    sint32 InnerOuter;

    if (speed < 0)
        InnerOuter = 0x2000280;
    else
        InnerOuter = 0xA00020;
    item->pos.y_pos -= 384;
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0);
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0x2000);
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0x4000);
    TriggerShockwave((PHD_VECTOR *)&item->pos, InnerOuter, speed, col | 0x18000000, 0x6000);
    item->pos.y_pos += 384;
}

void ThrowSticks(ITEM_INFO *item)
{
    sint16 rnd;
    sint16 i;

    i = 0;
    last_throw = 0;
    item->trigger_flags = 0;
    for (; i < 4; i++) {
        rnd = GetRandomControl() & 1;
        last_throw += rnd;
        if (rnd)
            item->trigger_flags |= 1 << i;
    }
    if (!last_throw)
        last_throw = 6;
    item->hit_points = 120;
    for (i = 0; i < 3; i++)
        items[senet_item[i]].trigger_flags = 1;
}

void ControlGodHead(sint16 item_number)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (TriggerActive(item)) {
        switch (item->pos.y_rot) {
        case 0:
            item->pos.z_pos &= ~1023;
            break;
        case 0x4000:
            item->pos.x_pos &= ~1023;
            break;
        case -0x8000:
            item->pos.z_pos |= 1023;
            break;
        case -0x4000:
            item->pos.x_pos |= 1023;
            break;
        }
        if (!item->item_flags[0]) {
            if (item->item_flags[1] < 4096) {
                item->item_flags[1] += 128;
            } else {
                item->item_flags[0] = 1;
                item->item_flags[1] = 4096;
                item->item_flags[2] = 210;
            }
        } else if (item->item_flags[2]) {
            item->item_flags[2]--;
        } else if (item->item_flags[1] >= 128) {
            item->item_flags[1] -= 128;
        } else {
            KillItem(item_number);
        }
    }
}

void DrawGodHead(ITEM_INFO *item)
{
    ROOM_INFO *r;
    PHD_VECTOR scalar;
    sint16 **meshpp;
    sint32 rate, clip, oldAlpha;
    sint16 *frm[2];

    r = &room[item->room_number];
    phd_left = r->left;
    phd_right = r->right;
    phd_top = r->top;
    phd_bottom = r->bottom;
    GetFrames(item, frm, &rate);
    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    phd_RotX(item->pos.x_rot);
    phd_RotZ(item->pos.z_rot);
    phd_RotY(item->pos.y_rot);
    clip = S_GetObjectBounds(frm[0]);
    if (clip) {
        meshpp = &meshes[objects[item->object_number].mesh_index];
        scalar.y = 0x4000;
        scalar.x = 0x4000;
        scalar.z = item->item_flags[1] << 2;
        ScaleCurrentMatrix(&scalar);
        CalculateObjectLighting(item, frm[0]);
        rate = item->item_flags[1] >> 5;
        oldAlpha = GlobalAlpha;
        if (rate >= 128)
            rate = 255;
        else
            rate <<= 1;
        GlobalAlpha = rate << 24;
        phd_PutPolygons(*meshpp, clip);
        GlobalAlpha = oldAlpha;
    }
    phd_left = 0;
    phd_right = phd_winwidth;
    phd_top = 0;
    phd_bottom = phd_winheight;
    phd_PopMatrix();
}

void GameStixCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;

    item = &items[item_number];
    if (input & IN_ACTION && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH && lara.gun_status == LG_NO_ARMS && !item->active ||
        lara.IsMoving && lara.GeneralPtr == (void *)item_number) {
        l->pos.y_rot ^= 0x8000;
        if (TestLaraPosition(GameStixBounds, item, l)) {
            if (MoveLaraPosition(&GameStixPos, item, l)) {
                l->anim_number = ANIM_THROWSTIX;
                l->frame_number = anims[ANIM_THROWSTIX].frame_base;
                l->current_anim_state = AS_CONTROLLED;
                lara.IsMoving = 0;
                lara.head_y_rot = 0;
                lara.head_x_rot = 0;
                lara.torso_y_rot = 0;
                lara.torso_x_rot = 0;
                lara.gun_status = LG_HANDS_BUSY;
                item->status = ITEM_ACTIVE;
                AddActiveItem(item_number);
            } else
                lara.GeneralPtr = (void *)item_number;
        }
        l->pos.y_rot ^= 0x8000;
    } else
        ObjectCollision(item_number, l, coll);
}
