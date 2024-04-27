
#include "dict.h"
#include "fnv1a.h"
#include "myers.h"
#include "vector.h"
#include "shim.h"

#include <stddef.h>
#include <errno.h>

#define LRU_MAX 64

struct dict {
    u8 * string;
    u32 * ht;
    u32 words;
    vector(completion) lru;
};

dict * dict_create(FILE * source) {
    dict * d = malloc(sizeof(dict));
    if(!d) { return NULL; }
    memset(d, 0, sizeof(dict));
    fseek(source, 0, SEEK_END);
    u32 file_size = ftell(source);
    fseek(source, 0, SEEK_SET);
    d->string = malloc(file_size);
    If(!d->string, free(d); return NULL)
    u8 * line = NULL; u32 len = 0, size = 0;
    while (d_getdelim(&line, &len, '\n', source) != -1) {
        int length = strlen(line);
        If(length > 31,
            fprintf(stderr, "Dictionary line too long: %s, skipped.\n", line);
            continue)
        memcpy(d->string + size, line, length - 1);
        size += length - 1; d->string[size++] = '\0'; d->words++;
    }
    free(line);
    d->ht = calloc(d->words + d->words / 8, sizeof(unsigned));
    If(!d->ht, free(d->string); free(d); return NULL)
    Fi(size,
        u32 wlen = strlen(d->string + i), hash = fnv1a_bytes(d->string + i, wlen, fnv1a_seed);
        while (d->ht[hash % (d->words + d->words / 8)]) hash++;
        d->ht[hash % (d->words + d->words / 8)] = i; i += wlen)
    return d;
}

s8 dict_find(dict * d, u8 * word) {
    for (u32 hash = fnv1a_bytes(word, strlen(word), fnv1a_seed), i, j;
        d->ht[hash % (d->words + d->words / 8)]; hash++) {
        for (i = d->ht[hash % (d->words + d->words / 8)], j = 0;
             d->string[i] && d->string[i] == word[j]; i++, j++);
        If(!d->string[i] && !word[j], return 1)
    }
    return 0;
}

completion dict_myers(dict * d, u8 * word) {
    // Check if LRU contains the word.
    Fi(vector_size(d->lru),
        completion cmp = d->lru[i];
        If(!strcmp(word, cmp.word),
            Fj(i, d->lru[j] = d->lru[j - 1])
            return d->lru[0] = cmp))
    completion c;
    memset(c.candidates, 0, 10 * sizeof(u8 *));
    Fi(10, c.distances[i] = 0x7FFFFFFF);
    u8 * wordptr = d->string;  u32 wl = strlen(word);
    Fi(d->words,
        s32 d = myers(wordptr, strlen(wordptr), word, wl);
        Fx(10, If(d < c.distances[x], Fj(9 - x,
                c.distances[j + x] = c.distances[j + x - 1];
                c.candidates[j + x] = c.candidates[j + x - 1]);
            c.distances[x] = d; c.candidates[x] = wordptr; break))
        wordptr += strlen(wordptr) + 1)
    c.word = d_strdup(word);
    If(vector_size(d->lru) >= LRU_MAX, free(d->lru[0].word); vector_erase(d->lru, 0))
    vector_push_back(d->lru, c);
    return c;
}

void dict_free(dict * d) {
    Fi(vector_size(d->lru), free(d->lru[i].word));
    vector_free(d->lru);
    free(d->string);
    free(d->ht);
    free(d);
}

