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

void GameStixControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    ITEM_INFO *piece;
    sint32 x, z;
    sint16 i, num, change;
    sint16 piece_num, room_number;

    item = &items[item_number];
    if (item->trigger_flags > -1) {
        if (item->hit_points == 100)
            SoundEffect(SFX_SPINNING_PUZZLE, &item->pos, SFX_DEFAULT);
        for (i = 0; i < 4; i++) {
            if (item->hit_points < 100 - 2 * i) {
                item->item_flags[i] -= item->hit_points << 7;
                if (item->hit_points < 40 - 2 * i) {
                    if (item->item_flags[i] > -4096 && item->item_flags[i] < 4096 && item->trigger_flags & 1 << i)
                        item->item_flags[i] = 0;
                    else if ((item->item_flags[i] > 28672 || item->item_flags[i] < -28672) && !(item->trigger_flags & 1 << i))
                        item->item_flags[i] = -0x8000;
                }
            }
        }
        item->hit_points--;
        if (!item->hit_points) {
            for (i = 0; i < 3; i++)
                items[senet_item[i]].trigger_flags = 0;
            item->trigger_flags = -1;
            if (piece_moving == -1 && !last_throw) {
                RemoveActiveItem(item_number);
                item->status = ITEM_INACTIVE;
            }
        }
        return;
    }
    if (piece_moving > -1) {
        num = (piece_moving >= 3) + 1;
        piece = &items[senet_item[piece_moving]];
        piece->flags |= IFL_TRIGGERED;
        piece->after_death = 48;
        piece_num = senet_piece[piece_moving];
        if (piece_num == -1 || piece_num >= 5) {
            if (piece_num == -1)
                piece_num = 16;
            x = SenetTargetX + 1024;
            z = SenetTargetZ + ((piece_num - 5) << 10);
        } else {
            x = SenetTargetX + (num << 11) - 2048;
            z = SenetTargetZ + ((4 - piece_num) << 10);
        }
        if (abs(x - piece->pos.x_pos) < 128)
            piece->pos.x_pos = x;
        else if (x > piece->pos.x_pos)
            piece->pos.x_pos += 128;
        else
            piece->pos.x_pos -= 128;
        if (abs(z - piece->pos.z_pos) < 128)
            piece->pos.z_pos = z;
        else if (z > piece->pos.z_pos)
            piece->pos.z_pos += 128;
        else
            piece->pos.z_pos -= 128;
        room_number = piece->room_number;
        GetFloor(piece->pos.x_pos, piece->pos.y_pos - 32, piece->pos.z_pos, &room_number);
        if (piece->room_number != room_number)
            ItemNewRoom(senet_item[piece_moving], room_number);
        if (x == piece->pos.x_pos && z == piece->pos.z_pos) {
            piece->after_death = 0;
            if (piece_num == 16) {
                if (num == 1) {
                    ShockwaveExplosion(piece, 0x6060E0, -32);
                    ShockwaveExplosion(piece, 0x6060E0, 48);
                } else {
                    ShockwaveExplosion(piece, 0xFF8020, -32);
                    ShockwaveExplosion(piece, 0xFF8020, 48);
                }
                KillItem(senet_item[piece_moving]);
                if (CheckSenetWinner(num)) {
                    for (i = 0; i < level_items; i++) {
                        item2 = &items[i];
                        if (item2->object_number >= GAME_PIECE1 && item2->object_number <= WHEEL_OF_FORTUNE) {
                            item2->flags |= IFL_INVISIBLE | IFL_CODEBITS;
                            RemoveActiveItem(i);
                            item2->status = ITEM_INACTIVE;
                            item2->after_death = 1;
                        }
                    }
                }
            } else {
                for (i = 0; i < 6; i++) {
                    if (piece_moving != i) {
                        piece = &items[senet_item[i]];
                        if (x == piece->pos.x_pos && z == piece->pos.z_pos) {
                            if (num == 1)
                                ShockwaveExplosion(piece, 0xFF8020, -64);
                            else
                                ShockwaveExplosion(piece, 0x6060E0, -64);
                            piece->pos.x_pos = SenetTargetX - (num << 12) + 7168;
                            piece->pos.z_pos = SenetTargetZ + ((i % 3) << 10);
                            room_number = piece->room_number;
                            GetFloor(piece->pos.x_pos, piece->pos.y_pos - 32, piece->pos.z_pos, &room_number);
                            if (piece->room_number != room_number)
                                ItemNewRoom(senet_item[i], room_number);
                            if (num == 1)
                                ShockwaveExplosion(piece, 0xFF8020, -64);
                            else
                                ShockwaveExplosion(piece, 0x6060E0, -64);
                        }
                    }
                }
            }
            if (!last_throw) {
                RemoveActiveItem(item_number);
                piece->status = ITEM_INACTIVE;
            }
            piece_moving = -1;
        }
        return;
    }
    if (last_throw == -1) {
        ThrowSticks(item);
        for (i = 3; i < 6; i++) {
            MakeMove(i, last_throw);
            if (last_throw == -1 || !last_throw)
                break;
        }
        if (!last_throw || last_throw == 6)
            last_throw = -1;
        else
            last_throw = 0;
    } else if (!last_throw) {
        ThrowSticks(item);
        change = 0;
        for (i = 0; i < 3; i++) {
            if (senet_piece[i] != -1 && last_throw && senet_piece[i] + last_throw < 17 && !(senet_board[senet_piece[i] + last_throw] & 1))
                change = 1;
        }
        if (!change) {
            if (last_throw == 6)
                last_throw = 0;
            else
                last_throw = -1;
        }
    }
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
