#include "lara1gun.h"
#include "larafire.h"
#include "anim.h"
#include "delstuff.h"
#include "objnums.h"

void undraw_shotgun_meshes(sint32 weapon_type)
{
    sint32 obj;

    obj = WeaponObject(weapon_type);
    lara.mesh_ptrs[LM_RHAND] = meshes[objects[LARA].mesh_index + 2 * LM_RHAND];
    lara.back_gun = obj;
}
