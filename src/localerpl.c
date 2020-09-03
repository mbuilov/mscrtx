/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* localerpl.c */

/* Default replacements - mostly direct calls to standard C library API.  */

#include <errno.h>
#include <wctype.h>
#include <ctype.h>

#define LOCALE_RPL_IMPL
#include "mscrtx/localerpl.h"
#include "mscrtx/locale_helpers.h" /* for set_locale_helper */

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

#ifndef FALLTHROUGH
# ifdef __clang__
#  define FALLTHROUGH __attribute__((fallthrough))
# else
#  define FALLTHROUGH
# endif
#endif

#ifndef _MSC_VER
#define _fread_nolock fread
#define _fwrite_nolock fwrite
#endif

/* stack-buffer for c32 -> wchar_t transformation */
#define COLL_BUF_SZ 512

/* stack-buffer to form 'name=value' string */
#define SETENV_BUF_SIZE 1024

static char *rpl_setlocale(int cat, const char *locale)
{
	char *const ret = locale != NULL ?
		/* Change current locale.  */
		set_locale_helper(cat, locale) :
		/* Query current locale.  */
		setlocale(cat, NULL);
	return ret;
}

static int rpl_mb_cur_max(void)
{
	return MB_CUR_MAX;
}

static size_t rpl_mbrtoc16(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	const size_t ret = mbrtowc(pwc, s, n, ps);
	assert(*pwc < 0xD800 || 0xDFFF < *pwc); /* assume not a utf16-surrograte */
	return ret;
}

static size_t rpl_mbrtoc32(unsigned *pwi, const char *s, size_t n, mbstate_t *ps)
{
	void *const pwi_ = pwi;
	const size_t ret = mbrtowc((wchar_t*)pwi_, s, n, ps);
	*(unsigned*)pwi_ = *(wchar_t*)pwi_;
	assert(*pwi < 0xD800 || 0xDFFF < *pwi); /* assume not a utf16-surrograte */
	return ret;
}

static size_t rpl_c16rtomb(char *s, wchar_t wc, mbstate_t *ps)
{
	assert(wc < 0xD800 || 0xDFFF < wc); /* assume not a utf16-surrograte */
	return wcrtomb(s, wc, ps);
}

static size_t rpl_c32rtomb(char *s, unsigned wi, mbstate_t *ps)
{
	assert((wchar_t)wi == wi);
	assert(wi < 0xD800 || 0xDFFF < wi); /* assume not a utf16-surrograte */
	return wcrtomb(s, (wchar_t)wi, ps);
}

A_Use_decl_annotations
size_t localerpl_rpl_c32slen(const unsigned *str)
{
	const unsigned *s = str;
	while (*s)
		s++;
	return (size_t)(s - str);
}

static unsigned *cast_unsigned_ptr(const unsigned *p)
{
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual" /* cast from type 'const unsigned int*' to type 'unsigned int*' casts away qualifiers */
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual" /* cast from 'const unsigned int *' to 'unsigned int *' drops const qualifier */
#endif
	return (unsigned*)p;
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

A_Use_decl_annotations
unsigned *localerpl_rpl_c32schr(const unsigned *s, unsigned c)
{
	for (;; s++) {
		if (*s == c)
			return cast_unsigned_ptr(s);
		if (!*s)
			return NULL;
	}
}

A_Use_decl_annotations
unsigned *localerpl_rpl_c32srchr(const unsigned *s, unsigned c)
{
	const unsigned *r = NULL;
	for (;; s++) {
		if (*s == c)
			r = s;
		if (!*s)
			return cast_unsigned_ptr(r);
	}
}

A_Use_decl_annotations
unsigned *localerpl_rpl_c32schrnul(const unsigned *s, unsigned c)
{
	for (;; s++) {
		if (*s == c || !*s)
			return cast_unsigned_ptr(s);
	}
}

A_Use_decl_annotations
int localerpl_rpl_c32scmp(const unsigned *s1, const unsigned *s2)
{
	for (;; s1++, s2++) {
		if (*s1 < *s2)
			return -1;
		if (*s1 != *s2)
			return 1;
		if (!*s1)
			return 0;
	}
}

static void c32_as_wchars(const unsigned *s, wchar_t *d)
{
	do {
		assert((wchar_t)*s == *s);
	} while (L'\0' != (*d++ = (wchar_t)*s++));
}

static int proc_c32s(const unsigned *s1, const unsigned *s2,
	int (*fn)(const wchar_t *ws1, const wchar_t *ws2))
{
	int ret;
	wchar_t buf[COLL_BUF_SZ], *ws1, *ws2;
	const size_t len1 = localerpl_rpl_c32slen(s1);
	const size_t len2 = localerpl_rpl_c32slen(s2);
	size_t avail = sizeof(buf)/sizeof(buf[0]);
	if (len1 < avail) {
		ws1 = buf;
		avail -= len1 + 1;
	}
	else {
		ws1 = (wchar_t*)malloc(sizeof(*ws1)*(len1 + 1));
		if (!ws1)
			return -2;
	}
	if (len2 < avail)
		ws2 = &buf[sizeof(buf)/sizeof(buf[0])] - avail;
	else {
		ws2 = (wchar_t*)malloc(sizeof(*ws2)*(len2 + 1));
		if (!ws2) {
			if (ws1 != buf)
				free(ws1);
			return -2;
		}
	}
	c32_as_wchars(s1, ws1);
	c32_as_wchars(s2, ws2);
	ret = (*fn)(ws1, ws2);
	if (ws1 != buf)
		free(ws1);
	if (ws2 != &buf[sizeof(buf)/sizeof(buf[0])] - avail)
		free(ws2);
	return ret;
}

static int rpl_c32scoll(const unsigned *s1, const unsigned *s2)
{
	return proc_c32s(s1, s2, wcscoll);
}

static int rpl_c32sicmp(const unsigned *s1, const unsigned *s2)
{
	return proc_c32s(s1, s2, _wcsicmp);
}

static unsigned rpl_c32tolower(unsigned c)
{
	assert((wint_t)c == c);
	return towlower((wint_t)c);
}

static unsigned rpl_c32toupper(unsigned c)
{
	assert((wint_t)c == c);
	return towupper((wint_t)c);
}

static int rpl_c32isascii(unsigned c)
{
	assert((wint_t)c == c);
	return iswascii((wint_t)c);
}

static int rpl_c32isalnum(unsigned c)
{
	assert((wint_t)c == c);
	return iswalnum((wint_t)c);
}

static int rpl_c32isalpha(unsigned c)
{
	assert((wint_t)c == c);
	return iswalpha((wint_t)c);
}

static int rpl_c32isblank(unsigned c)
{
	assert((wint_t)c == c);
	return iswblank((wint_t)c);
}

static int rpl_c32iscntrl(unsigned c)
{
	assert((wint_t)c == c);
	return iswcntrl((wint_t)c);
}

static int rpl_c32isdigit(unsigned c)
{
	assert((wint_t)c == c);
	return iswdigit((wint_t)c);
}

static int rpl_c32isgraph(unsigned c)
{
	assert((wint_t)c == c);
	return iswgraph((wint_t)c);
}

static int rpl_c32islower(unsigned c)
{
	assert((wint_t)c == c);
	return iswlower((wint_t)c);
}

static int rpl_c32isprint(unsigned c)
{
	assert((wint_t)c == c);
	return iswprint((wint_t)c);
}

static int rpl_c32ispunct(unsigned c)
{
	assert((wint_t)c == c);
	return iswpunct((wint_t)c);
}

static int rpl_c32isspace(unsigned c)
{
	assert((wint_t)c == c);
	return iswspace((wint_t)c);
}

static int rpl_c32isupper(unsigned c)
{
	assert((wint_t)c == c);
	return iswupper((wint_t)c);
}

static int rpl_c32isxdigit(unsigned c)
{
	assert((wint_t)c == c);
	return iswxdigit((wint_t)c);
}

static c32ctype_t rpl_c32ctype(char const *name)
{
	(void)sizeof(int[1-2*(sizeof(c32ctype_t) < sizeof(wctype_t))]);
	return (c32ctype_t)wctype(name);
}

static int rpl_c32isctype(unsigned c, c32ctype_t desc)
{
	assert((wint_t)c == c);
	return iswctype((wint_t)c, (wctype_t)desc);
}

static size_t rpl_mbstoc32s(unsigned *dst, const char *src, size_t n)
{
	void *const dst_ = dst;
	const size_t ret = mbstowcs((wchar_t*)dst_, src, n);
	if ((size_t)-1 != ret && dst_) {
		size_t i = ret + (ret < n);
		while (i) {
			wchar_t *const s = (wchar_t*)dst_;
			i--;
			assert(s[i] < 0xD800 || 0xDFFF < s[i]); /* assume not a utf16-surrograte */
			((unsigned*)dst_)[i] = s[i];
		}
	}
	return ret;
}

static size_t rpl_c32stombs(char *dst, const unsigned *src, size_t n)
{
	size_t sz;
	mbstate_t ps = {
#ifndef __cplusplus
		0
#endif
	};
	char buf[6/*MB_CUR_MAX*/];
	if (dst) {
		const char *const de = dst + n;
		if (!n)
			return 0;
		for (; (size_t)(de - dst) >= sizeof(buf); dst += sz) {
			assert((wchar_t)*src == *src);
			assert(*src < 0xD800 || 0xDFFF < *src); /* assume not a utf16-surrograte */
			sz = wcrtomb(dst, (wchar_t)*src++, &ps);
			if ((size_t)-1 == sz)
				return (size_t)-1;
			if (!*dst)
				return (size_t)(dst - (de - n));
		}
		while (dst < de) {
			assert((wchar_t)*src == *src);
			assert(*src < 0xD800 || 0xDFFF < *src); /* assume not a utf16-surrograte */
			sz = wcrtomb(buf, (wchar_t)*src++, &ps);
			if ((size_t)-1 == sz)
				return (size_t)-1;
			if (sz > (size_t)(de - dst))
				sz = (size_t)(de - dst);
			dst += sz;
			{
				char *const c = buf + sz;
				switch (sz) {
					default: assert(0);
					FALLTHROUGH;
					/* fallthrough */
					case 5: dst[-5] = c[-5];
					FALLTHROUGH;
					/* fallthrough */
					case 4: dst[-4] = c[-4];
					FALLTHROUGH;
					/* fallthrough */
					case 3: dst[-3] = c[-3];
					FALLTHROUGH;
					/* fallthrough */
					case 2: dst[-2] = c[-2];
					FALLTHROUGH;
					/* fallthrough */
					case 1: dst[-1] = c[-1];
				}
			}
			if ('\0' == buf[0])
				return (size_t)(dst - 1 - (de - n));
		}
		return (size_t)(dst - (de - n));
	}
	{
		size_t len = 0;
		for (;; len += sz) {
			assert((wchar_t)*src == *src);
			assert(*src < 0xD800 || 0xDFFF < *src); /* assume not a utf16-surrograte */
			sz = wcrtomb(buf, (wchar_t)*src++, &ps);
			if ((size_t)-1 == sz)
				return (size_t)-1;
			if ('\0' == buf[0])
				return len;
		}
	}
}

static size_t rpl_wcstoc32s(unsigned *dst, const wchar_t *src, size_t n)
{
	if (dst) {
		size_t i = 0;
		for (; i < n; i++) {
			assert(src[i] < 0xD800 || 0xDFFF < src[i]); /* assume not a utf16-surrograte */
			if (L'\0' == (dst[i] = src[i]))
				break;
		}
		return i;
	}
	return wcslen(src);
}

static size_t rpl_c32stowcs(wchar_t *dst, const unsigned *src, size_t n)
{
	if (dst) {
		size_t i = 0;
		for (; i < n; i++) {
			assert((wchar_t)src[i] == src[i]);
			assert(src[i] < 0xD800 || 0xDFFF < src[i]); /* assume not a utf16-surrograte */
			if (L'\0' == (dst[i] = (wchar_t)src[i]))
				break;
		}
		return i;
	}
	return localerpl_rpl_c32slen(src);
}

static int rpl_mkstemp(char *templ)
{
	char *const ret = _mktemp(templ);
	if (ret) {
		const int fd = _open(ret,
			_O_RDWR | _O_CREAT | _O_EXCL,
			_S_IREAD | _S_IWRITE);
		return fd;
	}
	return -1;
}

static char **rpl_environ(void)
{
	return _environ;
}

static int rpl_setenv(const char *name, const char *value, int overwrite)
{
	int ret;
	char stack_buf[SETENV_BUF_SIZE], *buf = stack_buf;
	size_t name_len, value_sz, sz;

	if (!overwrite && getenv(name))
		return 0;

	name_len = strlen(name);
	value_sz = strlen(value) + 1;

	sz = name_len + 1;

	if (value_sz > (size_t)-1 - sz) {
		errno = E2BIG;
		return -1;
	}

	sz += value_sz;

	if (sz > sizeof(SETENV_BUF_SIZE)) {
		buf = (char*)malloc(sz);
		if (!buf)
			return -1;
	}

	memcpy(buf, name, name_len);
	buf[name_len] = '=';
	memcpy(buf + name_len + 1, value, value_sz);

	ret = _putenv(buf);

	if (buf != stack_buf)
		free(buf);

	return ret;
}

static int rpl_unsetenv(const char *name)
{
	return rpl_setenv(name, "", 1);
}

static int rpl_clearenv(void)
{
	_environ = NULL;
	return 0;
}

static int rpl_open(const char *name, int flags, ...)
{
	int mode;
	va_list args;
	va_start(args, flags);
	mode = va_arg(args, int);
	va_end(args);
	return _open(name, flags, mode);
}

static const struct localerpl rpl_funcs = {
	rpl_open,
	fopen,
	_read,
	_write,
	_fread_nolock,
	_fwrite_nolock,
	putchar,
	fputc,
	getchar,
	fgetc,
	puts,
	fputs,
	fgets,
	_mkdir,
	_rmdir,
	remove,
	_unlink,
	rename,
	_chdir,
	_stat64,
	_chmod,
	printf,
	fprintf,
	vprintf,
	vfprintf,
	strerror,
	strftime,
	rpl_mkstemp,
	rpl_environ,
	getenv,
	rpl_setenv,
	rpl_unsetenv,
	rpl_clearenv,
	rpl_setlocale,
	rpl_mb_cur_max,
	btowc,
	mblen,
	mbrlen,
	rpl_mbrtoc16,
	rpl_mbrtoc32,
	rpl_c16rtomb,
	rpl_c32rtomb,
	mbstowcs,
	wcstombs,
	strcoll,
	_stricmp,
	tolower,
	toupper,
	__isascii,
	isalnum,
	isalpha,
	isblank,
	iscntrl,
	isdigit,
	isgraph,
	islower,
	isprint,
	ispunct,
	isspace,
	isupper,
	isxdigit,
	rpl_c32scoll,
	rpl_c32sicmp,
	rpl_c32tolower,
	rpl_c32toupper,
	rpl_c32isascii,
	rpl_c32isalnum,
	rpl_c32isalpha,
	rpl_c32isblank,
	rpl_c32iscntrl,
	rpl_c32isdigit,
	rpl_c32isgraph,
	rpl_c32islower,
	rpl_c32isprint,
	rpl_c32ispunct,
	rpl_c32isspace,
	rpl_c32isupper,
	rpl_c32isxdigit,
	rpl_c32ctype,
	rpl_c32isctype,
	rpl_mbstoc32s,
	rpl_c32stombs,
	rpl_wcstoc32s,
	rpl_c32stowcs,
	_spawnvp,
	_spawnl,
	_popen,
};

const struct localerpl *localerpl = &rpl_funcs;

/* from utf8env.c */
extern const struct localerpl utf8_funcs;

void change_localerpl(int to_utf8)
{
	localerpl = to_utf8 ? &utf8_funcs : &rpl_funcs;
}

A_Use_decl_annotations
int localerpl_is_utf8(void)
{
	return localerpl == &utf8_funcs;
}
