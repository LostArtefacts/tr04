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
