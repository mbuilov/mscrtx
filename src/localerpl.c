/******************************************************************************
* Library of replacement/missing functions of the Microsoft's CRT API.
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/mscrtx
* Licensed under GPL version 3 or any later version, see COPYING
******************************************************************************/

/* localerpl.c */

#include <errno.h>
#include <wctype.h>
#include <ctype.h>

#define LOCALE_RPL_IMPL
#include "mscrtx/localerpl.h"
#include "mscrtx/locale_helpers.h"
#include "libutf16/utf8_cstd.h"
#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf8_to_utf16_one.h"
#include "unicode_ctype/unicode_ctype.h"
#include "unicode_ctype/unicode_toupper.h"
#include "mscrtx/utf8env.h"
#include "mscrtx/utf16cvt.h"
#include "mscrtx/console_setup.h"
#include "mscrtx/consoleio.h"

/* not defined under MinGW.org */
#ifndef INT_MAX
#define INT_MAX ((unsigned)-1/2)
#endif

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

#ifndef __WINGW32__
#define vsnwprintf _vsnwprintf
#endif

/* stack buffers */
#define PATH_BUF_SIZE         260
#define FPRINTF_BUF_SIZE      512
#define MBCONV_BUF_SIZE       512
#define COLL_BUF_SZ           512
#define POPEN_CMD_BUF_SIZE    512
#define SPAWN_CMD_BUF_SIZE    260
#define SPAWN_ARGPTR_BUF_SIZE 64
#define STRFTIME_BUF_SIZE     256

/* static buffers */
#define UTF8_STRERROR_BUF_SIZE 1024

/* stack-buffer to form 'name=value' string */
#define SETENV_BUF_SIZE 1024

static int g_localerpl_is_utf8 = 0;

void localerpl_change(int to_utf8)
{
	g_localerpl_is_utf8 = to_utf8;
}

A_Use_decl_annotations
int localerpl_is_utf8(void)
{
	return g_localerpl_is_utf8;
}

#define localerpl_is_utf8() g_localerpl_is_utf8

A_Use_decl_annotations
size_t localerpl_c32slen(const unsigned *str)
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
unsigned *localerpl_c32schr(const unsigned *s, unsigned c)
{
	for (;; s++) {
		if (*s == c)
			return cast_unsigned_ptr(s);
		if (!*s)
			return NULL;
	}
}

A_Use_decl_annotations
unsigned *localerpl_c32srchr(const unsigned *s, unsigned c)
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
unsigned *localerpl_c32schrnul(const unsigned *s, unsigned c)
{
	for (;; s++) {
		if (*s == c || !*s)
			return cast_unsigned_ptr(s);
	}
}

A_Use_decl_annotations
int localerpl_c32scmp(const unsigned *s1, const unsigned *s2)
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

A_Use_decl_annotations
char *localerpl_setlocale(int category, const char *locale/*NULL?*/)
{
	char *current;

	if (locale != NULL)
		/* Change current locale.  */
		return set_locale_helper(category, locale);

	/* Query current locale.  */
	current = setlocale(category, NULL);

	if (current && localerpl_is_utf8())
		/* Replace resulting code page.  */
		return locale_helper_add_utf8_cp(category, current);

	return current;
}

A_Use_decl_annotations
int localerpl_open(const char *name, int flags, ...)
{
	int mode, fd;
	va_list args;

	va_start(args, flags);
	mode = va_arg(args, int);
	va_end(args);

	if (localerpl_is_utf8()) {
		wchar_t path_buf[PATH_BUF_SIZE];
		wchar_t *const wpath = CVT_UTF8_TO_16_Z(name, path_buf);
		if (!wpath)
			return -1;
		fd = _wopen(wpath, flags, mode);
		if (wpath != path_buf)
			free(wpath);
	}
	else
		fd = _open(name, flags, mode);

	return fd;
}

enum file_op {
	OP_MKDIR,
	OP_RMDIR,
	OP_REMOVE,
	OP_UNLINK,
	OP_CHDIR
};

static int utf8_file_op(const char *path, const enum file_op op)
{
	wchar_t path_buf[PATH_BUF_SIZE];
	wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
	int ret = -1;
	if (wpath) {
		switch (op) {
			case OP_MKDIR:
				ret = _wmkdir(wpath);
				break;
			case OP_RMDIR:
				ret = _wrmdir(wpath);
				break;
			case OP_REMOVE:
				ret = _wremove(wpath);
				break;
			case OP_UNLINK:
				ret = _wunlink(wpath);
				break;
			case OP_CHDIR:
				ret = _wchdir(wpath);
				break;
		}
		if (wpath != path_buf)
			free(wpath);
	}
	return ret;
}

A_Use_decl_annotations
int localerpl_mkdir(const char *dirname)
{
	return localerpl_is_utf8() ? utf8_file_op(dirname, OP_MKDIR) : _mkdir(dirname);
}

A_Use_decl_annotations
int localerpl_rmdir(const char *dirname)
{
	return localerpl_is_utf8() ? utf8_file_op(dirname, OP_RMDIR) : _rmdir(dirname);
}

A_Use_decl_annotations
int localerpl_remove(const char *pathname)
{
	return localerpl_is_utf8() ? utf8_file_op(pathname, OP_REMOVE) : remove(pathname);
}

A_Use_decl_annotations
int localerpl_unlink(const char *pathname)
{
	return localerpl_is_utf8() ? utf8_file_op(pathname, OP_UNLINK) : _unlink(pathname);
}

A_Use_decl_annotations
int localerpl_chdir(const char *path)
{
	return localerpl_is_utf8() ? utf8_file_op(path, OP_CHDIR) : _chdir(path);
}

A_Use_decl_annotations
int localerpl_stat(const char *path, struct __stat64 *buf)
{
	if (localerpl_is_utf8()) {
		wchar_t path_buf[PATH_BUF_SIZE];
		wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
		int ret;
		if (!wpath)
			return -1;
		ret = _wstat64(wpath, buf);
		if (wpath != path_buf)
			free(wpath);
		return ret;
	}
	return _stat64(path, buf);
}

A_Use_decl_annotations
int localerpl_chmod(const char *path, int mode)
{
	if (localerpl_is_utf8()) {
		wchar_t path_buf[PATH_BUF_SIZE];
		wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
		int ret;
		if (!wpath)
			return -1;
		ret = _wchmod(wpath, mode);
		if (wpath != path_buf)
			free(wpath);
		return ret;
	}
	return _chmod(path, mode);
}

A_Use_decl_annotations
int localerpl_rename(const char *old_name, const char *new_name)
{
	if (localerpl_is_utf8()) {
		int ret;
		wchar_t buf[PATH_BUF_SIZE*2], *wp_new = buf;
		size_t wp_old_sz;
		wchar_t *const wp_old = CVT_UTF8_TO_16_Z_SZ(old_name, buf, &wp_old_sz);

		if (!wp_old)
			return -1;

		if (buf == wp_old)
			wp_new = buf + wp_old_sz;
		else
			wp_old_sz = 0;

		wp_new = cvt_utf8_to_16_z(new_name, wp_new, (size_t)(&buf[sizeof(buf)/sizeof(buf[0])] - wp_new));

		if (!wp_new) {
			if (!wp_old_sz)
				free(wp_old);
			return -1;
		}

		ret = _wrename(wp_old, wp_new);

		if (!wp_old_sz)
			free(wp_old);

		if (wp_new != buf + wp_old_sz)
			free(wp_new);

		return ret;
	}
	return rename(old_name, new_name);
}

static wchar_t conv_fmode(char c)
{
	switch (c) {
		case '+': return L'+';
		case 'a': return L'a';
		case 'r': return L'r';
		case 'w': return L'w';
		case 'b': return L'b';
		case 't': return L't';
		case 'c': return L'c';
		case 'n': return L'n';
		case 'S': return L'S';
		case 'R': return L'R';
		case 'T': return L'T';
		case 'D': return L'D';
		default: return L'\0';
	}
}

A_Use_decl_annotations
FILE *localerpl_fopen(const char *path, const char *mode)
{
	if (localerpl_is_utf8()) {
		/* Check open mode.  Recoding via ccs=... is not supported.  */
		wchar_t wmode[sizeof("+arwbtcnSRTD")];
		unsigned i = 0;

		for (;;) {
			wmode[i] = conv_fmode(mode[i]);
			if (wmode[i] == L'\0') {
				if (mode[i] == '\0')
					break;
				errno = EINVAL;
				return NULL;
			}
			if (++i == sizeof(wmode)/sizeof(wmode[0])) {
				errno = EINVAL;
				return NULL;
			}
		}

		{
			FILE *f;
			wchar_t path_buf[PATH_BUF_SIZE];
			wchar_t *const wpath = CVT_UTF8_TO_16_Z(path, path_buf);
			if (!wpath)
				return NULL;
			f = _wfopen(wpath, wmode);
			if (wpath != path_buf)
				free(wpath);
			return f;
		}
	}
	return fopen(path, mode);
}

static int localerpl_read_con(int fd, void *buf, unsigned count/*>0*/)
{
	if (count > INT_MAX) {
		errno = E2BIG;
		return -1;
	}

	return read_console(fd, buf, count);
}

A_Use_decl_annotations
int localerpl_read(int fd, void *buf, unsigned count)
{
	int fmode;

	if (count == 0)
		return 0;

	if (-1 == (fmode = turn_on_console_fd(fd)))
		return _read(fd, buf, count);

	{
		const int n = localerpl_read_con(fd, buf, count);
		(void)turn_off_console_fd(fd, fmode);
		return n;
	}
}

static size_t localerpl_fread_con(void *buf, size_t size/*>0*/, size_t nmemb/*>0*/, FILE *stream)
{
	size_t n;

	/* note: (size_t)-1 is an error indicator */
	if (nmemb > (size_t)-2/size) {
		errno = E2BIG;
		return 0;
	}

	n = fread_console(buf, (size*nmemb)/*[1..(size_t)-2]*/, stream);
	if ((size_t)-1 != n)
		return n/size;

	if (!ferror(stream))
		errno = EILSEQ; /* set at least errno to indicate an error */
	return 0;
}

A_Use_decl_annotations
size_t localerpl_fread(void *buf, size_t size, size_t nmemb, FILE *stream)
{
	int fmode;

	if (size == 0 || nmemb == 0)
		return 0;

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		return _fread_nolock(buf, size, nmemb, stream);

	{
		const size_t n = localerpl_fread_con(buf, size, nmemb, stream);
		(void)turn_off_console_fd(_fileno(stream), fmode);
		return n;
	}
}

static int localerpl_fgetc_con(FILE *stream)
{
	unsigned char c;
	const size_t n = fread_console(&c, 1, stream);
	return n == 1 ? c : EOF;
}

A_Use_decl_annotations
int localerpl_fgetc(FILE *stream)
{
	int fmode;

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		return fgetc(stream);

	{
		const int n = localerpl_fgetc_con(stream);
		(void)turn_off_console_fd(_fileno(stream), fmode);
		return n;
	}
}

A_Use_decl_annotations
int localerpl_getchar(void)
{
	return localerpl_fgetc(stdin);
}

static char *localerpl_fgets_con(char *s, int size, FILE *stream)
{
	size_t n;

	if (size <= 0)
		return NULL; /* Bad parameters.  Should not happen.  */

	if (size < 2) {
		*s = '\0';
		return s;
	}

	n = fread_console_nl(s, (size_t)size - 1, stream);
	if ((size_t)-1 != n) {
		s[n] = '\0';
		return s;
	}

	return NULL;
}

A_Use_decl_annotations
char *localerpl_fgets(char *s, int size, FILE *stream)
{
	int fmode;

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		return fgets(s, size, stream);

	{
		char *const ret = localerpl_fgets_con(s, size, stream);
		(void)turn_off_console_fd(_fileno(stream), fmode);
		return ret;
	}
}

static int localerpl_write_con(int fd, const void *buf, unsigned count/*>0*/)
{
	if (count > INT_MAX) {
		errno = E2BIG;
		return -1;
	}

	return write_console(fd, buf, count) ? -1 : (int)count;
}

A_Use_decl_annotations
int localerpl_write(int fd, const void *buf, unsigned count)
{
	int fmode;

	if (count == 0)
		return 0;

	if (-1 == (fmode = turn_on_console_fd(fd)))
		return _write(fd, buf, count);

	{
		const int n = localerpl_write_con(fd, buf, count);
		(void)turn_off_console_fd(fd, fmode);
		return n;
	}
}

static size_t localerpl_fwrite_con(const void *buf, size_t size/*>0*/, size_t nmemb/*>0*/, FILE *stream)
{
	if (nmemb > (size_t)-1/size) {
		errno = E2BIG;
		return 0;
	}

	if (!fwrite_console(buf, size*nmemb, stream))
		return nmemb;

	if (!ferror(stream))
		errno = EILSEQ; /* set at least errno to indicate an error */
	return 0;
}

A_Use_decl_annotations
size_t localerpl_fwrite(const void *buf, size_t size, size_t nmemb, FILE *stream)
{
	int fmode;

	if (size == 0 || nmemb == 0) {
		errno = 0;
		return 0;
	}

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		return _fwrite_nolock(buf, size, nmemb, stream);

	{
		const size_t n = localerpl_fwrite_con(buf, size, nmemb, stream);
		(void)turn_off_console_fd(_fileno(stream), fmode);
		return n;
	}
}

static int localerpl_fputc_con(int c, FILE *stream)
{
	const unsigned char ch = (unsigned char)c;
	return fwrite_console(&ch, 1, stream) ? EOF : c;
}

A_Use_decl_annotations
int localerpl_fputc(int c, FILE *stream)
{
	int fmode;

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		return fputc(c, stream);

	{
		const int n = localerpl_fputc_con(c, stream);
		(void)turn_off_console_fd(_fileno(stream), fmode);
		return n;
	}
}

int localerpl_putchar(int c)
{
	return localerpl_fputc(c, stdout);
}

static int localerpl_fputs_con(const char *s, FILE *stream)
{
	const size_t len = strlen(s);
	return len && fwrite_console(s, len, stream) ? EOF : 0;
}

A_Use_decl_annotations
int localerpl_fputs(const char *s, FILE *stream)
{
	int fmode;

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		return fputs(s, stream);

	{
		const int n = localerpl_fputs_con(s, stream);
		(void)turn_off_console_fd(_fileno(stream), fmode);
		return n;
	}
}

static int localerpl_puts_con(const char *s, FILE *stream)
{
	const size_t len = strlen(s);
	const int r = len
		? fwrite_console_nl(s, len, stream)
		: fwrite_console("\n", 1, stream);
	return r ? EOF : 0;
}

A_Use_decl_annotations
int localerpl_puts(const char *s)
{
	int fmode;

	if (-1 == (fmode = turn_on_console_fd(_fileno(stdout))))
		return puts(s);

	{
		const int n = localerpl_puts_con(s, stdout);
		(void)turn_off_console_fd(_fileno(stdout), fmode);
		return n;
	}
}

A_Use_decl_annotations
int localerpl_mb_cur_max(void)
{
	return localerpl_is_utf8() ? UTF8_MAX_LEN : MB_CUR_MAX;
}

A_Use_decl_annotations
wint_t localerpl_btowc(int c)
{
	if (!localerpl_is_utf8())
		return btowc(c);
	if (c != EOF) {
		const utf8_char_t r = (utf8_char_t)c;
		if (utf8_is_one_byte(r))
			return utf8_one_byte_to_utf16(r);
	}
	return WEOF;
}

A_Use_decl_annotations
int localerpl_mblen(const char *s, size_t n)
{
	return localerpl_is_utf8() ? utf8_mblen((const utf8_char_t*)s, n) : mblen(s, n);
}

A_Use_decl_annotations
size_t localerpl_mbrlen(const char *s, size_t n, mbstate_t *ps)
{
	mbstate_t ips = {
#ifndef __cplusplus
		0
#endif
	};
	if (!ps)
		ps = &ips;
	/* will use mbstate_t object as utf8_state_t */
	(void)sizeof(int[1-2*(sizeof(mbstate_t) < sizeof(utf8_state_t))]);
	return localerpl_is_utf8() ?
		utf8_mbrlen((const utf8_char_t*)s, n, (utf8_state_t*)ps) : mbrlen(s, n, ps);
}

A_Use_decl_annotations
size_t localerpl_mbrtoc16(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	mbstate_t ips = {
#ifndef __cplusplus
		0
#endif
	};
	if (!ps)
		ps = &ips;
	if (!localerpl_is_utf8()) {
		const size_t ret = mbrtowc(pwc, s, n, ps);
		assert((size_t)-1 == ret || (size_t)-2 == ret || !pwc ||
			!utf16_is_surrogate(*pwc)); /* assume not a utf16-surrograte */
		return ret;
	}
	/* will use mbstate_t object as utf8_state_t */
	(void)sizeof(int[1-2*(sizeof(mbstate_t) < sizeof(utf8_state_t))]);
	return utf8_mbrtoc16(pwc, (const utf8_char_t*)s, n, (utf8_state_t*)ps);
}

A_Use_decl_annotations
size_t localerpl_mbrtoc32(unsigned *pwi, const char *s, size_t n, mbstate_t *ps)
{
	mbstate_t ips = {
#ifndef __cplusplus
		0
#endif
	};
	if (!ps)
		ps = &ips;
	if (!localerpl_is_utf8()) {
		void *const pwi_ = pwi;
		const size_t ret = mbrtowc((wchar_t*)pwi_, s, n, ps);
		if (pwi_)
			*(unsigned*)pwi_ = *(wchar_t*)pwi_;
		assert((size_t)-1 == ret || (size_t)-2 == ret || !pwi ||
			!utf16_is_surrogate(*pwi)); /* assume not a utf16-surrograte */
		return ret;
	}
	/* will use mbstate_t object as utf8_state_t */
	(void)sizeof(int[1-2*(sizeof(mbstate_t) < sizeof(utf8_state_t))]);
	return utf8_mbrtoc32(pwi, (const utf8_char_t*)s, n, (utf8_state_t*)ps);
}

A_Use_decl_annotations
size_t localerpl_c16rtomb(char *s, wchar_t wc, mbstate_t *ps)
{
	mbstate_t ips = {
#ifndef __cplusplus
		0
#endif
	};
	if (!ps)
		ps = &ips;
	if (!localerpl_is_utf8()) {
		assert(!utf16_is_surrogate(wc)); /* assume not a utf16-surrograte */
		return wcrtomb(s, wc, ps);
	}
	/* will use mbstate_t object as utf8_state_t */
	(void)sizeof(int[1-2*(sizeof(mbstate_t) < sizeof(utf8_state_t))]);
	return utf8_c16rtomb((utf8_char_t*)s, wc, (utf8_state_t*)ps);
}

A_Use_decl_annotations
size_t localerpl_c32rtomb(char *s, unsigned wi, mbstate_t *ps)
{
	mbstate_t ips = {
#ifndef __cplusplus
		0
#endif
	};
	if (!ps)
		ps = &ips;
	if (!localerpl_is_utf8()) {
		assert((wchar_t)wi == wi);
		assert(!utf16_is_surrogate(wi)); /* assume not a utf16-surrograte */
		return wcrtomb(s, (wchar_t)wi, ps);
	}
	/* will use mbstate_t object as utf8_state_t */
	(void)sizeof(int[1-2*(sizeof(mbstate_t) < sizeof(utf8_state_t))]);
	return utf8_c32rtomb((utf8_char_t*)s, wi, (utf8_state_t*)ps);
}

A_Use_decl_annotations
size_t localerpl_mbstowcs(wchar_t *wcstr, const char *mbstr, size_t count)
{
	return localerpl_is_utf8()
		? utf8_mbstoc16s(wcstr, (const utf8_char_t*)mbstr, count)
		: mbstowcs(wcstr, mbstr, count);
}

A_Use_decl_annotations
size_t localerpl_wcstombs(char *mbstr, const wchar_t *wcstr, size_t count)
{
	return localerpl_is_utf8()
		? utf8_c16stombs((utf8_char_t*)mbstr, wcstr, count)
		: wcstombs(mbstr, wcstr, count);
}

static void c32s_as_wchars(const unsigned *s, wchar_t *d)
{
	do {
		assert((wchar_t)*s == *s);
	} while (L'\0' != (*d++ = (wchar_t)*s++));
}

static int proc_c32s(const unsigned *s1, const unsigned *s2, const int do_coll)
{
	int ret;
	wchar_t buf[COLL_BUF_SZ], *ws1, *ws2;
	const size_t len1 = localerpl_c32slen(s1);
	const size_t len2 = localerpl_c32slen(s2);
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
	c32s_as_wchars(s1, ws1);
	c32s_as_wchars(s2, ws2);
	ret = do_coll ? wcscoll(ws1, ws2) : _wcsicmp(ws1, ws2);
	if (ws1 != buf)
		free(ws1);
	if (ws2 != &buf[sizeof(buf)/sizeof(buf[0])] - avail)
		free(ws2);
	return ret;
}

A_Use_decl_annotations
int localerpl_strcoll(const char *s1, const char *s2)
{
	if (localerpl_is_utf8()) {
		int ret = -2;
		wchar_t buf[COLL_BUF_SZ];
		size_t ws1_sz;
		wchar_t *const ws1 = CVT_UTF8_TO_16_Z_SZ(s1, buf, &ws1_sz);
		if (ws1) {
			const size_t offs = (buf == ws1) ? ws1_sz : 0;
			wchar_t *const ws2 = cvt_utf8_to_16_z(s2, buf + offs, sizeof(buf)/sizeof(buf[0]) - offs);
			if (ws2) {
				ret = wcscoll(ws1, ws2);
				if (buf + offs != ws2)
					free(ws2);
			}
			if (!offs)
				free(ws1);
		}
		return ret;
	}
	return strcoll(s1, s2);
}

A_Use_decl_annotations
int localerpl_c32scoll(const unsigned *s1, const unsigned *s2)
{
	if (localerpl_is_utf8()) {
		int ret = -2;
		wchar_t buf[COLL_BUF_SZ];
		size_t ws1_sz;
		wchar_t *const ws1 = CVT_UTF32_TO_16_Z_SZ(s1, buf, &ws1_sz);
		if (ws1) {
			const size_t offs = (buf == ws1) ? ws1_sz : 0;
			wchar_t *const ws2 = cvt_utf32_to_16_z(s2, buf + offs, sizeof(buf)/sizeof(buf[0]) - offs);
			if (ws2) {
				ret = wcscoll(ws1, ws2);
				if (buf + offs != ws2)
					free(ws2);
			}
			if (!offs)
				free(ws1);
		}
		return ret;
	}
	return proc_c32s(s1, s2, /*do_coll:*/1);
}

A_Use_decl_annotations
int localerpl_stricmp(const char *s1, const char *s2)
{
	if (localerpl_is_utf8()) {
		for (;;) {
			utf32_char_t w1, w2;
			s1 = (const char*)utf8_to_utf32_one_z(&w1, (const utf8_char_t*)s1);
			s2 = (const char*)utf8_to_utf32_one_z(&w2, (const utf8_char_t*)s2);
			if (!s1 || !s2)
				return (int)((unsigned)-1/2); /* INT_MAX */
			w1 = unicode_tolower(w1);
			w2 = unicode_tolower(w2);
			if (w1 < w2)
				return -1;
			if (w1 != w2)
				return 1;
			if (!w1)
				return 0;
		}
	}
	return _stricmp(s1, s2);
}

A_Use_decl_annotations
int localerpl_c32sicmp(const unsigned *s1, const unsigned *s2)
{
	if (localerpl_is_utf8()) {
		for (;; s1++, s2++) {
			const unsigned w1 = unicode_tolower(*s1);
			const unsigned w2 = unicode_tolower(*s2);
			if (w1 < w2)
				return -1;
			if (w1 != w2)
				return 1;
			if (!w1)
				return 0;
		}
	}
	return proc_c32s(s1, s2, /*do_coll:*/0);
}

A_Use_decl_annotations
int localerpl_tolower(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			(int)unicode_tolower((unsigned)c) : c : tolower(c);
}

A_Use_decl_annotations
int localerpl_toupper(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			(int)unicode_toupper((unsigned)c) : c : toupper(c);
}

A_Use_decl_annotations
int localerpl_isascii(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) : __isascii(c);
}

A_Use_decl_annotations
int localerpl_isalnum(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isalnum((unsigned)c) : 0 : isalnum(c);
}

A_Use_decl_annotations
int localerpl_isalpha(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isalpha((unsigned)c) : 0 : isalpha(c);
}

A_Use_decl_annotations
int localerpl_isblank(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isblank((unsigned)c) : 0 : isblank(c);
}

A_Use_decl_annotations
int localerpl_iscntrl(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_iscntrl((unsigned)c) : 0 : iscntrl(c);
}

A_Use_decl_annotations
int localerpl_isdigit(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isdigit((unsigned)c) : 0 : isdigit(c);
}

A_Use_decl_annotations
int localerpl_isgraph(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isgraph((unsigned)c) : 0 : isgraph(c);
}

A_Use_decl_annotations
int localerpl_islower(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_islower((unsigned)c) : 0 : islower(c);
}

A_Use_decl_annotations
int localerpl_isprint(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isprint((unsigned)c) : 0 : isprint(c);
}

A_Use_decl_annotations
int localerpl_ispunct(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_ispunct((unsigned)c) : 0 : ispunct(c);
}

A_Use_decl_annotations
int localerpl_isspace(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isspace((unsigned)c) : 0 : isspace(c);
}

A_Use_decl_annotations
int localerpl_isupper(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isupper((unsigned)c) : 0 : isupper(c);
}

A_Use_decl_annotations
int localerpl_isxdigit(int c)
{
	return localerpl_is_utf8() ?
		utf8_is_one_byte((unsigned)c) ?
			unicode_isxdigit((unsigned)c) : 0 : isxdigit(c);
}

A_Use_decl_annotations
unsigned localerpl_c32tolower(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_tolower(c);
	assert((wint_t)c == c);
	return towlower((wint_t)c);
}

A_Use_decl_annotations
unsigned localerpl_c32toupper(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_toupper(c);
	assert((wint_t)c == c);
	return towupper((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isascii(unsigned c)
{
	if (localerpl_is_utf8())
		return utf8_is_one_byte(c);
	assert((wint_t)c == c);
	return iswascii((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isalnum(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isalnum(c);
	assert((wint_t)c == c);
	return iswalnum((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isalpha(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isalpha(c);
	assert((wint_t)c == c);
	return iswalpha((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isblank(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isblank(c);
	assert((wint_t)c == c);
	return iswblank((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32iscntrl(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_iscntrl(c);
	assert((wint_t)c == c);
	return iswcntrl((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isdigit(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isdigit(c);
	assert((wint_t)c == c);
	return iswdigit((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isgraph(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isgraph(c);
	assert((wint_t)c == c);
	return iswgraph((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32islower(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_islower(c);
	assert((wint_t)c == c);
	return iswlower((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isprint(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isprint(c);
	assert((wint_t)c == c);
	return iswprint((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32ispunct(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_ispunct(c);
	assert((wint_t)c == c);
	return iswpunct((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isspace(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isspace(c);
	assert((wint_t)c == c);
	return iswspace((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isupper(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isupper(c);
	assert((wint_t)c == c);
	return iswupper((wint_t)c);
}

A_Use_decl_annotations
int localerpl_c32isxdigit(unsigned c)
{
	if (localerpl_is_utf8())
		return unicode_isxdigit(c);
	assert((wint_t)c == c);
	return iswxdigit((wint_t)c);
}

A_Use_decl_annotations
c32ctype_t localerpl_c32ctype(const char *name)
{
	if (localerpl_is_utf8())
		return (c32ctype_t)unicode_ctype(name);
	(void)sizeof(int[1-2*(sizeof(c32ctype_t) < sizeof(wctype_t))]);
	return (c32ctype_t)wctype(name);
}

A_Use_decl_annotations
int localerpl_c32isctype(unsigned c, c32ctype_t desc)
{
	if (localerpl_is_utf8())
		return unicode_isctype(c, (int)desc);
	assert((wint_t)c == c);
	return iswctype((wint_t)c, (wctype_t)desc);
}

A_Use_decl_annotations
size_t localerpl_mbstoc32s(unsigned *dst, const char *src, size_t n)
{
	if (!localerpl_is_utf8()) {
		void *const dst_ = dst;
		const size_t ret = mbstowcs((wchar_t*)dst_, src, n);
		if ((size_t)-1 != ret && dst_) {
			size_t i = ret + (ret < n);
			while (i) {
				wchar_t *const s = (wchar_t*)dst_;
				i--;
				assert(!utf16_is_surrogate(s[i])); /* assume not a utf16-surrograte */
				((unsigned*)dst_)[i] = s[i];
			}
		}
		return ret;
	}
	return utf8_mbstoc32s(dst, (const utf8_char_t*)src, n);
}

static size_t rpl_c32stombs(char *dst, const unsigned *src, const size_t n)
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
			assert(!utf16_is_surrogate(*src)); /* assume not a utf16-surrograte */
			sz = wcrtomb(dst, (wchar_t)*src++, &ps);
			if ((size_t)-1 == sz)
				return (size_t)-1;
			if (!*dst)
				return (size_t)(dst - (de - n));
		}
		while (dst < de) {
			assert((wchar_t)*src == *src);
			assert(!utf16_is_surrogate(*src)); /* assume not a utf16-surrograte */
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
			assert(!utf16_is_surrogate(*src)); /* assume not a utf16-surrograte */
			sz = wcrtomb(buf, (wchar_t)*src++, &ps);
			if ((size_t)-1 == sz)
				return (size_t)-1;
			if ('\0' == buf[0])
				return len;
		}
	}
}

A_Use_decl_annotations
size_t localerpl_c32stombs(char *dst, const unsigned *src, size_t n)
{
	return localerpl_is_utf8() ?
		utf8_c32stombs((utf8_char_t*)dst, src, n) : rpl_c32stombs(dst, src, n);
}

A_Use_decl_annotations
size_t localerpl_wcstoc32s(unsigned *dst, const wchar_t *src, size_t n)
{
	if (!localerpl_is_utf8()) {
		if (dst) {
			size_t i = 0;
			for (; i < n; i++) {
				assert(!utf16_is_surrogate(src[i])); /* assume not a utf16-surrograte */
				if (L'\0' == (dst[i] = src[i]))
					break;
			}
			return i;
		}
		return wcslen(src);
	}
	return utf8_c16stoc32s(dst, src, n);
}

A_Use_decl_annotations
size_t localerpl_c32stowcs(wchar_t *dst, const unsigned *src, size_t n)
{
	if (!localerpl_is_utf8()) {
		if (dst) {
			size_t i = 0;
			for (; i < n; i++) {
				assert((wchar_t)src[i] == src[i]);
				assert(!utf16_is_surrogate(src[i])); /* assume not a utf16-surrograte */
				if (L'\0' == (dst[i] = (wchar_t)src[i]))
					break;
			}
			return i;
		}
		return localerpl_c32slen(src);
	}
	return utf8_c32stoc16s(dst, src, n);
}

A_Use_decl_annotations
intptr_t localerpl_spawnvp(int mode, const char *cmdname, const char *const *argv)
{
	if (localerpl_is_utf8()) {
		wchar_t cmd_buf[SPAWN_CMD_BUF_SIZE];
		wchar_t *argptr_buf[SPAWN_ARGPTR_BUF_SIZE], **wargv = argptr_buf;
		wchar_t *const wcmd = CVT_UTF8_TO_16_Z(cmdname, cmd_buf);
		const char *const *a;
		intptr_t ret = -1;
		size_t n;

		if (!wcmd)
			return -1;

		/* Count arguments.  */
		for (a = argv; *a; a++);
		n = (size_t)(a - argv);

		if (n >= sizeof(argptr_buf)/sizeof(argptr_buf[0]))
			wargv = (wchar_t**)malloc((n + 1)*sizeof(*wargv));

		if (wargv) {
			/* Convert each argument.  */
			for (a = argv; *a; a++) {
				wchar_t *const wa = cvt_utf8_to_16_z(*a, NULL, 0);
				if (!wa)
					break;
				wargv[a - argv] = wa;
			}
			n = (size_t)(a - argv);
			wargv[n] = NULL;

			if (!*a)
				ret = _wspawnvp(mode, wcmd, (const wchar_t *const *)wargv);

			while (n)
				free(wargv[--n]);
			if (wargv != argptr_buf)
				free(wargv);
		}

		if (wcmd != cmd_buf)
			free(wcmd);
		return ret;
	}
	return _spawnvp(mode, cmdname, argv);
}

A_Use_decl_annotations
intptr_t localerpl_spawnl_utf8(int mode, const char *cmdname, ...)
{
	wchar_t cmd_buf[SPAWN_CMD_BUF_SIZE];
	wchar_t *argptr_buf[SPAWN_ARGPTR_BUF_SIZE], **wargv = argptr_buf, **pwa;
	wchar_t *const wcmd = CVT_UTF8_TO_16_Z(cmdname, cmd_buf);
	size_t n = 0;
	const char *a;
	intptr_t ret = -1;
	va_list args;

	if (!wcmd)
		return -1;

	/* Count arguments.  */
	va_start(args, cmdname);
	for (;; n++) {
		a = va_arg(args, const char *);
		if (!a)
			break;
	}
	va_end(args);

	if (n >= sizeof(argptr_buf)/sizeof(argptr_buf[0]))
		wargv = (wchar_t**)malloc((n + 1)*sizeof(*wargv));

	if (wargv) {

		/* Convert each argument.  */
		va_start(args, cmdname);
		for (n = 0;; n++) {
			a = va_arg(args, const char *);
			if (a) {
				wchar_t *const wa = cvt_utf8_to_16_z(a, NULL, 0);
				if (!wa)
					break;
				wargv[n] = wa;
			}
			else
				break;
		}
		wargv[n] = NULL;
		va_end(args);

		if (!a)
			ret = _wspawnvp(mode, wcmd, (const wchar_t *const *)wargv);

		for (pwa = wargv; *pwa; pwa++)
			free(*pwa);
		if (wargv != argptr_buf)
			free(wargv);
	}

	if (wcmd != cmd_buf)
		free(wcmd);
	return ret;
}

A_Use_decl_annotations
FILE *localerpl_popen(const char *command, const char *mode)
{
	if (localerpl_is_utf8()) {
		wchar_t wmode[3];

		if ('r' == mode[0])
			wmode[0] = L'r';
		else if ('w' == mode[0])
			wmode[0] = L'w';
		else {
			errno = EINVAL;
			return NULL;
		}

		if ('\0' == mode[1])
			wmode[1] = L'\0';
		else {
			if ('b' == mode[1])
				wmode[1] = L'b';
			else if ('t' == mode[1])
				wmode[1] = L't';
			else {
				errno = EINVAL;
				return NULL;
			}

			if ('\0' != mode[2]) {
				errno = EINVAL;
				return NULL;
			}
			wmode[2] = L'\0';
		}

		{
			FILE *f;
			wchar_t cmd_buf[POPEN_CMD_BUF_SIZE];
			wchar_t *const wcmd = CVT_UTF8_TO_16_Z(command, cmd_buf);
			if (!wcmd)
				return NULL;

			f = _wpopen(wcmd, wmode);
			if (wcmd != cmd_buf)
				free(wcmd);
			return f;
		}
	}
	return _popen(command, mode);
}

A_Use_decl_annotations
char *localerpl_strerror(int error_number)
{
	if (localerpl_is_utf8()) {
		static char strerror_buf[UTF8_STRERROR_BUF_SIZE];
		wchar_t *err = _wcserror(error_number);
		if (err != NULL) {
			const utf16_char_t *w = err;
			utf8_char_t *b = (utf8_char_t*)strerror_buf;
			size_t n = utf16_to_utf8_z_partial(&w, &b, UTF8_STRERROR_BUF_SIZE - 1);
			if (n == 0) {
#define ERR_MSG "failed to convert error message to utf8\n"
				(void)sizeof(int[1-2*(sizeof(strerror_buf) < sizeof(ERR_MSG))]);
				strcpy(strerror_buf, ERR_MSG);
#undef ERR_MSG
			}
			else if (n >= UTF8_STRERROR_BUF_SIZE)
				*b = '\0';
			return strerror_buf;
		}
		return NULL;
	}
	return strerror(error_number);
}

A_Use_decl_annotations
size_t localerpl_strftime(char *s, size_t mx, const char *fmt, const struct tm *t)
{
	if (localerpl_is_utf8()) {
		size_t r, wfmt_sz, rem_sz;
		wchar_t strftime_buf[STRFTIME_BUF_SIZE], *wout_buf;
		wchar_t *const wfmt = CVT_UTF8_TO_16_Z_SZ(fmt, strftime_buf, &wfmt_sz);
		if (!wfmt)
			return 0;

		if (wfmt != strftime_buf)
			wfmt_sz = 0;

		rem_sz = sizeof(strftime_buf)/sizeof(strftime_buf[0]) - wfmt_sz;
		wout_buf = strftime_buf + wfmt_sz;
		r = wcsftime(wout_buf, rem_sz, wfmt, t);

		if (!r) {
			/* May be output buffer is too small?
			   One wchar_t per one utf8-byte in the output buffer should be enough.  */
			if (mx > (size_t)-1/sizeof(wchar_t)) {
				errno = EINVAL;
				goto out;
			}

			wout_buf = (wchar_t*)malloc(sizeof(wchar_t)*mx);
			if (!wout_buf)
				goto out;

			r = wcsftime(wout_buf, mx, wfmt, t);
		}

		if (r) {
			const utf16_char_t *w = (const utf16_char_t*)wout_buf;
			utf8_char_t *b = (utf8_char_t*)s;
			r = utf16_to_utf8_partial(&w, &b, mx, r);
			if (r < mx)
				s[r] = '\0';
			else
				r = 0; /* Output buffer is too small.  */
		}

		if (wout_buf != strftime_buf + wfmt_sz)
			free(wout_buf);

out:
		if (!wfmt_sz)
			free(wfmt);
		return r;
	}

#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral" /* warning: format not a string literal, format string not checked */
#endif
	return strftime(s, mx, fmt, t);
#if defined __GNUC__ && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif
}

static int utf8_rpl_mkstemp(char *templ)
{
	int fd;
	wchar_t path_buf[PATH_BUF_SIZE], *wbuf, *wtempl;
	const size_t len = strlen(templ);
	size_t sz = len + 1;
	if (len == 0) {
		errno = EINVAL;
		return -1;
	}

	wbuf = CVT_UTF8_TO_16(templ, &sz, path_buf);
	if (!wbuf)
		return -1;

	wtempl = _wmktemp(wbuf);
	if (!wtempl) {
		if (wbuf != path_buf)
			free(wbuf);
		return -1;
	}

	/* update template */
	{
		const utf16_char_t *w = wtempl;
		utf8_char_t *b = (utf8_char_t*)templ;
		const size_t x = utf16_to_utf8_z_partial(&w, &b, len + 1);
		if (x < 2 || x > len + 1) {
			if (wbuf != path_buf)
				free(wbuf);
			errno = EINVAL;
			return -1;
		}
	}

	fd = _wopen(wtempl,
			_O_RDWR | _O_CREAT | _O_EXCL,
			_S_IREAD | _S_IWRITE);

	if (wbuf != path_buf)
		free(wbuf);
	return fd;
}


A_Use_decl_annotations
int localerpl_mkstemp(char *templ)
{
	if (!localerpl_is_utf8()) {
		char *const ret = _mktemp(templ);
		if (ret) {
			const int fd = _open(ret,
				_O_RDWR | _O_CREAT | _O_EXCL,
				_S_IREAD | _S_IWRITE);
			return fd;
		}
		return -1;
	}
	return utf8_rpl_mkstemp(templ);
}

A_Use_decl_annotations
char **localerpl_environ(void)
{
	return localerpl_is_utf8() ? utf8_environ() : _environ;
}

A_Use_decl_annotations
char *localerpl_getenv(const char *name)
{
	return localerpl_is_utf8() ? utf8_getenv(name) : getenv(name);
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

A_Use_decl_annotations
int localerpl_setenv(const char *name, const char *value, int overwrite)
{
	return localerpl_is_utf8()
		? utf8_setenv(name, value, overwrite)
		: rpl_setenv(name, value, overwrite);
}

A_Use_decl_annotations
int localerpl_unsetenv(const char *name)
{
	return localerpl_is_utf8()
		? utf8_unsetenv(name)
		: rpl_setenv(name, "", /*overwrite:*/1);
}

int localerpl_clearenv(void)
{
	if (localerpl_is_utf8())
		return utf8_clearenv();
	_environ = NULL;
	return 0;
}

A_Use_decl_annotations
int localerpl_printf(const char *format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = localerpl_vprintf(format, args);
	va_end(args);
	return ret;
}

A_Use_decl_annotations
int localerpl_fprintf(FILE *stream, const char *format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = localerpl_vfprintf(stream, format, args);
	va_end(args);
	return ret;
}

A_Use_decl_annotations
int localerpl_vprintf(const char *format, va_list ap)
{
	return localerpl_vfprintf(stdout, format, ap);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
#endif
ATTRIBUTE_PRINTF(format, 2, 0)
static int localerpl_vfprintf_con(FILE *stream, const char *format, va_list ap)
{
	char stack_buf[FPRINTF_BUF_SIZE], *buf = stack_buf;
	const int n = vsnprintf(buf, sizeof(stack_buf), format, ap);

	if (n == 0)
		return 0;

	if (n < 0)
		return -1;

	if ((unsigned)n > sizeof(stack_buf)) {
		buf = (char*)malloc((unsigned)n);
		if (!buf)
			return -1;
		(void)vsnprintf(buf, (unsigned)n, format, ap);
	}

	{
		const int r = fwrite_console(buf, (unsigned)n, stream);
		if (buf != stack_buf)
			free(buf);
		return r ? -1 : n;
	}
}

#ifndef NDEBUG
static int is_percent_escaped(const char format[], const char *const s)
{
	/* check if '%' is escaped by '%' */
	const char *b = s;
	for (; b != format; b--) {
		if (b[-1] != '%')
			break;
	}
	return (int)(1 & (size_t)(s - b));
}

static int is_percent_escaped_w(const wchar_t format[], const wchar_t *const s)
{
	/* check if '%' is escaped by '%' */
	const wchar_t *b = s;
	for (; b != format; b--) {
		if (b[-1] != L'%')
			break;
	}
	return (int)(1 & (size_t)(s - b));
}

static int printf_format_contains(const char *f, const char x[], const unsigned xlen)
{
	for (; *f; f += xlen) {
		const char *const s = strstr(f, x);
		if (!s)
			return 0;
		if (!is_percent_escaped(f, s))
			return 1;
	}
	return 0;
}

static int printf_format_contains_w(const wchar_t *f, const wchar_t x[], const unsigned xlen)
{
	for (; *f; f += xlen) {
		const wchar_t *const s = wcsstr(f, x);
		if (!s)
			return 0;
		if (!is_percent_escaped_w(f, s))
			return 1;
	}
	return 0;
}
#endif /* !NDEBUG */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
#endif
ATTRIBUTE_PRINTF(format, 2, 0)
static int rpl_vfprintf(FILE *stream, const char *format, va_list ap)
{
	int fmode;

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		return vfprintf(stream, format, ap);

	{
		const int r = localerpl_vfprintf_con(stream, format, ap);
		(void)turn_off_console_fd(_fileno(stream), fmode);
		return r;
	}
}

A_Use_decl_annotations
int localerpl_vfprintf(FILE *stream, const char *format, va_list ap)
{
#ifndef NDEBUG
	/* check that no "%ls"/"%lc" in format string:
	  - normally, for "%ls"/"%lc" format specifier, printf should convert wide-character
	  string/character argument to multibyte string/character according to current locale,
	  but system printf implementation does not know about our emulated UTF-8 locale.  */
	assert(!printf_format_contains(format, "%ls", 3));
	assert(!printf_format_contains(format, "%lc", 3));
	assert(!printf_format_contains(format, "%ws", 3));
	assert(!printf_format_contains(format, "%S", 2));
#endif
	return rpl_vfprintf(stream, format, ap);
}

A_Use_decl_annotations
int localerpl_wprintfmb(const wchar_t *format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = localerpl_vwprintfmb(format, args);
	va_end(args);
	return ret;
}

A_Use_decl_annotations
int localerpl_fwprintfmb(FILE *stream, const wchar_t *format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = localerpl_vfwprintfmb(stream, format, args);
	va_end(args);
	return ret;
}

A_Use_decl_annotations
int localerpl_vwprintfmb(const wchar_t *format, va_list ap)
{
	return localerpl_vfwprintfmb(stdout, format, ap);
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Check_return
A_At(stream, A_Inout)
A_At(s, A_In_z)
#endif
static int fwritemb(FILE *stream, const wchar_t *s)
{
	/* convert to multibyte string and write to stream */
	char mb_buf[MBCONV_BUF_SIZE];

	assert(*s); /* non-empty string */

	if (localerpl_is_utf8()) {
		do {
			utf8_char_t *b = (utf8_char_t*)mb_buf;
			const size_t sz = utf16_to_utf8_z_partial(&s, &b, sizeof(mb_buf));
			if (!sz) {
				errno = EILSEQ;
				return -1;
			}

			{
				const size_t to_write = (sz <= sizeof(mb_buf))
					? sz - 1/*do not write terminating '\0'*/
					: (size_t)((char*)b - mb_buf);

				assert(to_write);

				if (to_write != _fwrite_nolock(mb_buf, 1, to_write, stream))
					return -1;
			}
		} while (s[-1]);
	}
	else {
		mbstate_t ps = {
#ifndef __cplusplus
			0
#endif
		};

		do {
			const size_t len = wcsrtombs(mb_buf, &s, sizeof(mb_buf), &ps);
			if ((size_t)-1 == len)
				return -1;

			assert(len);

			if (len != _fwrite_nolock(mb_buf, 1, len, stream))
				return -1;
		} while (s);
	}

	return 0;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_arg(1)
#endif
ATTRIBUTE_WPRINTF(format, 2, 0)
static int rpl_vfwprintfmb(FILE *stream, const wchar_t *format, va_list ap)
{
	int fmode, r, n;

	wchar_t stack_buf[FPRINTF_BUF_SIZE], *buf = stack_buf;
	size_t buf_size = sizeof(stack_buf)/sizeof(stack_buf[0]);

	for (;;) {
		n = vsnwprintf(buf, buf_size, format, ap);
		if (!n) {
			if (buf != stack_buf)
				free(buf);
			return 0;
		}
		if (n > 0)
			break;
		/* make a bigger buffer and try again */
		if (buf != stack_buf)
			free(buf);
		if (buf_size < 65536)
			buf_size *= 2;
		else if (buf_size <= INT_MAX - 65536)
			buf_size += 65536;
		else {
			errno = ENOMEM;
			return -1;
		}
		/* make sure will not overflow size_t */
		(void)sizeof(int[1-2*!(INT_MAX <= (size_t)-1/sizeof(wchar_t))]);
		buf = (wchar_t*)malloc(buf_size*sizeof(wchar_t));
		if (!buf)
			return -1;
	}

	if (-1 == (fmode = turn_on_console_fd(_fileno(stream))))
		r = fwritemb(stream, buf);
	else {
		r = fwrite_console_w(buf, (unsigned)n, stream);
		(void)turn_off_console_fd(_fileno(stream), fmode);
	}

	if (buf != stack_buf)
		free(buf);

	return r ? -1 : n;
}

A_Use_decl_annotations
int localerpl_vfwprintfmb(FILE *stream, const wchar_t *format, va_list ap)
{
#ifndef NDEBUG
	/* check that no "%s"/"%c" in format string:
	  - normally, for "%s"/"%c" format specifier, wprintf should convert multibyte-character
	  string/character argument to wide-character string/character according to current locale,
	  but system wprintf implementation does not know about our emulated UTF-8 locale.  */
	assert(!printf_format_contains_w(format, L"%s", 2));
	assert(!printf_format_contains_w(format, L"%c", 2));
	assert(!printf_format_contains_w(format, L"%hs", 3));
	assert(!printf_format_contains_w(format, L"%S", 2));
	/* "%llu"/"%zu" are not supported by msvcrt.dll under WindowsXP:
	  instead of "%llu" - use "%I64u",
	  instead of "%zu" - use "%u"/"%I64u" depending on size of size_t. */
	assert(!printf_format_contains_w(format, L"%llu", 4));
	assert(!printf_format_contains_w(format, L"%zu", 3));
#endif
	return rpl_vfwprintfmb(stream, format, ap);
}
