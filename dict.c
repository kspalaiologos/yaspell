
#include "dict.h"
#include "fnv1a.h"
#include "myers.h"
#include "vector.h"

#include <stddef.h>
#include <errno.h>

#define LRU_MAX 64
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

struct dict {
    u8 * string;
    u32 * hash_table;
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
    if(!d->string) { free(d); return NULL; }
    
    u8 * line = NULL;
    u32 len = 0, size = 0;
    while (d_getdelim(&line, &len, '\n', source) != -1) {
        int length = strlen(line);
        if (length > 31) {
            fprintf(stderr, "Dictionary line too long: %s, skipped.\n", line);
            continue;
        }
        memcpy(d->string + size, line, length - 1);
        size += length - 1;
        d->string[size] = '\0';
        size++; d->words++;
    }
    free(line);

    d->hash_table = calloc(d->words + d->words / 8, sizeof(unsigned));
    if(!d->hash_table) { free(d->string); free(d); return NULL; }

    for (u32 i = 0; i < size; i++) {
        u32 wlen = strlen(d->string + i);
        u32 hash = fnv1a_bytes(d->string + i, wlen, fnv1a_seed);
        while (d->hash_table[hash % (d->words + d->words / 8)] != 0)
            hash++;
        d->hash_table[hash % (d->words + d->words / 8)] = i;
        i += wlen;
    }

    return d;
}

s8 dict_find(dict * d, u8 * word) {
    u32 hash = fnv1a_bytes(word, strlen(word), fnv1a_seed);
    while (d->hash_table[hash % (d->words + d->words / 8)] != 0) {
        u32 i = d->hash_table[hash % (d->words + d->words / 8)];
        u32 j = 0;
        while (d->string[i] && d->string[i] == word[j])
            i++, j++;
        if (!d->string[i] && !word[j]) return 1;
        hash++;
    }
    return 0;
}

u8 * strdup(const u8 * s) {
    u32 len = strlen(s);
    u8 * t = malloc(len + 1);
    if (!t) return NULL;
    memcpy(t, s, len + 1);
    return t;
}

completion dict_myers(dict * d, u8 * word) {
    // Check if LRU contains the word.
    for (u32 i = 0; i < vector_size(d->lru); i++) {
        completion cmp = d->lru[i];
        if (strcmp(word, cmp.word) == 0) {
            for (u32 j = i; j > 0; j--) {
                d->lru[j] = d->lru[j - 1];
            }
            d->lru[0] = cmp;
            return cmp;
        }
    }
    completion c;
    memset(c.candidates, 0, 10 * sizeof(u8 *));
    for (u32 i = 0; i < 10; i++) {
        c.distances[i] = 0x7FFFFFFF;
    }
    u8 * wordptr = d->string;  u32 wl = strlen(word);
    for (u32 i = 0; i < d->words; i++) {
        s32 distance = myers(wordptr, strlen(wordptr), word, wl);
        for (u32 j = 0; j < 10; j++) {
            if (distance < c.distances[j]) {
                for (u32 k = 9; k > j; k--) {
                    c.distances[k] = c.distances[k - 1];
                    c.candidates[k] = c.candidates[k - 1];
                }
                c.distances[j] = distance;
                c.candidates[j] = wordptr;
                break;
            }
        }
        wordptr += strlen(wordptr) + 1;
    }
    c.word = strdup(word);
    if (vector_size(d->lru) >= LRU_MAX) {
        free(d->lru[0].word);
        vector_erase(d->lru, 0);
    }
    vector_push_back(d->lru, c);
    return c;
}

void dict_free(dict * d) {
    for (u32 i = 0; i < vector_size(d->lru); i++) {
        free(d->lru[i].word);
    }
    vector_free(d->lru);
    free(d->string);
    free(d->hash_table);
    free(d);
}

