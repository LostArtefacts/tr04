#include "lighting.h"
#include "3dmath.h"

void ApplyMatrix(sint32 *matrix, PHD_VECTOR *start, PHD_VECTOR *dest)
{
    dest->x = (start->x * matrix[M00] + start->y * matrix[M01] + start->z * matrix[M02]) >> W2V_SHIFT;
    dest->y = (start->x * matrix[M10] + start->y * matrix[M11] + start->z * matrix[M12]) >> W2V_SHIFT;
    dest->z = (start->x * matrix[M20] + start->y * matrix[M21] + start->z * matrix[M22]) >> W2V_SHIFT;
}
