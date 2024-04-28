
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "email.h"
#include "dict.h"
#include "transform.h"
#include "vector.h"

int main(int argc, char * argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <dictionary1> <dictionary2> ...\n", argv[0]);
        return 1;
    }
    
    vector(dict *) d = NULL;
    for (int i = 2; i < argc; i++) {
        FILE * f = fopen(argv[i], "r");
        if (!f) {
            fprintf(stderr, "Failed to open dictionary `%s'.\n", argv[i]);
            return 1;
        }
        dict * d_ = dict_create(f);
        if (!d_) {
            fprintf(stderr, "Failed to load dictionary `%s'.\n", argv[i]);
            return 1;
        }
        vector_push_back(d, d_);
        fclose(f);
    }

    FILE * input = fopen(argv[1], "r");
    assert(input);

    u8 word_buf[128];
    s32 word = -1;

    for(int c; (c = fgetc(input)) != EOF;) {
        ungetc(c, input);
        find_email(input);  find_url(input);
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
            Fi(vector_size(d), If (transform_and_check(d[i], word_buf), word = -1; break))
            If (word == -1, continue)
            // This can not be saved. Offer completions.
            Fi(vector_size(d),
                completion c = dict_myers(d[i], word_buf);
                If (c.distances[0] + 1 >= len, continue)
                printf("From dictionary #%d: Misspelled word: `%s'. Candidates:\n", i, word_buf);
                Fx(10, If (c.candidates[i], printf("`%s' ", c.candidates[x])))
                printf("\n"))
            word = -1;
        }
    }

    fclose(input);
    Fi(vector_size(d), dict_free(d[i]))
    vector_free(d);
    return 0;
}

