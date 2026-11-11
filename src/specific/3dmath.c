#include "3dmath.h"

void SetupZRange(sint32 znear, sint32 zfar)
{
    f_zfar = (float)zfar;
    f_znear = (float)znear;
    phd_zfar = zfar;
    phd_znear = znear;
    f_perspoznear = f_persp / f_znear;
    f_mznear = (float)(znear >> W2V_SHIFT);
    f_mzfar = (float)(zfar >> W2V_SHIFT);
    f_mperspoznear = f_mpersp / f_mznear;
    f_moneoznear = mone / f_mznear;
    f_b = f_mzfar * f_mznear * 0.99F / (f_mznear - f_mzfar);
    f_a = 0.005F - f_b / f_mznear;
    f_b = -f_b;
    f_boo = f_b / mone;
}
