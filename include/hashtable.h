/*
 * Copyright (c) 2015 Rafael Kallis <rafael@rafaelkallis.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#define hashtable_insufficient_memory_error()   \
    do{                                         \
     fprintf(stderr,"Insufficient memory.\n");  \
     exit(EXIT_FAILURE);                        \
    }while(0)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* Modify at your own risk */

    typedef uint64_t                            hash_type;

    #define HASHTABLE_DEFAULT_INIT_SIZE         (2)

    #define HASHTABLE_WORD_SIZE                 (64)

    #define HASHTABLE_MAX_HASH                  (UINT64_MAX)

    #define HASHTABLE_LOAD_FACTOR               1

    #define HASHTABLE_RANDOM                    (((uint64_t)rand()<<32)+(uint64_t)rand())
    #define HASHTABLE_SRANDOM                   (srand(time(NULL)^(intptr_t)&printf))

    static bool nondeterministic_seed = false;

/*                                  */

struct hashtable_bucket{
    void *                      data;
    struct hashtable_bucket *   next;
};

struct hashtable{
    hash_type                   items,seed;
    uint8_t                     bucket_size_exponent;
    struct hashtable_bucket **  table;
    hash_type                   (*hash_func)(void * data);
    bool                        (*equal_func)(void * data1, void * data2);
};

typedef struct hashtable hashtable;

struct hashtable *          hashtable_init(hash_type (*hash_func)(void * data),
                                           bool (*equal_func)(void * data1,void * data2));
struct hashtable *          hashtable_init_size(hash_type (*hash_func)(void * data),
                                                bool (*equal_func)(void * data1,void * data2),
                                                hash_type init_size);
void                        hashtable_insert(struct hashtable * _hashtable,
                                             void * data);
void *                      hashtable_query(struct hashtable * _hashtable,
                                            void * data);
void                        hashtable_delete(struct hashtable * _hashtable,
                                             void * data,
                                             void (*destroy)(void* data));
void                        hashtable_stats(struct hashtable * _hashtable);
void                        hashtable_print(struct hashtable * _hashtable);
void                        hashtable_optimize(struct hashtable * _hashtable);

#endif
