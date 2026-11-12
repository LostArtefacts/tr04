#ifndef TRAIN_H
#define TRAIN_H

#include "items.h"

typedef struct train_static {
    uint16 type;
    sint16 zoff;
} TRAIN_STATIC;

extern sint16 dels_handy_train_map[128];
extern TRAIN_STATIC dels_handy_train_map2[64];

void DrawTrainObjects(void);
void DrawTrainFloor(void);
void InitialiseTrainJeep(sint16 item_number);
void TrainJeepControl(sint16 item_number);

#endif
