#ifndef SPECIFIC_LOADSAVE_H
#define SPECIFIC_LOADSAVE_H

#include "../types.h"

void S_DrawDashBar(sint32 pos);
void S_DrawAirBar(sint32 pos);
void S_DrawHealthBar(sint32 pos);
void CheckKeyConflicts(void);

extern sint32 SoundQuality;
extern sint32 ControlMethod;

#endif
