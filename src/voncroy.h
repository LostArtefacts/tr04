#ifndef VONCROY_H
#define VONCROY_H

#include "items.h"

extern uint8 VonCroyCutFlags[64];
extern char bVoncroyCutScene;
extern sint32 actualRoomNumber;
extern sint32 actualFOV;
extern PHD_VECTOR actualCameraTarget;
extern PHD_VECTOR actualCameraPos;

void InitialiseVoncroy(sint16 item_number);
void ClearCutSceneCamera(void);

#endif
