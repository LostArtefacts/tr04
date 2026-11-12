#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setup.h"
#include "camera.h"
#include "control.h"
#include "specific/audio.h"
#include "footprnt.h"
#include "deltapak.h"
#include "tomb4fx.h"
#include "effect2.h"
#include "specific/function_stubs.h"
#include "specific/specificfx.h"
#include "specific/3dmath.h"
#include "anim.h"
#include "objnums.h"
#include "box.h"
#include "items.h"
#include "pickup.h"
#include "room.h"
#include "lara.h"
#include "laramisc.h"
#include "gameflow.h"
#include "draw.h"
#include "hair.h"
#include "init.h"
#include "switch.h"
#include "traps.h"
#include "moveblok.h"
#include "flmtorch.h"
#include "collide.h"
#include "rope.h"
#include "objects.h"
#include "deathsld.h"
#include "sas.h"

void InitialiseLara(void)
{
    ITEM_INFO *l;
    sint32 item_num;
    sint16 gun;

    item_num = lara.item_number;
    if (item_num == NO_ITEM)
        return;
    l = lara_item;
    l->data = &lara;
    l->collidable = 0;
    memset(&lara, 0, sizeof(LARA_INFO));
    lara.look = 1;
    lara.item_number = item_num;
    lara.hit_direction = -1;
    lara.air = 1800;
    lara.vehicle = NO_ITEM;
    lara.weapon_item = NO_ITEM;
    lara.water_surface_dist = 100;
    lara.holster = LARA_HOLSTERS_PISTOLS;
    lara.location = -1;
    lara.highest_location = -1;
    lara.RopePtr = -1;
    l->hit_points = 1000;
    lara.gun_status = LG_NO_ARMS;
    if (gfLevelFlags & GF_YOUNGLARA)
        gun = WEAPON_NONE;
    else
        gun = WEAPON_PISTOLS;
    lara.request_gun_type = lara.gun_type = lara.last_gun_type = gun;
    LaraInitialiseMeshes();
    lara.pistols_type_carried = 9;
    lara.binoculars = 1;
    lara.num_flares = 3;
    lara.num_small_medipack = 3;
    lara.num_large_medipack = 1;
    lara.num_pistols_ammo = -1;
    if (Gameflow->DemoDisc)
        lara.crowbar = 1;
    lara.beetle_uses = 3;
    InitialiseLaraAnims(l);
    DashTimer = 120;
}

void InitialiseObjects(void)
{
    OBJECT_INFO *obj;
    sint32 i, n;

    obj = objects;
    for (i = NUMBER_OBJECTS; i > 0; i--, obj++) {
        obj->initialise = 0;
        obj->collision = 0;
        obj->control = 0;
        obj->intelligent = 0;
        obj->save_position = 0;
        obj->save_hitpoints = 0;
        obj->save_flags = 0;
        obj->save_anim = 0;
        obj->water_creature = 0;
        obj->using_drawanimating_item = 1;
        obj->save_mesh = 0;
        obj->draw_routine = DrawAnimatingItem;
        obj->ceiling = 0;
        obj->floor = 0;
        obj->pivot_length = 0;
        obj->radius = 10;
        obj->shadow_size = 0;
        obj->hit_points = -16384;
        obj->explodable_meshbits = 0;
        obj->draw_routine_extra = 0;
        obj->frame_base = (sint16 *)((sint32)obj->frame_base + (char *)frames);
        obj->object_mip = 0;
    }
    BaddyObjects();
    ObjectObjects();
    TrapObjects();
    InitialiseHair();
    InitialiseEffects();
    NumRPickups = 0;
    CurrentSequence = 0;
    SequenceResults[0][1][2] = 0;
    SequenceResults[0][2][1] = 1;
    SequenceResults[1][0][2] = 2;
    SequenceResults[1][2][0] = 3;
    SequenceResults[2][0][1] = 4;
    SequenceResults[2][1][0] = 5;
    memset(SequenceUsed, 0, 6);
    memset(LibraryTab, 0, 8);
    n = gfNumMips;
    for (i = 0; i < n; i++) {
        obj = &objects[((gfMips[i] & 0xF) << 1) + ANIMATING1];
        obj->object_mip = (gfMips[i] & 0xF0) << 6;
    }
}

void TrapObjects(void)
{
    OBJECT_INFO *obj;
    sint32 i;

    obj = &objects[FLOOR_4BLADE];
    obj->control = Control4xFloorRoofBlade;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[ROOF_4BLADE];
    obj->control = Control4xFloorRoofBlade;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[BIRD_BLADE];
    obj->control = ControlBirdBlade;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[SETH_BLADE];
    obj->initialise = InitialiseSethBlade;
    obj->control = ControlSethBlade;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[CHAIN];
    obj->control = ControlChain;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[PLOUGH];
    obj->control = ControlPlough;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[CATWALK_BLADE];
    obj->control = ControlCatwalkBlade;
    obj->collision = GenericDeadlyBoundingBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[SPIKEBALL];
    obj->control = ControlSpikeball;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[HAMMER];
    obj->control = ControlHammer;
    obj->collision = GenericSphereBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[MOVING_BLADE];
    obj->control = ControlMovingBlade;
    obj->collision = GenericDeadlyBoundingBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[PLINTH_BLADE];
    obj->control = ControlPlinthBlade;
    obj->collision = GenericDeadlyBoundingBoxCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[COG];
    obj->control = ControlAnimatingSlots;
    obj->collision = CogCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[STARGATE];
    obj->control = ControlStargate;
    obj->collision = StargateCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[ROLLINGBALL];
    obj->control = ControlRollingBall;
    obj->collision = RollingBallCollision;
    obj->save_position = 1;
    obj->save_flags = 1;
    obj = &objects[BURNING_FLOOR];
    obj->initialise = InitialiseBurningFloor;
    obj->control = ControlBurningFloor;
    obj->save_position = 1;
    obj->save_flags = 1;
    obj = &objects[TWOBLOCK_PLATFORM];
    obj->initialise = InitialiseTwoBlockPlatform;
    obj->control = ControlTwoBlockPlatform;
    obj->floor = TwoBlockPlatformFloor;
    obj->ceiling = TwoBlockPlatformCeiling;
    obj->save_position = 1;
    obj->save_flags = 1;
    obj = &objects[KILL_ALL_TRIGGERS];
    obj->control = KillAllCurrentItems;
    obj->draw_routine = 0;
    obj->hit_points = 0;
    obj->using_drawanimating_item = 0;
    obj->save_flags = 1;
    obj = &objects[DEATH_SLIDE];
    obj->initialise = InitialiseDeathSlide;
    obj->control = ControlDeathSlide;
    obj->collision = DeathSlideCollision;
    obj->save_position = 1;
    obj->save_flags = 1;
    obj->save_anim = 1;
    for (i = FALLING_BLOCK; i < FALLING_BLOCK2 + 1; i++) {
        obj = &objects[i];
        obj->initialise = InitialiseFallingBlock2;
        obj->collision = FallingBlockCollision;
        obj->control = FallingBlock;
        obj->floor = FallingBlockFloor;
        obj->ceiling = FallingBlockCeiling;
        obj->save_position = 1;
        obj->save_flags = 1;
        obj->save_anim = 1;
    }
    obj = &objects[SMASHABLE_BIKE_WALL];
    obj->initialise = InitialiseFallingBlock2;
    obj->control = ControlSmashableBikeWall;
    obj->collision = ObjectCollision;
    obj->save_flags = 1;
    obj = &objects[SMASHABLE_BIKE_FLOOR];
    obj->initialise = InitialiseFallingBlock2;
    obj->control = ControlFallingBlock2;
    obj->collision = ObjectCollision;
    obj->floor = TwoBlockPlatformFloor;
    obj->ceiling = TwoBlockPlatformCeiling;
    obj->save_position = 1;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[FALLING_CEILING];
    obj->control = FallingCeiling;
    obj->collision = TrapCollision;
    obj->save_position = 1;
    obj->save_flags = 1;
    obj->save_anim = 1;
    for (i = PUSHABLE_OBJECT1; i <= PUSHABLE_OBJECT5; i++) {
        obj = &objects[i];
        obj->initialise = InitialiseMovingBlock;
        obj->control = MovableBlock;
        obj->collision = MovableBlockCollision;
        obj->save_position = 1;
        obj->save_flags = 1;
    }
    obj = &objects[PLANET_EFFECT];
    obj->initialise = InitialisePlanetEffect;
    obj->control = ControlPlanetEffect;
    obj->draw_routine = DrawPlanetEffect;
    obj->save_flags = 1;
    obj->using_drawanimating_item = 0;
    obj->save_mesh = 1;
    obj = &objects[DART_EMITTER];
    obj->control = DartEmitterControl;
    obj->draw_routine = 0;
    obj->save_flags = 1;
    obj->using_drawanimating_item = 0;
    obj = &objects[HOMING_DART_EMITTER];
    obj->control = DartEmitterControl;
    obj->draw_routine = 0;
    obj->save_flags = 1;
    obj->using_drawanimating_item = 0;
    obj = &objects[DARTS];
    obj->control = DartsControl;
    obj->collision = ObjectCollision;
    obj->draw_routine = S_DrawDarts;
    obj->using_drawanimating_item = 0;
    obj->shadow_size = 128;
    obj = &objects[SAS_DRAG_BLOKE];
    obj->control = ControlAnimatingSlots;
    obj->collision = DragSASCollision;
    obj->save_position = 1;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj = &objects[SPRINKLER];
    obj->control = ControlSprinkler;
    obj->collision = FireCollision;
    obj->save_flags = 1;
    obj = &objects[FLAME];
    obj->control = FlameControl;
    obj->draw_routine = 0;
    obj->using_drawanimating_item = 0;
    obj = &objects[FLAME_EMITTER];
    obj->initialise = InitialiseFlameEmitter;
    obj->control = FlameEmitterControl;
    obj->collision = FireCollision;
    obj->draw_routine = 0;
    obj->using_drawanimating_item = 0;
    obj->save_flags = 1;
    obj = &objects[FLAME_EMITTER2];
    obj->initialise = InitialiseFlameEmitter2;
    obj->control = FlameEmitter2Control;
    obj->collision = FireCollision;
    obj->draw_routine = 0;
    obj->using_drawanimating_item = 0;
    obj->save_flags = 1;
    obj = &objects[FLAME_EMITTER3];
    obj->control = FlameEmitter3Control;
    obj->draw_routine = 0;
    obj->using_drawanimating_item = 0;
    obj->save_flags = 1;
    init_all_ropes();
    obj = &objects[ROPE];
    obj->initialise = InitialiseRope;
    obj->control = RopeControl;
    obj->collision = RopeCollision;
    obj->draw_routine = 0;
    obj->using_drawanimating_item = 0;
    obj->save_flags = 1;
    obj = &objects[POLEROPE];
    obj->collision = PoleCollision;
    obj->save_flags = 1;
    obj = &objects[TRIGGER_TRIGGERER];
    obj->initialise = ControlTriggerTriggerer;
    obj->control = ControlTriggerTriggerer;
    obj->draw_routine = 0;
    obj->using_drawanimating_item = 0;
    obj->save_flags = 1;
    obj = &objects[MINE];
    obj->initialise = InitialiseMineHelicopter;
    obj->control = ControlMineHelicopter;
    obj->collision = MineCollision;
    obj->HitEffect = 3;
}

void GetCarriedItems(void)
{
    ITEM_INFO *baddy;
    ITEM_INFO *pickup;
    sint32 i;
    sint16 item_num;

    for (i = 0, baddy = items; i < level_items; i++, baddy++) {
        baddy->carried_item = NO_ITEM;
        if (objects[baddy->object_number].intelligent && baddy->object_number != SCORPION) {
            item_num = room[baddy->room_number].item_number;
            while (item_num != NO_ITEM) {
                pickup = &items[item_num];
                if (pickup->pos.x_pos == baddy->pos.x_pos && pickup->pos.z_pos == baddy->pos.z_pos && abs(pickup->pos.y_pos - baddy->pos.y_pos) < 256 && objects[pickup->object_number].collision == PickUpCollision) {
                    pickup->carried_item = baddy->carried_item;
                    baddy->carried_item = item_num;
                    RemoveDrawnItem(item_num);
                    pickup->room_number = 255;
                    break;
                }
                item_num = pickup->next_item;
            }
        }
    }
}

void GetAIPickups(void)
{
    ITEM_INFO *item;
    AIOBJECT *aiObj;
    sint32 i;
    sint16 aiObjNum;

    for (i = 0, item = items; i < level_items; i++, item++) {
        if (objects[item->object_number].intelligent) {
            item->ai_bits = 0;
            for (aiObjNum = 0; aiObjNum < nAIObjects; aiObjNum++) {
                aiObj = &AIObjects[aiObjNum];
                if (aiObj->x == item->pos.x_pos && aiObj->z == item->pos.z_pos && aiObj->room_number == item->room_number && aiObj->object_number < AI_PATROL2) {
                    item->ai_bits |= 1 << (aiObj->object_number - AI_GUARD);
                    item->item_flags[3] = aiObj->trigger_flags;
                    aiObj->room_number = 255;
                }
            }
        }
    }
}

void BuildOutsideTable(void)
{
    ROOM_INFO *r;
    uint8 *pTable, *oTable, *cTable;
    sint32 max_slots, roomx, roomy, cont, offset, z, z2;
    sint32 i, x, y, rx, ry, j;
    char flipped[256];

    max_slots = 0;
    OutsideRoomOffsets = (sint16 *)malloc(0x5B2);
    OutsideRoomTable = (uint8 *)malloc(0xB640);
    memset(OutsideRoomTable, 0xFF, 0xB640);
    memset(flipped, 0, 255);
    for (i = 0; i < number_rooms; i++) {
        r = &room[i];
        if (r->flipped_room != -1)
            flipped[r->flipped_room] = 1;
    }
    r = &room[0];
    printf("X %d, Y %d, Z %d, Xs %d, Ys %d\n", r->x, r->y, r->z, r->x_size, r->y_size);
    for (y = 0; y < 108; y += 4) {
        for (x = 0; x < 108; x += 4) {
            for (i = 0; i < number_rooms; i++) {
                r = &room[i];
                if (!flipped[i]) {
                    roomx = (r->z >> 10) + 1;
                    roomy = (r->x >> 10) + 1;
                    cont = 0;
                    for (ry = 0; ry < 4; ry++) {
                        for (rx = 0; rx < 4; rx++) {
                            if (x + rx >= roomx && x + rx < roomx + r->x_size - 2 && y + ry >= roomy && y + ry < roomy + r->y_size - 2) {
                                cont = 1;
                                break;
                            }
                        }
                    }
                    if (cont) {
                        pTable = &OutsideRoomTable[64 * ((x >> 2) + 27 * (y >> 2))];
                        for (j = 0; j < 64; j++) {
                            if (pTable[j] == 0xFF) {
                                pTable[j] = i;
                                if (j > max_slots)
                                    max_slots = j;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    oTable = OutsideRoomTable;
    for (y = 0; y < 27; y++) {
        for (x = 0; x < 27; x++) {
            z = 0;
            offset = x + y * 27;
            pTable = &OutsideRoomTable[64 * (x + 27 * y)];
            while (pTable[z] != 0xFF)
                z++;
            if (!z)
                OutsideRoomOffsets[offset] = -1;
            else if (z == 1)
                OutsideRoomOffsets[offset] = *pTable | 0x8000;
            else {
                cTable = OutsideRoomTable;
                while (cTable < oTable) {
                    if (!memcmp(cTable, pTable, z)) {
                        OutsideRoomOffsets[offset] = (sint16)((sint32)cTable - (sint32)OutsideRoomTable);
                        break;
                    }
                    z2 = 0;
                    while (cTable[z2] != 0xFF)
                        z2++;
                    cTable += z2 + 1;
                }
                if (cTable >= oTable) {
                    OutsideRoomOffsets[offset] = (sint16)((sint32)oTable - (sint32)OutsideRoomTable);
                    do {
                        *oTable++ = *pTable++;
                        z--;
                    } while (z);
                    *oTable++ = 0xFF;
                }
            }
        }
    }
    printf("Ouside room table = %d bytes, max_slots = %d\n", oTable - OutsideRoomTable, max_slots);
}

void InitialiseGameFlags(void)
{
    memset(flipmap, 0, sizeof(flipmap));
    memset(flip_stats, 0, sizeof(flip_stats));
    flip_status = 0;
    memset(cd_flags, 0, sizeof(cd_flags));
    flipeffect = -1;
    IsAtmospherePlaying = 0;
    camera.underwater = 0;
}

void SetupGame(void)
{
    SeedRandomDraw(0xD371F947);
    SeedRandomControl(0xD371F947);
    wibble = 0;
    ClearFootPrints();
    InitBinoculars();
    InitTarget();
    trainmappos = 0;
    InitialiseGameFlags();
    InitialiseLara();
    GetCarriedItems();
    GetAIPickups();
    SeedRandomDraw(0xD371F947);
    SeedRandomControl(0xD371F947);
}

void InitTarget(void)
{
    sint16 **meshpp;
    sint16 *meshp;
    sint16 *vtx;
    sint32 nVtx;
    sint32 i;

    meshpp = &meshes[objects[TARGET_GRAPHICS].mesh_index];
    meshp = *meshpp;
    targetMeshP = (MESH_DATA *)meshp;
    nVtx = meshp[4] & 0xFF;
    vtx = meshp + 6;
    for (i = 0; i < nVtx; i++) {
        *vtx++ = (sint16)(phd_centerx + 80 * *vtx / 96);
        *vtx = (sint16)(phd_centery + 60 * *vtx / 224);
        vtx += 3;
    }
}

void ClearFootPrints(void)
{
    sint32 i;

    for (i = 0; i < 32; i++)
        FootPrint[i].Active = 0;
    FootPrintNum = 0;
}

void reset_cutseq_vars(void)
{
    cutseq_num = 0;
    cutseq_trig = 0;
    GLOBAL_playing_cutseq = 0;
    GLOBAL_cutseq_frame = 0;
    SetFadeClip(0, 1);
}
