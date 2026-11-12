#include "spotcam.h"

void InitSpotCamSequences(void)
{
    sint32 s, cc, ce, i;

    bTrackCamInit = 0;
    if (number_spotcams) {
        ce = 0;
        s = SpotCam[0].sequence;
        cc = 1;
        for (i = 1; i < number_spotcams; i++) {
            if (SpotCam[i].sequence == s)
                cc++;
            else {
                CameraCnt[ce] = cc;
                SpotRemap[s] = ce;
                s = SpotCam[i].sequence;
                cc = 1;
                ce++;
            }
        }
        CameraCnt[ce] = cc;
        SpotRemap[s] = ce;
    }
}
