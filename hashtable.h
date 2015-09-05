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
#include <math.h>
#include <time.h>

/* Specify your parameters here */


    // Recommended load factor: 1
    #define hashtable_MAX_LOAD_FACTOR   1

    // uint8_t  : 256                           Max Buckets
    // uint16_t : 65'536                        Max Buckets
    // uint32_t : 4'294'967'296                 Max Buckets
    // uint64_t : 18'446'744'073'709'551'616    Max Buckets

/* Modify at your own risk */

    #define hashtable_HASH_BITS     (sizeof(hash_type)*8)

/* Portable Solution with poor randomness */

    #define hashtable_RAND_64       ((((uint64_t) rand() <<  0) & 0x000000000000FFFFull) |  \
                                     (((uint64_t) rand() << 16) & 0x00000000FFFF0000ull) |  \
                                     (((uint64_t) rand() << 32) & 0x0000FFFF00000000ull) |  \
                                     (((uint64_t) rand() << 48) & 0xFFFF000000000000ull))

    #define hashtable_RAND_32       ((((uint32_t) rand() <<  0) & 0x000000000000FFFFull) |  \
                                     (((uint32_t) rand() << 16) & 0x00000000FFFF0000ull))

    #define hashtable_RAND_16         (((uint16_t) rand() << 0) & 0x000000000000FFFFull)

    #define hashtable_RAND_8           (((uint8_t) rand() >> 8) & 0x00000000000000FFull)

    /* Compatible with BSD Based Systems */

//    #define hashtable_RAND_64       ((((uint64_t) arc4random() <<  0) & 0x00000000FFFFFFFFull) |   \
//                                     (((uint64_t) arc4random() << 32) & 0xFFFFFFFF00000000ull))
//
//    #define hashtable_RAND_32       arc4random()
//
//    #define hashtable_RAND_16       (arc4random() >> 16)
//
//    #define hashtable_RAND_8        (arc4random() >> 24)

   #define hashtable_RANDOM        (hashtable_HASH_BITS==64? hashtable_RAND_64 : (hashtable_HASH_BITS==32? hashtable_RAND_32 : (hashtable_HASH_BITS==16? hashtable_RAND_16 : hashtable_RAND_8)))

/*                                  */

static uint8_t srand_check = 0;

struct hashtable_bucket_chaining{
    void * data;
    struct hashtable_bucket_chaining * next;
};
unsigned __int128 rafael;
struct hashtable_chaining{
    hash_type items,seed;
    uint8_t bucket_size_exponent;
    struct hashtable_bucket_chaining ** table;
    void * (*get_key)(void* data);
};

typedef struct hashtable_chaining hashtable;

/* Static Functions */
static inline struct hashtable_bucket_chaining * hashtable_new_bucket_chaining(void * data, struct hashtable_bucket_chaining * next);
static inline hash_type     hashtable_hash(void * key, hash_type seed, uint8_t cap_order);
static double               hashtable_Get_Occupied_Ratio(struct hashtable_chaining * _hashtable);
static hash_type            hashtable_Get_Biggest_Chain(struct hashtable_chaining * _hashtable);
static double               hashtable_Get_Loadfactor(struct hashtable_chaining * _hashtable);
static void                 hashtable_Rehash(struct hashtable_chaining * _hashtable, uint8_t old_size_exponent, uint8_t new_size_exponent);
static void *               hashtable_realloc_zero(void * pBuffer, size_t oldSize, size_t newSize);
static void                 hashtable_Expand(struct hashtable_chaining * _hashtable);
static void                 hashtable_Collapse(struct hashtable_chaining * _hashtable);

/* Public Functions */
struct hashtable_chaining * hashtable_Init(void * (*get_key)(void*), hash_type init_size);
void                        hashtable_Insert(struct hashtable_chaining * _hashtable, void * data);
void *                      hashtable_Query(struct hashtable_chaining * _hashtable, void * data, int (*compar)(const void*, const void*));
void                        hashtable_Delete(struct hashtable_chaining * _hashtable, void * data, int (*compar)(const void*, const void*),void (*destroy)(void* data));
void                        hashtable_Stats(struct hashtable_chaining * _hashtable);
void                        hashtable_Print(struct hashtable_chaining * _hashtable);
void                        hashtable_Optimize(struct hashtable_chaining * _hashtable);

/* Beggining of Definitions */
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

static inline hash_type hashtable_hash(key, seed, cap_order)
    void * key;
    hash_type seed;
    uint8_t cap_order;
{
    return (hash_type)(seed * *(hash_type*)key) >> (hashtable_HASH_BITS - cap_order);
}

static void hashtable_Rehash(_hashtable, old_size_exponent, new_size_exponent)
    struct hashtable_chaining * _hashtable;
    uint8_t old_size_exponent;
    uint8_t new_size_exponent;
{
    hash_type i,hash;
    struct hashtable_bucket_chaining *temp,*prev;
    _hashtable->seed = hashtable_RANDOM; /* Optional, increases Security*/
    
    for(i=0;i<1<<old_size_exponent;i++){
        for(temp = _hashtable->table[i], prev=NULL;
            temp;
            prev = i==hash?temp:NULL, temp = i==hash?temp->next:_hashtable->table[i])
        {
            hash=hashtable_hash(_hashtable->get_key(temp->data), _hashtable->seed, new_size_exponent);
            if(hash!=i){
                if(prev)prev->next = temp->next;
                else _hashtable->table[i]=temp->next;

                temp->next = _hashtable->table[hash];
                _hashtable->table[hash]=temp;
            }
        }
    }
}

static void* hashtable_realloc_zero(pBuffer, oldSize, newSize)
void * pBuffer;
size_t oldSize;
size_t newSize;
{
    void* pNew = realloc(pBuffer, newSize);
    if ( newSize > oldSize && pNew ) {
        size_t diff = newSize - oldSize;
        void* pStart = ((char*)pNew) + oldSize;
        memset(pStart, 0, diff);
    }
    return pNew;
}

static void hashtable_Expand(_hashtable)
struct hashtable_chaining * _hashtable;
{
    struct hashtable_bucket_chaining **safe;
    
    _hashtable->bucket_size_exponent++;
    if((safe =
        (struct hashtable_bucket_chaining**)hashtable_realloc_zero
        (_hashtable->table,
         sizeof(struct hashtable_bucket_chaining*) * (1<<(_hashtable->bucket_size_exponent-1)),
        sizeof(struct hashtable_bucket_chaining*)*(1<<_hashtable->bucket_size_exponent))
        )== NULL) hashtable_insufficient_memory_error();
        
    _hashtable->table = safe;
    hashtable_Rehash(_hashtable, _hashtable->bucket_size_exponent-1, _hashtable->bucket_size_exponent);
}

static void hashtable_Collapse(_hashtable)
struct hashtable_chaining * _hashtable;
{
    struct hashtable_bucket_chaining **safe;
    
    _hashtable->bucket_size_exponent--;
    hashtable_Rehash(_hashtable, _hashtable->bucket_size_exponent+1, _hashtable->bucket_size_exponent);
    if((safe =
        (struct hashtable_bucket_chaining**)hashtable_realloc_zero
        (_hashtable->table,
         sizeof(struct hashtable_bucket_chaining*) * (1<<(_hashtable->bucket_size_exponent+1)),
         sizeof(struct hashtable_bucket_chaining*)*(1<<_hashtable->bucket_size_exponent))
        )== NULL) hashtable_insufficient_memory_error();
        
    _hashtable->table = safe;
}

static double hashtable_Get_Occupied_Ratio(_hashtable)
struct hashtable_chaining * _hashtable;
{
    hash_type occupied,i;
    for(occupied = 0, i = 0; i < 1<<_hashtable->bucket_size_exponent; i++)
        if(_hashtable->table[i]) occupied++;
    return (double)occupied/(1<<_hashtable->bucket_size_exponent);
}

static hash_type hashtable_Get_Biggest_Chain(_hashtable)
struct hashtable_chaining * _hashtable;
{
    hash_type max_global,max_chain,i;
    struct hashtable_bucket_chaining * temp;
    for(i = 0, max_global=0; i < 1<<_hashtable->bucket_size_exponent; i++){
        for(temp = _hashtable->table[i], max_chain=0; temp; temp=temp->next)
            max_chain++;
        max_global = max_chain > max_global ? max_chain : max_global;
    }
    return max_global;
}

static double hashtable_Get_Loadfactor(_hashtable)
struct hashtable_chaining * _hashtable;
{
    return (double)_hashtable->items/(1<<_hashtable->bucket_size_exponent);
}

struct hashtable_chaining * hashtable_Init(get_key, init_size)
void * (*get_key)(void*);
hash_type init_size;
{
    if(!srand_check)srand(time(0)),srand_check = (uint8_t) rand();
        
    struct hashtable_chaining * new_hashtable;
    
    if((new_hashtable = (struct hashtable_chaining*) malloc(sizeof(struct hashtable_chaining))) == NULL)
        hashtable_insufficient_memory_error();
        
    new_hashtable->items                = 0;
    new_hashtable->get_key              = get_key;
    new_hashtable->bucket_size_exponent = (uint8_t)ceil(log2(init_size < 2 ? 2 : init_size));
    new_hashtable->seed                 = hashtable_RANDOM;
    
    if ((new_hashtable->table=(struct hashtable_bucket_chaining **)calloc(1<<new_hashtable->bucket_size_exponent,sizeof(struct hashtable_bucket_chaining*))) == NULL)
        hashtable_insufficient_memory_error();
        
    return new_hashtable;
}

void hashtable_Insert(_hashtable, data)
    struct hashtable_chaining * _hashtable;
    void * data;
{
    if( _hashtable->items/(1<<_hashtable->bucket_size_exponent)>= hashtable_MAX_LOAD_FACTOR)
        hashtable_Expand(_hashtable);
    hash_type hash = hashtable_hash(_hashtable->get_key(data),_hashtable->seed,_hashtable->bucket_size_exponent);
    _hashtable->table[hash] = hashtable_new_bucket_chaining(data, _hashtable->table[hash]);
    _hashtable->items++;
}

void * hashtable_Query(_hashtable, data, compar)
    struct hashtable_chaining * _hashtable;
    void * data;
    int (*compar)(const void*, const void*);
{
    hash_type hash = hashtable_hash(_hashtable->get_key(data),_hashtable->seed,_hashtable->bucket_size_exponent);
    struct hashtable_bucket_chaining * temp = _hashtable->table[hash];
    
    while(temp){
        if(compar(temp->data, data) == 0 ) return temp->data;
        else temp = temp->next;
    }
    
    return NULL;
}

void hashtable_Delete(_hashtable, data, compar, destroy)
    struct hashtable_chaining * _hashtable;
    void * data;
    int (*compar)(const void*, const void*);
    void (*destroy)(void* data);
{
    hash_type hash = hashtable_hash(_hashtable->get_key(data),_hashtable->seed, _hashtable->bucket_size_exponent);
    struct hashtable_bucket_chaining * temp = _hashtable->table[hash], * prev = NULL;
    
    while(temp){
        if(!compar(temp->data,data))break;
        else prev = temp, temp = temp->next;
    }
    if (temp){
        if (prev) prev->next = temp->next;
        else _hashtable->table[hash] = temp->next;
            
        destroy(temp->data);
        free(temp);
    }
    if(_hashtable->items/(1<<_hashtable->bucket_size_exponent)<=hashtable_MAX_LOAD_FACTOR/4)
        hashtable_Collapse(_hashtable);
}

void hashtable_Stats(_hashtable)
    struct hashtable_chaining * _hashtable;
{
    double nBuckets = 1<<_hashtable->bucket_size_exponent;
    double occupiedR = hashtable_Get_Occupied_Ratio(_hashtable);
    printf(" * Hashtable Statistics *\n");
    printf("——————————————————————————\n");
    printf("Seed         = %llu \n",(uint64_t)_hashtable->seed);
    printf("#Items       = %llu \n",(uint64_t)_hashtable->items);
    printf("#Buckets     = %llu \n",(uint64_t)nBuckets);
    printf("BiggestChain = %llu \n",(uint64_t)hashtable_Get_Biggest_Chain(_hashtable));
    printf("%%Occupied    = %0.2f%% \n",100*occupiedR);
    printf("%%Empty       = %0.2f%% ",100*(1-occupiedR));
    printf("(Expected: %0.2f%%, Diff = %+0.2f%%)\n",
           100*pow(1-1/nBuckets, _hashtable->items),
           100*(1-occupiedR)-100*pow(1-1/nBuckets, _hashtable->items));
    printf("LoadFactor   = %0.2f\n",hashtable_Get_Loadfactor(_hashtable));
    printf("——————————————————————————\n");

}

void hashtable_Print(_hashtable)
struct hashtable_chaining * _hashtable;
{
    hash_type i;
    struct hashtable_bucket_chaining * temp;
    
    printf(" * Hashtable Content *\n");
    printf("———————————————————————");

    for(i=0;i<1<<_hashtable->bucket_size_exponent;i++){
        printf("\n%llu:",(uint64_t)i);
        for(temp=_hashtable->table[i];temp;temp=temp->next){
            printf("->(%d)",*(int*)_hashtable->get_key(temp->data));
        }
    }
    printf("\n———————————————————————\n");
}

void hashtable_Optimize(_hashtable)
struct hashtable_chaining * _hashtable;
{
//    double nBuckets = 1<<_hashtable->bucket_size_exponent;
    while((1-hashtable_Get_Occupied_Ratio(_hashtable))-pow(1-1/(double)(1<<_hashtable->bucket_size_exponent), _hashtable->items) > 0) hashtable_Rehash(_hashtable, _hashtable->bucket_size_exponent, _hashtable->bucket_size_exponent);
}

#endif
