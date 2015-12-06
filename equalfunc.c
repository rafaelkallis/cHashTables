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

#include "equalfunc.h"

bool equal_char(void * data1, void * data2){
    return *(char*)data1 == *(char*)data2;
}
bool equal_short(void * data1, void * data2){
    return *(short*)data1 == *(short*)data2;
}
bool equal_int(void * data1, void * data2){
    return *(int*)data1 == *(int*)data2;
}
bool equal_float(void * data1, void * data2){
    return *(float*)data1 == *(float*)data2;
}
bool equal_double(void * data1, void * data2){
    return *(double*)data1 == *(double*)data2;
}

bool equal_int8_t(void * data1, void * data2){
    return *(int8_t*)data1 = *(int8_t*)data2;
}
bool equal_int16_t(void * data1, void * data2){
    return *(int16_t*)data1 = *(int16_t*)data2;
}
bool equal_int32_t(void * data1, void * data2){
    return *(int32_t*)data1 = *(int32_t*)data2;
}
bool equal_int64_t(void * data1, void * data2){
    return *(int64_t*)data1 = *(int64_t*)data2;
}

bool equal_uint8_t(void * data1, void * data2){
    return *(uint8_t*)data1 = *(uint8_t*)data2;
}
bool equal_uint16_t(void * data1, void * data2){
    return *(uint16_t*)data1 = *(uint16_t*)data2;
}
bool equal_uint32_t(void * data1, void * data2){
    return *(uint32_t*)data1 = *(uint32_t*)data2;
}
bool equal_uint64_t(void * data1, void * data2){
    return *(uint64_t*)data1 = *(uint64_t*)data2;
}

bool equal_string(void * data1, void * data2){
    return strcmp((char*)data1,(char*)data2) == 0;
}