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
#include "../include/hashtable.h"

static inline struct hashtable_bucket * hashtable_new_bucket(void * data,
                                                             struct hashtable_bucket * next){
    struct hashtable_bucket * new_bucket;
    if ((new_bucket = (struct hashtable_bucket*)malloc(sizeof(struct hashtable_bucket))) == NULL)
        hashtable_insufficient_memory_error();
    new_bucket->data = data;
    new_bucket->next = next;
    return new_bucket;
}

static inline hash_type hashtable_hash(hashtable * _hashtable,
                                       void * data) {
    return (hash_type)(_hashtable->seed * _hashtable->hash_func(data)) >> (HASHTABLE_WORD_SIZE - _hashtable->bucket_size_exponent);
}

static void hashtable_rehash(struct hashtable * _hashtable,
                             uint8_t old_size_exponent) {
    hash_type i,hash;
    struct hashtable_bucket *temp,*prev;
    _hashtable->seed = HASHTABLE_RANDOM; /* Optional, increases Security*/

    for(i=0;i<1<<old_size_exponent;i++){
        for(temp = _hashtable->table[i], prev=NULL;
            temp;
            prev = i==hash?temp:NULL, temp = i==hash?temp->next:_hashtable->table[i])
        {
            hash=hashtable_hash(_hashtable,temp->data);
            if(hash!=i){
                if(prev)prev->next = temp->next;
                else _hashtable->table[i]=temp->next;

                temp->next = _hashtable->table[hash];
                _hashtable->table[hash]=temp;
            }
        }
    }
}

static void* hashtable_realloc_zero(void * pBuffer,
                                    size_t oldSize,
                                    size_t newSize)
{
    void* pNew = realloc(pBuffer, newSize);
    if ( newSize > oldSize && pNew ) {
        size_t diff = newSize - oldSize;
        void* pStart = ((char*)pNew) + oldSize;
        memset(pStart, 0, diff);
    }
    return pNew;
}

static void hashtable_expand(struct hashtable * _hashtable) {
    struct hashtable_bucket **safe;

    _hashtable->bucket_size_exponent++;
    if((safe =
                (struct hashtable_bucket**)hashtable_realloc_zero
                        (_hashtable->table,
                         sizeof(struct hashtable_bucket*) * (1<<(_hashtable->bucket_size_exponent-1)),
                         sizeof(struct hashtable_bucket*)*(1<<_hashtable->bucket_size_exponent))
       )== NULL) hashtable_insufficient_memory_error();

    _hashtable->table = safe;
    hashtable_rehash(_hashtable, _hashtable->bucket_size_exponent-1);
}

static void hashtable_collapse(struct hashtable * _hashtable){
    struct hashtable_bucket **safe;

    _hashtable->bucket_size_exponent--;
    hashtable_rehash(_hashtable, _hashtable->bucket_size_exponent+1);
    if((safe =
                (struct hashtable_bucket**)hashtable_realloc_zero
                        (_hashtable->table,
                         sizeof(struct hashtable_bucket*) * (1<<(_hashtable->bucket_size_exponent+1)),
                         sizeof(struct hashtable_bucket*)*(1<<_hashtable->bucket_size_exponent))
       )== NULL) hashtable_insufficient_memory_error();

    _hashtable->table = safe;
}

static double hashtable_get_cccupied_ratio(struct hashtable * _hashtable){
    hash_type i;
    uint32_t occupied;
    for(occupied = 0, i = 0; i < 1<<_hashtable->bucket_size_exponent; i++){
        if(_hashtable->table[i]) occupied++;
        if(i == HASHTABLE_MAX_HASH)break;
    }
    return (double)occupied/(1<<_hashtable->bucket_size_exponent);
}

static hash_type hashtable_get_biggest_chain(struct hashtable * _hashtable){
    hash_type i;
    uint32_t max_global,max_chain;
    struct hashtable_bucket * temp;
    for(i = 0, max_global=0; i < 1<<_hashtable->bucket_size_exponent; i++){
        for(temp = _hashtable->table[i], max_chain=0; temp; temp=temp->next){
            max_chain++;
        }
        max_global = max_chain > max_global ? max_chain : max_global;
        if(i == HASHTABLE_MAX_HASH)break;
    }
    return max_global;
}

static double hashtable_get_loadfactor(struct hashtable * _hashtable){
    return (double)_hashtable->items/(1<<_hashtable->bucket_size_exponent);
}

static double hashtable_get_occupied_ratio(struct hashtable * _hashtable){
    uint_fast64_t total = ((uint_fast64_t)1<<_hashtable->bucket_size_exponent);
    uint_fast64_t hits = 0;
    while(--total+1){
        hits = _hashtable->table[total]!=NULL? hits+1 : hits;
    }
    return (double)hits/(1<<_hashtable->bucket_size_exponent);
}

struct hashtable * hashtable_init(hash_type (*hash_func)(void * data),
                                  bool (*equal_func)(void * data1, void * data2)) {
    return hashtable_init_size(hash_func,equal_func,HASHTABLE_DEFAULT_INIT_SIZE);
}

struct hashtable * hashtable_init_size(hash_type (*hash_func)(void * data),
                                       bool (*equal_func)(void * data1, void * data2),
                                       hash_type init_size) {
    if(!nondeterministic_seed) HASHTABLE_SRANDOM, nondeterministic_seed = true;

    struct hashtable * new_hashtable;

    if((new_hashtable = (struct hashtable*) malloc(sizeof(struct hashtable))) == NULL)
        hashtable_insufficient_memory_error();

    new_hashtable->items                = 0;
    new_hashtable->bucket_size_exponent = (uint8_t)ceil(log2(init_size < 2 ? 2 : init_size));
    new_hashtable->seed                 = HASHTABLE_RANDOM;
    new_hashtable->hash_func            = hash_func;
    new_hashtable->equal_func           = equal_func;

    if ((new_hashtable->table=(struct hashtable_bucket **)calloc(1<<new_hashtable->bucket_size_exponent,sizeof(struct hashtable_bucket*))) == NULL)
        hashtable_insufficient_memory_error();

    return new_hashtable;
}

void hashtable_insert(struct hashtable * _hashtable,
                      void * data){
    if( _hashtable->items/(1<<_hashtable->bucket_size_exponent) >= HASHTABLE_LOAD_FACTOR)
        hashtable_expand(_hashtable);
    hash_type hash = hashtable_hash(_hashtable,data);
    _hashtable->table[hash] = hashtable_new_bucket(data, _hashtable->table[hash]);
    _hashtable->items++;
}

void * hashtable_query(struct hashtable * _hashtable,
                       void * data) {
    hash_type hash = hashtable_hash(_hashtable,data);
    struct hashtable_bucket * temp = _hashtable->table[hash];

    while(temp){
        if(_hashtable->equal_func(temp->data,data)) return temp->data;
        else temp = temp->next;
    }
    return NULL;
}

void hashtable_delete(struct hashtable * _hashtable,
                      void * data,
                      void (*destroy)(void* data)) {
    hash_type hash = hashtable_hash(_hashtable,data);
    struct hashtable_bucket * temp = _hashtable->table[hash], * prev = NULL;

    while(temp){
        if(_hashtable->equal_func(temp->data,data))break;
        else prev = temp, temp = temp->next;
    }
    if (temp){
        if (prev) prev->next = temp->next;
        else _hashtable->table[hash] = temp->next;

        destroy(temp->data);
        free(temp);
    }
    if(_hashtable->items/(1<<_hashtable->bucket_size_exponent) <= HASHTABLE_LOAD_FACTOR / 4)
        hashtable_collapse(_hashtable);
}

void hashtable_stats(struct hashtable * _hashtable) {
    double nBuckets = 1<<_hashtable->bucket_size_exponent;
    double occupiedR = hashtable_get_occupied_ratio(_hashtable);
    printf(" * Hashtable Statistics *\n");
    printf("——————————————————————————\n");
    printf("Seed         = %llu \n",(uint64_t)_hashtable->seed);
    printf("#Items       = %llu \n",(uint64_t)_hashtable->items);
    printf("#Buckets     = %llu \n",(uint64_t)nBuckets);
    printf("BiggestChain = %llu \n",(uint64_t)hashtable_get_biggest_chain(_hashtable));
    printf("%%Occupied    = %0.2f%% \n",100*occupiedR);
    printf("%%Empty       = %0.2f%% ",100*(1-occupiedR));
    printf("(Expected: %0.2f%%, Diff = %+0.2f%%)\n",
           100*pow(1-1/nBuckets, _hashtable->items),
           100*(1-occupiedR)-100*pow(1-1/nBuckets, _hashtable->items));
    printf("LoadFactor   = %0.2f\n",hashtable_get_loadfactor(_hashtable));
    printf("——————————————————————————\n");

}

void hashtable_print(struct hashtable * _hashtable) {
    hash_type i;
    struct hashtable_bucket * temp;

    printf(" * Hashtable Content *\n");
    printf("———————————————————————");

    for(i=0;i<1<<_hashtable->bucket_size_exponent;i++){
        printf("\n%llu:",i);
        for(temp=_hashtable->table[i];temp;temp=temp->next){
            printf("->(%llu)",*(uint64_t *)temp->data);
        }
        if(i == HASHTABLE_MAX_HASH)break;
    }
    printf("\n———————————————————————\n");
}

void hashtable_optimize(struct hashtable * _hashtable) {
    while((1-hashtable_get_occupied_ratio(_hashtable))-pow(1-1/(double)(1<<_hashtable->bucket_size_exponent), _hashtable->items) > 0)
        hashtable_rehash(_hashtable, _hashtable->bucket_size_exponent);
}