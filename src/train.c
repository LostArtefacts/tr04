#include "train.h"
#include "lara.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "camera.h"
#include "sound.h"
#include "specific/3dmath.h"
#include "specific/specificfx.h"
#include "specific/output.h"
#include "gameflow.h"
#include "draw.h"
#include "setup.h"

void DrawTrainFloor(void)
{
    sint32 x;

    x = lara_item->pos.x_pos;
    lara_item->pos.x_pos = camera.pos.x;
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos & -1024, 0, 52224);
    DrawTrainStrips();
    camera.bounce = -12;
    phd_PopMatrix();
    DrawTrainObjects();
    lara_item->pos.x_pos = x;
}

void DrawTrainObjects(void)
{
    TRAIN_STATIC *p;
    sint16 *obj;
    sint32 x, i;

    trainmappos = (trainmappos + (gfUVRotate << 5)) % 0x60000;
    obj = &dels_handy_train_map[96 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144) & 0x1F)];
    x = trainmappos % 6144 - 24576;
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos / 6144 * 6144, 256, 47168);
    for (i = 8; i > 0; i--) {
        phd_PutPolygons_train(meshes[static_objects[*obj++].mesh_number], x);
        x += 6144;
    }
    phd_PopMatrix();
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos / 6144 * 6144 + x, 256, 47168);
    phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], 0);
    phd_PopMatrix();
    obj = &dels_handy_train_map[32 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144 + 8) & 0x1F)];
    x = trainmappos % 6144 - 24576;
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos / 6144 * 6144, 256, 58304);
    phd_RotY(32760);
    for (i = 8; i > 0; i--) {
        phd_PutPolygons_train(meshes[static_objects[*obj++].mesh_number], -x);
        x += 6144;
    }
    phd_PopMatrix();
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos / 6144 * 6144 + x, 256, 58304);
    phd_RotY(32760);
    phd_PutPolygons_train(meshes[static_objects[*obj].mesh_number], 0);
    phd_PopMatrix();
    p = &dels_handy_train_map2[32 - ((trainmappos / 6144 - lara_item->pos.x_pos / 6144) & 0x1F)];
    x = trainmappos % 6144 - 24576;
    phd_PushMatrix();
    for (i = 8; i > 0; i--) {
        if (p->type != 0xFFFF) {
            phd_TranslateAbs(lara_item->pos.x_pos / 6144 * 6144, 256, p->zoff + 52224);
            phd_PutPolygons_train(meshes[static_objects[p->type].mesh_number], x);
        }
        x += 6144;
        p++;
    }
    if (p->type != 0xFFFF) {
        phd_TranslateAbs(lara_item->pos.x_pos / 6144 * 6144 + x, 256, p->zoff + 52224);
        phd_PutPolygons_train(meshes[static_objects[p->type].mesh_number], 0);
    }
    phd_PopMatrix();
}

void InitialiseTrainJeep(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint16 i;

    item = &items[item_number];
    item->item_flags[0] = -80;
    item2 = items;
    for (i = 0; i < level_items; i++, item2++) {
        if (item != item2 && item2->trigger_flags == item->trigger_flags) {
            item->item_flags[1] = i;
            item2->pos.y_pos = item->pos.y_pos - 1024;
            item2->item_flags[0] = -80;
            return;
        }
    }
}

void TrainJeepControl(sint16 item_number)
{
    ITEM_INFO *item;
    ITEM_INFO *item2;
    sint16 room_number;

    item = &items[item_number];
    if (item->item_flags[0] != -80) {
        if (item->item_flags[2] > 1024)
            item->item_flags[2] -= 512;
    } else if (item->item_flags[2] < 0x4000)
        item->item_flags[2] += 32;
    SoundEffect(SFX_JEEP_MOVE, &item->pos, (item->item_flags[2] << 9) + (0x1000000 | SFX_SETPITCH));
    room_number = item->room_number;
    item->pos.x_pos += item->item_flags[0];
    AnimateItem(item);
    GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
    if (room_number != item->room_number)
        ItemNewRoom(item_number, room_number);
    item2 = &items[item->item_flags[1]];
    if (item2->item_flags[0] > -40 || item2->hit_points < 1)
        item->item_flags[0] += 3;
    if (item->item_flags[0] > 400)
        KillItem(item_number);
}
