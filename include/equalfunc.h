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

#ifndef C_HASH_EQUALFUNC_H
#define C_HASH_EQUALFUNC_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

bool equal_char(void * data1, void * data2);
bool equal_short(void * data1, void * data2);
bool equal_int(void * data1, void * data2);
bool equal_float(void * data1, void * data2);
bool equal_double(void * data1, void * data2);

bool equal_int8_t(void * data1, void * data2);
bool equal_int16_t(void * data1, void * data2);
bool equal_int32_t(void * data1, void * data2);
bool equal_int64_t(void * data1, void * data2);

bool equal_uint8_t(void * data1, void * data2);
bool equal_uint16_t(void * data1, void * data2);
bool equal_uint32_t(void * data1, void * data2);
bool equal_uint64_t(void * data1, void * data2);

bool equal_string(void * data1, void * data2);

#endif //C_HASH_EQUALFUNC_H
