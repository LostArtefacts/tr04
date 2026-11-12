#ifndef SETUP_H
#define SETUP_H

#include "types.h"

extern sint32 trainmappos;

void InitialiseLara(void);
void GetCarriedItems(void);
void GetAIPickups(void);
void InitialiseGameFlags(void);
void InitTarget(void);
void ClearFootPrints(void);
void reset_cutseq_vars(void);
void SetupGame(void);

#endif
