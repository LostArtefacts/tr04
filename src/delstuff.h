#ifndef DELSTUFF_H
#define DELSTUFF_H

#include "3dglodef.h"

enum lara_mesh {
    LM_HIPS,
    LM_LTHIGH,
    LM_LSHIN,
    LM_LFOOT,
    LM_RTHIGH,
    LM_RSHIN,
    LM_RFOOT,
    LM_TORSO,
    LM_RINARM,
    LM_ROUTARM,
    LM_RHAND,
    LM_LINARM,
    LM_LOUTARM,
    LM_LHAND,
    LM_HEAD
};

void GetLaraJointPos(PHD_VECTOR *pos, sint32 node);
void SetLaraUnderwaterNodes(void);
void CalcLaraMatrices(sint32 flag);
void Rich_CalcLaraMatrices_Normal(sint16 *frame, sint32 *bone, sint32 flag);

extern uint8 LaraNodeUnderwater[15];
extern sint32 LaraNodeAmbient[2];
extern sint32 lara_joint_matrices[180];
extern sint32 lara_mesh_sweetness_table[15];
extern sint32 bLaraUnderWater;
extern char bLaraInWater;
extern sint16 *GLaraShadowframe;

#endif
