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

#include "hashtable.h"

#ifndef C_HASH_HASHFUNC_H
#define C_HASH_HASHFUNC_H

hash_type hash_int8_t(void * _int8_t);
hash_type hash_int16_t(void * _int16_t);
hash_type hash_int32_t(void * _int32_t);
hash_type hash_int64_t(void * _int64_t);

hash_type hash_uint8_t(void * _uint8_t);
hash_type hash_uint16_t(void * _uint16_t);
hash_type hash_uint32_t(void * _uint32_t);
hash_type hash_uint64_t(void * _uint64_t);

hash_type hash_char(void * _char);
hash_type hash_short(void * _short);
hash_type hash_int(void * _int);
hash_type hash_float(void * _float);
hash_type hash_double(void * _double);

hash_type hash_string(void * _string);

#endif //C_HASH_HASHFUNC_H
