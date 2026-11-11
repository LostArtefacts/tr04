#ifndef SPECIFIC_CMDLINE_H
#define SPECIFIC_CMDLINE_H

#include "../types.h"

void CLSetup(char *cmd);
void CLNoFMV(char *cmd);
char *MapASCIIToANSI(char *s, char *d);

extern uint8 start_setup;
extern uint8 fmvs_disabled;
extern char ASCIIToANSITable[7][2];

#endif
