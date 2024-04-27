
#ifndef _MYERS_H
#define _MYERS_H

#include "common.h"
#include <string.h>

static s32 myers(u8 * t, s32 n, u8 * p, s32 m) {
    if (n > m) {
        u8 * tmp = t; t = p; p = tmp;
        s32 tmp2 = n; n = m; m = tmp2;
    }
    u32 pv, mv, ph, mh, xv, xh;
    u32 eq, hb, peq[256];
    s32 score =  m;
    u8 i, j;
    memset(peq, 0, sizeof(peq));
    Fi(m, peq[p[i]] |= (u32) 1 << i)
    pv = -1; mv = 0; hb = 1 << (m - 1);
    Fi(n,
        eq = peq[t[i]]; xv = eq | mv;
        xh = (((eq & pv) + pv) ^ pv) | eq;
        ph = mv | ~(xh | pv); mh = pv & xh;
        if (ph & hb) score++; if (mh & hb) score--;
        ph <<= 1; mv = ph & (pv = (mh << 1) | ~ (xv | ph))
    )
    return score;
}

#endif
