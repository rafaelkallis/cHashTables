/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

/*
 * This code is derived from the full C implementation, which is in turn
 * derived from the canonical C++ PCG implementation. The C++ version
 * has many additional features and is preferable if you can use C++ in
 * your project.
 */

#ifndef PCG_BASIC_H_INCLUDED
#define PCG_BASIC_H_INCLUDED 1

#include <inttypes.h>
#include <time.h>

#if __SIZEOF_INT128__
    typedef __uint128_t pcg128_t;
    #define PCG_128BIT_CONSTANT(high,low)   ((((pcg128_t)high) << 64) + low)
    #define PCG_HAS_128BIT_OPS 1
#endif

#if __cplusplus
extern "C" {
#endif

struct pcg_state_setseq_64 {    
    uint64_t state;             
    uint64_t inc;
};

typedef struct pcg_state_setseq_64 pcg32_random_t;

    
#define PCG32_INITIALIZER   { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }
    
static pcg32_random_t pcg32_global = PCG32_INITIALIZER;

uint32_t pcg32_random(){
    uint64_t oldstate = pcg32_global.state;
    
    pcg32_global.state = oldstate * 6364136223846793005ULL + pcg32_global.inc;
    
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = (uint64_t)oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
   
uint32_t pcg32_random_bounded(uint32_t bound){
    uint32_t threshold = -bound % bound;
    
    while(1) {
        uint32_t r = pcg32_random();
        if (r >= threshold) return r % bound;
    }
}
    
void pcg32_srandom(){
    uint64_t initstate  = time(NULL) ^ (intptr_t)&printf;
    uint64_t initseq    = 5;
    
    pcg32_global.state = 0U;
    pcg32_global.inc = (initseq << 1u) | 1u;
    pcg32_random();
    pcg32_global.state += initstate;
    pcg32_random();
}

#if PCG_HAS_128BIT_OPS
    
    struct pcg_state_setseq_128 {
        pcg128_t state;
        pcg128_t inc;
    };
    
    typedef struct pcg_state_setseq_128     pcg64_random_t;
    
#define PCG_DEFAULT_MULTIPLIER_128 PCG_128BIT_CONSTANT(2549297995355413924ULL,4865540595714422341ULL)
    
#define PCG64_INITIALIZER   {PCG_128BIT_CONSTANT(0x979c9a98d8462005ULL, 0x7d3e9cb6cfe0549bULL),       \
                             PCG_128BIT_CONSTANT(0x0000000000000001ULL, 0xda3e39cb94b95bdbULL) }
    
    static pcg64_random_t pcg64_global = PCG64_INITIALIZER;
    
    uint64_t pcg64_random()
    {
        uint64_t oldstate   = pcg64_global.state;
        pcg64_global.state  = oldstate * PCG_DEFAULT_MULTIPLIER_128 + pcg64_global.inc;
        
        uint64_t xorshifted = (((pcg128_t)oldstate >> 64u)) ^ oldstate;
        uint32_t rot        = ((pcg128_t)oldstate >> 122u);
        
        return (xorshifted >> rot) | (xorshifted << ((- rot) & 63));
    }
    
    uint64_t pcg64_random_bounded(uint64_t bound){
        uint64_t threshold = -bound % bound;
        
        while(1){
            uint64_t r = pcg64_random();
            if (r >= threshold) return r % bound;
        }
    }
    
    void pcg64_srandom(){
        uint64_t initstate  = time(NULL) ^ (intptr_t)&printf;
        uint64_t initseq    = 5;
        
        pcg64_global.state = 0U;
        pcg64_global.inc = (initseq << 1u) | 1u;
        pcg64_random();
        pcg64_global.state += initstate;
        pcg64_random();
    }
    
#else   // PCG_HAS_128BIT_OPS
    
    #define PCG64_INITIALIZER_FALLBACK { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }
    #define PCG_64BIT_CONSTANT(high,low)    ((((uint64_t)high) << 32) + low)
    
    static pcg32_random_t pcg64_global = PCG64_INITIALIZER_FALLBACK;
    
    uint64_t pcg64_random()
    {
        return PCG_64BIT_CONSTANT(pcg32_random(),pcg32_random());
    }
    
    uint64_t pcg64_random_bounded(uint64_t bound)
    {
        uint64_t threshold = -bound % bound;
        
        while(1) {
            uint64_t r = PCG_64BIT_CONSTANT(pcg32_random(),pcg32_random());
            if (r >= threshold) return r % bound;
        }
    }
    
    void pcg64_srandom(){
        pcg32_srandom();
    }

    
#endif // PCG_HAS_128BIT_OPS

#if __cplusplus
}
#endif // __cplusplus

#endif // PCG_BASIC_H_INCLUDED
