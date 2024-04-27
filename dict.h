
#ifndef _DICT_H
#define _DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct dict dict;

typedef struct {
    u8 * word;
    s32 distances[10];
    u8 * candidates[10];
} completion;

dict * dict_create(FILE * source);
s8 dict_find(dict * d, u8 * word);
void dict_free(dict * d);
completion dict_myers(dict * d, u8 * word);

#endif

