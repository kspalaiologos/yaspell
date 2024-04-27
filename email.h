
#ifndef _EMAIL_H_
#define _EMAIL_H_

#include "vector.h"
#include "common.h"
#include <stdio.h>

// Return 0 if the string does not start with an e-mail address.
// Return the length of the e-mail address if it does.
// Assumes that the input is seekable and moves the file pointer past the end of the e-mail address.
u32 find_email(FILE * in);

// Return 0 if the string does not start with a URL.
// Return the length of the URL if it does.
// Assumes that the input is seekable and moves the file pointer past the end of the URL.
u32 find_url(FILE * in);

#endif

