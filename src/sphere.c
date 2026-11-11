#include "sphere.h"
#include "lara.h"

sint32 TestCollision(ITEM_INFO *item, ITEM_INFO *l)
{
    SPHERE *laraSpheres;
    sint32 nItemSpheres, nLaraSpheres, ir, lr, i, j, x, y, z, dx, dy, dz;
    SPHERE *itemSpheres;
    uint32 touch_bits;

    touch_bits = 0;
    nItemSpheres = GetSpheres(item, Slist, 1);
    if (l != lara_item)
        GotLaraSpheres = 0;
    if (!GotLaraSpheres) {
        nLaraSpheres = GetSpheres(l, LaraSpheres, 1);
        NumLaraSpheres = nLaraSpheres;
        if (l == lara_item)
            GotLaraSpheres = 1;
    } else {
        nLaraSpheres = NumLaraSpheres;
    }
    for (i = 0; i < nItemSpheres; i++) {
        itemSpheres = &Slist[i];
        ir = itemSpheres->r;
        if (ir > 0) {
            x = itemSpheres->x;
            y = itemSpheres->y;
            z = itemSpheres->z;
            for (j = 0; j < nLaraSpheres; j++) {
                laraSpheres = &LaraSpheres[j];
                lr = laraSpheres->r;
                if (lr > 0) {
                    dx = laraSpheres->x - x;
                    dy = laraSpheres->y - y;
                    dz = laraSpheres->z - z;
                    lr += ir;
                    if (dx * dx + dy * dy + dz * dz < lr * lr) {
                        touch_bits |= 1 << i;
                        break;
                    }
                }
            }
        }
    }
    item->touch_bits = touch_bits;
    return touch_bits;
}
