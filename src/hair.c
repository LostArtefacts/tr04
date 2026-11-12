#include <stdlib.h>
#include <string.h>
#include "hair.h"
#include "anim.h"
#include "objnums.h"
#include "delstuff.h"
#include "specific/function_stubs.h"
#include "specific/output.h"

void InitialiseHair(void)
{
    OBJECT_INFO *obj;
    HAIR_STRUCT *hptr;
    sint32 *bone;
    sint32 i, j;

    for (i = 0; i < 2; i++) {
        obj = &objects[HAIR];
        bone = &bones[obj->bone_index];
        bone += 4;
        hptr = &hairs[i][0];
        hptr->pos.y_rot = 0;
        hptr->pos.x_rot = -16384;
        first_hair[i] = 1;
        for (j = 1; j < 7; j++, bone += 4) {
            hptr->pos.x_pos = bone[1];
            hptr->pos.y_pos = bone[2];
            hptr->pos.z_pos = bone[3];
            hptr->pos.x_rot = -16384;
            hptr->pos.y_rot = hptr->pos.z_rot = 0;
            hptr->vel.x = hptr->vel.y = hptr->vel.z = 0;
            hptr++;
        }
    }
}

void GetCorrectStashPoints(sint32 pigtail, sint32 hair_node, sint32 skin_node)
{
    HAIR_STRUCT *hair;
    uint16 num, rot1, rot2;
    sint32 i;

    num = 0;
    hair = &hairs[pigtail][hair_node - 1];
    if (!hair_node)
        rot1 = (uint16)(0x8000 - (CamRot.y << 4));
    else
        rot1 = hair->pos.y_rot;
    rot2 = hair[2].pos.y_rot;
    while (abs(rot1 - rot2) > 0x2000 && abs(rot1 - rot2) < 0xE000) {
        rot1 += 0x4000;
        num++;
    }
    for (i = 0; i < 5; i++)
        ScratchVertNums[skin_node][i] = HairRotScratchVertNums[num][i];
}
