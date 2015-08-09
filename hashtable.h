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

#define hashtable_MULT_CONSTANT 0.6180339887498948482
#define hashtable_MAX_LOAD_FACTOR 0.5

#define hashtable_insufficient_memory_error()   \
    do{                                         \
     fprintf(stderr,"Insufficient memory.\n");  \
     exit(EXIT_FAILURE);                        \
    }while(0)

#include <math.h>
#include <time.h>
#include "murmur3.h"

/* Specify your hash data type here */

    typedef uint32_t hash_type;

/*                                  */

enum hashtable_bucket_status{FREE, OCCUPIED, DELETED};

struct hashtable_bucket{
    enum hashtable_bucket_status status;
    void * data;
};

struct hashtable{
    hash_type seed,items,buckets;
    struct hashtable_bucket * table;
    void * (*get_key)(void* data);
};

typedef struct hashtable hashtable;

static int srand_check = 0;

static inline hash_type hashtable_hash(_hashtable, data, probe_n)
    struct hashtable * _hashtable;
    void* data;
    hash_type probe_n;
{
    hash_type hash;
    MurmurHash3_x64_128(_hashtable->get_key(data), sizeof(hash_type), _hashtable->seed, &hash);
    return _hashtable->buckets * fmod((hashtable_MULT_CONSTANT * hash)+probe_n, 1);
}

struct hashtable * hashtable_Init(get_key, init_size)
    void * (*get_key)(void*);
    hash_type init_size;
{
    if(!srand_check) srand(time(0)), srand_check = rand();
        
    struct hashtable * new_hashtable;
    
    if((new_hashtable = (struct hashtable*) malloc(sizeof(struct hashtable))) == NULL)
        hashtable_insufficient_memory_error();
        
#warning maybe replaceable with rehash()
    new_hashtable->seed         = rand();
    new_hashtable->items        = 0;
    new_hashtable->buckets      = init_size;
    new_hashtable->get_key      = get_key;
        
    if ((new_hashtable->table   = (struct hashtable_bucket *)malloc(sizeof(struct hashtable_bucket) * init_size)) == NULL)
        hashtable_insufficient_memory_error();
        
    return new_hashtable;
}

void hashtable_Insert(_hashtable, data)
    struct hashtable * _hashtable;
    void * data;
{
#warning check for loadfactor
#warning dynamic resize maybe
    register hash_type probe_n = 0, hash;
    while(_hashtable->table[hash = hashtable_hash(_hashtable, data, probe_n)].status == OCCUPIED){ probe_n++; }
    
#warning maybe data doesnt get assigned
    _hashtable->table[hash].data = data;
}

void * hashtable_Query(_hashtable, data, compar)
    struct hashtable * _hashtable;
    void * data;
    int (*compar)(void*,void*);
{
    register hash_type probe_n = 0, hash;
    
    while(_hashtable->table[hash = hashtable_hash(_hashtable, data, probe_n)].status != FREE){
        if(compar(_hashtable->table[hash].data, data) == 0 )
            return _hashtable->table[hash].data;
        else
            probe_n++;
    }
    return NULL;
}

void hashtable_Delete(_hashtable, data, compar)
    struct hashtable * _hashtable;
    void * data;
    int (*compar)(void*,void*);
{
    register hash_type probe_n = 0, hash;
    
    while(_hashtable->table[hash = hashtable_hash(_hashtable, data, probe_n)].status != FREE){
        if(compar(_hashtable->table[hash].data, data) == 0 )
            _hashtable->table[hash].status = DELETED;
        else
            probe_n++;
    }
}

#endif
