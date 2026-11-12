#include "hair.h"
#include "anim.h"
#include "objnums.h"

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
