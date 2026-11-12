#include "delstuff.h"
#include "lara.h"
#include "specific/3dmath.h"

void GetLaraJointPos(PHD_VECTOR *pos, sint32 node)
{
    phd_PushMatrix();
    phd_mxptr[M00] = lara_joint_matrices[node * 12 + M00];
    phd_mxptr[M01] = lara_joint_matrices[node * 12 + M01];
    phd_mxptr[M02] = lara_joint_matrices[node * 12 + M02];
    phd_mxptr[M03] = lara_joint_matrices[node * 12 + M03];
    phd_mxptr[M10] = lara_joint_matrices[node * 12 + M10];
    phd_mxptr[M11] = lara_joint_matrices[node * 12 + M11];
    phd_mxptr[M12] = lara_joint_matrices[node * 12 + M12];
    phd_mxptr[M13] = lara_joint_matrices[node * 12 + M13];
    phd_mxptr[M20] = lara_joint_matrices[node * 12 + M20];
    phd_mxptr[M21] = lara_joint_matrices[node * 12 + M21];
    phd_mxptr[M22] = lara_joint_matrices[node * 12 + M22];
    phd_mxptr[M23] = lara_joint_matrices[node * 12 + M23];
    phd_TranslateRel(pos->x, pos->y, pos->z);
    pos->x = phd_mxptr[M03] >> W2V_SHIFT;
    pos->y = phd_mxptr[M13] >> W2V_SHIFT;
    pos->z = phd_mxptr[M23] >> W2V_SHIFT;
    pos->x += lara_item->pos.x_pos;
    pos->y += lara_item->pos.y_pos;
    pos->z += lara_item->pos.z_pos;
    phd_PopMatrix();
}
