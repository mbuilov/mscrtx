/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* utf16cvt.c */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "libutf16/utf8_to_utf16.h"
#include "libutf16/utf32_to_utf16.h"
#include "mscrtx/utf16cvt.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

A_Use_decl_annotations
wchar_t *cvt_utf8_to_16_z_reserve(const char str[], wchar_t buf[]/*NULL?*/,
	const size_t buf_sz, size_t *const sz/*in,out*/, size_t *const u8sz/*out*/)
{
	const utf8_char_t *q = (const utf8_char_t*)str;
	utf16_char_t *b = buf;
	const size_t n = utf8_to_utf16_z(&q, &b, buf_sz);
	wchar_t *r = buf;

	if (!n) {
		errno = EILSEQ;
		return NULL;
	}

	if (n > buf_sz) {
		size_t conveted = 0;
		const size_t reserve = *sz; /* <= (size_t)-1/sizeof(wchar_t) */

		if (n > (size_t)-1/sizeof(wchar_t) - reserve) {
			errno = E2BIG;
			return NULL;
		}

		r = (wchar_t*)malloc((n + reserve)*sizeof(wchar_t));
		if (!r)
			return NULL;

		if (b != buf) {
			conveted = (size_t)(b - buf);
			memcpy(r + reserve, buf, conveted*sizeof(wchar_t));
		}

		q = utf8_to_utf16_z_unsafe(q, &r[reserve + conveted]);
	}

	*u8sz = (size_t)(q - (const utf8_char_t*)str);
	*sz = n;
	return r;
}

A_Use_decl_annotations
wchar_t *cvt_utf8_to_16_z_sz(const char str[], wchar_t buf[]/*NULL?*/,
	const size_t buf_sz, size_t *const sz/*out*/)
{
	size_t u8sz;
	*sz = 0;
	return cvt_utf8_to_16_z_reserve(str, buf, buf_sz, sz, &u8sz);
}

A_Use_decl_annotations
wchar_t *cvt_utf8_to_16_z(const char str[], wchar_t buf[]/*NULL?*/, const size_t buf_sz)
{
	size_t sz = 0, u8sz;
	return cvt_utf8_to_16_z_reserve(str, buf, buf_sz, &sz, &u8sz);
}

A_Use_decl_annotations
wchar_t *cvt_utf8_to_16(const char str[], size_t *const len/*>0,in,out*/,
	wchar_t buf[]/*NULL?*/, const size_t buf_sz)
{
	const utf8_char_t *q = (const utf8_char_t*)str;
	const utf8_char_t *const qe = q + *len;
	utf16_char_t *b = buf;
	const size_t n = utf8_to_utf16(&q, &b, buf_sz, (size_t)(qe - q)/*>0*/);
	wchar_t *r = buf;

	if (!n) {
		errno = EILSEQ;
		return NULL;
	}

	if (n > buf_sz) {
		size_t conveted = 0;

		if (n > (size_t)-1/sizeof(wchar_t)) {
			errno = E2BIG;
			return NULL;
		}

		r = (wchar_t*)malloc(n*sizeof(wchar_t));
		if (!r)
			return NULL;

		if (b != buf) {
			conveted = (size_t)(b - buf);
			memcpy(r, buf, conveted*sizeof(wchar_t));
		}

		(void)utf8_to_utf16_unsafe(q, &r[conveted], (size_t)(qe - q));
	}

	*len = n;
	return r;
}

A_Use_decl_annotations
wchar_t *cvt_utf32_to_16_z_sz(const unsigned str[], wchar_t buf[]/*NULL?*/,
	const size_t buf_sz, size_t *const sz/*out*/)
{
	const utf32_char_t *w = (const utf32_char_t*)str;
	utf16_char_t *b = buf;
	const size_t n = utf32_to_utf16_z(&w, &b, buf_sz);
	wchar_t *r = buf;

	if (!n) {
		errno = EILSEQ;
		return NULL;
	}

	if (n > buf_sz) {
		size_t conveted = 0;

		r = (wchar_t*)malloc(n*sizeof(wchar_t));
		if (!r)
			return NULL;

		if (b != buf) {
			conveted = (size_t)(b - buf);
			memcpy(r, buf, conveted*sizeof(wchar_t));
		}

		(void)utf32_to_utf16_z_unsafe(w, &r[conveted]);
	}

	*sz = n;
	return r;
}

A_Use_decl_annotations
wchar_t *cvt_utf32_to_16_z(const unsigned str[], wchar_t buf[]/*NULL?*/, const size_t buf_sz)
{
	size_t sz;
	return cvt_utf32_to_16_z_sz(str, buf, buf_sz, &sz);
}
