
#ifndef _SHIM_H
#define _SHIM_H

#include "common.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

s32 d_getdelim(u8 ** s, u32 * n, u8 delim, FILE * f) {
    if (!s || !n) {
        errno = EINVAL;
        return -1;
    }
    if (!*s) {
        *n = 128;
        *s = malloc(*n);
        if (!*s) {
            return -1;
        }
    }
    u32 i = 0;
    for (int c; (c = fgetc(f)) != EOF;) {
        if (i + 1 >= *n) {
            u32 m = *n;
            *n *= 2;
            u8 * t = realloc(*s, *n);
            if (!t) {
                return -1;
            }
            *s = t;
            memset(*s + m, 0, m);
        }
        (*s)[i++] = c;
        if (c == delim) {
            break;
        }
    }
    if (i == 0) {
        return -1;
    }
    (*s)[i] = 0;
    return i;
}

u8 * d_strdup(const u8 * s) {
    u32 len = strlen(s);
    u8 * t = malloc(len + 1);
    if (!t) return NULL;
    memcpy(t, s, len + 1);
    return t;
}

#endif

