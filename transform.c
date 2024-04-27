
#include "dict.h"
#include "common.h"

#include <ctype.h>

typedef void (* forward)(u8 * buf, u32 len, void ** data);
typedef void (* backward)(u8 * buf, u32 len, void ** data);

struct transform {
    forward f;
    backward b;
};

// Flip the case of the first letter.
static void change_case_forward(u8 * buf, u32 len, void ** data) {
    if (isalpha(buf[0]) && len > 0) buf[0] ^= 0x20;
}
static void change_case_backward(u8 * buf, u32 len, void ** data) {
    if (isalpha(buf[0]) && len > 0) buf[0] ^= 0x20;
}

// All caps.
static void all_caps_forward(u8 * buf, u32 len, void ** data) {
    s8 all_upcase = 1;
    for (u32 i = 0; i < len; i++) if (isalpha(buf[i]) && !isupper(buf[i])) { all_upcase = 0; break; }
    if (all_upcase) for (u32 i = 0; i < len; i++) if (isalpha(buf[i])) buf[i] = tolower(buf[i]);
    *data = malloc(1); if (!*data) abort(); ((u8 *) *data)[0] = all_upcase;
}
static void all_caps_backward(u8 * buf, u32 len, void ** data) {
    if (*((u8 *) *data)) for (u32 i = 0; i < len; i++) if (isalpha(buf[i])) buf[i] = toupper(buf[i]);
    free(*data);
}

// Trailing apostrophe
static void trailing_apostrophe_forward(u8 * buf, u32 len, void ** data) {
    *data = malloc(1); if (!*data) abort();
    if (len == 0 || buf[len - 1] != '\'') { ((u8 *) *data)[0] = 0; return; }
    buf[len - 1] = '\0'; ((u8 *) *data)[0] = 1;
}
static void trailing_apostrophe_backward(u8 * buf, u32 len, void ** data) {
    if (*((u8 *) *data)) buf[len - 1] = '\'';
    free(*data);
}

// Leading apostrophe
static void leading_apostrophe_forward(u8 * buf, u32 len, void ** data) {
    *data = malloc(1); if (!*data) abort();
    if (len == 0 || buf[0] != '\'') { ((u8 *) *data)[0] = 0; return; }
    memmove(buf, buf + 1, len - 1); buf[len - 1] = '\0'; ((u8 *) *data)[0] = 1;
}
static void leading_apostrophe_backward(u8 * buf, u32 len, void ** data) {
    if (*((u8 *) *data)) { memmove(buf + 1, buf, len - 1); buf[0] = '\''; }
    free(*data);
}

// All transforms
struct transform transforms[] = {
    { trailing_apostrophe_forward, trailing_apostrophe_backward },
    { leading_apostrophe_forward, leading_apostrophe_backward },
    { change_case_forward, change_case_backward },
    { all_caps_forward, all_caps_backward },
};

// Apply all permutations of transforms to the word. Check if any matches.
s8 transform_and_check(dict * d, u8 * word) {
    // First check the word as is.
    if (dict_find(d, word)) return 1;
    u32 len = strlen(word);
    for (u32 perm = 0b0000; perm < 0b10000; perm++) {
        u8 buf[128]; memcpy(buf, word, len + 1);
        void * data[4] = { NULL };
        for (u32 i = 0; i < 4; i++) {
            if (perm & (1 << i)) transforms[i].f(buf, len, &data[i]);
        }
        if (dict_find(d, buf)) return 1;
        for (u32 i = 0; i < 4; i++) {
            if (perm & (1 << i)) transforms[i].b(buf, len, &data[i]);
        }
    }
    return 0;
}

