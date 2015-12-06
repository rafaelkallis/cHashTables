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

#include "../include/hashfunc.h"

hash_type hash_int8_t(void * _int8_t){
    return *(int8_t*)_int8_t;
}

hash_type hash_int16_t(void * _int16_t){
    return *(int16_t*)_int16_t;
}
hash_type hash_int32_t(void * _int32_t){
    return *(int32_t*)_int32_t;
}
hash_type hash_int64_t(void * _int64_t){
    return *(int64_t*)_int64_t;
}
hash_type hash_uint8_t(void * _uint8_t){
    return *(uint8_t*)_uint8_t;
}

hash_type hash_uint16_t(void * _uint16_t){
    return *(uint16_t*)_uint16_t;
}
hash_type hash_uint32_t(void * _uint32_t){
    return *(uint32_t*)_uint32_t;
}
hash_type hash_uint64_t(void * _uint64_t){
    return *(uint64_t*)_uint64_t;
}

hash_type hash_char(void * _char){
    return *(char*)_char;
}
hash_type hash_short(void * _short){
    return *(short*)_short;
}
hash_type hash_int(void * _int){
    return *(int*)_int;
}
hash_type hash_float(void * _float){
    return *(float*)_float;
}
hash_type hash_double(void * _double){
    return *(double*)_double;
}

hash_type hash_string(void * _string){
    unsigned long hash = 5381;
    int c;
    unsigned char * str = _string;

    while ((c = *str++))
        hash = hash * 33 + c;

    return (hash_type)hash;
}