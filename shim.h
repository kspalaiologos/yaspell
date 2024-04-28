
#ifndef _SHIM_H
#define _SHIM_H

#include "common.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

s32 d_getdelim(u8 ** s, u32 * n, u8 delim, FILE * f);
u8 * d_strdup(const u8 * s);

struct optparse {
    char **argv;
    int permute;
    int optind;
    int optopt;
    char *optarg;
    char errmsg[64];
    int subopt;
};

enum optparse_argtype {
    OPTPARSE_NONE,
    OPTPARSE_REQUIRED,
    OPTPARSE_OPTIONAL
};

struct optparse_long {
    const char *longname;
    int shortname;
    enum optparse_argtype argtype;
};

void optparse_init(struct optparse *options, char **argv);
int optparse(struct optparse *options, const char *optstring);
int optparse_long(struct optparse *options, const struct optparse_long *longopts, int *longindex);
char *optparse_arg(struct optparse *options);

#endif

