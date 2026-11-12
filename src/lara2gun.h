#ifndef LARA2GUN_H
#define LARA2GUN_H

#include "lara.h"

typedef struct pistol_def {
    sint16 ObjectNum;
    uint8 Draw1Anim2;
    uint8 Draw1Anim;
    uint8 Draw2Anim;
    uint8 RecoilAnim;
} PISTOL_DEF;

void set_arm_info(LARA_ARM *arm, sint32 frame);
void draw_pistol_meshes(sint32 weapon_type);
void undraw_pistol_mesh_left(sint32 weapon_type);
void undraw_pistol_mesh_right(sint32 weapon_type);

extern PISTOL_DEF PistolTable[4];

void ready_pistols(int weapon_type);
void draw_pistols(sint32 weapon_type);
void undraw_pistols(sint32 weapon_type);
void AnimatePistols(sint32 weapon_type);
void PistolHandler(sint32 weapon_type);

#endif
