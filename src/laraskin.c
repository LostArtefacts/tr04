#include <string.h>

#include "laraskin.h"
#include "anim.h"

extern uint8 SkinJoints[14][4];
extern PHD_VECTOR *SkinXYZPtr;
extern uint8 PointsToCalc[26][12];
extern sint8 ScratchVertNums[40][12];

void PushXYZ(void)
{
    PHD_VECTOR *ptr;

    ptr = SkinXYZPtr++;
    SkinXYZPtr->x = ptr->x;
    SkinXYZPtr->y = ptr->y;
    SkinXYZPtr->z = ptr->z;
}

void PopXYZ(void)
{
    SkinXYZPtr--;
}

void OptomiseSkinningData(void)
{
    OBJECT_INFO *object;
    sint16 **meshpp;
    sint16 *JointMesh, *MeshJ, *MeshNormals, *Src, *Dest;
    sint16 VertTable[128];
    sint16 NormalTable[128];
    sint32 c, lp, lp1, numvertsj, numvertstocalc, padval, numtris, numquads;
    uint8 RemapTable[32];

    object = &objects[LARA_SKIN_JOINTS];
    meshpp = &meshes[object->mesh_index + 1];

    for (c = 0; c < 14; ++c) {
        JointMesh = *meshpp;
        meshpp++;
        numvertsj = JointMesh[5] & 0xFF;
        numvertstocalc = JointMesh[5] & 0xFF;

        if (!numvertstocalc)
            numvertstocalc = (JointMesh[5] >> 8) & 0xFF;

        lp = 0;

        while (PointsToCalc[c][lp] != 255)
            lp++;

        if (numvertsj) {
            for (lp1 = 0; lp1 < lp; lp1++)
                RemapTable[PointsToCalc[c][lp1]] = (uint8)lp1;

            padval = lp;

            for (lp1 = 0; ScratchVertNums[SkinJoints[c][2]][lp1] != -1; lp1++) {
                RemapTable[ScratchVertNums[SkinJoints[c][2]][lp1]] = (uint8)padval;
                ScratchVertNums[SkinJoints[c][2]][lp1] = (sint8)padval;
                padval++;
            }

            for (lp1 = 0; ScratchVertNums[SkinJoints[c][3]][lp1] != -1; lp1++) {
                RemapTable[ScratchVertNums[SkinJoints[c][3]][lp1]] = (uint8)padval;
                ScratchVertNums[SkinJoints[c][3]][lp1] = (sint8)padval;
                padval++;
            }

            MeshJ = &JointMesh[6];
            MeshNormals = &JointMesh[3 * numvertstocalc + 7];

            for (lp1 = 0; lp1 < numvertsj; lp1++) {
                VertTable[4 * lp1] = MeshJ[0];
                VertTable[4 * lp1 + 1] = MeshJ[1];
                VertTable[4 * lp1 + 2] = MeshJ[2];
                NormalTable[4 * lp1] = MeshNormals[0];
                NormalTable[4 * lp1 + 1] = MeshNormals[1];
                NormalTable[4 * lp1 + 2] = MeshNormals[2];
                VertTable[4 * lp1 + 3] = 0;
                NormalTable[4 * lp1 + 3] = 0;
                MeshJ += 3;
                MeshNormals += 3;
            }

            for (lp1 = 0; lp1 < lp; lp1++) {
                Src = &VertTable[4 * PointsToCalc[c][lp1]];
                Dest = &JointMesh[3 * (RemapTable[PointsToCalc[c][lp1]] + 2)];
                Dest[0] = Src[0];
                Dest[1] = Src[1];
                Dest[2] = Src[2];
                Src = &NormalTable[4 * PointsToCalc[c][lp1]];
                Dest = &JointMesh[3 * (RemapTable[PointsToCalc[c][lp1]] + lp) + 7];
                Dest[0] = Src[0];
                Dest[1] = Src[1];
                Dest[2] = Src[2];
            }

            Dest = &JointMesh[6 * numvertstocalc + 7];
            numquads = *Dest;
            Dest++;

            for (lp1 = 0; lp1 < numquads; lp1++) {
                Dest[0] = RemapTable[Dest[0]];
                Dest[1] = RemapTable[Dest[1]];
                Dest[2] = RemapTable[Dest[2]];
                Dest[3] = RemapTable[Dest[3]];
                Dest += 6;
            }

            numtris = *Dest;
            Dest++;

            for (lp1 = 0; lp1 < numtris; lp1++) {
                Dest[0] = RemapTable[Dest[0]];
                Dest[1] = RemapTable[Dest[1]];
                Dest[2] = RemapTable[Dest[2]];
                Dest += 5;
            }

            JointMesh[5] = (sint16)lp;
            JointMesh[3 * (lp + 2)] = 0;
            Src = &JointMesh[6 * numvertstocalc + 7];
            Dest = &JointMesh[6 * lp + 7];
            numquads = 6 * *Src + 1;
            memcpy(Dest, Src, numquads * sizeof(sint16));
            Src += numquads;
            Dest += numquads;
            numtris = 5 * *Src + 1;
            memcpy(Dest, Src, numtris * sizeof(sint16));
        }
    }
}
