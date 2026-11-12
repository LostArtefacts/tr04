#include <string.h>

#include "draw.h"
#include "specific/3dmath.h"
#include "specific/output.h"
#include "camera.h"
#include "delstuff.h"
#include "deltapak.h"
#include "health.h"
#include "anim.h"
#include "effects.h"
#include "specific/lighting.h"
#include "lara.h"
#include "lara_states.h"
#include "sound.h"
#include "gameflow.h"
#include "specific/polyinsert.h"
#include "specific/specificfx.h"
#include "train.h"
#include "objnums.h"
#include "tomb4fx.h"
#include "effect2.h"
#include "mirror.h"
#include "rope.h"
#include "scarab.h"
#include "croc.h"
#include "specific/function_stubs.h"

#define S_LongMemCpy(d, s, n) memcpy(d, s, (n) * sizeof(sint32))

sint32 DrawPhaseGame(void)
{
    CalcLaraMatrices(0);
    phd_PushUnitMatrix();
    CalcLaraMatrices(1);
    phd_PopMatrix();
    if (GLOBAL_playing_cutseq)
        frigup_lara();
    SetLaraUnderwaterNodes();
    DrawRooms(camera.pos.room_number);
    DrawGameInfo(1);
    S_OutputPolyList();
    camera.number_frames = S_DumpScreen();
    S_AnimateTextures(camera.number_frames);
    return camera.number_frames;
}

void DrawRooms(sint16 CurrentRoom)
{
    ROOM_INFO *r;
    DYNAMIC *dptr;
    sint32 lx, ly, lz, lr, i;

    current_room = CurrentRoom;
    r = &room[CurrentRoom];
    r->test_left = 0;
    phd_left = 0;
    r->test_top = 0;
    phd_top = 0;
    r->test_right = phd_winxmax;
    phd_right = phd_winxmax;
    r->test_bottom = phd_winymax;
    phd_bottom = phd_winymax;
    outside = r->flags & ROOM_OUTSIDE;
    camera_underwater = r->flags & ROOM_UNDERWATER;
    r->bound_active = 2;
    draw_room_list[0] = CurrentRoom;
    room_list_start = 0;
    room_list_end = 1;
    number_draw_rooms = 0;

    if (outside) {
        outside_top = 0;
        outside_left = 0;
        outside_right = phd_winxmax;
        outside_bottom = phd_winymax;
    } else {
        outside_left = phd_winxmax;
        outside_top = phd_winymax;
        outside_bottom = 0;
        outside_right = 0;
    }

    GetRoomBounds();
    InitialiseFogBulbs();
    CreateFXBulbs();

    if (outside) {
        if (objects[HORIZON].loaded) {
            if (BinocularRange)
                AlterFOV(14560 - BinocularRange);

            phd_PushMatrix();
            phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);

            if (gfLevelFlags & GF_LIGHTNING) {
                if (!LightningCount && !LightningRand) {
                    if (!(GetRandomDraw() & 127)) {
                        LightningCount = (GetRandomDraw() & 0x1F) + 16;
                        dLightningRand = (GetRandomDraw() & 0xFF) + 256;
                        LightningSFXDelay = (GetRandomDraw() & 3) + 12;
                    }
                } else {
                    UpdateSkyLightning();
                    if (LightningSFXDelay > -1)
                        LightningSFXDelay--;
                    if (!LightningSFXDelay)
                        SoundEffect(SFX_THUNDER_RUMBLE, 0, SFX_DEFAULT);
                }
            }

            nPolyType = 6;
            phd_PushMatrix();

            if (gfLevelFlags & GF_LAYER1) {
                phd_RotY(32760);
                if (gfLevelFlags & GF_LIGHTNING)
                    DrawFlatSky((((LightningRGB[0] | (44 << 8)) << 8 | LightningRGB[1]) << 8) | LightningRGB[2], SkyPos, -1536, 4);
                else
                    DrawFlatSky(*(uint32 *)&gfLayer1Col, SkyPos, -1536, 4);
            }

            if (gfLevelFlags & GF_LAYER2)
                DrawFlatSky(0xFF000000 | *(uint32 *)&gfLayer2Col, SkyPos2, -1536, 2);

            if (gfLevelFlags & (GF_LAYER1 | GF_LAYER2))
                OutputSky();

            phd_PopMatrix();

            if (gfLevelFlags & GF_HORIZON) {
                phd_PutPolygonSkyMesh(meshes[objects[HORIZON].mesh_index], -1);
                OutputSky();
            }

            phd_PopMatrix();

            if (BinocularRange)
                AlterFOV(7 * (2080 - BinocularRange));
        } else
            outside = -1;
    }

    if (objects[LARA].loaded && !(lara_item->flags & IFL_INVISIBLE)) {
        nPolyType = 4;

        if (lara_item->mesh_bits) {
            DrawLara(lara_item, 0);
            phd_PushMatrix();

            if (lara.right_arm.flash_gun) {
                phd_mxptr[M00] = lara_matrices[132 + M00];
                phd_mxptr[M01] = lara_matrices[132 + M01];
                phd_mxptr[M02] = lara_matrices[132 + M02];
                phd_mxptr[M03] = lara_matrices[132 + M03];
                phd_mxptr[M10] = lara_matrices[132 + M10];
                phd_mxptr[M11] = lara_matrices[132 + M11];
                phd_mxptr[M12] = lara_matrices[132 + M12];
                phd_mxptr[M13] = lara_matrices[132 + M13];
                phd_mxptr[M20] = lara_matrices[132 + M20];
                phd_mxptr[M21] = lara_matrices[132 + M21];
                phd_mxptr[M22] = lara_matrices[132 + M22];
                phd_mxptr[M23] = lara_matrices[132 + M23];
                SetGunFlash(lara.gun_type);
            }

            if (lara.left_arm.flash_gun) {
                phd_mxptr[M00] = lara_matrices[168 + M00];
                phd_mxptr[M01] = lara_matrices[168 + M01];
                phd_mxptr[M02] = lara_matrices[168 + M02];
                phd_mxptr[M03] = lara_matrices[168 + M03];
                phd_mxptr[M10] = lara_matrices[168 + M10];
                phd_mxptr[M11] = lara_matrices[168 + M11];
                phd_mxptr[M12] = lara_matrices[168 + M12];
                phd_mxptr[M13] = lara_matrices[168 + M13];
                phd_mxptr[M20] = lara_matrices[168 + M20];
                phd_mxptr[M21] = lara_matrices[168 + M21];
                phd_mxptr[M22] = lara_matrices[168 + M22];
                phd_mxptr[M23] = lara_matrices[168 + M23];
                SetGunFlash(lara.gun_type);
            }

            phd_PopMatrix();
            DrawGunflashes();
        }

        if (gfLevelFlags & GF_MIRROR && lara_item->room_number == gfMirrorRoom)
            Draw_Mirror_Lara();
    }

    InitDynamicLighting();
    nPolyType = 0;

    dptr = dynamics;
    for (i = MAX_DYNAMICS; i > 0; i--, dptr++) {
        if (dptr->on) {
            if (dptr->x < 0)
                dptr->x = 0;
            if (dptr->z < 0)
                dptr->z = 0;
        }
    }

    for (i = 0; i < number_draw_rooms; i++)
        S_InsertRoom(draw_rooms[i]);

    if (gfLevelFlags & GF_TRAIN)
        DrawTrainFloor();

    DrawGunshells();
    nPolyType = 3;

    if (GLOBAL_playing_cutseq)
        DrawCutSeqActors();

    DrawRopeList();
    S_DrawSparks();
    lx = lara_item->pos.x_pos;
    ly = lara_item->pos.y_pos;
    lz = lara_item->pos.z_pos;
    lr = lara_item->room_number;
    lara_item->pos.x_pos = camera.pos.x;
    lara_item->pos.y_pos = camera.pos.y;
    lara_item->pos.z_pos = camera.pos.z;
    lara_item->room_number = camera.pos.room_number;
    DoUwEffect();
    S_DrawFires();
    S_DrawSmokeSparks();
    S_DrawSplashes();
    DrawBubbles();
    DrawDebris();
    DrawBlood();
    DrawDrips();
    DrawShockwaves();
    DrawScarabs();
    DrawLocusts();
    DrawLightning();
    lara_item->pos.x_pos = lx;
    lara_item->pos.y_pos = ly;
    lara_item->pos.z_pos = lz;
    lara_item->room_number = lr;

    if (gfLevelFlags & GF_LENSFLARE)
        SetUpLensFlare(gfLensFlare.x, gfLensFlare.y - 4096, gfLensFlare.z, 0);

    if (LaserSightActive)
        DrawLaserSightSprite();

    for (i = 0; i < number_draw_rooms; i++)
        PrintObjects(draw_rooms[i]);
}

void S_InsertRoom(sint16 room_number)
{
    ROOM_INFO *r;

    current_room = room_number;
    r = &room[room_number];
    phd_TranslateAbs(0, 0, 0);
    phd_left = r->left;
    phd_right = r->right;
    phd_top = r->top;
    phd_bottom = r->bottom;
    _InsertRoom(r, 1);
}

void PrintObjects(sint16 room_number)
{
    ROOM_INFO *r;
    MESH_INFO *mesh;
    sint32 clip, i;
    ITEM_INFO *item;
    sint32 item_number, fx_number;

    current_room = room_number;
    nPolyType = 1;
    r = &room[room_number];
    r->bound_active = 0;
    phd_PushMatrix();
    phd_TranslateAbs(r->x, r->y, r->z);
    phd_left = r->left;
    phd_right = r->right;
    phd_top = r->top;
    phd_bottom = r->bottom;
    if (gfLevelFlags & GF_TRAIN) {
        phd_left = 0;
        phd_top = 0;
        phd_right = phd_winxmax + 1;
        phd_bottom = phd_winymax + 1;
    }
    mesh = r->mesh;
    for (i = r->num_meshes; i > 0; i--, mesh++) {
        if (mesh->Flags & 1) {
            phd_PushMatrix();
            phd_TranslateAbs(mesh->x, mesh->y, mesh->z);
            phd_RotY(mesh->y_rot);
            clip = S_GetObjectBounds(&static_objects[mesh->static_number].x_minp);
            if (clip) {
                S_CalculateStaticMeshLight(mesh->x, mesh->y, mesh->z, mesh->shade, r);
                phd_PutPolygons(meshes[static_objects[mesh->static_number].mesh_number], clip);
            }
            phd_PopMatrix();
        }
    }
    nPolyType = 2;
    phd_left = 0;
    phd_top = 0;
    phd_right = phd_winxmax + 1;
    phd_bottom = phd_winymax + 1;
    for (item_number = r->item_number; item_number != NO_ITEM; item_number = items[item_number].next_item) {
        item = &items[item_number];
        ClipRoomNum = room_number;
        if (item->status != ITEM_INVISIBLE) {
            if (item->after_death)
                GlobalAlpha = 0xFE000000 * item->after_death;
            if (objects[item->object_number].draw_routine)
                objects[item->object_number].draw_routine(item);
            if (objects[item->object_number].draw_routine_extra)
                objects[item->object_number].draw_routine_extra(item);
            GlobalAlpha = 0xFF000000;
        }
        if (item->after_death < 128 && item->after_death > 0)
            item->after_death++;
        if (item->after_death == 128)
            KillItem(item_number);
    }
    nPolyType = 3;
    for (fx_number = r->fx_number; fx_number != NO_ITEM; fx_number = effects[fx_number].next_fx)
        DrawEffect(fx_number);
    phd_PopMatrix();
    r->left = phd_winxmax;
    r->top = phd_winymax;
    r->right = 0;
    r->bottom = 0;
}

void GetRoomBounds(void)
{
    ROOM_INFO *r;
    sint16 *door;
    sint32 rn, drn;

    while (room_list_start != room_list_end) {
        rn = draw_room_list[room_list_start % 128];
        room_list_start++;
        r = &room[rn];
        r->bound_active -= 2;
        if (r->test_left < r->left)
            r->left = r->test_left;
        if (r->test_top < r->top)
            r->top = r->test_top;
        if (r->test_right > r->right)
            r->right = r->test_right;
        if (r->test_bottom > r->bottom)
            r->bottom = r->test_bottom;
        if (!(r->bound_active & 1)) {
            draw_rooms[number_draw_rooms++] = (sint16)rn;
            r->bound_active |= 1;
            if (r->flags & ROOM_OUTSIDE)
                outside = ROOM_OUTSIDE;
        }
        if (r->flags & ROOM_OUTSIDE) {
            if (r->left < outside_left)
                outside_left = r->left;
            if (r->right > outside_right)
                outside_right = r->right;
            if (r->top < outside_top)
                outside_top = r->top;
            if (r->bottom > outside_bottom)
                outside_bottom = r->bottom;
        }
        phd_PushMatrix();
        phd_TranslateAbs(r->x, r->y, r->z);
        door = r->door;
        if (door) {
            for (drn = *door++; drn > 0; drn--) {
                rn = *door++;
                if (door[0] * (r->x + door[3] - w2v_matrix[M03]) + door[1] * (r->y + door[4] - w2v_matrix[M13]) + door[2] * (r->z + door[5] - w2v_matrix[M23]) < 0)
                    SetRoomBounds(door, rn, r);
                door += 15;
            }
        }
        phd_PopMatrix();
    }
}

void SetRoomBounds(sint16 *door, sint32 rn, ROOM_INFO *actualRoom)
{
    sint32 i, z_behind, z_toofar;
    sint32 left, right, top, bottom;
    sint32 xv, yv, zv, xs, ys;
    sint32 *mptr;
    ROOM_INFO *r;
    PHD_VECTOR *dest, *last;

    r = &room[rn];
    if (r->left <= actualRoom->test_left && r->right >= actualRoom->test_right && r->top <= actualRoom->test_top && r->bottom >= actualRoom->test_bottom)
        return;

    left = actualRoom->test_right;
    right = actualRoom->test_left;
    top = actualRoom->test_bottom;
    bottom = actualRoom->test_top;
    door += 3;
    mptr = phd_mxptr;
    dest = vbufdoor;
    z_behind = z_toofar = 0;
    for (i = 0; i < 4; i++, dest++, door += 3) {
        dest->x = xv = (sint32)(*(mptr + 0) * *door + *(mptr + 1) * *(door + 1) + *(mptr + 2) * *(door + 2) + *(mptr + 3));
        dest->y = yv = (sint32)(*(mptr + 4) * *door + *(mptr + 5) * *(door + 1) + *(mptr + 6) * *(door + 2) + *(mptr + 7));
        dest->z = zv = (sint32)(*(mptr + 8) * *door + *(mptr + 9) * *(door + 1) + *(mptr + 10) * *(door + 2) + *(mptr + 11));
        if (zv > 0) {
            if (zv > phd_zfar)
                z_toofar++;
            zv /= phd_persp;
            if (zv) {
                xs = xv / zv + phd_centerx;
                ys = yv / zv + phd_centery;
            } else {
                xs = (xv >= 0) ? phd_right : phd_left;
                ys = (yv >= 0) ? phd_bottom : phd_top;
            }
            if (xs - 1 < left)
                left = xs - 1;
            if (xs + 1 > right)
                right = xs + 1;
            if (ys - 1 < top)
                top = ys - 1;
            if (ys + 1 > bottom)
                bottom = ys + 1;
        } else
            z_behind++;
    }

    if (z_behind == 4)
        return;
    if (z_toofar == 4 && outside == 0)
        return;

    if (z_behind > 0) {
        dest = vbufdoor;
        last = dest + 3;
        for (i = 0; i < 4; i++, last = dest, dest++) {
            if ((dest->z <= 0) ^ (last->z <= 0)) {
                if (dest->x < 0 && last->x < 0)
                    left = 0;
                else if (dest->x > 0 && last->x > 0)
                    right = phd_winxmax;
                else {
                    left = 0;
                    right = phd_winxmax;
                }
                if (dest->y < 0 && last->y < 0)
                    top = 0;
                else if (dest->y > 0 && last->y > 0)
                    bottom = phd_winymax;
                else {
                    top = 0;
                    bottom = phd_winymax;
                }
            }
        }
    }

    if (left < actualRoom->test_left)
        left = actualRoom->test_left;
    if (right > actualRoom->test_right)
        right = actualRoom->test_right;
    if (top < actualRoom->test_top)
        top = actualRoom->test_top;
    if (bottom > actualRoom->test_bottom)
        bottom = actualRoom->test_bottom;
    if (left >= right || top >= bottom)
        return;

    if (r->bound_active & 2) {
        if (left < r->test_left)
            r->test_left = (sint16)left;
        if (top < r->test_top)
            r->test_top = (sint16)top;
        if (right > r->test_right)
            r->test_right = (sint16)right;
        if (bottom > r->test_bottom)
            r->test_bottom = (sint16)bottom;
    } else {
        draw_room_list[(room_list_end++) % 128] = rn;
        r->bound_active |= 2;
        r->test_left = (sint16)left;
        r->test_right = (sint16)right;
        r->test_top = (sint16)top;
        r->test_bottom = (sint16)bottom;
    }
}

void DrawEffect(sint16 fx_num)
{
    FX_INFO *fx;
    OBJECT_INFO *obj;

    fx = &effects[fx_num];
    obj = &objects[fx->object_number];
    if (obj->draw_routine && obj->loaded) {
        phd_PushMatrix();
        phd_TranslateAbs(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
        if (phd_mxptr[M23] > phd_znear && phd_mxptr[M23] < phd_zfar) {
            phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
            if (obj->nmeshes) {
                nullsub_1(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx->room_number, &duff_item[0].il);
                phd_PutPolygons(meshes[obj->mesh_index], -1);
            } else {
                nullsub_1(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, fx->room_number, &duff_item[0].il);
                phd_PutPolygons(meshes[fx->frame_number], -1);
            }
        }
        phd_PopMatrix();
    }
}

void calc_animating_item_clip_window(ITEM_INFO *item, sint16 *bounds)
{
    ROOM_INFO *r;
    sint16 *door;
    sint32 xMin, xMax, yMin, yMax, zMin, zMax;
    sint32 xMinR, xMaxR, yMinR, yMaxR, zMinR, zMaxR;
    sint32 xMinD, xMaxD, yMinD, yMaxD, zMinD, zMaxD;
    sint16 rotatedBounds[6];
    sint32 nDoors;

    r = &room[ClipRoomNum];
    if (item->object_number >= ANIMATING1 && item->object_number <= ANIMATING16 ||
        item->object_number >= DOOR_TYPE1 && item->object_number <= DOOR_TYPE8) {
        phd_left = r->left;
        phd_right = r->right;
        phd_top = r->top;
        phd_bottom = r->bottom;
        return;
    }
    phd_PushUnitMatrix();
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    phd_mxptr[M03] = phd_mxptr[M13] = phd_mxptr[M23] = 0;
    mRotBoundingBoxNoPersp(bounds, rotatedBounds);
    phd_PopMatrix();
    xMin = item->pos.x_pos + rotatedBounds[0];
    xMax = item->pos.x_pos + rotatedBounds[1];
    yMin = item->pos.y_pos + rotatedBounds[2];
    yMax = item->pos.y_pos + rotatedBounds[3];
    zMin = item->pos.z_pos + rotatedBounds[4];
    zMax = item->pos.z_pos + rotatedBounds[5];
    xMinR = r->x + 1024;
    xMaxR = xMinR + ((r->y_size - 2) << 10);
    yMinR = r->maxceiling;
    yMaxR = r->minfloor;
    zMinR = r->z + 1024;
    zMaxR = zMinR + ((r->x_size - 2) << 10);
    if (xMin >= xMinR && xMax <= xMaxR && yMin >= yMinR && yMax <= yMaxR && zMin >= zMinR && zMax <= zMaxR) {
        phd_left = r->left;
        phd_right = r->right;
        phd_top = r->top;
        phd_bottom = r->bottom;
        return;
    }
    if (camera.pos.room_number != item->room_number && camera.pos.room_number != ClipRoomNum) {
        door = r->door;
        nDoors = *door++;
        for (; nDoors > 0; nDoors--, door += 16) {
            if (door[0] != camera.pos.room_number)
                continue;
            xMinD = xMaxD = door[4];
            yMinD = yMaxD = door[5];
            zMinD = zMaxD = door[6];
            if (door[7] < xMinD)
                xMinD = door[7];
            else if (door[7] > xMaxD)
                xMaxD = door[7];
            if (door[8] < yMinD)
                yMinD = door[8];
            else if (door[8] > yMaxD)
                yMaxD = door[8];
            if (door[9] < zMinD)
                zMinD = door[9];
            else if (door[9] > zMaxD)
                zMaxD = door[9];
            if (door[10] < xMinD)
                xMinD = door[10];
            else if (door[10] > xMaxD)
                xMaxD = door[10];
            if (door[11] < yMinD)
                yMinD = door[11];
            else if (door[11] > yMaxD)
                yMaxD = door[11];
            if (door[12] < zMinD)
                zMinD = door[12];
            else if (door[12] > zMaxD)
                zMaxD = door[12];
            if (door[13] < xMinD)
                xMinD = door[13];
            else if (door[13] > xMaxD)
                xMaxD = door[13];
            if (door[14] < yMinD)
                yMinD = door[14];
            else if (door[14] > yMaxD)
                yMaxD = door[14];
            if (door[15] < zMinD)
                zMinD = door[15];
            else if (door[15] > zMaxD)
                zMaxD = door[15];
            xMinD += xMinR - 1024;
            xMaxD += xMinR - 1024;
            yMinD += yMinR;
            yMaxD += yMinR;
            zMinD += zMinR - 1024;
            zMaxD += zMinR - 1024;
            if (xMinD <= xMax && xMaxD >= xMin && yMinD <= yMax && yMaxD >= yMin && zMinD <= zMax && zMaxD >= zMin)
                break;
        }
        if (!nDoors) {
            phd_left = r->left;
            phd_right = r->right;
            phd_top = r->top;
            phd_bottom = r->bottom;
        }
    }
}

void DrawAnimatingItem(ITEM_INFO *item)
{
    OBJECT_INFO *obj;
    BITE_INFO *bite;
    sint16 **meshpp;
    sint32 *bone;
    sint16 *frm[2];
    sint16 *data;
    sint16 *rot;
    sint16 *rot2;
    sint32 frac, rate, clip, bit, i, flags;
    sint16 rnd;

    frac = GetFrames(item, frm, &rate);
    obj = &objects[item->object_number];
    if (obj->shadow_size)
        S_PrintShadow(obj->shadow_size, frm[0], item, 0);
    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    if (obj->object_mip && (obj + 1)->loaded && phd_mxptr[M23] >> 16 > obj->object_mip)
        obj++;
    if (item->object_number >= ENEMY_JEEP && item->object_number <= SETHA_MIP) {
        phd_left = 0;
        phd_right = phd_winwidth;
        phd_top = 0;
        phd_bottom = phd_winheight;
    } else
        calc_animating_item_clip_window(item, frm[0]);
    clip = S_GetObjectBounds(frm[0]);
    if (clip) {
        CalculateObjectLighting(item, frm[0]);
        data = (sint16 *)item->data;
        if (!data)
            data = no_rotation;
        bit = 1;
        meshpp = &meshes[obj->mesh_index];
        bone = &bones[obj->bone_index];
        if (!frac) {
            phd_TranslateRel(frm[0][6], frm[0][7], frm[0][8]);
            rot = frm[0] + 9;
            gar_RotYXZsuperpack(&rot, 0);
            if (item->mesh_bits & 1) {
                if (item->meshswap_meshbits & 1)
                    phd_PutPolygons(meshpp[1], clip);
                else
                    phd_PutPolygons(meshpp[0], clip);
            }
            meshpp += 2;
            for (i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2) {
                flags = bone[0];
                if (flags & 1)
                    phd_PopMatrix();
                if (flags & 2)
                    phd_PushMatrix();
                phd_TranslateRel(bone[1], bone[2], bone[3]);
                gar_RotYXZsuperpack(&rot, 0);
                if (flags & 0x1C) {
                    if (flags & 8)
                        phd_RotY(*data++);
                    if (flags & 4)
                        phd_RotX(*data++);
                    if (flags & 16)
                        phd_RotZ(*data++);
                }
                bit <<= 1;
                if (bit & item->mesh_bits) {
                    if (bit & item->meshswap_meshbits)
                        phd_PutPolygons(meshpp[1], clip);
                    else
                        phd_PutPolygons(meshpp[0], clip);
                }
                if (item->fired_weapon && i == EnemyBites[obj->bite_offset].mesh_num - 1) {
                    bite = &EnemyBites[obj->bite_offset];
                    phd_PushMatrix();
                    phd_RotX(-16380);
                    phd_TranslateRel(bite->x, bite->y, bite->z);
                    nullsub_1(600);
                    phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], clip);
                    phd_PopMatrix();
                    item->fired_weapon--;
                }
            }
        } else {
            InitInterpolate(frac, rate);
            phd_TranslateRel_ID(frm[0][6], frm[0][7], frm[0][8], frm[1][6], frm[1][7], frm[1][8]);
            rot = frm[0] + 9;
            rot2 = frm[1] + 9;
            gar_RotYXZsuperpack_I(&rot, &rot2, 0);
            if (item->mesh_bits & 1) {
                if (item->meshswap_meshbits & 1)
                    phd_PutPolygons_I(meshpp[1], clip);
                else
                    phd_PutPolygons_I(meshpp[0], clip);
            }
            meshpp += 2;
            for (i = 0; i < obj->nmeshes - 1; i++, bone += 4, meshpp += 2) {
                flags = bone[0];
                if (flags & 1)
                    phd_PopMatrix_I();
                if (flags & 2)
                    phd_PushMatrix_I();
                phd_TranslateRel_I(bone[1], bone[2], bone[3]);
                gar_RotYXZsuperpack_I(&rot, &rot2, 0);
                if (flags & 0x1C) {
                    if (flags & 8)
                        phd_RotY_I(*data++);
                    if (flags & 4)
                        phd_RotX_I(*data++);
                    if (flags & 16)
                        phd_RotZ_I(*data++);
                }
                bit <<= 1;
                if (bit & item->mesh_bits) {
                    if (bit & item->meshswap_meshbits)
                        phd_PutPolygons_I(meshpp[1], clip);
                    else
                        phd_PutPolygons_I(meshpp[0], clip);
                }
                if (item->fired_weapon && i == EnemyBites[obj->bite_offset].mesh_num - 1) {
                    rnd = GetRandomDraw();
                    bite = &EnemyBites[obj->bite_offset];
                    phd_PushMatrix_I();
                    phd_TranslateRel_I(bite->x, bite->y, bite->z);
                    phd_RotYXZ_I(0, -16380, (sint16)((rnd << 14) + (rnd >> 2) - 4096));
                    InterpolateMatrix();
                    nullsub_1(600);
                    phd_PutPolygons(meshes[objects[GUN_FLASH].mesh_index], clip);
                    phd_PopMatrix_I();
                    item->fired_weapon--;
                }
            }
        }
    }
    phd_left = 0;
    phd_top = 0;
    phd_right = phd_winwidth;
    phd_bottom = phd_winheight;
    phd_PopMatrix();
}

void InitInterpolate(sint32 frac, sint32 rate)
{
    IM_frac = frac;
    IM_rate = rate;
    IMptr = IMstack;
    S_LongMemCpy(IMstack, phd_mxptr, 12);
}

void phd_PopMatrix_I(void)
{
    phd_PopMatrix();
    IMptr -= 12;
}

void phd_PushMatrix_I(void)
{
    phd_PushMatrix();
    S_LongMemCpy(IMptr + 12, IMptr, 12);
    IMptr += 12;
}

void phd_RotY_I(sint16 ang)
{
    sint32 *oldm;

    phd_RotY(ang);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotY(ang);
    phd_mxptr = oldm;
}

void phd_RotX_I(sint16 ang)
{
    sint32 *oldm;

    phd_RotX(ang);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotX(ang);
    phd_mxptr = oldm;
}

void phd_RotZ_I(sint16 ang)
{
    sint32 *oldm;

    phd_RotZ(ang);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotZ(ang);
    phd_mxptr = oldm;
}

void phd_TranslateRel_I(sint32 x, sint32 y, sint32 z)
{
    sint32 *oldm;

    phd_TranslateRel(x, y, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_TranslateRel(x, y, z);
    phd_mxptr = oldm;
}

void phd_TranslateRel_ID(sint32 x, sint32 y, sint32 z, sint32 x2, sint32 y2, sint32 z2)
{
    sint32 *oldm;

    phd_TranslateRel(x, y, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_TranslateRel(x2, y2, z2);
    phd_mxptr = oldm;
}

void phd_RotYXZ_I(sint16 y, sint16 x, sint16 z)
{
    sint32 *oldm;

    phd_RotYXZ(y, x, z);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    phd_RotYXZ(y, x, z);
    phd_mxptr = oldm;
}

void gar_RotYXZsuperpack_I(sint16 **pprot1, sint16 **pprot2, sint32 skip)
{
    sint32 *oldm;

    gar_RotYXZsuperpack(pprot1, skip);
    oldm = phd_mxptr;
    phd_mxptr = IMptr;
    gar_RotYXZsuperpack(pprot2, skip);
    phd_mxptr = oldm;
}

void gar_RotYXZsuperpack(sint16 **pprot, sint32 skip)
{
    uint16 *prot;
    sint32 packed;

    while (skip) {
        prot = *pprot;
        if (*prot & (3 << 14))
            (*pprot) += 1;
        else
            (*pprot) += 2;
        skip--;
    }
    prot = *pprot;
    switch (*prot >> 14) {
    case 0:
        packed = (*prot << 16) + *(prot + 1);
        phd_RotYXZpack(packed);
        (*pprot) += 2;
        return;
    case 1:
        phd_RotX((sint16)((*prot & 0xFFF) << 4));
        break;
    case 2:
        phd_RotY((sint16)((*prot & 0xFFF) << 4));
        break;
    default:
        phd_RotZ((sint16)((*prot & 0xFFF) << 4));
        break;
    }
    (*pprot) += 1;
}

void phd_PutPolygons_I(sint16 *ptr, sint32 clip)
{
    phd_PushMatrix();
    InterpolateMatrix();
    phd_PutPolygons(ptr, clip);
    phd_PopMatrix();
}

void InterpolateMatrix(void)
{
    sint32 *mptr, *iptr;
    sint32 frac, rate;

    mptr = phd_mxptr;
    iptr = IMptr;
    frac = IM_frac;
    rate = IM_rate;
    if (rate == 2 || (frac == 2 && rate == 4)) {
        mptr[0] = (iptr[0] + mptr[0]) >> 1;
        mptr[1] = (iptr[1] + mptr[1]) >> 1;
        mptr[2] = (iptr[2] + mptr[2]) >> 1;
        mptr[3] = (iptr[3] + mptr[3]) >> 1;
        mptr[4] = (iptr[4] + mptr[4]) >> 1;
        mptr[5] = (iptr[5] + mptr[5]) >> 1;
        mptr[6] = (iptr[6] + mptr[6]) >> 1;
        mptr[7] = (iptr[7] + mptr[7]) >> 1;
        mptr[8] = (iptr[8] + mptr[8]) >> 1;
        mptr[9] = (iptr[9] + mptr[9]) >> 1;
        mptr[10] = (iptr[10] + mptr[10]) >> 1;
        mptr[11] = (iptr[11] + mptr[11]) >> 1;
    } else if (frac == 1) {
        mptr[0] += (iptr[0] - mptr[0]) >> 2;
        mptr[1] += (iptr[1] - mptr[1]) >> 2;
        mptr[2] += (iptr[2] - mptr[2]) >> 2;
        mptr[3] += (iptr[3] - mptr[3]) >> 2;
        mptr[4] += (iptr[4] - mptr[4]) >> 2;
        mptr[5] += (iptr[5] - mptr[5]) >> 2;
        mptr[6] += (iptr[6] - mptr[6]) >> 2;
        mptr[7] += (iptr[7] - mptr[7]) >> 2;
        mptr[8] += (iptr[8] - mptr[8]) >> 2;
        mptr[9] += (iptr[9] - mptr[9]) >> 2;
        mptr[10] += (iptr[10] - mptr[10]) >> 2;
        mptr[11] += (iptr[11] - mptr[11]) >> 2;
    } else {
        mptr[0] = iptr[0] - ((iptr[0] - mptr[0]) >> 2);
        mptr[1] = iptr[1] - ((iptr[1] - mptr[1]) >> 2);
        mptr[2] = iptr[2] - ((iptr[2] - mptr[2]) >> 2);
        mptr[3] = iptr[3] - ((iptr[3] - mptr[3]) >> 2);
        mptr[4] = iptr[4] - ((iptr[4] - mptr[4]) >> 2);
        mptr[5] = iptr[5] - ((iptr[5] - mptr[5]) >> 2);
        mptr[6] = iptr[6] - ((iptr[6] - mptr[6]) >> 2);
        mptr[7] = iptr[7] - ((iptr[7] - mptr[7]) >> 2);
        mptr[8] = iptr[8] - ((iptr[8] - mptr[8]) >> 2);
        mptr[9] = iptr[9] - ((iptr[9] - mptr[9]) >> 2);
        mptr[10] = iptr[10] - ((iptr[10] - mptr[10]) >> 2);
        mptr[11] = iptr[11] - ((iptr[11] - mptr[11]) >> 2);
    }
}

void InterpolateArmMatrix(sint32 *mx)
{
    sint32 *mptr, *iptr;
    sint32 frac, rate;

    mptr = phd_mxptr;
    iptr = IMptr;
    frac = IM_frac;
    rate = IM_rate;
    mptr[M00] = mx[M00];
    mptr[M01] = mx[M01];
    mptr[M02] = mx[M02];
    mptr[M10] = mx[M10];
    mptr[M11] = mx[M11];
    mptr[M12] = mx[M12];
    mptr[M20] = mx[M20];
    mptr[M21] = mx[M21];
    mptr[M22] = mx[M22];
    if (rate == 2 || (frac == 2 && rate == 4)) {
        mptr[M03] = (iptr[M03] + mptr[M03]) >> 1;
        mptr[M13] = (iptr[M13] + mptr[M13]) >> 1;
        mptr[M23] = (iptr[M23] + mptr[M23]) >> 1;
    } else if (frac == 1) {
        mptr[M03] += (iptr[M03] - mptr[M03]) >> 2;
        mptr[M13] += (iptr[M13] - mptr[M13]) >> 2;
        mptr[M23] += (iptr[M23] - mptr[M23]) >> 2;
    } else {
        mptr[M03] = iptr[M03] - ((iptr[M03] - mptr[M03]) >> 2);
        mptr[M13] = iptr[M13] - ((iptr[M13] - mptr[M13]) >> 2);
        mptr[M23] = iptr[M23] - ((iptr[M23] - mptr[M23]) >> 2);
    }
}

void CalculateObjectLighting(ITEM_INFO *item, sint16 *frame)
{
    sint32 *mptr;
    sint32 x, y, z;

    if (item->shade < 0) {
        phd_PushUnitMatrix();
        mptr = phd_mxptr;
        mptr[M03] = mptr[M13] = mptr[M23] = 0;
        phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
        phd_TranslateRel((frame[0] + frame[1]) >> 1, (frame[2] + frame[3]) >> 1, (frame[4] + frame[5]) >> 1);
        x = item->pos.x_pos + (phd_mxptr[M03] >> 14);
        y = item->pos.y_pos + (phd_mxptr[M13] >> 14);
        z = item->pos.z_pos + (phd_mxptr[M23] >> 14);
        phd_PopMatrix();
        current_item = item;
        item->il.item_pos.x = x;
        item->il.item_pos.y = y;
        item->il.item_pos.z = z;
        CalcAmbientLight(item);
        CreateLightList(item);
    } else
        S_CalculateStaticMeshLight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->shade & 0x7FFF, &room[item->room_number]);
}

void CalculateObjectLightingLara(void)
{
    PHD_VECTOR pos;
    sint16 room_no;

    if (GLOBAL_playing_cutseq)
        CalculateObjectLightingLaraCutSeq();
    else {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
        if (lara_item->anim_number == ANIM_DUCKBREATHE || lara_item->anim_number == ANIM_ALL4S || lara_item->anim_number == ANIM_BREATH) {
            pos.x = lara_item->pos.x_pos;
            if (lara_item->anim_number == ANIM_BREATH)
                pos.y = lara_item->pos.y_pos - 512;
            else
                pos.y = lara_item->pos.y_pos - 192;
            pos.z = lara_item->pos.z_pos;
            room_no = lara_item->room_number;
            GetFloor(pos.x, pos.y, pos.z, &room_no);
        } else {
            GetLaraJointPos(&pos, 7);
            room_no = lara_item->room_number;
            GetFloor(pos.x, pos.y, pos.z, &room_no);
        }
        current_item = lara_item;
        lara_item->il.item_pos.x = pos.x;
        lara_item->il.item_pos.y = pos.y;
        lara_item->il.item_pos.z = pos.z;
        CalcAmbientLight(lara_item);
        CreateLightList(lara_item);
    }
}

sint32 GetFrames(ITEM_INFO *item, sint16 *frm[], sint32 *rate)
{
    ANIM_STRUCT *anim;
    sint32 frame;
    sint32 first, second;
    sint32 frame_size;
    sint32 interp, rat;

    frame = item->frame_number;
    anim = &anims[item->anim_number];
    frm[0] = frm[1] = anim->frame_ptr;
    rat = *rate = anim->interpolation & 0xff;
    frame_size = anim->interpolation >> 8;
    frame -= anim->frame_base;
    first = frame / rat;
    interp = frame % rat;
    frm[0] += first * frame_size;
    frm[1] = frm[0] + frame_size;
    if (interp == 0)
        return 0;
    second = (first + 1) * rat;
    if (second > anim->frame_end)
        *rate = anim->frame_end - second + rat;
    return interp;
}

sint16 *GetBoundsAccurate(ITEM_INFO *item)
{
    sint32 i, frac, rate;
    sint16 *bptr, *frmptr[2];

    frac = GetFrames(item, frmptr, &rate);
    if (frac == 0)
        return frmptr[0];
    bptr = interpolated_bounds;
    for (i = 0; i < 6; i++, bptr++, frmptr[0]++, frmptr[1]++)
        *bptr = *frmptr[0] + ((*frmptr[1] - *frmptr[0]) * frac) / rate;
    return interpolated_bounds;
}

sint16 *GetBestFrame(ITEM_INFO *item)
{
    sint32 frac;
    sint32 rate;
    sint16 *frmptr[2];

    frac = GetFrames(item, frmptr, &rate);
    if (frac <= (rate >> 1))
        return frmptr[0];
    else
        return frmptr[1];
}

void mRotBoundingBoxNoPersp(sint16 *bounds, sint16 *rotatedBounds)
{
    SVECTOR pos[8];
    sint32 x, y, z, i;
    sint16 xMin, xMax, yMin, yMax, zMin, zMax;

    xMin = bounds[0];
    xMax = bounds[1];
    yMin = bounds[2];
    yMax = bounds[3];
    zMin = bounds[4];
    zMax = bounds[5];
    pos[0].x = xMin;
    pos[0].y = yMin;
    pos[0].z = zMin;
    pos[1].x = xMax;
    pos[1].y = yMin;
    pos[1].z = zMin;
    pos[2].x = xMax;
    pos[2].y = yMax;
    pos[2].z = zMin;
    pos[3].x = xMin;
    pos[3].y = yMax;
    pos[3].z = zMin;
    pos[4].x = xMin;
    pos[4].y = yMin;
    pos[4].z = zMax;
    pos[5].x = xMax;
    pos[5].y = yMin;
    pos[5].z = zMax;
    pos[6].x = xMax;
    pos[6].y = yMax;
    pos[6].z = zMax;
    pos[7].x = xMin;
    pos[7].y = yMax;
    pos[7].z = zMax;
    xMin = 0x7FFF;
    yMin = 0x7FFF;
    zMin = 0x7FFF;
    xMax = -0x7FFF;
    yMax = -0x7FFF;
    zMax = -0x7FFF;
    for (i = 0; i < 8; i++) {
        x = pos[i].x * phd_mxptr[M00] + pos[i].y * phd_mxptr[M01] + pos[i].z * phd_mxptr[M02] + phd_mxptr[M03];
        y = pos[i].x * phd_mxptr[M10] + pos[i].y * phd_mxptr[M11] + pos[i].z * phd_mxptr[M12] + phd_mxptr[M13];
        z = pos[i].x * phd_mxptr[M20] + pos[i].y * phd_mxptr[M21] + pos[i].z * phd_mxptr[M22] + phd_mxptr[M23];
        if (x < xMin)
            xMin = (sint16)x;
        if (x > xMax)
            xMax = (sint16)x;
        if (y < yMin)
            yMin = (sint16)y;
        if (y > yMax)
            yMax = (sint16)y;
        if (z < zMin)
            zMin = (sint16)z;
        if (z > zMax)
            zMax = (sint16)z;
    }
    rotatedBounds[0] = xMin;
    rotatedBounds[1] = xMax;
    rotatedBounds[2] = yMin;
    rotatedBounds[3] = yMax;
    rotatedBounds[4] = zMin;
    rotatedBounds[5] = zMax;
}

void UpdateSkyLightning(void)
{
    sint32 i;

    if (LightningCount > 0) {
        LightningCount--;
        if (!LightningCount) {
            LightningRand = (GetRandomDraw() & 0x7F) + 400;
            dLightningRand = 0;
        } else {
            dLightningRand = GetRandomDraw() & 0x1FF;
            LightningRand += (dLightningRand - LightningRand) >> 1;
        }
    } else {
        if (LightningRand >= 4)
            LightningRand -= LightningRand >> 2;
        else
            LightningRand = 0;
    }
    for (i = 0; i < 3; i++) {
        LightningRGB[i] = LightningRGBs[i] + ((LightningRGBs[i] * LightningRand) >> 8);
        if (LightningRGB[i] > 255)
            LightningRGB[i] = 255;
    }
}

void RenderIt(sint16 CurrentRoom)
{
    ROOM_INFO *r;
    sint32 i;

    current_room = CurrentRoom;
    r = &room[CurrentRoom];
    r->test_left = 0;
    phd_left = 0;
    r->test_top = 0;
    phd_top = 0;
    r->test_right = phd_winxmax;
    phd_right = phd_winxmax;
    r->test_bottom = phd_winymax;
    phd_bottom = phd_winymax;
    outside = r->flags & ROOM_OUTSIDE;
    camera_underwater = r->flags & ROOM_UNDERWATER;
    r->bound_active = 2;
    draw_room_list[0] = CurrentRoom;
    room_list_start = 0;
    room_list_end = 1;
    number_draw_rooms = 0;

    if (outside) {
        outside_top = 0;
        outside_left = 0;
        outside_right = phd_winxmax;
        outside_bottom = phd_winymax;
    } else {
        outside_left = phd_winxmax;
        outside_top = phd_winymax;
        outside_bottom = 0;
        outside_right = 0;
    }

    GetRoomBounds();
    InitialiseFogBulbs();
    CreateFXBulbs();

    if (outside) {
        if (objects[HORIZON].loaded) {
            if (BinocularRange)
                AlterFOV(14560 - BinocularRange);

            phd_PushMatrix();
            phd_TranslateAbs(camera.pos.x, camera.pos.y, camera.pos.z);
            nPolyType = 6;
            phd_PushMatrix();

            if (gfLevelFlags & GF_LAYER1) {
                phd_RotY(32760);
                if (gfLevelFlags & GF_LIGHTNING)
                    DrawFlatSky((((LightningRGB[0] | (44 << 8)) << 8 | LightningRGB[1]) << 8) | LightningRGB[2], SkyPos, -1536, 4);
                else
                    DrawFlatSky(*(uint32 *)&gfLayer1Col, SkyPos, -1536, 4);
            }

            if (gfLevelFlags & GF_LAYER2)
                DrawFlatSky(0xFF000000 | *(uint32 *)&gfLayer2Col, SkyPos2, -1536, 2);

            if (gfLevelFlags & (GF_LAYER1 | GF_LAYER2))
                OutputSky();

            phd_PopMatrix();

            if (gfLevelFlags & GF_HORIZON) {
                phd_PutPolygonSkyMesh(meshes[objects[HORIZON].mesh_index], -1);
                OutputSky();
            }

            phd_PopMatrix();
        } else
            outside = -1;
    }

    InitDynamicLighting();
    nPolyType = 0;

    for (i = 0; i < number_draw_rooms; i++)
        S_InsertRoom(draw_rooms[i]);

    if (gfLevelFlags & GF_TRAIN)
        DrawTrainFloor();

    for (i = 0; i < number_draw_rooms; i++)
        PrintObjects(draw_rooms[i]);
}
