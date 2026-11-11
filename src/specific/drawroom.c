#include "drawroom.h"

void InitBuckets(void)
{
    sint32 n;

    for (n = 0; n < 20; n++) {
        Bucket[n].tpage = -1;
        Bucket[n].nVtx = 0;
    }
}
