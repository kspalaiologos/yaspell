
#ifndef _FNV1A_H
#define _FNV1A_H

#include "common.h"

static const u32 fnv1a_seed = 0x811C9DC5;

static inline u32 fnv1a_char(u8 c, u32 hash) {
    return (c ^ hash) * 0x01000193U;
}

static inline u32 fnv1a_bytes(u8 * s, u32 len, u32 hash) {
    for (u32 i = 0; i < len; i++)
        hash = (s[i] ^ hash) * 0x01000193U;
    return hash;
}

#endif
