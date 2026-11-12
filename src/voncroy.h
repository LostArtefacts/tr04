#ifndef VONCROY_H
#define VONCROY_H

#include "items.h"
#include "box.h"
#include "3dglodef.h"

typedef struct voncroy_cut_data {
    PHD_VECTOR CameraPos;
    PHD_VECTOR CameraTarget;
    sint32 FOV;
} VONCROY_CUT_DATA;

extern uint8 VonCroyCutFlags[64];
extern char bVoncroyCutScene;
extern sint32 actualRoomNumber;
extern sint32 actualFOV;
extern PHD_VECTOR actualCameraTarget;
extern PHD_VECTOR actualCameraPos;
extern uint8 VonCroyCutIndices[64];
extern sint16 VonCroyCutTracks[64];
extern VONCROY_CUT_DATA VonCroyCutscenes[15];

void InitialiseVoncroy(sint16 item_number);
void ClearCutSceneCamera(void);
void SetCutSceneCamera(ITEM_INFO *item);
void GetAIEnemy(CREATURE_INFO *info, sint32 tfl);
void DoVonCroyCutscene(ITEM_INFO *item, CREATURE_INFO *info);

#endif
