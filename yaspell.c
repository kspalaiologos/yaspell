
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "email.h"
#include "dict.h"
#include "transform.h"

int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <dictionary> <input file>\n", argv[0]);
        return 1;
    }
    FILE * src = fopen(argv[1], "r");
    assert(src);
    dict * d = dict_create(src);
    assert(d);
    fclose(src);

    printf("Dictionary loaded.\n");

    FILE * input = fopen(argv[2], "r");
    assert(input);

    u8 word_buf[128];
    s32 word = -1;

    for(int c; (c = fgetc(input)) != EOF;) {
        ungetc(c, input);
        u32 email_len = find_email(input);
        if (email_len) {
            printf("E-mail address of length %u found.\n", email_len);
            continue;
        }
        u32 url_len = find_url(input);
        if (url_len) {
            printf("URL of length %u found.\n", url_len);
            continue;
        }
        c = fgetc(input);
        if ((isalpha(c) || c == '\'') && word == -1) {
            word = ftell(input);
        } else if (!(isalnum(c) || c == '\'') && word != -1) {
            u32 len = ftell(input) - word;
            if (len > 64) {
                // Word too long, we will happily skip it assuming that it was probably not meant to be spellchecked.
                word = -1;
                continue;
            }
            fseek(input, word - 1, SEEK_SET);
            fread(word_buf, 1, len, input);
            // Determine if there are any alphabetic characters in the word
            s8 valid = 0;
            for (u32 j = 0; j < len; j++) {
                if (isalpha(word_buf[j]))
                    { valid = 1; break; }
            }
            if(!valid) {
                word = -1;
                continue;
            }
            word_buf[len] = '\0';
            // Is the word in the dictionary?
            if (transform_and_check(d, word_buf)) { word = -1; continue; }
            // This can not be saved. Offer completions.
            completion c = dict_myers(d, word_buf);
            if (c.distances[0] + 1 >= len) {
                // No candidate is close enough to the word.
                // Probably random gibberish, we will skip it.
                // This should be behind a CLI knob.
                word = -1;
                continue;
            }
            printf("Misspelled word: `%s'\n", word_buf);
            printf("Candidates: ");
            for (u32 i = 0; i < 10; i++) {
                if (c.candidates[i]) {
                    printf("`%s' ", c.candidates[i]);
                }
            }
            printf("\n");
            for (u32 i = 0; i < 10; i++) {
                if (c.candidates[i]) {
                    printf("%d ", c.distances[i]);
                }
            }
            printf("\n");
            word = -1;
        }
    }

    fclose(input);
    dict_free(d);
    return 0;
}

