#include "train.h"
#include "lara.h"
#include "anim.h"
#include "box.h"
#include "control.h"
#include "objnums.h"
#include "camera.h"
#include "sound.h"
#include "specific/3dmath.h"
#include "specific/specificfx.h"

void DrawTrainFloor(void)
{
    sint32 x;

    x = lara_item->pos.x_pos;
    lara_item->pos.x_pos = camera.pos.x;
    phd_PushMatrix();
    phd_TranslateAbs(lara_item->pos.x_pos & -1024, 0, 52224);
    DrawTrainStrips();
    camera.bounce = -12;
    phd_PopMatrix();
    DrawTrainObjects();
    lara_item->pos.x_pos = x;
}
