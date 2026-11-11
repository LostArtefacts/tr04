#include "larafire.h"

sint32 CheckForHoldingState(sint32 state)
{
    sint16 *holds;

    holds = HoldStates;
    while (*holds >= 0) {
        if (state == *holds)
            return 1;
        holds++;
    }
    return 0;
}
