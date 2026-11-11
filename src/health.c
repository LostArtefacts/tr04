#include "health.h"

sint32 FlashIt(void)
{
    if (flash_count) {
        flash_count--;
    } else {
        flash_state ^= 1;
        flash_count = 5;
    }
    return flash_state;
}
