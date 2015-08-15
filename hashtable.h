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

#define hashtable_MAX_LOAD_FACTOR 0.5

#define fractional_part(whole) ((whole - (long)whole))

#define hashtable_insufficient_memory_error()   \
    do{                                         \
     fprintf(stderr,"Insufficient memory.\n");  \
     exit(EXIT_FAILURE);                        \
    }while(0)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "murmur3.h"

/* Specify your hash data type here */

    typedef uint32_t hash_type;

    typedef uint32_t key_type;

/*                                  */

struct hashtable_bucket_chaining{
    void * data;
    struct hashtable_bucket_chaining * next;
};

struct hashtable_chaining{
    hash_type items,buckets;
    struct hashtable_bucket_chaining ** table;
    key_type * (*get_key)(void* data);
};

typedef struct hashtable_chaining hashtable;

static inline struct hashtable_bucket_chaining * hashtable_new_bucket_chaining(data,next)
    void * data;
    struct hashtable_bucket_chaining * next;
{
    struct hashtable_bucket_chaining * new_bucket_chaining;
    if ((new_bucket_chaining = (struct hashtable_bucket_chaining*)malloc(sizeof(struct hashtable_bucket_chaining))) == NULL)
        hashtable_insufficient_memory_error();
    new_bucket_chaining->data = data;
    new_bucket_chaining->next = next;
    return new_bucket_chaining;
}

/* Multiplicative hash using Knuth's famous 1/GoldenRatio Constant */
/*          h(x) = m * fractional_part(x * 0.618) */
static inline hash_type hashtable_hash(buckets, key)
    hash_type buckets;
    key_type * key;
{
    double fraction = 0.6180339887498948482 * *key;
    return (hash_type)(buckets * (fraction - (long)fraction));
}

struct hashtable_chaining * hashtable_Init(get_key, init_size)
    key_type * (*get_key)(void*);
    hash_type init_size;
{
    struct hashtable_chaining * new_hashtable;
    
    if((new_hashtable = (struct hashtable_chaining*) malloc(sizeof(hashtable))) == NULL)
        hashtable_insufficient_memory_error();
        
#warning maybe replaceable with rehash()
    new_hashtable->items        = 0;
    new_hashtable->buckets      = init_size;
    new_hashtable->get_key      = get_key;
        
    if ((new_hashtable->table   = (struct hashtable_bucket_chaining **)malloc(sizeof(struct hashtable_bucket_chaining) * init_size)) == NULL)
        hashtable_insufficient_memory_error();
        
    return new_hashtable;
}

void hashtable_Insert(_hashtable, data)
    struct hashtable_chaining * _hashtable;
    void * data;
{
#warning check for loadfactor
#warning dynamic resize maybe
    hash_type hash = hashtable_hash(_hashtable->buckets, _hashtable->get_key(data)); /*hashtable_hash_f(_hashtable->buckets, *(double*)data);*/
    _hashtable->table[hash] = hashtable_new_bucket_chaining(data, _hashtable->table[hash]);
}

void * hashtable_Query(_hashtable, data, compar)
    struct hashtable_chaining * _hashtable;
    void * data;
    int (*compar)(void*,void*);
{
    hash_type hash = hashtable_hash(_hashtable->buckets, data);
    struct hashtable_bucket_chaining * iterator = _hashtable->table[hash];
    
    while(iterator){
        if(compar(iterator->data, data) == 0 ) return iterator;
        else iterator = iterator->next;
    }
    
    return NULL;
}

void hashtable_Delete(_hashtable, data, compar, destroy)
    struct hashtable_chaining * _hashtable;
    void * data;
    int (*compar)(void*,void*);
    void (*destroy)(void* data);
{
    hash_type hash = hashtable_hash(_hashtable->buckets, data);
    struct hashtable_bucket_chaining * temp = _hashtable->table[hash], * prev = NULL;
    
    while(temp){
        if(compar(temp->data,data)==0)break;
        else prev = temp, temp = temp->next;
    }
    
    if (temp){
        if (prev) prev ->next = temp->next;
        else _hashtable->table[hash] = temp->next;
                
        destroy(temp->data);
        free(temp);
    }
}

#endif
