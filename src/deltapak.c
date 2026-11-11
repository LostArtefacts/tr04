#include "deltapak.h"
#include "lara.h"
#include "objnums.h"
#include "anim.h"
#include "control.h"
#include "lara2gun.h"
#include "hair.h"
#include "specific/function_stubs.h"

void *cutseq_malloc(sint32 size)
{
    return game_malloc(size, 0);
}

void finish_cutseq(void)
{
    GLOBAL_playing_cutseq = 0;
    InitialiseHair();
}

void fourth_cutseq_init(void)
{
    old_lara_holster = lara.holster;
    lara.holster = LARA_HOLSTERS;
    draw_pistol_meshes(1);
}
