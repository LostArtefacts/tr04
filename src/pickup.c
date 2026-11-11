#include "pickup.h"
#include "lara.h"
#include "lara_states.h"
#include "anim.h"
#include "room.h"
#include "control.h"
#include "draw.h"
#include "objects.h"
#include "newinv.h"
#include "sound.h"
#include "specific/input.h"
#include "camera.h"

void AnimatingPickUp(sint16 item_number)
{
    if ((items[item_number].trigger_flags & 0x3F) == 2)
        AnimateItem(&items[item_number]);
}

void RegeneratePickups(void)
{
    ITEM_INFO *item;
    sint16 *ammo;
    sint16 objnum;
    sint16 i;

    for (i = 0; i < NumRPickups; i++) {
        item = &items[RPickups[i]];
        if (item->status == ITEM_INVISIBLE) {
            objnum = item->object_number;
            if (objnum >= CROSSBOW_AMMO1_ITEM && objnum <= CROSSBOW_AMMO3_ITEM) {
                ammo = &lara.num_crossbow_ammo1;
                if (!ammo[objnum - CROSSBOW_AMMO1_ITEM])
                    item->status = ITEM_INACTIVE;
            } else if (objnum >= GRENADE_GUN_AMMO1_ITEM && objnum <= GRENADE_GUN_AMMO3_ITEM) {
                ammo = &lara.num_grenade_ammo1;
                if (!ammo[objnum - GRENADE_GUN_AMMO1_ITEM])
                    item->status = ITEM_INACTIVE;
            } else if (objnum >= SHOTGUN_AMMO1_ITEM && objnum <= SHOTGUN_AMMO2_ITEM) {
                ammo = &lara.num_shotgun_ammo1;
                if (!ammo[objnum - SHOTGUN_AMMO1_ITEM])
                    item->status = ITEM_INACTIVE;
            } else if (objnum == SIXSHOOTER_AMMO_ITEM) {
                ammo = &lara.num_revolver_ammo;
                if (!*ammo)
                    item->status = ITEM_INACTIVE;
            }
        }
    }
}

sint16 *FindPlinth(ITEM_INFO *item)
{
    ITEM_INFO *plinth;
    ROOM_INFO *r;
    MESH_INFO *mesh;
    sint16 *p;
    sint16 *o;
    sint32 i;
    sint16 item_num;

    r = &room[item->room_number];
    mesh = r->mesh;
    for (i = r->num_meshes; i > 0; i--) {
        if (mesh->Flags & 1 && item->pos.x_pos == mesh->x && item->pos.z_pos == mesh->z) {
            p = GetBestFrame(item);
            o = &static_objects[mesh->static_number].x_minc;
            if (p[0] <= o[1] && p[1] >= o[0] && p[4] <= o[5] && p[5] >= o[4] && (o[0] || o[1]))
                break;
        }
        mesh++;
    }
    if (i)
        return o;
    for (item_num = r->item_number; item_num != NO_ITEM; item_num = plinth->next_item) {
        plinth = &items[item_num];
        if (item != plinth && objects[plinth->object_number].collision != PickUpCollision && item->pos.x_pos == plinth->pos.x_pos && item->pos.y_pos <= plinth->pos.y_pos && item->pos.z_pos == plinth->pos.z_pos)
            break;
    }
    if (item_num != NO_ITEM)
        return GetBestFrame(&items[item_num]);
    return 0;
}

void KeyHoleCollision(sint16 item_number, ITEM_INFO *l, COLL_INFO *coll)
{
    ITEM_INFO *item;
    sint32 key, hole;

    item = &items[item_number];
    if ((input & IN_ACTION || GLOBAL_inventoryitemchosen != NO_ITEM) && !BinocularRange && lara.gun_status == LG_NO_ARMS && l->current_anim_state == AS_STOP && l->anim_number == ANIM_BREATH || lara.IsMoving && lara.GeneralPtr == (void *)item_number) {
        key = GLOBAL_inventoryitemchosen - KEY_ITEM1;
        hole = item->object_number - KEY_HOLE1;
        if (TestLaraPosition(KeyHoleBounds, item, l)) {
            if (!lara.IsMoving) {
                if (item->status != ITEM_INACTIVE)
                    return;
                if (GLOBAL_inventoryitemchosen == NO_ITEM) {
                    if (have_i_got_object(hole + KEY_ITEM1))
                        GLOBAL_enterinventory = hole + KEY_ITEM1;
                    return;
                }
                if (key != hole)
                    return;
            }
            if (MoveLaraPosition(&KeyHolePosition, item, l)) {
                remove_inventory_item(hole + KEY_ITEM1);
                l->anim_number = ANIM_USEKEY;
                l->frame_number = anims[ANIM_USEKEY].frame_base;
                l->current_anim_state = AS_USEKEY;
                lara.IsMoving = 0;
                lara.head_y_rot = 0;
                lara.head_x_rot = 0;
                lara.torso_y_rot = 0;
                lara.torso_x_rot = 0;
                lara.gun_status = LG_HANDS_BUSY;
                item->status = ITEM_ACTIVE;
            } else
                lara.GeneralPtr = (void *)item_number;
            GLOBAL_inventoryitemchosen = NO_ITEM;
        } else if (lara.IsMoving && lara.GeneralPtr == (void *)item_number) {
            lara.IsMoving = 0;
            lara.gun_status = LG_NO_ARMS;
        }
    } else if (item->object_number <= KEY_HOLE6)
        ObjectCollision(item_number, l, coll);
}

void PuzzleDoneCollision(sint16 item_num, ITEM_INFO *l, COLL_INFO *coll)
{
    if (items[item_num].trigger_flags != 999)
        ObjectCollision(item_num, l, coll);
}

void PuzzleDone(ITEM_INFO *item, sint16 item_number)
{
    item->object_number += 12;
    item->anim_number = objects[item->object_number].anim_index;
    item->frame_number = anims[item->anim_number].frame_base;
    item->current_anim_state = item->goal_anim_state = anims[item->anim_number].current_anim_state;
    item->required_anim_state = 0;
    AddActiveItem(item_number);
    item->flags |= IFL_CODEBITS;
    item->status = ITEM_ACTIVE;
}

sint32 KeyTrigger(sint16 item_num)
{
    ITEM_INFO *item;
    sint32 oldkey;

    item = &items[item_num];
    if ((item->status != ITEM_ACTIVE || lara.gun_status == LG_HANDS_BUSY) && (!KeyTriggerActive || lara.gun_status != LG_HANDS_BUSY))
        return -1;
    oldkey = KeyTriggerActive;
    if (!KeyTriggerActive)
        item->status = ITEM_DEACTIVATED;
    KeyTriggerActive = 0;
    return oldkey;
}

sint32 PickupTrigger(sint16 item_num)
{
    ITEM_INFO *item;

    item = &items[item_num];
    if ((sint16)(item->flags & IFL_CLEARBODY) || item->status != ITEM_INVISIBLE || item->item_flags[3] != 1 || item->trigger_flags & 128)
        return 0;
    KillItem(item_num);
    return 1;
}
